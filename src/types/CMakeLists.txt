# This will be an interface library for type definitions

add_library(types INTERFACE)

target_include_directories(types
  INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/user_interface/
  ${CMAKE_CURRENT_SOURCE_DIR}/user_interface/styles/
  ${CMAKE_CURRENT_SOURCE_DIR}/core/
  ${CMAKE_CURRENT_SOURCE_DIR}/events/
  ${CMAKE_CURRENT_SOURCE_DIR}/assets/
  ${CMAKE_CURRENT_SOURCE_DIR}/test_structs/
  ${CMAKE_CURRENT_SOURCE_DIR}/flatbuffers/generated/


)

target_link_libraries(types
  INTERFACE
  SFML::Graphics
  stduuid
  interfaces
  flatbuffers
  flatbuffers_headers

)

add_subdirectory(flatbuffers)
