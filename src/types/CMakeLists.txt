# This will be an interface library for type definitions

add_library(types
entity/JointInstance.cpp)

target_include_directories(types
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/user_interface/
  ${CMAKE_CURRENT_SOURCE_DIR}/user_interface/styles/
  ${CMAKE_CURRENT_SOURCE_DIR}/core/
  ${CMAKE_CURRENT_SOURCE_DIR}/engine/
  ${CMAKE_CURRENT_SOURCE_DIR}/scenes/
  ${CMAKE_CURRENT_SOURCE_DIR}/logic/
  ${CMAKE_CURRENT_SOURCE_DIR}/entity/
  ${CMAKE_CURRENT_SOURCE_DIR}/configuration/
  ${CMAKE_CURRENT_SOURCE_DIR}/components/
  ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/
  ${CMAKE_CURRENT_SOURCE_DIR}/events/
  ${CMAKE_CURRENT_SOURCE_DIR}/assets/
  ${CMAKE_CURRENT_SOURCE_DIR}/test_structs/
  ${CMAKE_CURRENT_SOURCE_DIR}/flatbuffers/generated/
  ${CMAKE_CURRENT_SOURCE_DIR}/helpers/



)

target_link_libraries(types
  PUBLIC
  SFML::Graphics
  stduuid
  flatbuffers
  flatbuffers_headers

)

add_subdirectory(flatbuffers)
