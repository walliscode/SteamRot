/////////////////////////////////////////////////
/// @file
/// @brief Implements a scene configurator that uses default settings for Scenes
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ISceneConfigurator.h"
#include "LogicFactory.h"
#include "scene_types_generated.h"
#include <expected>
#include <variant>

namespace steamrot {
/////////////////////////////////////////////////
ISceneConfigurator::ISceneConfigurator() {};

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ISceneConfigurator::ConfigureScene(Scene &scene, const SceneType scene_type) {

  // Configure SceneInfo
  auto info_result = ConfigureSceneInfo(scene, scene_type);
  if (!info_result.has_value())
    return std::unexpected(info_result.error());

  // Configure SceneResources
  auto resources_result = ConfigureSceneResources(scene, scene_type);
  if (!resources_result.has_value())
    return std::unexpected(resources_result.error());

  // Configure SceneConfig
  auto config_result = ConfigureSceneConfig(scene, scene_type);
  if (!config_result.has_value())
    return std::unexpected(config_result.error());

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
