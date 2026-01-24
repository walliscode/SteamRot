/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for configuring scene data
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_scene_data.h"
#include "configure_asset_config.h"

namespace steamrot::data::configure {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureSceneInfo(SceneInfo &info, const SceneInfoFbs *fb_info) {

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
ConfigureSceneResourcesConfig(SceneResourcesConfig &config,
                              const SceneResourcesConfigFbs *fb_config) {
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
ConfigureSceneDataFromData(SceneData &scene_data, const SceneDataFbs *fb_data) {
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
    auto asset_result = data::configure::ConfigureAssetConfig(
        scene_data.scene_asset_config, fb_data->asset_config());
    if (!asset_result)
      return std::unexpected(asset_result.error());
  }

  return std::monostate{};
}

} // namespace steamrot::data::configure
