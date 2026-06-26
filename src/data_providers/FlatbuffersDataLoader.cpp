/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersDataLoader class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include "FailInfo.h"
#include "fragment_generated.h"
#include "input_action_config_generated.h"
#include "logic_config_generated.h"
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
  case SceneType::TEST: {
    scene_file_prefix = "test";
    break;
  }
  case SceneType::TITLE: {
    scene_file_prefix = "title";
    break;
  }
  case SceneType::CRAFTING: {
    scene_file_prefix = "crafting";
    break;
  }
  case SceneType::SPATIAL_ANALYSIS: {
    scene_file_prefix = "spatial_analysis";
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
std::expected<std::vector<const UIStyleDataFbs *>, FailInfo>
FlatbuffersDataLoader::ProvideUIStylesData() const {

  // get the UIStyleDirectory
  std::filesystem::path ui_style_dir = paths::GetUIStylesDirectory();

  // find all fles in the directiry with styles.bin extension
  std::vector<const UIStyleDataFbs *> ui_styles;

  for (const auto &entry : std::filesystem::directory_iterator(ui_style_dir)) {

    // check for .bin files ending with .styles.bin
    if (entry.path().extension() == ".bin" &&
        entry.path().filename().string().ends_with(".styles.bin")) {

      // load the style data using the generated function from flatbuffers
      const steamrot::UIStyleDataFbs *style_data =
          GetUIStyleDataFbs(LoadBinaryData(entry.path()));
      if (style_data) {
        ui_styles.push_back(style_data);
      } else {
        // unexpected error if style data is null
        std::string error_message =
            std::format("UIStyleDataFbs pointer is null for file: {}",
                        entry.path().string());
        return std::unexpected(
            FailInfo(FailMode::FlatbuffersDataNotFound, error_message));
      }
    }
  }

  return ui_styles;
}

/////////////////////////////////////////////////
std::expected<const EngineDataFbs *, FailInfo>
FlatbuffersDataLoader::ProvideEngineDataFbs() const {
  // get engine directory from defaults
  std::filesystem::path engine_dir = paths::GetDefaultEngineDirectory();
  // construct the file path
  std::filesystem::path engine_data_path =
      engine_dir / "default.engine_data.bin";
  // check if the file exists
  if (!std::filesystem::exists(engine_data_path)) {
    std::string error_message = std::format("Engine data file not found: {}",
                                            engine_data_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }
  // load the engine data
  const steamrot::EngineDataFbs *engine_data =
      GetEngineDataFbs(LoadBinaryData(engine_data_path));
  if (!engine_data) {
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "EngineDataFbs pointer is null"));
  }
  return engine_data;
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
std::expected<const LogicConfigCollectionFbs *, FailInfo>
FlatbuffersDataLoader::ProvideLogicConfigCollectionFbs() const {

  // get defaults directory
  std::filesystem::path defaults_dir = paths::GetDefaultsDirectory();
  // construct the file path
  std::filesystem::path logic_config_path =
      defaults_dir / "logic_config/logic_config.bin";
  // check if the file exists
  if (!std::filesystem::exists(logic_config_path)) {
    std::string error_message = std::format("Logic config file not found: {}",
                                            logic_config_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }
  // load the logic config collection data
  const steamrot::LogicConfigCollectionFbs *logic_config_collection_data =
      GetLogicConfigCollectionFbs(LoadBinaryData(logic_config_path));
  if (!logic_config_collection_data) {
    return std::unexpected(
        FailInfo(FailMode::FlatbuffersDataNotFound,
                 "LogicConfigCollectionFbs pointer is null"));
  }

  return logic_config_collection_data;
}

/////////////////////////////////////////////////
std::expected<const UserPreferencesDataFbs *, FailInfo>
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
  const steamrot::UserPreferencesDataFbs *preferences_data =
      GetUserPreferencesDataFbs(LoadBinaryData(preferences_path));

  if (!preferences_data) {
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "UserPreferencesDataFbs pointer is null"));
  }

  return preferences_data;
}
/////////////////////////////////////////////////
std::expected<std::vector<const FragmentFbs *>, FailInfo>
FlatbuffersDataLoader::ProvideAllFragmentData() const {
  // create return vector
  std::vector<const FragmentFbs *> fragments;

  // get the fragments directory
  std::filesystem::path fragments_dir = paths::GetAssetsDirectory();
  fragments_dir /= "fragments";

  // iterate over all .bin files in the fragments directory
  for (const auto &entry : std::filesystem::directory_iterator(fragments_dir)) {
    // check for .bin files
    if (entry.path().extension() == ".bin") {
      // load the fragment data using the generated function from flatbuffers
      const steamrot::FragmentFbs *fragment_data =
          GetFragmentFbs(LoadBinaryData(entry.path()));
      if (fragment_data) {
        fragments.push_back(fragment_data);
      } else {
        // unexpected error if fragment data is null
        std::string error_message = std::format(
            "FragmentFbs pointer is null for file: {}", entry.path().string());
        return std::unexpected(
            FailInfo(FailMode::FlatbuffersDataNotFound, error_message));
      }
    }
  }

  return fragments;
}

/////////////////////////////////////////////////
std::expected<std::vector<const JointFbs *>, FailInfo>
FlatbuffersDataLoader::ProvideAllJointData() const {
  // create return vector
  std::vector<const JointFbs *> joints;

  // get the joints directory
  std::filesystem::path joints_dir = paths::GetAssetsDirectory();
  joints_dir /= "joints";
  // iterate over all .bin files in the joints directory
  for (const auto &entry : std::filesystem::directory_iterator(joints_dir)) {
    // check for .bin files
    if (entry.path().extension() == ".bin") {
      // load the joint data using the generated function from flatbuffers
      const steamrot::JointFbs *joint_data =
          GetJointFbs(LoadBinaryData(entry.path()));
      if (joint_data) {
        joints.push_back(joint_data);
      } else {
        // unexpected error if joint data is null
        std::string error_message = std::format(
            "JointFbs pointer is null for file: {}", entry.path().string());
        return std::unexpected(
            FailInfo(FailMode::FlatbuffersDataNotFound, error_message));
      }
    }
  }
  return joints;
}

/////////////////////////////////////////////////
std::expected<const InputActionConfigFbs *, FailInfo>
FlatbuffersDataLoader::ProvideDefaultInputActionConfigFbs() const {
  // get input actions directory
  std::filesystem::path input_actions_dir =
      paths::GetDefaultInputActionsDirectory();
  // construct the file path
  std::filesystem::path input_action_path =
      input_actions_dir / "default.input_action_config.bin";
  // check if the file exists
  if (!std::filesystem::exists(input_action_path)) {
    std::string error_message = std::format(
        "Input action config file not found: {}", input_action_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }
  // load the input action config data
  const steamrot::InputActionConfigFbs *input_action_config =
      GetInputActionConfigFbs(LoadBinaryData(input_action_path));
  if (!input_action_config) {
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "InputActionConfigFbs pointer is null"));
  }
  return input_action_config;
}
} // namespace steamrot
