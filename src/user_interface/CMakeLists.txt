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

