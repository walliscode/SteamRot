
configure_file(
paths.cpp.in
paths.cpp
@ONLY)

add_library(data_providers
  FlatbuffersEngineDataProvider.cpp
  FlatbuffersSceneManagerDataProvider.cpp
  FlatbuffersSceneDataProvider.cpp
  FlatbuffersSubscriberViewer.cpp
  FlatbuffersUIStyleDataProvider.cpp
  FlatbuffersSaveDataProvider.cpp
  DataAccessFactory.cpp
  DataLoader.cpp
  FlatbuffersDataLoader.cpp
  data_file_utils.cpp
  paths.cpp
  asset_config_factory.cpp
  configure_engine_data.cpp

)

target_include_directories(data_providers
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/configure/
)

target_link_libraries(data_providers
  PUBLIC
  SFML::Graphics
  events
  entity
  types
)
