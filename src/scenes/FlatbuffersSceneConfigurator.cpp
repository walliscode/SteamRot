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
#include "uuid.h"
#include <expected>
#include <variant>

namespace steamrot {
/////////////////////////////////////////////////

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::ConfigureSceneInfo(Scene &scene,
                                                 const SceneData *scene_data) {

  // check for null SceneData
  if (!scene_data)
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "SceneData pointer is null"));

  // cast to derived SceneData type
  FbsSceneData *fbs_scene_data =
      dynamic_cast<FbsSceneData *>(const_cast<SceneData *>(scene_data));

  // check its valid
  if (!fbs_scene_data)
    return std::unexpected(
        FailInfo(FailMode::InvalidCast, "SceneData is not FbsSceneData"));

  // i tried to add valid uuid checking here, but as it is type uuid, it HAS to
  // be valid. Error checking should happen at data provider not configurator

  // if UUID is nil, generate one
  if (fbs_scene_data->scene_info.id.is_nil()) {
    scene.GetSceneInfo().id = uuids::uuid_system_generator{}();
  }

  scene.GetSceneInfo().type = fbs_scene_data->scene_info.type;
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::ConfigureSceneResources(
    Scene &scene, const SceneData *scene_data) {

  // check for null SceneData
  if (!scene_data)
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "SceneData pointer is null"));

  // cast to derived SceneData type
  FbsSceneData *fbs_scene_data =
      dynamic_cast<FbsSceneData *>(const_cast<SceneData *>(scene_data));

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

  // check for null SceneData
  if (!scene_data)
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "SceneData pointer is null"));

  // cast to derived SceneData type
  FbsSceneData *fbs_scene_data =
      dynamic_cast<FbsSceneData *>(const_cast<SceneData *>(scene_data));

  // check its valid
  if (!fbs_scene_data)
    return std::unexpected(
        FailInfo(FailMode::InvalidCast, "SceneData is not FbsSceneData"));
  // SceneConfig not active at the moment so nothing else to add
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::ConfigureEntities(Scene &scene,
                                                const SceneData *scene_data) {

  // check for null SceneData
  if (!scene_data)
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "SceneData pointer is null"));

  // cast to derived SceneData type
  FbsSceneData *fbs_scene_data =
      dynamic_cast<FbsSceneData *>(const_cast<SceneData *>(scene_data));

  // check its valid
  if (!fbs_scene_data)
    return std::unexpected(
        FailInfo(FailMode::InvalidCast, "SceneData is not FbsSceneData"));

  // check for SceneDataFbs
  if (!fbs_scene_data->scene_data_fbs)
    return std::unexpected(
        FailInfo(FailMode::FlatbuffersDataNotFound, "SceneDataFbs not found"));

  // check for entity data
  if (!fbs_scene_data->scene_data_fbs->entity_collection())
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "No entity data found in SceneData"));

  return std::monostate{};
}
} // namespace steamrot
