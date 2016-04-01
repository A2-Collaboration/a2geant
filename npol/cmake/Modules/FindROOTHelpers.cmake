# This generates a ROOT dictionary from a LinkDef file by using rootcint
function (ROOT_GENERATE_DICTIONARY HEADERS LINKDEF_FILE DICTFILE INCLUDE_DIRS)
  # construct -I arguments
  foreach(f ${INCLUDE_DIRS})
    list(APPEND INCLUDE_DIRS_ARGS -I"${f}")   
  endforeach() 

  # also add the outfile with extension .h 
  get_filename_component(DICTFILEDIR ${DICTFILE} PATH)
  get_filename_component(DICTFILENAME_WE ${DICTFILE} NAME_WE)
  get_filename_component(DICTFILENAME ${DICTFILE} NAME)
  set(DICTFILES ${DICTFILE} "${DICTFILEDIR}/${DICTFILENAME_WE}.h")

  # and ensure the output directory exists
  file(MAKE_DIRECTORY ${DICTFILEDIR})
  
  # prepare rootcint command
  if(CMAKE_SYSTEM_NAME MATCHES Linux)
    set(LDPREFIX "LD_LIBRARY_PATH")
  elseif(CMAKE_SYSTEM_NAME MATCHES Darwin)
    set(LDPREFIX "DYLD_LIBRARY_PATH")
  else()
    message(FATAL_ERROR "Unsupported System for ROOT Dictionary generation")
  endif()

  add_custom_command(OUTPUT ${DICTFILES}
    COMMAND
    ${LDPREFIX}=${ROOT_LIBRARY_DIR}
    ROOTSYS=${ROOTSYS}
    ${ROOT_CINT_EXECUTABLE}
    -f "${DICTFILE}" -c -p ${INCLUDE_DIRS_ARGS} ${HEADERS} "${LINKDEF_FILE}"
    DEPENDS ${HEADERS} ${LINKDEF_FILE}
    )

  # this little trick re-runs cmake if the LINKDEF_FILE was changed
  # this is needed since rootcint needs an up-to-date list of input files
  file(RELATIVE_PATH STAMP_FILE ${CMAKE_SOURCE_DIR} ${LINKDEF_FILE})
  string(REPLACE "/" "_" STAMP_FILE ${STAMP_FILE})
  set(STAMP_FILE "${CMAKE_BINARY_DIR}/cmake/stamps/${STAMP_FILE}.stamp")
  configure_file("${LINKDEF_FILE}" "${STAMP_FILE}" COPYONLY)
  set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${STAMP_FILE}")
  
endfunction()
