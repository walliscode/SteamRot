/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SceneFactory class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "FailInfo.h"
#include "GameContext.h"
#include "ISceneConfigurator.h"
#include "Scene.h"
#include "scene_types_generated.h"
#include <expected>
#include <memory>

namespace steamrot {

class SceneFactory {
protected:
  /////////////////////////////////////////////////
  /// @brief Reference to the scene configurator strategy
  /////////////////////////////////////////////////
  ISceneConfigurator &m_scene_configurator;

  /////////////////////////////////////////////////
  /// @brief Refrerence to the game context object (global resources)
  /////////////////////////////////////////////////
  const GameContext &m_game_context;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for SceneFactory
  ///
  /// @param game_context [TODO:parameter]
  /// @param scene_configurator [TODO:parameter]
  /// @param type [TODO:parameter]
  /////////////////////////////////////////////////
  SceneFactory(const GameContext &game_context,
               ISceneConfigurator &scene_configurator);

  /////////////////////////////////////////////////
  /// @brief Use the factory to create and configure a scene
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateSceneByType(const SceneType scene_type);

  /////////////////////////////////////////////////
  /// @brief Use the configurator to create and configure a scene
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateAndConfigureScene(const SceneType scene_type);
};
} // namespace steamrot
