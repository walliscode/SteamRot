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
#include "FlatbuffersEntityConfigurator.h"
#include "Scene.h"
#include "uuid.h"
#include <SFML/System/Vector2.hpp>
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

  // check texture sizes are not 0
  if (scene_data->scene_resources_config.texture_width == 0 ||
      scene_data->scene_resources_config.texture_height == 0) {
    return std::unexpected(
        FailInfo(FailMode::BadValue,
                 "SceneResourcesConfig has invalid texture dimensions (0)"));
  }

  // create size vector
  sf::Vector2u size{scene_data->scene_resources_config.texture_width,
                    scene_data->scene_resources_config.texture_height};

  auto texture_resize_result =
      scene.m_scene_resources.scene_texture.resize(size);
  if (!texture_resize_result)
    return std::unexpected(FailInfo(FailMode::ResourceCreationFailure,
                                    "Failed to resize scene render texture"));
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

  // check for the entities collection
  if (!fbs_scene_data->entity_collection)
    return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                    "EntityCollectionFbs not found"));

  // instantiate FlatbuffersEntityConfigurator
  FlatbuffersEntityConfigurator entity_configurator(
      scene.GetSceneContext().event_handler,
      *fbs_scene_data->entity_collection);

  // configure EMP on scene
  auto emp_config_result = entity_configurator.ConfigureEntityMemoryPool(
      scene.GetSceneContext().scene_entities);
  if (!emp_config_result)
    return std::unexpected(emp_config_result.error());

  return std::monostate{};
}
} // namespace steamrot
