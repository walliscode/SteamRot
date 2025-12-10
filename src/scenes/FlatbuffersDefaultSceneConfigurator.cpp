/////////////////////////////////////////////////
/// @file
/// @brief Implements a scene configurator that uses default settings for
/// Flatbuffers scenes.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDefaultSceneConfigurator.h"
#include "FailInfo.h"
#include <expected>
#include <variant>

namespace steamrot {
/////////////////////////////////////////////////

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersDefaultSceneConfigurator::ConfigureSceneInfo(
    Scene &scene, const SceneType scene_type) {

  // load in SceneDataFbs

  auto load_result = m_data_loader.ProvideDefaultSceneData(scene_type);
  if (!load_result)
    return std::unexpected(load_result.error());
  // guard statements
  if (!load_result.value()->scene_info())
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "SceneDataFbs missing SceneInfo"));

  // pull out scene info
  auto &scene_info_fbs = *(load_result.value()->scene_info());

  // if no scene id, generate one
  // TODO: implement uuid generation

  // if scene id check it is a valid uuid
  // TODO: implement uuid validation

  // set scene type
  scene.GetSceneInfo().type = scene_info_fbs.scene_type();

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersDefaultSceneConfigurator::ConfigureSceneResources(
    Scene &scene, const SceneType scene_type) {

  // load in SceneResources data from Flatbuffers
  auto load_result = m_data_loader.ProvideDefaultSceneData(scene_type);
  if (!load_result)
    return std::unexpected(load_result.error());

  // guard statements
  if (!load_result.value()->scene_resources())
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "SceneDataFbs missing SceneResources"));
  // pull out scene resources
  auto &scene_resources_fbs = *(load_result.value()->scene_resources());

  // set resources from fbs data
  // [TODO: implement resource loading]
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersDefaultSceneConfigurator::ConfigureSceneConfig(
    Scene &scene, const SceneType scene_type) {

  // SceneConfig not active at the moment
  return std::monostate{};
}
} // namespace steamrot
