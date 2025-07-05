/**
 * @file SceneManager.h
 * @brief Declares the SceneManager class and related functionality.
 */

#pragma once

// Headers
#include "GameContext.h"
#include "Scene.h"
#include "SceneFactory.h"
#include "TexturesPackage.h"
#include "uuid.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <unordered_set>

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
  /// @brief Instance of SceneFactory class. Used to create Scene objects.
  /////////////////////////////////////////////////
  SceneFactory m_scene_factory;

  /////////////////////////////////////////////////
  /// @brief Context from GameEngine, providing access to game-wide resources
  /////////////////////////////////////////////////
  const GameContext m_game_context;

  /////////////////////////////////////////////////
  /// @brief Map of scenes, keyed by their unique UUIDs.
  /////////////////////////////////////////////////
  std::unordered_map<uuids::uuid, std::unique_ptr<Scene>> m_scenes;

  std::unordered_set<EventType> m_scene_manager_event_types;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking a GameContext object.
  ///
  /// @param game_context GameContext object
  /////////////////////////////////////////////////
  SceneManager(const GameContext game_context);

  /**
   * @brief Start up the SceneManager for the game.
   */
  void StartUp();

  /**
   * @brief Clear all scenes and load the title scene.
   * @return The UUID of the loaded title scene.
   */
  uuids::uuid LoadTitleScene();

  /////////////////////////////////////////////////
  /// @brief Clear all scenes and load the crafting scene.
  ///
  /////////////////////////////////////////////////
  uuids::uuid LoadCraftingScene();

  /**
   * @brief Update all Scenes.
   */
  void UpdateScenes();

  /**
   * @brief Add a new scene from default data.
   * @param scene_type The type of scene to add.
   * @param pool_size The pool size for the scene.
   */
  void AddSceneFromDefault(const SceneType &scene_type, size_t pool_size);

  /**
   * @brief Collate textures from relevant scenes and provide as package.
   * @return A TexturesPackage containing textures from scenes.
   */
  TexturesPackage ProvideTexturesPackage();
};

} // namespace steamrot
