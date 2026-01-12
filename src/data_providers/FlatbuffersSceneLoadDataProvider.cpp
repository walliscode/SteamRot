/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersSceneDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneLoadDataProvider.h"
#include "FlatbuffersDataLoader.h"
#include "FlatbuffersEntityImporter.h"
#include "SceneData.h"
#include "asset_config_factory.h"
#include "scene_data_generated.h"
#include <expected>
#include <variant>

namespace steamrot {
/////////////////////////////////////////////////
FlatbuffersSceneLoadDataProvider::FlatbuffersSceneLoadDataProvider(
    EventHandler &event_handler)
    : m_event_handler(event_handler) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneLoadDataProvider::ConfigureSceneInfo(
    SceneInfo &info, const SceneInfoFbs *fb_info) const {
  if (!fb_info) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound, "SceneInfoFbs is null"});
  }
  // configure scene type, data must be present
  if (!fb_info->scene_type()) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "Scene type is missing in SceneInfoFbs"});
  } else {
    info.type = fb_info->scene_type();
  }

  // configure uuid, if not present, leave as it will be generated later
  // however, if present then it needs to be a valid uuid string, error if not
  if (fb_info->scene_id()) {
    auto id_result = uuids::uuid::from_string(fb_info->scene_id()->c_str());
    if (!id_result) {
      return std::unexpected(
          FailInfo{FailMode::InvalidUUID,
                   "Scene ID in SceneInfoFbs is not a valid UUID string"});
    } else {
      info.id = id_result.value();
    }
  }

  return std::monostate{};
}
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneLoadDataProvider::ConfigureSceneResourcesConfig(
    SceneResourcesConfig &config,
    const SceneResourcesConfigFbs *fb_config) const {
  if (!fb_config) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "SceneResourcesConfigFbs is null"});
  }

  // configure texture, data must be present
  if (!fb_config->texture_width()) {

    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "Texture width is missing in SceneResourcesConfigFbs"});
  } else {
    config.texture_width = fb_config->texture_width();
  }
  if (!fb_config->texture_height()) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "Texture height is missing in SceneResourcesConfigFbs"});
  } else {
    config.texture_height = fb_config->texture_height();
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneLoadDataProvider::ConfigureSceneDataFromData(
    SceneData &scene_data, const SceneDataFbs *fb_data) const {
  if (!fb_data) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound, "SceneDataFbs is null"});
  }

  // Configure SceneInfo
  auto info_result =
      ConfigureSceneInfo(scene_data.scene_info, fb_data->scene_info());
  if (!info_result)
    return std::unexpected(info_result.error());

  // Configure SceneResourcesConfig
  if (fb_data->scene_resources_config()) {
    auto resources_result = ConfigureSceneResourcesConfig(
        scene_data.scene_resources_config, fb_data->scene_resources_config());
    if (!resources_result)
      return std::unexpected(resources_result.error());
  }

  // Configure AssetConfig
  if (fb_data->asset_config()) {
    auto asset_result = ConfigureAssetConfig(scene_data.scene_asset_config,
                                             fb_data->asset_config());
    if (!asset_result)
      return std::unexpected(asset_result.error());
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<SceneData, FailInfo>
FlatbuffersSceneLoadDataProvider::ProvideDefaultSceneData(
    const SceneType scene_type) const {

  // Load SceneData based on scene type
  FlatbuffersDataLoader data_loader;
  auto load_data_result = data_loader.ProvideDefaultSceneData(scene_type);
  if (!load_data_result.has_value()) {
    return std::unexpected(load_data_result.error());
  }
  const SceneDataFbs &fb_data = *load_data_result.value();

  // Pass to ProvideSceneDataFromData to create SceneData
  return ProvideSceneDataFromData(&fb_data);
}
/////////////////////////////////////////////////
std::expected<SceneData, FailInfo>
FlatbuffersSceneLoadDataProvider::ProvideSceneDataFromData(
    const SceneDataFbs *fb_data) const {

  // create SceneData to return
  SceneData scene_data;

  // configure SceneData
  auto configure_data_result = ConfigureSceneDataFromData(scene_data, fb_data);
  if (!configure_data_result)
    return std::unexpected(configure_data_result.error());

  // Create EntityImporter with FlatBuffers entity collection
  auto entity_importer = std::make_unique<FlatbuffersEntityImporter>(
      m_event_handler, *fb_data->entity_collection());
  if (!entity_importer) {
    return std::unexpected(FailInfo{
        FailMode::NullPointer, "Failed to create FlatbuffersEntityImporter"});
  }
  scene_data.entity_transport = std::move(entity_importer);

  // return the SceneData
  return scene_data;
}

} // namespace steamrot
