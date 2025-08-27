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
)

catch_discover_tests(test_user_interface)
