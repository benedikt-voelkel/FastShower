# Find VMCFastSim

set(VMCFastSim_FOUND FALSE)
if(NOT VMCFastSim_DIR)
  message(STATUS "LWTNN cannot be found. Specify -DVMCFastSim_DIR pointing to the top
                  of the install tree.")
  return()
endif()

set(LIBRARY_NAME VMCFastSim)

find_path(VMCFastSim_INCLUDE_DIRS
          NAMES ${LIBRARY_NAME}/FastSim.h
          PATHS ${VMCFastSim_DIR}/include
)
find_path(VMCFastSim_LIBRARY_DIR
          NAMES "lib${LIBRARY_NAME}.so"
          PATHS ${VMCFastSim_DIR}/lib
)

if(NOT VMCFastSim_INCLUDE_DIR AND NOT VMCFastSim_LIBRARY_DIR)
  message(FATAL_ERROR "Could not find VMCFastSim")
endif()

set(VMCFastSim_LIBRARIES "-L${VMCFastSim_LIBRARY_DIR} -l${LIBRARY_NAME}")
#set(VMCFastSim_LIBRARIES "${LIBRARY_NAME}")

# Everything found
message(STATUS "Found VMCFastSim")
message(STATUS "Found VMCFastSim includes in ${VMCFastSim_INCLUDE_DIRS}")
message(STATUS "Found VMCFastSim libraries in ${VMCFastSim_LIBRARY_DIR}")

set(VMCFastSim_FOUND TRUE)
