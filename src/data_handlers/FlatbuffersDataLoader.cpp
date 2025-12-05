/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersDataLoader class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include "FailInfo.h"
#include "Fragment.h"
#include "assets_generated.h"
#include "core_data_generated.h"
#include "engine_config_generated.h"
#include "engine_data_generated.h"
#include "fragments_generated.h"
#include "paths.h"
#include "scene_data_generated.h"
#include "ui_style_generated.h"
#include "user_preferences_generated.h"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <expected>
#include <format>

namespace steamrot {

/////////////////////////////////////////////////
std::expected<Fragment, FailInfo>
FlatbuffersDataLoader::ProvideFragment(const std::string &fragment_name) const {
  // check if the bin file exists
  std::filesystem::path fragment_path =
      paths::GetFragmentDirectory() / (fragment_name + ".fragment.bin");

  if (!std::filesystem::exists(fragment_path)) {
    FailInfo fail_info(
        FailMode::FlatbuffersDataNotFound,
        std::format("Fragment file not found: {}", fragment_path.string()));
    return std::unexpected(fail_info);
  }

  const steamrot::FragmentData *fragment_data =
      GetFragmentData(LoadBinaryData(fragment_path));

  Fragment fragment;

  // check every possible field, not all flatbuffers data types are required
  // as this operation is not frequent we shall make it belts and braces
  if (!fragment_data->name()) {
    FailInfo fail_info(FailMode::FlatbuffersDataNotFound,
                       "Fragment name not found in fragment data");
    return std::unexpected(fail_info);
  }

  fragment.m_name = fragment_data->name()->str();

  // handle socket data
  if (!fragment_data->socket_data())
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "fragment socket data not found"));

  // handle socket data vertices
  if (fragment_data->socket_data()->vertices()->size() == 0)
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "fragment socket data vertices not found"));

  for (const auto &vertex : *fragment_data->socket_data()->vertices()) {
    if (!vertex->x() || !vertex->y())
      return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                      "vertex from socket data is incomplete"));

    // add vector data to fragment sockets
    fragment.m_sockets.emplace_back(vertex->x(), vertex->y());
  }

  // handle render overlays
  if (fragment_data->render_overlay_data()->views()->empty())
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "fragment render views not found"));

  // handle view triangles
  for (const auto &view : *fragment_data->render_overlay_data()->views()) {
    if (view->triangles()->empty()) {
      return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                      "view triangles not found"));
    }
    // handle triangle vertices
    for (const auto &triangle : *view->triangles()) {
      if (triangle->vertices()->size() != 3) {
        return std::unexpected(
            FailInfo(FailMode::FlatbuffersDataNotFound,
                     "fragment triangles must have 3 vertices"));
      }
    }

    // handle view direction
    if (!view->direction()) {
      return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                      "view direction not found"));
    }

    // add view to fragment
    sf::VertexArray view_to_add(sf::PrimitiveType::Triangles);
    for (const auto &triangle : *view->triangles()) {
      for (const auto &vertex : *triangle->vertices()) {
        // create a vertex with position and color
        sf::Vertex vertex_to_add(
            sf::Vector2f(vertex->position()->x(), vertex->position()->y()),
            sf::Color(vertex->color()->r(), vertex->color()->g(),
                      vertex->color()->b(), vertex->color()->a()));
        view_to_add.append(vertex_to_add);
      }
    }

    // add to m_overlays
    fragment.m_overlays[view->direction()] = view_to_add;
  }

  return fragment;
}

/////////////////////////////////////////////////
std::expected<std::map<std::string, Fragment>, FailInfo>
FlatbuffersDataLoader::ProvideAllFragments(
    std::vector<std::string> fragment_names) const {

  std::map<std::string, Fragment> fragments;

  for (const auto &fragment_name : fragment_names) {
    auto fragment_result = ProvideFragment(fragment_name);
    // pass up any errors
    if (!fragment_result.has_value()) {
      return std::unexpected(fragment_result.error());
    }
    fragments[fragment_name] = fragment_result.value();
  }
  return fragments;
}

