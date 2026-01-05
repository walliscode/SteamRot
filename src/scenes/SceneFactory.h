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
#include "Scene.h"
#include "scene_types_generated.h"
#include <expected>
#include <memory>

namespace steamrot {

class SceneFactory {
protected:
  /////////////////////////////////////////////////
  /// @brief Refrerence to the game context object (global resources)
  /////////////////////////////////////////////////
  const GameContext &m_game_context;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for SceneFactory
  ///
  /// @param game_context GameContext object providing access to global
  /// resources
  /////////////////////////////////////////////////
  SceneFactory(const GameContext &game_context);

  /////////////////////////////////////////////////
  /// @brief Use the factory to create and configure a scene
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateEmptyScene(const SceneType scene_type);

  /////////////////////////////////////////////////
  /// @brief Creates and configures a scene from provided scene data
  ///
  /// @param scene_data Data to configure the scene from
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateSceneFromData(std::unique_ptr<SceneData> scene_data);

  /////////////////////////////////////////////////
  /// @brief Create and configure a scene from default data
  ///
  /// @param scene_type SceneType enum value representing the type of scene to
  /// create
  /////////////////////////////////////////////////
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateSceneFromDefault(const SceneType scene_type);
};
} // namespace steamrot
