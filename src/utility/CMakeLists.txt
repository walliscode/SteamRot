
add_library(utilities
   DataManager.cpp
   SchemaChecker.cpp

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

target_link_libraries(utilities PUBLIC
nlohmann_json::nlohmann_json
data
logger
flatbuffers_headers
flatbuffers
systems
)