/////////////////////////////////////////////////
std::expected<const EngineDataFbs *, FailInfo>
FlatbuffersDataLoader::ProvideEngineData() const {
  // get engine directory from defaults
  std::filesystem::path engine_dir = paths::GetDefaultEngineDirectory();

  // construct the file path
  std::filesystem::path engine_data_path = engine_dir / "engine_data.bin";

  // check if the file exists
  if (!std::filesystem::exists(engine_data_path)) {
    std::string error_message = std::format(
        "Engine resources data not found: {}", engine_data_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }
  // load the game engine data
  const steamrot::EngineDataFbs *engine_data =
      GetEngineDataFbs(LoadBinaryData(engine_data_path));
  return engine_data;
}

/////////////////////////////////////////////////
std::expected<const SceneManagerData *, FailInfo>
FlatbuffersDataLoader::ProvideSceneManagerData() const {
  // get scene manager directory from defaults
  std::filesystem::path scene_manager_dir =
      paths::GetDefaultSceneManagerDirectory();

  // construct the file path
  std::filesystem::path scene_manager_path =
      scene_manager_dir / "scene_manager.bin";
  // check if the file exists
  if (!std::filesystem::exists(scene_manager_path)) {
    std::string error_message = std::format("Scene Manager file not found: {}",
                                            scene_manager_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }
  // load the scene manager data
  const steamrot::SceneManagerData *scene_manager_data =
      GetSceneManagerData(LoadBinaryData(scene_manager_path));

  return scene_manager_data;
}

/////////////////////////////////////////////////
std::expected<const SceneDataData *, FailInfo>
FlatbuffersDataLoader::ProvideDefaultSceneData(
    const SceneType scene_type) const {

  // get file prefix from scene type
  std::string scene_file_prefix;
  switch (scene_type) {
  case SceneType::SceneType_UNKNOWN: {
    scene_file_prefix = "unknown";
    break;
  }
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
      scene_dir / (scene_file_prefix + ".scenes.bin");

  // check if the file exists
  if (!std::filesystem::exists(scene_path)) {
    std::string error_message =
        std::format("Scene file not found: {}", scene_file_prefix);
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }

  // load the scene data
  const steamrot::SceneDataData *scene_data =
      GetSceneDataData(LoadBinaryData(scene_path));

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

  const SceneDataData *scene_data = scene_data_result.value();
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
std::expected<const UIStyleData *, FailInfo>
FlatbuffersDataLoader::ProvideUIStylesData(
    const std::string &style_name) const {
  // get the UIStyleDirectory
  std::filesystem::path ui_style_dir = paths::GetUIStylesDirectory();

  // construct the file path
  std::filesystem::path ui_style_path =
      ui_style_dir / (style_name + ".styles.bin");
  // check if the file exists
  if (!std::filesystem::exists(ui_style_path)) {
    std::string error_message =
        std::format("UI Style file not found: {}", ui_style_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }
  // load the UI style data
  const steamrot::UIStyleData *ui_style_data =
      GetUIStyleData(LoadBinaryData(ui_style_path));
  if (!ui_style_data) {
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "UIStyleData pointer is null"));
  }

  return ui_style_data;
}

/////////////////////////////////////////////////
std::expected<const ContextData *, FailInfo>
FlatbuffersDataLoader::ProvideContextData() const {
  // get context directory from defaults
  std::filesystem::path context_dir = paths::GetDefaultContextDirectory();

  // construct the file path
  std::filesystem::path context_path = context_dir / "context_data.bin";

  // check if the file exists
  if (!std::filesystem::exists(context_path)) {
    std::string error_message =
        std::format("Context data file not found: {}", context_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }

  // load the context data
  const steamrot::ContextData *context_data =
      GetContextData(LoadBinaryData(context_path));

  if (!context_data) {
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "ContextData pointer is null"));
  }

  return context_data;
}

/////////////////////////////////////////////////
std::expected<const EngineCoreDataFbs *, FailInfo>
FlatbuffersDataLoader::ProvideEngineCoreData() const {
  // Load from EngineData
  auto engine_result = ProvideEngineData();
  if (!engine_result.has_value()) {
    return std::unexpected(engine_result.error());
  }

  const EngineDataFbs *engine_data = engine_result.value();
  if (!engine_data->engine_core()) {
    return std::unexpected(
        FailInfo(FailMode::FlatbuffersDataNotFound,
                 "EngineCoreDataFbs not found in EngineData"));
  }

  return engine_data->engine_core();
}

/////////////////////////////////////////////////
std::expected<const SceneCoreDataFbs *, FailInfo>
FlatbuffersDataLoader::ProvideSceneCoreData(const SceneType scene_type) const {
  // Load from SceneData for the specified scene type
  auto scene_data_result = ProvideDefaultSceneData(scene_type);
  if (!scene_data_result.has_value()) {
    return std::unexpected(scene_data_result.error());
  }

  const SceneDataData *scene_data = scene_data_result.value();

  // scene_core is optional, so it's okay if it's not present
  // Return nullptr if not configured - caller should handle defaults
  return scene_data->scene_core();
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
  const SceneDataData *scene_data = scene_data_result.value();
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

/////////////////////////////////////////////////
std::expected<const EngineConfigData *, FailInfo>
FlatbuffersDataLoader::ProvideEngineConfigData() const {
  // First check for user-specific engine config
  std::filesystem::path user_engine_dir = paths::GetUserDirectory() / "engine";
  std::filesystem::path user_config_path =
      user_engine_dir / "engine_config.bin";

  // If user config exists, load it
  if (std::filesystem::exists(user_config_path)) {
    const steamrot::EngineConfigData *config_data =
        GetEngineConfigData(LoadBinaryData(user_config_path));

    if (config_data) {
      return config_data;
    }
    // If user config exists but failed to load, fall through to defaults
  }

  // Load default engine config
  std::filesystem::path default_engine_dir = paths::GetDefaultEngineDirectory();
  std::filesystem::path default_config_path =
      default_engine_dir / "engine_config.bin";

  if (!std::filesystem::exists(default_config_path)) {
    std::string error_message = std::format("Engine config file not found: {}",
                                            default_config_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }

  const steamrot::EngineConfigData *config_data =
      GetEngineConfigData(LoadBinaryData(default_config_path));

  if (!config_data) {
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "EngineConfigData pointer is null"));
  }

  return config_data;
}

} // namespace steamrot
