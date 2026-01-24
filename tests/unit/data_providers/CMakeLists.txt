configure_file(
  FlatbuffersDataLoader.test.cpp.in
  FlatbuffersDataLoader.test.cpp
  @ONLY
)

# Compile UI element test data
include(compile_ui_element_test_data.cmake)

add_executable(test_data_providers
  DataAccessFactory.test.cpp
  FlatbuffersDataLoader.test.cpp
  FlatbuffersEngineDataProvider.test.cpp
  FlatbuffersSceneManagerDataProvider.test.cpp
  FlatbuffersSceneDataProvider.test.cpp
  FlatbuffersSubscriberViewer.test.cpp
  FlatbuffersSaveDataProvider.test.cpp
  FlatbuffersUIStyleDataProvider.test.cpp
  FlatbuffersUIElementProvider.test.cpp
  configure_asset_config.test.cpp
  configure_engine_data.test.cpp
  configure_save_data.test.cpp
  configure_scene_data.test.cpp
  configure_scene_manager_data.test.cpp
  configure_ui_styles.test.cpp
  configure_ui_elements.test.cpp
)


target_link_libraries(test_data_providers
  PRIVATE
  Catch2::Catch2WithMain
  data_providers
  types
  events
  context_test_helpers
)

# Ensure test data is compiled before building tests
add_dependencies(test_data_providers compile_ui_element_unit_test_data)

catch_discover_tests(test_data_providers)
