/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the ISceneConfigurator interface.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "FailInfo.h"
#include "LogicFactory.h"
#include "Scene.h"
#include "SceneData.h"
#include <expected>
#include <variant>
namespace steamrot {
/////////////////////////////////////////////////
/// @class ISceneConfigurator
/// @brief Provides an interface for configuring different aspects of a scene.
///
/// Primarily designed to provide a strategy pattern for the SceneFactory
/////////////////////////////////////////////////
class ISceneConfigurator {

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor for ISceneConfigurator
  /////////////////////////////////////////////////
  ISceneConfigurator() = default;

  /////////////////////////////////////////////////
  /// @brief Virtual destructor for ISceneConfigurator to ensure derived classes
  /// implement a destructor
  /////////////////////////////////////////////////
  virtual ~ISceneConfigurator() {};

  /////////////////////////////////////////////////
  /// @brief Wrapper function for all scene configuration
  ///
  /// @param scene Scene to configure
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData *scene_data) {

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

    // Configure LogicMap
    auto logic_result = ConfigureLogicMap(scene);
    if (!logic_result.has_value())
      return std::unexpected(logic_result.error());

    return std::monostate();
  }
  /////////////////////////////////////////////////
  /// @brief Virtual function to configure SceneInfo struct
  ///
  /// @param scene Scene to configure
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene, const SceneData *scene_data) = 0;

  /////////////////////////////////////////////////
  /// @brief Virtual function to configure SceneResources struct
  ///
  /// @param scene Scene to configure
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene, const SceneData *scene_data) = 0;

  /////////////////////////////////////////////////
  /// @brief Virtual function to configure SceneConfig struct
  ///
  /// @param scene Scene to configure
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneConfig(Scene &scene, const SceneData *scene_data) = 0;

  /////////////////////////////////////////////////
  /// @brief Configure the logic map for the scene
  ///
  /// This should be set in stone, not configurable by data or the user.
  /// @param scene  Scene to configure
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ConfigureLogicMap(Scene &scene) {

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
};
} // namespace steamrot
