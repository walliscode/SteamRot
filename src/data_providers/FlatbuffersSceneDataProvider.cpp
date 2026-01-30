/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersSceneDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneDataProvider.h"
#include "FlatbuffersDataLoader.h"
#include "FlatbuffersEntityConfigurator.h"
#include "SceneData.h"
#include "configure_asset_config.h"
#include "configure_scene_data.h"
#include "scene_data_generated.h"
#include <expected>
#include <variant>

namespace steamrot {
/////////////////////////////////////////////////
FlatbuffersSceneDataProvider::FlatbuffersSceneDataProvider(
    EventHandler &event_handler)
    : m_event_handler(event_handler) {}

/////////////////////////////////////////////////
FlatbuffersSceneDataProvider::FlatbuffersSceneDataProvider(
    EventHandler &event_handler, const SceneDataFbs *scene_data)
    : m_scene_data_fbs(scene_data), m_event_handler(event_handler) {}

/////////////////////////////////////////////////
std::expected<SceneData, FailInfo>
FlatbuffersSceneDataProvider::CreateSceneData(const SceneType scene_type) {

  // Load SceneData based on scene type
  FlatbuffersDataLoader data_loader;
  auto load_data_result = data_loader.ProvideDefaultSceneData(scene_type);
  if (!load_data_result.has_value()) {
    return std::unexpected(load_data_result.error());
  }

  // assign to member variable
  m_scene_data_fbs = load_data_result.value();

  // Create SceneData object
  SceneData scene_data;

  // configure SceneData
  auto configure_data_result = ConfigureSceneData(scene_data);
  if (!configure_data_result)
    return std::unexpected(configure_data_result.error());

  return scene_data;
}
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneDataProvider::ConfigureSceneData(SceneData &scene_data) const {

  // Validate that m_scene_data_fbs is not null
  if (!m_scene_data_fbs) {
    return std::unexpected(FailInfo{
        FailMode::NullPointer, "FlatBuffers SceneDataFbs pointer is null"});
  }

  // call the free functions to configure SceneData
  // Configure SceneInfo
  auto info_result = data::configure::ConfigureSceneInfo(
      scene_data.scene_info, m_scene_data_fbs->scene_info());
  if (!info_result)
    return std::unexpected(info_result.error());

  // Configure SceneResourcesConfig
  if (m_scene_data_fbs->scene_resources_config()) {
    auto resources_result = data::configure::ConfigureSceneResourcesConfig(
        scene_data.scene_resources_config,
        m_scene_data_fbs->scene_resources_config());
    if (!resources_result)
      return std::unexpected(resources_result.error());
  }

  // Configure AssetConfig
  if (m_scene_data_fbs->asset_config()) {
    auto asset_result = data::configure::ConfigureAssetConfig(
        scene_data.scene_asset_config, m_scene_data_fbs->asset_config());
    if (!asset_result)
      return std::unexpected(asset_result.error());
  }

  // check for entity data, error if missing
  if (!m_scene_data_fbs->entity_collection()) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "FlatBuffers SceneDataFbs missing entity data"});
  } else {
    scene_data.entity_transport = m_scene_data_fbs->entity_collection();
  }

  // assign entity configurator
  scene_data.entity_configurator =
      std::make_unique<FlatbuffersEntityConfigurator>(m_event_handler);

  return std::monostate{};
}

} // namespace steamrot
