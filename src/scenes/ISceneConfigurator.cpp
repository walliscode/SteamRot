/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ISceneConfigurator interface.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ISceneConfigurator.h"
#include "Scene.h"
#include "SceneContext.h"
namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ISceneConfigurator::ConfigureScene(Scene &scene, const SceneData *scene_data) {

  // Configure SceneInfo
  auto info_result = ConfigureSceneInfo(scene, scene_data);
  if (!info_result.has_value())
    return std::unexpected(info_result.error());

  // Configure SceneResources
  auto resources_result = ConfigureSceneResources(scene, scene_data);
  if (!resources_result.has_value())
    return std::unexpected(resources_result.error());

  // Configure SceneConfig
  auto config_result = ConfigureSceneConfig(scene, scene_data);
  if (!config_result.has_value())
    return std::unexpected(config_result.error());

  // 4. Configure Entities
  auto entities_result = ConfigureEntities(scene, scene_data);
  if (!entities_result.has_value())
    return std::unexpected(entities_result.error());

  // Configure LogicMap
  auto logic_result = ConfigureLogicMap(scene);
  if (!logic_result.has_value())
    return std::unexpected(logic_result.error());

  return std::monostate();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ISceneConfigurator::ConfigureLogicMap(Scene &scene) {

  // Get SceneContext from the scene
  SceneContext scene_context = scene.GetSceneContext();

  // Get scene type from scene info
  SceneType scene_type = scene.GetSceneInfo().type;

  // Create LogicFactory
  LogicFactory logic_factory(scene_type, scene_context);

  // Create logic map
  auto logic_map_result = logic_factory.CreateLogicMap();
  if (!logic_map_result.has_value())
    return std::unexpected(logic_map_result.error());

  // Store logic map in scene resources
  scene.m_scene_resources.logic_map = std::move(logic_map_result.value());

  return std::monostate();
}
} // namespace steamrot
