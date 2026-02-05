
configure_file(
paths.cpp.in
paths.cpp
@ONLY)

add_library(data_providers
  FlatbuffersEngineDataProvider.cpp
  FlatbuffersEngineSnapshotProvider.cpp
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
  configure/configure_engine_data.cpp
  configure/configure_engine_snapshot.cpp
  configure/configure_asset_config.cpp
  configure/configure_save_data.cpp
  configure/configure_scene_manager_data.cpp
  configure/configure_ui_styles.cpp
  configure/configure_scene_data.cpp
  configure/configure_ui_elements.cpp
  configure/configure_grimoire.cpp

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
