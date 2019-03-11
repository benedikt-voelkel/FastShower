# @author Benedikt Volkel
# @brief  cmake setup for FastShower using VMC

set(MODULE_NAME "FastShower")

# Minimum version of CMake
CMAKE_MINIMUM_REQUIRED(VERSION 3.5.0 FATAL_ERROR)

project(${MODULE_NAME})

# Include for building tests, this generates option BUILD_TESTING which is ON
# by default
include(CTest)

################################################################################
# Extend module file path
################################################################################
set(CMAKE_MODULE_PATH
    ${CMAKE_SOURCE_DIR}/cmake
    ${CMAKE_MODULE_PATH}
)

################################################################################
# Basic configurations
################################################################################
# Sources for built are here.
set(CXX_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(CXX_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(MACRO_SOURCE_DIR ${CMAKE_SOURCE_DIR}/macro)

# Install paths
set(INSTALL_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/lib)
set(INSTALL_BINARY_DIR ${CMAKE_INSTALL_PREFIX}/bin)
set(INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/${MODULE_NAME})
#set(INSTALL_MACRO_DIR ${CMAKE_INSTALL_PREFIX}/macro)
# Collect for configure script
#set(_configure_shared_library_paths)
#set(_configure_static_library_paths)

################################################################################
# Some options to decide about backend installation
################################################################################
#option(WITH_LWTNN "Build with LWTNN backend" ON)
#option(WITH_XGBoost "Build with XGBoost backend" ON)

################################################################################
# Find required packages
################################################################################

#########
# Boost #
#########

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
message(STATUS "Boost libraries at ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost libraries ${Boost_LIBRARIES}")

########
# ROOT #
########

# NOTE Rely on $ENV{ROOTSYS} at the moment
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS Core RIO MathCore VMC EG)
include(${ROOT_USE_FILE})
# Include ROOT headers
include_directories(${ROOT_INCLUDE_DIRS})

##########
# GEANT4 #
##########

find_package(Geant4 REQUIRED CONFIG)
# Include GEANT4 headers
include_directories(${Geant4_INCLUDE_DIRS})


#######
# VGM #
#######

find_package(VGM REQUIRED)
# Include VGM headers
include_directories(${VGM_INCLUDE_DIRS})

##############
# GEANT4_VMC #
##############

find_package(Geant4VMC REQUIRED CONFIG)
# Include GEANT4_VMC headers
include_directories(${Geant4VMC_INCLUDE_DIRS})


##############
# VMCFastSim #
##############

find_package(VMCFastSim REQUIRED)
# Include VMCFastSim headers
include_directories(${VMCFastSim_INCLUDE_DIRS})

################################################################################
# Set C++ standard
################################################################################

# NOTE So far only based on ROOT
message(STATUS "Derive CXX standard from ROOT installation")
SET(CMAKE_CXX_STANDARD 11)
execute_process(
    COMMAND $ENV{ROOTSYS}/bin/root-config --cflags
    OUTPUT_VARIABLE ROOT_C_FLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE)
# Find the cxx standard
string(FIND ${ROOT_C_FLAGS} "-std=" POSITION)
if (${POSITION} GREATER -1)
  string(SUBSTRING ${ROOT_C_FLAGS} ${POSITION} 11 ROOT_CXX_STD)
  if(${ROOT_CXX_STD} STREQUAL "-std=c++1z " OR ${ROOT_CXX_STD} STREQUAL "-std=c++17 ")
    SET(CMAKE_CXX_STANDARD 17)
  elseif(${ROOT_CXX_STD} STREQUAL "-std=c++1y " OR ${ROOT_CXX_STD} STREQUAL "-std=c++14 ")
    SET(CMAKE_CXX_STANDARD 14)
  endif()
endif()
message(STATUS "Compile with ${CMAKE_CXX_STANDARD}")


################################################################################
# Compiler and linker flags
################################################################################
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O2 -g")


################################################################################
# Base sources and headers
################################################################################
# HEADERS and LIBS sum everything required for building.
# NOTE So far everything is compiled into one lib.
set(SRCS
   ${CXX_SOURCE_DIR}/FastShowerCalorHit.cxx
   ${CXX_SOURCE_DIR}/FastShowerCalorimeterSD.cxx
   ${CXX_SOURCE_DIR}/FastShowerDetectorConstruction.cxx
   ${CXX_SOURCE_DIR}/FastShowerMCApplication.cxx
   ${CXX_SOURCE_DIR}/FastShowerMCStack.cxx
   ${CXX_SOURCE_DIR}/FastShowerPrimaryGenerator.cxx
)
set(HEADERS
   #${CXX_INCLUDE_DIR}/FastShower.h
   ${CXX_INCLUDE_DIR}/FastShowerCalorHit.h
   ${CXX_INCLUDE_DIR}/FastShowerCalorimeterSD.h
   ${CXX_INCLUDE_DIR}/FastShowerDetectorConstruction.h
   ${CXX_INCLUDE_DIR}/FastShowerMCApplication.h
   ${CXX_INCLUDE_DIR}/FastShowerMCStack.h
   ${CXX_INCLUDE_DIR}/FastShowerPrimaryGenerator.h
)

################################################################################
# ROOT LinkDef
################################################################################
SET(ROOT_DICT_LINKDEF_FILE "${CXX_SOURCE_DIR}/${MODULE_NAME}LinkDef.h")
message(STATUS "LinkDef at ${ROOT_DICT_LINKDEF_FILE}")
SET(ROOT_DICT_NAME "G__${MODULE_NAME}")

ROOT_GENERATE_DICTIONARY(${ROOT_DICT_NAME} ${HEADERS} LINKDEF ${ROOT_DICT_LINKDEF_FILE})
# Files produced by the dictionary generation
SET(ROOT_DICT_LIB_FILES
    "${PROJECT_BINARY_DIR}/lib${MODULE_NAME}_rdict.pcm"
    "${PROJECT_BINARY_DIR}/lib${MODULE_NAME}.rootmap"
)

################################################################################
# Used for any additional directories and libraries to be included and used
# for built
################################################################################
#set(INCLUDES)
#set(LIBS)
#set(SCRIPTS)


################################################################################
# Check for backends
################################################################################
#include(DeployBackends)


################################################################################
# Setup for library built and installation
################################################################################

include_directories(${CXX_INCLUDE_DIR})
set(LIBRARY_NAME ${MODULE_NAME})
add_library(${LIBRARY_NAME} SHARED ${SRCS} "${ROOT_DICT_NAME}.cxx")
target_link_libraries(${LIBRARY_NAME} ${ROOT_LIBRARIES})
#list(APPEND _configure_shared_library_paths ${INSTALL_LIBRARY_DIR})

add_executable(runFastShower ${CXX_SOURCE_DIR}/runFastShower.cxx)
target_link_libraries(runFastShower ${LIBRARY_NAME} ${Boost_LIBRARIES} ${Geant4VMC_LIBRARIES} ${Geant4_LIBRARIES} ${VMCFastSim_LIBRARIES} ${ROOT_LIBRARIES})

################################################################################
# Configure the config and setup script
################################################################################
# This project
#set(SETUP_SCRIPT_IN ${CONFIG_SOURCE_DIR}/setup.in)
#set(SETUP_SCRIPT_OUT ${CMAKE_BINARY_DIR}/setup)
#include(BuildConfig)

################################################################################
# Build tests
################################################################################
#if(NOT BUILD_TESTING)
#  message(WARNING "No tests are built.")
#else()
#  add_subdirectory(test)
#endif(NOT BUILD_TESTS)

################################################################################
# Install the project
################################################################################
# Install headers
install(FILES ${HEADERS} DESTINATION ${INSTALL_INCLUDE_DIR})
# Install libraries
install(TARGETS ${LIBRARY_NAME} DESTINATION ${INSTALL_LIBRARY_DIR})
# Install binaries
install(TARGETS runFastShower DESTINATION ${INSTALL_BINARY_DIR})
# Install the ROOT dictionary files
install(FILES ${ROOT_DICT_LIB_FILES} DESTINATION ${INSTALL_LIBRARY_DIR})
