/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersSceneDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneDataProvider.h"
#include "FlatbuffersDataLoader.h"
#include "SceneData.h"
#include "asset_config_factory.h"
#include "scene_data_generated.h"
#include <expected>

namespace steamrot {
/////////////////////////////////////////////////
FlatbuffersSceneDataProvider::FlatbuffersSceneDataProvider(
    EventHandler &event_handler)
    : m_event_handler(event_handler) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneDataProvider::ConfigureSceneInfo(
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
FlatbuffersSceneDataProvider::ConfigureSceneResourcesConfig(
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
std::expected<std::unique_ptr<SceneData>, FailInfo>
FlatbuffersSceneDataProvider::ProvideSceneDataFromData(
    const SceneDataFbs *scene_data_fbs) const {

  // if (!scene_data_fbs) {
  //   return std::unexpected(
  //       FailInfo{FailMode::FlatbuffersDataNotFound, "SceneDataFbs is null"});
  // }
  // // create SceneData
  // FbsSceneData scene_data;
  //
  // // configure the SceneInfo
  // auto configure_info_result =
  //     ConfigureSceneInfo(scene_data.scene_info,
  //     scene_data_fbs->scene_info());
  // if (!configure_info_result.has_value()) {
  //   return std::unexpected(configure_info_result.error());
  // }
  //
  // // configure the SceneResourcesConfig
  // if (scene_data_fbs->scene_resources_config()) {
  //   auto configure_resources_result =
  //       ConfigureSceneResourcesConfig(scene_data.scene_resources_config,
  //                                     scene_data_fbs->scene_resources_config());
  //   if (!configure_resources_result.has_value()) {
  //     return std::unexpected(configure_resources_result.error());
  //   }
  // }
  // // populate the AssetConfig
  // if (scene_data_fbs->asset_config()) {
  //   auto configure_asset_result = ConfigureAssetConfig(
  //       scene_data.scene_asset_config, scene_data_fbs->asset_config());
  //   if (!configure_asset_result.has_value()) {
  //     return std::unexpected(configure_asset_result.error());
  //   }
  // }
  // // assign pointer to Flatbuffers entity collection
  // scene_data.entity_collection = scene_data_fbs->entity_collection();
  return std::make_unique<SceneData>();
}

/////////////////////////////////////////////////
std::expected<std::unique_ptr<SceneData>, FailInfo>
FlatbuffersSceneDataProvider::ProvideDefaultSceneData(
    const SceneType scene_type) const {

  // Load SceneData based on scene type
  FlatbuffersDataLoader data_loader;
  auto load_data_result = data_loader.ProvideDefaultSceneData(scene_type);
  if (!load_data_result.has_value()) {
    return std::unexpected(load_data_result.error());
  }
  const SceneDataFbs &fb_data = *load_data_result.value();

  auto scene_data_result = ProvideSceneDataFromData(&fb_data);
  if (!scene_data_result.has_value()) {
    return std::unexpected(scene_data_result.error());
  }
  return scene_data_result;
}
/////////////////////////////////////////////////
std::expected<SceneLoadData, FailInfo>
FlatbuffersSceneDataProvider::ProvideSceneLoadData(
    const SceneType scene_type) const {

  // Load FlatBuffers data
  FlatbuffersDataLoader data_loader;
  auto load_result = data_loader.ProvideDefaultSceneData(scene_type);
  if (!load_result)
    return std::unexpected(load_result.error());

  const SceneDataFbs &fb_data = *load_result.value();

  // Create SceneData (lightweight, no entity pointer)
  SceneData scene_data;

  // Configure SceneInfo
  auto info_result =
      ConfigureSceneInfo(scene_data.scene_info, fb_data.scene_info());
  if (!info_result)
    return std::unexpected(info_result.error());

  // Configure SceneResourcesConfig
  if (fb_data.scene_resources_config()) {
    auto resources_result = ConfigureSceneResourcesConfig(
        scene_data.scene_resources_config, fb_data.scene_resources_config());
    if (!resources_result)
      return std::unexpected(resources_result.error());
  }

  // Configure AssetConfig
  if (fb_data.asset_config()) {
    auto asset_result = ConfigureAssetConfig(scene_data.scene_asset_config,
                                             fb_data.asset_config());
    if (!asset_result)
      return std::unexpected(asset_result.error());
  }

  // Create EntityImporter with FlatBuffers entity collection
  auto entity_importer = std::make_unique<FlatbuffersEntityImporter>(
      m_event_handler, *fb_data.entity_collection());

  // Return both
  return SceneLoadData{.scene_data = std::move(scene_data),
                       .entity_importer = std::move(entity_importer)};
  if (!fb_data.entity_collection()) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "entity_collection not found in SceneDataFbs"});
  }
}
} // namespace steamrot
