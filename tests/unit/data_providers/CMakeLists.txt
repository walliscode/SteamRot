configure_file(
  FlatbuffersDataLoader.test.cpp.in
  FlatbuffersDataLoader.test.cpp
  @ONLY
)

add_executable(test_data_providers
  FlatbuffersDataLoader.test.cpp
  FlatbuffersAssetDataProvider.test.cpp
  FlatbuffersEngineDataProvider.test.cpp
  FlatbuffersSceneManagerDataProvider.test.cpp
  FlatbuffersSubscriberViewer.test.cpp
)

target_link_libraries(test_data_providers
  PRIVATE
  Catch2::Catch2WithMain
  data_providers
  types
  events
)

catch_discover_tests(test_data_providers)
