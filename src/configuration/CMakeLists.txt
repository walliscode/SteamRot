# Configuration library for UI element and style configurators

add_library(configuration
  FlatbuffersUIElementConfigurator.cpp
)

target_include_directories(configuration
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(configuration
  PUBLIC
  types
  interfaces
  events
  user_interface
  PRIVATE
  SFML::Graphics
)
