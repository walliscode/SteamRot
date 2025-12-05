add_library(core
  core_configuration.cpp
)

target_include_directories(core
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(core PUBLIC
  SFML::Graphics
  engine
  flatbuffers_headers
  data_providers
)
