/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersDataLoader class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include "FailInfo.h"
#include "assets_generated.h"
#include "paths.h"
#include "scene_data_generated.h"
#include "ui_style_generated.h"
#include "user_preferences_generated.h"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <expected>
#include <format>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
std::expected<const SceneDataFbs *, FailInfo>
FlatbuffersDataLoader::ProvideDefaultSceneData(
    const SceneType scene_type) const {

  // get file prefix from scene type
  std::string scene_file_prefix;
  switch (scene_type) {
  case SceneType::SceneType_TEST: {
    scene_file_prefix = "test";
    break;
  }
  case SceneType::SceneType_TITLE: {
    scene_file_prefix = "title";
    break;
  }
  case SceneType::SceneType_CRAFTING: {
    scene_file_prefix = "crafting";
    break;
  }
  default:
    return std::unexpected(
        FailInfo(FailMode::SceneTypeNotFound, "Invalid SceneType provided"));
  }

  // get the SceneDirectory
  std::filesystem::path scene_dir = paths::GetSceneDirectory();

  // construct the file path
  std::filesystem::path scene_path =
      scene_dir / (scene_file_prefix + ".scene_data.bin");

  // check if the file exists
  if (!std::filesystem::exists(scene_path)) {
    std::string error_message =
        std::format("Scene file not found: {}", scene_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }

  // load the scene data
  const steamrot::SceneDataFbs *scene_data =
      GetSceneDataFbs(LoadBinaryData(scene_path));

  return scene_data;
}

/////////////////////////////////////////////////

std::expected<const AssetCollection *, FailInfo>
FlatbuffersDataLoader::ProvideAssetData() const {

  // get asset manager directory from defaults
  std::filesystem::path asset_dir = paths::GetDefaultAssetManagerDirectory();

  // construct the file path
  std::filesystem::path asset_path = asset_dir / "asset_manager.bin";

  // check if the file exists
  if (!std::filesystem::exists(asset_path)) {
    std::string error_message =
        std::format("Asset file not found: {}", asset_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }
  // load the asset data
  const steamrot::AssetCollection *asset_data =
      GetAssetCollection(LoadBinaryData(asset_path));
  if (!asset_data) {
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "AssetCollection pointer is null"));
  }
  return asset_data;
}

/////////////////////////////////////////////////
std::expected<const AssetCollection *, FailInfo>
FlatbuffersDataLoader::ProvideAssetData(const SceneType scene_type) const {

  // get scene data
  auto scene_data_result = ProvideDefaultSceneData(scene_type);
  if (!scene_data_result.has_value()) {
    return std::unexpected(scene_data_result.error());
  }

  const SceneDataFbs *scene_data = scene_data_result.value();
  if (!scene_data) {
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "SceneData pointer is null"));
  }

  if (!scene_data->assets()) {
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "Asset collection not found"));
  }
  return scene_data->assets();
}

/////////////////////////////////////////////////
std::expected<std::vector<const UIStyleData *>, FailInfo>
FlatbuffersDataLoader::ProvideUIStylesData() const {
  // get the UIStyleDirectory
  std::filesystem::path ui_style_dir = paths::GetUIStylesDirectory();

  // find all fles in the directiry with styles.bin extension
  std::vector<const UIStyleData *> ui_styles;

  for (const auto &entry : std::filesystem::directory_iterator(ui_style_dir)) {

    // check for .bin files ending with .styles.bin
    if (entry.path().extension() == ".bin" &&
        entry.path().filename().string().ends_with(".styles.bin")) {

      // load the style data using the generated function from flatbuffers
      const steamrot::UIStyleData *style_data =
          GetUIStyleData(LoadBinaryData(entry.path()));
      if (style_data) {
        ui_styles.push_back(style_data);
      } else {
        // unexpected error if style data is null
        std::string error_message = std::format(
            "UIStyleData pointer is null for file: {}", entry.path().string());
        return std::unexpected(
            FailInfo(FailMode::FlatbuffersDataNotFound, error_message));
      }
    }
  }

  return ui_styles;
}



