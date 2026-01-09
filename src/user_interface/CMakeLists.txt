add_library(user_interface
FlatbuffersUIElementConfigurator.cpp
)

target_include_directories(user_interface
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(user_interface
  PRIVATE
  SFML::Graphics
  types
  events
  data_providers

)

