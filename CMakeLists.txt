# Setup the project
cmake_minimum_required(VERSION 2.6)
project(A2)

# check for in-source build, forbid it!
if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR AND NOT MSVC_IDE)
  message(FATAL_ERROR "\nIn-source build attempt detected!\n"
    "Please create a new directory (e.g. build) and run `cmake ..`.\n"
    "Also don't forget to delete the created CMakeCache.txt and CMakeFiles dir"
    )
endif()
    
include (cmake/settings.cmake)

message(STATUS "*** Build Type:      " ${CMAKE_BUILD_TYPE})
message(STATUS "*** Compiler Flags:  " ${DEFAULT_COMPILE_FLAGS})
message(STATUS "*** Install bin to:  " ${EXECUTABLE_OUTPUT_PATH})

# require a fairly recent ROOT version 5.30...
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(ROOT "5.30" REQUIRED)
include_directories(${ROOT_INCLUDES})
link_directories(${ROOT_LIBRARY_DIR})

# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()
message("Inc: ${Geant4_USE_FILE}")
include_directories(${Geant4_INCLUDE_DIRS})
add_definitions(${Geant4_DEFINITIONS})

#set(CMAKE_CXX_FLAGS                "${Geant4_CXX_FLAGS}")


# the project header files
include_directories(${PROJECT_SOURCE_DIR}/include)
  
# create the main A2 executable, link it to Geant4 and ROOT
# explicitly list all source files, this is recommended to detect
# changes in the source file list by the build system!
add_executable(A2 src/A2.cc
  src/A2CBOutput.cc src/A2CBSD.cc src/A2CryoTarget.cc
  src/A2DetCherenkov.cc src/A2DetCrystalBall.cc src/A2Detector.cc
  src/A2DetectorConstruction.cc src/A2DetectorMessenger.cc src/A2DetMWPC.cc
  src/A2DetPID.cc src/A2DetTAPS.cc src/A2DetTOF.cc src/A2EventAction.cc
  src/A2EventActionMessenger.cc src/A2Hit.cc src/A2MagneticField.cc
  src/A2PhysicsList.cc src/A2PhysicsListMessenger.cc src/A2PolarizedTarget.cc
  src/A2PrimaryGeneratorAction.cc src/A2PrimaryGeneratorMessenger.cc
  src/A2RunAction.cc src/A2SD.cc src/A2SolidTarget.cc src/A2SteppingAction.cc
  src/A2SteppingVerbose.cc src/A2Target.cc src/A2VisCBSD.cc src/A2VisHit.cc
  src/A2VisSD.cc src/A2WCSD.cc)
target_link_libraries(A2 ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
