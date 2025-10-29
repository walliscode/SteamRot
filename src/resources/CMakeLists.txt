add_library(resources
  resources_configuration.cpp
)

target_include_directories(resources
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(resources PUBLIC
  SFML::Graphics
  systems
  flatbuffers_headers
)
