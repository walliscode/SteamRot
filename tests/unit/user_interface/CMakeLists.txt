add_executable(test_user_interface
  styles/StylesConfigurator.test.cpp

)

target_include_directories(test_user_interface
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/styles
)

target_link_libraries(test_user_interface
  PUBLIC
  Catch2::Catch2WithMain
  context_test_helpers
  test_harness
  mock_subscriber_data
  types
)



catch_discover_tests(test_user_interface)
