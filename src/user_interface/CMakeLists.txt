add_library(user_interface
UIElement.cpp
UIElementFactory.cpp
)

target_include_directories(user_interface
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(user_interface
  PUBLIC
  SFML::Graphics
  systems
  flatbuffers
  flatbuffers_headers
  events
)

add_library(ui_styles
styles/StylesConfigurator.cpp
)

target_include_directories(ui_styles
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/styles
)

target_link_libraries(ui_styles
  PUBLIC
  SFML::Graphics
  logger
  data_handlers
)
