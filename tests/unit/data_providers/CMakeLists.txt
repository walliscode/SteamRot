configure_file(
  FlatbuffersDataLoader.test.cpp.in
  FlatbuffersDataLoader.test.cpp
  @ONLY
)

# Compile UI element test data
include(compile_ui_element_test_data.cmake)

# Compile engine snapshot test data
include(compile_engine_snapshot_test_data.cmake)

add_executable(test_data_providers
  DataAccessFactory.test.cpp
  FlatbuffersDataLoader.test.cpp
  FlatbuffersEngineDataProvider.test.cpp
  FlatbuffersEngineSnapshotProvider.test.cpp
  FlatbuffersSceneManagerDataProvider.test.cpp
  FlatbuffersSceneDataProvider.test.cpp
  FlatbuffersSaveDataProvider.test.cpp
  FlatbuffersUIStyleDataProvider.test.cpp
  FlatbuffersGrimoireMachinaProvider.test.cpp
  FlatbuffersLogicConfigCollectionProvider.test.cpp
  FlatbuffersInputActionConfigProvider.test.cpp
  configure_asset_config.test.cpp
  configure_engine_data.test.cpp
  configure_engine_snapshot.test.cpp
  configure_event.test.cpp
  configure_save_data.test.cpp
  configure_scene_data.test.cpp
  configure_scene_manager_data.test.cpp
  configure_ui_styles.test.cpp
  configure_ui_elements.test.cpp
  configure_grimoire.test.cpp
  configure_logic_config.test.cpp
  configure_input_action.test.cpp
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
add_dependencies(test_data_providers compile_engine_snapshot_unit_test_data)

catch_discover_tests(test_data_providers)
