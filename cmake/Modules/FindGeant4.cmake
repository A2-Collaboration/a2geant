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
  /opt/geant4 /opt/geant4-build
  PATH_SUFFIXES build
  NO_DEFAULT_PATH
  ) 

if(NOT Geant4_BUILD_DIR)
  message(FATAL_ERROR "Could not find Geant4 build directory containing Geant4Config.cmake, aborting.")
endif()

Message(STATUS "Looking for Geant4 build dir... - Found ${Geant4_BUILD_DIR}")


# this now makes find_package(Geant4...) available
include(${Geant4_BUILD_DIR}/Geant4Config.cmake)