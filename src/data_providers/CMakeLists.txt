
configure_file(
paths.cpp.in
paths.cpp
@ONLY)

add_library(data_providers
  FlatbuffersEngineDataProvider.cpp
  FlatbuffersAssetDataProvider.cpp
  FlatbuffersSceneManagerDataProvider.cpp
  FlatbuffersSceneDataProvider.cpp
  FlatbuffersSubscriberViewer.cpp
  provider_factory.cpp
  DataLoader.cpp
  FlatbuffersDataLoader.cpp
  data_file_utils.cpp
  paths.cpp
)

target_include_directories(data_providers
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(data_providers
  PUBLIC
  types
  SFML::Graphics
  events
  interfaces
)
