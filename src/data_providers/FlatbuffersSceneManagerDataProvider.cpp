/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersSceneManagerDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneManagerDataProvider.h"
#include "FlatbuffersDataLoader.h"
#include "configure_scene_manager_data.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<SceneManagerData, FailInfo>
FlatbuffersSceneManagerDataProvider::CreateSceneManagerData() const {

  // Load default SceneManagerData from file
  FlatbuffersDataLoader data_loader;
  auto result = data_loader.ProvideSceneManagerData();
  if (!result.has_value()) {
    return std::unexpected(result.error());
  }
  const SceneManagerDataFbs *scene_manager_data_fbs = result.value();
  if (!scene_manager_data_fbs) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "SceneManagerDataFbs data is null"});
  }

  // create SceneManagerData to populate
  SceneManagerData scene_manager_data;

  // configure SceneManagerData

  return scene_manager_data;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneManagerDataProvider::ConfigureSceneManagerData(
    SceneManagerData &scene_manager_data) const {

  // Load SceneManagerData from file
  FlatbuffersDataLoader data_loader;
  auto result = data_loader.ProvideSceneManagerData();
  if (!result.has_value()) {
    return std::unexpected(result.error());
  }
  const SceneManagerDataFbs *scene_manager_data_fbs = result.value();
  if (!scene_manager_data_fbs) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "SceneManagerDataFbs data is null"});
  }

  // confgiure SceneManagerData using free functions
  // configure scene manager state
  auto configure_state_result = data::configure::ConfigureSceneManagerState(
      scene_manager_data.scene_manager_state, scene_manager_data_fbs->state());
  if (!configure_state_result.has_value()) {
    return std::unexpected(configure_state_result.error());
  }
  return std::monostate{};
}
} // namespace steamrot
