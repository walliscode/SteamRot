add_executable(test_components
  CGrimoireMachina.test.cpp
  CUIState.test.cpp
)

target_link_libraries(test_components
PRIVATE
  Catch2::Catch2WithMain
  components
)

catch_discover_tests(test_components)
