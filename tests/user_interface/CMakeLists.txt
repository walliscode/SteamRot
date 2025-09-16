add_executable(test_user_interface
  styles/StylesConfigurator.test.cpp
  UIElementFactory.test.cpp
  ui_element_factory_helpers.cpp
)

target_include_directories(test_user_interface
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/styles
)

target_link_libraries(test_user_interface
  PUBLIC
  Catch2::Catch2WithMain
  ui_styles
  test_context
  mock_subscriber_data
)

add_library(user_interface_helpers
  ui_element_factory_helpers.cpp

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
