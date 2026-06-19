add_library(render_logic
  render_background.cpp
)

target_include_directories(render_logic
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(render_logic
  PRIVATE
  SFML::Graphics
)
