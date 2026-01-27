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
#include "GameContext.h"
#include "Scene.h"
#include "SceneInfoProvider.h"
#include "SceneManagerConfig.h"
#include "SceneManagerResources.h"
#include "SceneManagerState.h"
#include "SceneType.h"
#include "uuid.h"
#include <SFML/Graphics.hpp>
#include <expected>
#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @class SceneManager
/// @brief Manages the game's scenes, including creation, updating, and
/// providing textures
///
/////////////////////////////////////////////////
class SceneManager : public SceneInfoProvider {
private:
  /////////////////////////////////////////////////
  /// @brief Context from GameEngine, providing access to game-wide resources
  /////////////////////////////////////////////////
  const GameContext &m_game_context;

  /////////////////////////////////////////////////
  /// @brief Map of scenes, keyed by their unique UUIDs.
  ///
  /// Note: Kept at top level (not in SceneManagerResources) per user request
  /// to make it obvious and easy to access.
  /////////////////////////////////////////////////
  std::unordered_map<uuids::uuid, std::unique_ptr<Scene>> m_scenes;

  /////////////////////////////////////////////////
  /// @brief SceneManager-level resources (managers, systems)
  /////////////////////////////////////////////////
  SceneManagerResources m_scene_manager_resources;

  /////////////////////////////////////////////////
  /// @brief SceneManager configuration loaded from data files
  /////////////////////////////////////////////////
  SceneManagerConfig m_scene_manager_config;

  /////////////////////////////////////////////////
  /// @brief SceneManager runtime state (subscriptions, etc.)
  /////////////////////////////////////////////////
  SceneManagerState m_scene_manager_state;

  /////////////////////////////////////////////////
  /// @brief Go through all subscriptions, if active call relevant Logic
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ProcessSubscriptions();

  /////////////////////////////////////////////////
  /// @brief Updates all scennes by calling their various system methods.
  ///
  /// This encapsulation method means that all scenes have their systems/logic
  /// called in the same order.
  /////////////////////////////////////////////////
  void UpdateScenes();

public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking a GameContext object.
  ///
  /// @param game_context GameContext object
  /////////////////////////////////////////////////
  SceneManager(const GameContext &game_context);

  /////////////////////////////////////////////////
  /// @brief Start up the SceneManager, loading configuration and preparing
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> StartUp();

  /////////////////////////////////////////////////
  /// @brief A convenience function to load the title scene.
  ///
  /// If the title scene is called it should clear all other scenes and
  /// create a new one.
  /////////////////////////////////////////////////
  std::expected<uuids::uuid, FailInfo> LoadTitleScene();

  /////////////////////////////////////////////////
  /// @brief A convenience function to load the crafting scene.
  ///
  /// If the crafting scene is called it should clear all other scenes and
  /// create a new one.
  /////////////////////////////////////////////////
  std::expected<uuids::uuid, FailInfo> LoadCraftingScene();

  /////////////////////////////////////////////////
  /// @brief Cause the cascade of events that will add a scene from default
  /// data.
  ///
  /// @param scene_type An enum value representing the type of scene to create.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  AddSceneFromDefault(const SceneType &scene_type);

  /////////////////////////////////////////////////
  /// @brief Clears existing scenes and adds scenes from provided data
  ///
  /// We expect the data to be provided from sources such as SaveData or
  /// TestData
  /// @param scene_collection_data An instance of SceneCollectionData
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> AddScenesFromSceneCollectionData(
      const SceneCollectionData &scene_collection_data);

  /////////////////////////////////////////////////
  /// @brief Provides a map of textures based on given scene IDs
  ///
  /// @param scene_ids A vector of scene IDs to fetch textures for
  /////////////////////////////////////////////////
  const std::expected<std::unordered_map<uuids::uuid, std::reference_wrapper<
                                                          sf::RenderTexture>>,
                      FailInfo>
  ProvideTextures(std::vector<uuids::uuid> &scene_ids) const override;

  /////////////////////////////////////////////////
  /// @brief Provides information about all available scenes.
  /////////////////////////////////////////////////
  const std::expected<std::vector<SceneInfo>, FailInfo>
  ProvideAvailableSceneInfo() const override;

  /////////////////////////////////////////////////
  /// @brief Returns a constant reference to the subscriptions map for
  /// inspection
  /////////////////////////////////////////////////
  const std::vector<std::shared_ptr<Subscriber>> &GetSubscriptions() const;

  /////////////////////////////////////////////////
  /// @brief Returns all available Scenes for inspection
  /////////////////////////////////////////////////
  const std::unordered_map<uuids::uuid, std::unique_ptr<Scene>> &
  GetScenes() const;

  void ExecuteSceneManagerLevelLogic();

  /////////////////////////////////////////////////
  /// @brief Provides a snapshot of the current Scenes
  /////////////////////////////////////////////////
  std::expected<SceneCollectionData, FailInfo>
  CaptureSceneCollectionData() const;
};

} // namespace steamrot
