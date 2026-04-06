add_executable(test_resize_strategies
  DirectScaleResizeStrategy.test.cpp
  CappedScaleResizeStrategy.test.cpp
  ExtendedViewResizeStrategy.test.cpp
)

target_link_libraries(test_resize_strategies
  PRIVATE
  Catch2::Catch2WithMain
  types
  SFML::Graphics
)

catch_discover_tests(test_resize_strategies)
