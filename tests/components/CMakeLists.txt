add_executable(test_components
  CGrimoireMachina.test.cpp
  CUIState.test.cpp
  CMeta.test.cpp
  CUserInterface.test.cpp
  CMachinaForm.test.cpp
)

target_include_directories(test_components
PRIVATE
  ${CMAKE_SOURCE_DIR}/tests/components
)

target_link_libraries(test_components
PRIVATE
  Catch2::Catch2WithMain
  components
)

catch_discover_tests(test_components)
