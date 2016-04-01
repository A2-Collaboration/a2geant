# we make our life easy:
# we search for the Geant4 build directory,
# and then simply include the cmake file provided
# by Geant4 itself!

# the build directory contains
Message(STATUS "Looking for Geant4 build dir...")

find_path(Geant4_BUILD_DIR NAMES Geant4Config.cmake
  PATHS
  $ENV{G4INSTALL}
  "$ENV{G4INSTALL}-build"
  /home/dglazier/Work/cern/geant4/geant4.10.01/geant4.10.1-build
  /opt/geant4 /opt/geant4-build
  /cern/geant4 /cern/geant4-build
  PATH_SUFFIXES build
  NO_DEFAULT_PATH
  ) 

if(NOT Geant4_BUILD_DIR)
  message(FATAL_ERROR "Could not find Geant4 build directory containing Geant4Config.cmake, aborting.")
endif()

Message(STATUS "Looking for Geant4 build dir... - Found ${Geant4_BUILD_DIR}")

# this now makes find_package(Geant4...) available
include(${Geant4_BUILD_DIR}/Geant4Config.cmake)


# gather the G4 environment,
# but only G4*DATA variables (not the other crap)
execute_process(COMMAND
  ${CMAKE_SOURCE_DIR}/cmake/g4env.sh
  ${Geant4_BUILD_DIR}/geant4make.sh
  OUTPUT_VARIABLE G4_ENV_OUT
  )
# make it a list
string(REPLACE "\n" ";" G4_ENV_OUT "${G4_ENV_OUT}")

# start creating the file, this defines a preprocessor macro
# defining the 
set(G4_ENV_INC_FILE "${CMAKE_BINARY_DIR}/include/CMakeG4Env.hh")
file(WRITE ${G4_ENV_INC_FILE} "#ifndef CMakeG4Env_hh
#define CMakeG4Env_hh 1
#include <stdlib.h>
#define SET_G4_ENV_BY_CMAKE() \\
")
foreach(f ${G4_ENV_OUT})
  string(REPLACE "=" ";" name_val "${f}")
  list(GET name_val 0 name)
  list(GET name_val 1 val)
  #message("name ${name} ${val}")
  file(APPEND ${G4_ENV_INC_FILE}
    "setenv(\"${name}\",\"${val}\",0); \\
")
endforeach()

# just a newline for the last \
file(APPEND ${G4_ENV_INC_FILE} "
#endif")

# don't forget the include directory
include_directories("${CMAKE_BINARY_DIR}/include")
