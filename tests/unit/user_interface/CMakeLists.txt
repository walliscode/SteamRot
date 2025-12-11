add_executable(test_user_interface
  styles/StylesConfigurator.test.cpp

  ui_element_factory_test_helpers.cpp
)

target_include_directories(test_user_interface
  PRIVATE
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

add_library(user_interface_helpers
  ui_element_factory_test_helpers.cpp

)

target_include_directories(user_interface_helpers
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/styles
)

target_link_libraries(user_interface_helpers
  PUBLIC
  Catch2::Catch2WithMain
  ui_styles
  mock_subscriber_data
)

catch_discover_tests(test_user_interface)
