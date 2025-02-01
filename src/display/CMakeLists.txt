add_library(display
    DisplayManager.cpp

)

# check if set definitions exist
set(definition_list RESOURCES_DIR)

# foreach(definition_name IN LISTS definition_list)
#   if(NOT DEFINED ${definition_name})
#     message(FATAL_ERROR "Definition ${definition_name} does not exist")
#   endif()
# endforeach()
#

target_include_directories(display
  PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include/utils)

target_link_libraries(display PUBLIC
    SFML::Graphics
    nlohmann_json::nlohmann_json
)
