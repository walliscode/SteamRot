/////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersSceneManagerDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneManagerDataProvider.h"
#include "FailInfo.h"
#include "configure_scene_manager_data.h"

namespace steamrot {

/////////////////////////////////////////////////
std::expected<SceneManagerData, FailInfo>
FlatbuffersSceneManagerDataProvider::CreateSceneManagerData() const {

  // create return object
  SceneManagerData data;

  // configure scene manager data
  auto configure_result = ConfigureSceneManagerData(data);
  if (!configure_result.has_value()) {
    return std::unexpected(configure_result.error());
  }

  return data;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneManagerDataProvider::ConfigureSceneManagerData(
    SceneManagerData &scene_manager_data) const {

  // use flatbuffers data loader to get SceneManagerDataFbs
  auto load_scene_manager_data_result = m_loader.ProvideSceneManagerData();
  if (!load_scene_manager_data_result.has_value()) {
    return std::unexpected(load_scene_manager_data_result.error());
  }
  const SceneManagerDataFbs *fb_scene_manager_data =
      load_scene_manager_data_result.value();

  // populate SceneManagerState
  auto populate_state_result = data::configure::ConfigureSceneManagerState(
      scene_manager_data.scene_manager_state, fb_scene_manager_data->state());
  // check for errors
  if (!populate_state_result.has_value()) {
    return std::unexpected(populate_state_result.error());
  }

  return std::monostate{};
}
} // namespace steamrot
