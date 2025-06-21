/**
 * @file SceneManager.h
 * @brief Declares the SceneManager class and related functionality.
 */

#pragma once

// Headers
#include "GameContext.h"
#include "SceneFactory.h"
#include "TexturesPackage.h"
#include "uuid.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <memory>
#include <nlohmann/json.hpp>
#include <unordered_map>

namespace steamrot {

/**
 * @class SceneManager
 * @brief Manages scenes, assets, and game data for SteamRot.
 */
class SceneManager {
private:
  /**
   * @brief Scene factory for creating scenes.
   */
  SceneFactory m_scene_factory;

  const GameContext m_game_context;
  /**
   * @brief Map of uuid to Scene.
   */
  std::unordered_map<uuids::uuid, std::unique_ptr<Scene>> m_scenes;

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
