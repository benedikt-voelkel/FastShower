# Find Pythia6

set(pythia6_FOUND FALSE)
if(NOT pythia6_DIR)
  message(STATUS "pythia cannot be found. Specify -Dpythia6_DIR pointing to the top
                  of the install tree.")
  return()
endif()

set(LIBRARY_NAME pythia6)


find_path(pythia6_LIBRARY_DIR
          NAMES "lib${LIBRARY_NAME}.so"
          PATHS ${pythia6_DIR}/lib
)

if(NOT pythia6_LIBRARY_DIR)
  message(FATAL_ERROR "Could not find pythia6")
endif()

set(pythia6_LIBRARIES "-L${pythia6_LIBRARY_DIR} -l${LIBRARY_NAME}")

# Everything found
message(STATUS "Found pythia6")
message(STATUS "Found pythia6 libraries in ${pythia6_LIBRARY_DIR}")

set(pythia6_FOUND TRUE)
