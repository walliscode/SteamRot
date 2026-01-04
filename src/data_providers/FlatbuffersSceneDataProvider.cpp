/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersSceneDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneDataProvider.h"
#include "FbsSceneData.h"
#include "FlatbuffersDataLoader.h"
#include "asset_config_factory.h"
#include "scene_data_generated.h"
#include <expected>

namespace steamrot {

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
FlatbuffersSceneDataProvider::ProvideDefaultSceneData(
    const SceneType scene_type) const {

  // Load SceneData based on scene type
  FlatbuffersDataLoader data_loader;
  auto load_data_result = data_loader.ProvideDefaultSceneData(scene_type);
  if (!load_data_result.has_value()) {
    return std::unexpected(load_data_result.error());
  }
  const SceneDataFbs &fb_data = *load_data_result.value();

  // create SceneData
  FbsSceneData scene_data;

  // configure the SceneResourcesConfig
  if (fb_data.scene_resources_config()) {
    auto configure_resources_result = ConfigureSceneResourcesConfig(
        scene_data.scene_resources_config, fb_data.scene_resources_config());
    if (!configure_resources_result.has_value()) {
      return std::unexpected(configure_resources_result.error());
    }
  }

  // no UUID needed for default scene data
  scene_data.scene_info.type = scene_type;

  // populate the AssetConfig
  if (fb_data.asset_config()) {
    auto configure_asset_result = ConfigureAssetConfig(
        scene_data.scene_asset_config, fb_data.asset_config());
    if (!configure_asset_result.has_value()) {
      return std::unexpected(configure_asset_result.error());
    }
  }

  // assign pointer to Flatbuffers entity collection
  scene_data.entity_collection = fb_data.entity_collection();

  return std::make_unique<FbsSceneData>(scene_data);
}

} // namespace steamrot
