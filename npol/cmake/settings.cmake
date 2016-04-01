if(NOT DEFINED EXECUTABLE_OUTPUT_PATH)
  # set binary output to the source directory,
  # most users are used to this!
	set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}")
endif()
