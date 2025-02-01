
# create a list of files to check

set(files_to_check
  ${CMAKE_SOURCE_DIR}/resources/jsons/display_config.json
)
# check files exist, if not return fatal error

foreach(file ${files_to_check})
  if(NOT EXISTS ${file})
    message(FATAL_ERROR "File not found: ${file}")
  endif()
endforeach()
