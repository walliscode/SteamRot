/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SceneManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersConfigurator.h"
#include "GameContext.h"
#include "Scene.h"
#include "TexturesPackage.h"
#include "uuid.h"
#include <SFML/Graphics.hpp>
#include <expected>
#include <memory>
#include <unordered_map>
#include <variant>

namespace steamrot {

/////////////////////////////////////////////////
/// @class SceneManager
/// @brief Manages the game's scenes, including creation, updating, and
/// providing textures
///
/////////////////////////////////////////////////
class SceneManager {
private:
  /////////////////////////////////////////////////
  /// @brief Context from GameEngine, providing access to game-wide resources
  /////////////////////////////////////////////////
  const GameContext m_game_context;

  /////////////////////////////////////////////////
  /// @brief Map of scenes, keyed by their unique UUIDs.
  /////////////////////////////////////////////////
  std::unordered_map<uuids::uuid, std::unique_ptr<Scene>> m_scenes;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking a GameContext object.
  ///
  /// @param game_context GameContext object
  /////////////////////////////////////////////////
  SceneManager(const GameContext game_context);

  /////////////////////////////////////////////////
  /// @brief A convenience function to load the title scene.
  ///
  /// If the title scene is called it should clear all other scenes and create a
  /// new one.
  /////////////////////////////////////////////////
  std::expected<uuids::uuid, FailInfo> LoadTitleScene();

  /////////////////////////////////////////////////
  /// @brief A convenience function to load the crafting scene.
  ///
  /// If the crafting scene is called it should clear all other scenes and
  /// create a new one.
  /////////////////////////////////////////////////
  uuids::uuid LoadCraftingScene();

  /////////////////////////////////////////////////
  /// @brief Updates all scennes by calling their various system methods.
  ///
  /// This encapsulation method means that all scenes have their systems/logic
  /// called in the same order.
  /////////////////////////////////////////////////
  void UpdateScenes();

  /////////////////////////////////////////////////
  /// @brief Cause the cascade of events that will add a scene from default
  /// data.
  ///
  /// @param scene_type An enum value representing the type of scene to create.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  AddSceneFromDefault(const SceneType &scene_type);

  /////////////////////////////////////////////////
  /// @brief Provide a textures package by value to be passed along
  ///
  /// Currently not happy with this system, will improve it later.
  /////////////////////////////////////////////////
  TexturesPackage ProvideTexturesPackage();
};

} // namespace steamrot
