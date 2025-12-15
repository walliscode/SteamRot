/////////////////////////////////////////////////
/// @file
/// @brief Implements a scene configurator that uses default settings for
/// Flatbuffers scenes.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneConfigurator.h"
#include "FailInfo.h"
#include "FbsSceneData.h"
#include "Scene.h"
#include <expected>
#include <variant>

namespace steamrot {
/////////////////////////////////////////////////

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::ConfigureSceneInfo(Scene &scene,
                                                 const SceneData *scene_data) {

  // cast to derived SceneData type
  FbsSceneData *fbs_scene_data =
      const_cast<FbsSceneData *>(static_cast<const FbsSceneData *>(scene_data));

  // check its valid
  if (!fbs_scene_data)
    return std::unexpected(
        FailInfo(FailMode::InvalidCast, "SceneData is not FbsSceneData"));

  // [TODO: implement UUID checks, conversion or creation]

  scene.GetSceneInfo().type = fbs_scene_data->scene_info.type;
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::ConfigureSceneResources(
    Scene &scene, const SceneData *scene_data) {

  // cast to derived SceneData type
  FbsSceneData *fbs_scene_data =
      const_cast<FbsSceneData *>(static_cast<const FbsSceneData *>(scene_data));

  // check its valid
  if (!fbs_scene_data)
    return std::unexpected(
        FailInfo(FailMode::InvalidCast, "SceneData is not FbsSceneData"));

  // [TODO: implement resource loading from flatbuffers data]

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::ConfigureSceneConfig(
    Scene &scene, const SceneData *scene_data) {

  // cast to derived SceneData type
  FbsSceneData *fbs_scene_data =
      const_cast<FbsSceneData *>(static_cast<const FbsSceneData *>(scene_data));

  // check its valid
  if (!fbs_scene_data)
    return std::unexpected(
        FailInfo(FailMode::InvalidCast, "SceneData is not FbsSceneData"));

  // SceneConfig not active at the moment
  return std::monostate{};
}
} // namespace steamrot
