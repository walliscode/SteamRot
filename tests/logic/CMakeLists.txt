add_executable(test_logic
LogicFactory.test.cpp
UIRenderLogic.test.cpp
logic_helpers.cpp
)

target_include_directories(test_logic
PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test_logic
  PRIVATE
  Catch2::Catch2WithMain
  logic
  test_context
)

catch_discover_tests(test_logic)
