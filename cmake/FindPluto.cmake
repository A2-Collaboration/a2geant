# - Find EPICS instalation
#
# Variables defined by this module:
#
#   PLUTO_FOUND              Pluto library and include path were found
#
#   PLUTO_INCLUDE_PATH       The pluto inlcude path
#
#   PLUTO_LIBRARY            Full path of the pluto library
#
#   PLUTO_LIBRARY_DIR        Path to the pluto library
#

Message(STATUS "Looking for Pluto...")

Set(PLUTO_SEARCHPATH
	$ENV{PLUTOSYS}
	$ENV{HOME}/src/pluto
	$ENV{HOME}/pluto
	/opt/pluto/
)

Set(PLUTO_FOUND FALSE)

find_library(PLUTO_LIBRARY NAMES Pluto PATHS ${PLUTO_SEARCHPATH})
get_filename_component(PLUTO_LIBRARY_PATH ${PLUTO_LIBRARY} PATH)

find_path(PLUTO_INCLUDE_PATH NAMES PPlutoBulkDecay.h PATHS ${PLUTO_SEARCHPATH} PATH_SUFFIXES src)

if(NOT PLUTO_LIBRARY)
	Message(STATUS "Looking for pluto... Library not found!")
else()
	Message(STATUS "Looking for Pluto... PLUTO_LIBRARY: " ${PLUTO_LIBRARY})
endif()

if(NOT PLUTO_INCLUDE_PATH)
	Message(STATUS "Looking for Pluto...  Include path not found!")
else()
	Message(STATUS "Looking for Pluto... PLUTO_INCLUDE_PATH: " ${PLUTO_INCLUDE_PATH})
endif()

if( PLUTO_LIBRARY AND PLUTO_INCLUDE_PATH )
	Set(PLUTO_FOUND TRUE)
endif()

