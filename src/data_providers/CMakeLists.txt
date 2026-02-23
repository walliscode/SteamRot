
configure_file(
paths.cpp.in
paths.cpp
@ONLY)

add_library(data_providers
  FlatbuffersEngineDataProvider.cpp
  FlatbuffersEngineSnapshotProvider.cpp
  FlatbuffersSceneManagerDataProvider.cpp
  FlatbuffersSceneDataProvider.cpp
  FlatbuffersUIStyleDataProvider.cpp
  FlatbuffersSaveDataProvider.cpp
  FlatbuffersGrimoireMachinaProvider.cpp
  FlatbuffersLogicConfigCollectionProvider.cpp
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
  configure/configure_logic_config.cpp
  configure/configure_event.cpp
  configure/configure_subscriber.cpp
  configure/configure_input_action.cpp

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