/////////////////////////////////////////////////
std::expected<const EngineResourcesConfigFbs *, FailInfo>
FlatbuffersDataLoader::ProvideEngineResourcesConfigFbs() const {
  // get engine directory from defaults
  std::filesystem::path engine_dir = paths::GetDefaultEngineDirectory();

  // construct the file path
  std::filesystem::path engine_resources_path =
      engine_dir / "default.engine_resources_config.bin";

  // check if the file exists
  if (!std::filesystem::exists(engine_resources_path)) {
    std::string error_message =
        std::format("Engine resources config not found: {}",
                    engine_resources_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }
  // load the engine resources config
  const steamrot::EngineResourcesConfigFbs *engine_resources_config =
      GetEngineResourcesConfigFbs(LoadBinaryData(engine_resources_path));
  return engine_resources_config;
}

/////////////////////////////////////////////////
std::expected<const EngineConfigFbs *, FailInfo>
FlatbuffersDataLoader::ProvideEngineConfigFbs() const {
  // First check for user-specific engine config
  std::filesystem::path user_engine_dir = paths::GetUserDirectory() / "engine";
  std::filesystem::path user_config_path =
      user_engine_dir / "default.engine_config.bin";

  // If user config exists, load it
  if (std::filesystem::exists(user_config_path)) {
    const steamrot::EngineConfigFbs *config_data =
        GetEngineConfigFbs(LoadBinaryData(user_config_path));

    if (config_data) {
      return config_data;
    }
    // If user config exists but failed to load, fall through to defaults
  }

  // Load default engine config
  std::filesystem::path default_engine_dir = paths::GetDefaultEngineDirectory();
  std::filesystem::path default_config_path =
      default_engine_dir / "default.engine_config.bin";

  if (!std::filesystem::exists(default_config_path)) {
    std::string error_message = std::format("Engine config file not found: {}",
                                            default_config_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }

  const steamrot::EngineConfigFbs *config_data =
      GetEngineConfigFbs(LoadBinaryData(default_config_path));

  if (!config_data) {
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "EngineConfigFbs pointer is null"));
  }

  return config_data;
}

/////////////////////////////////////////////////
std::expected<const EngineStateFbs *, FailInfo>
FlatbuffersDataLoader::ProvideEngineStateFbs() const {
  // get engine directory from defaults
  std::filesystem::path engine_dir = paths::GetDefaultEngineDirectory();

  // construct the file path
  std::filesystem::path engine_state_path =
      engine_dir / "default.engine_state.bin";

  // check if the file exists
  if (!std::filesystem::exists(engine_state_path)) {
    std::string error_message = std::format("Engine state file not found: {}",
                                            engine_state_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }
  // load the engine state data
  const steamrot::EngineStateFbs *engine_state_data =
      GetEngineStateFbs(LoadBinaryData(engine_state_path));
  if (!engine_state_data) {
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "EngineStateFbs pointer is null"));
  }
  return engine_state_data;
}
/////////////////////////////////////////////////
std::expected<const SceneManagerDataFbs *, FailInfo>
FlatbuffersDataLoader::ProvideSceneManagerData() const {
  // get scene manager directory from defaults
  std::filesystem::path scene_manager_dir =
      paths::GetDefaultSceneManagerDirectory();
  // construct the file path
  std::filesystem::path scene_manager_path =
      scene_manager_dir / "default.scene_manager_data.bin";
  // check if the file exists
  if (!std::filesystem::exists(scene_manager_path)) {
    std::string error_message = std::format(
        "Scene manager data file not found: {}", scene_manager_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }
  // load the scene manager data
  const steamrot::SceneManagerDataFbs *scene_manager_data =
      GetSceneManagerDataFbs(LoadBinaryData(scene_manager_path));
  if (!scene_manager_data) {
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "SceneManagerDataFbs pointer is null"));
  }
  return scene_manager_data;
}
/////////////////////////////////////////////////
std::expected<const LogicCollectionData *, FailInfo>
FlatbuffersDataLoader::ProvideLogicCollectionData(
    const SceneType scene_type) const {
  // Load from SceneData for the specified scene type
  auto scene_data_result = ProvideDefaultSceneData(scene_type);
  if (!scene_data_result.has_value()) {
    return std::unexpected(scene_data_result.error());
  }
  const SceneDataFbs *scene_data = scene_data_result.value();
  if (!scene_data->logic_collection_data()) {
    return std::unexpected(
        FailInfo(FailMode::FlatbuffersDataNotFound,
                 "LogicCollectionData not found in SceneData"));
  }
  return scene_data->logic_collection_data();
}
/////////////////////////////////////////////////
std::expected<const UserPreferencesData *, FailInfo>
FlatbuffersDataLoader::ProvideDefaultUserPreferencesData() const {
  // get preferences directory
  std::filesystem::path preferences_dir = paths::GetPreferencesDirectory();

  // construct the file path - expects default.preferences.bin
  std::filesystem::path preferences_path =
      preferences_dir / "default.preferences.bin";

  // check if the file exists
  if (!std::filesystem::exists(preferences_path)) {
    std::string error_message = std::format(
        "Default preferences file not found: {}", preferences_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }

  // load the preferences data
  const steamrot::UserPreferencesData *preferences_data =
      GetUserPreferencesData(LoadBinaryData(preferences_path));

  if (!preferences_data) {
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "UserPreferencesData pointer is null"));
  }

  return preferences_data;
}

} // namespace steamrot
