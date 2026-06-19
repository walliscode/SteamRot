add_executable(test_render_logic
  render_background.test.cpp

)

target_link_libraries(test_render_logic
  PRIVATE
  Catch2::Catch2WithMain
  render_logic
  SFML::Graphics
  matchers
)

catch_discover_tests(test_render_logic)
