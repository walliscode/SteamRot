
add_library(utilities
   DataManager.cpp
)

# check if set definitions exist
set(definition_list
RESOURCES_DIR
DATA_OUT_DIR
)

# foreach(definition_name IN LISTS definition_list)
#   if(NOT DEFINED ${definition_name})
#     message(FATAL_ERROR "Definition ${definition_name} does not exist")
#   endif()
# endforeach()
#

target_include_directories(utilities
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/include/utils
  ${CMAKE_SOURCE_DIR}/include/constants
  ${CMAKE_SOURCE_DIR}/include/enums
)

# check if targets exist
set(target_list
display
magic_enum
entity)
foreach(target_name IN LISTS target_list)
  if(NOT TARGET ${target_name})
    message(FATAL_ERROR "Target ${target_name} does not exist")
  endif()
endforeach()


