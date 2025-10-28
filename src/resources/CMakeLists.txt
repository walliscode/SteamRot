add_library(resources
  ResourceConfigurator.cpp
)

target_include_directories(resources
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(resources PUBLIC
  SFML::Graphics
  systems
  flatbuffers_headers
)
