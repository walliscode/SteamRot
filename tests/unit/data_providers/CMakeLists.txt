add_executable(test_data_providers
  FlatbuffersEngineDataProvider.test.cpp
  FlatbuffersSceneDataProvider.test.cpp
  ISubscriberDataViewer.test.cpp
  SubscriberDataViewer.test.cpp
)

target_link_libraries(test_data_providers
  PRIVATE
  Catch2::Catch2WithMain
  data_providers
  config
)

catch_discover_tests(test_data_providers)
