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
#include "Subscriber.h"
#include "events_generated.h"
#include "scene_manager_generated.h"
#include "subscriber_config_generated.h"
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
  /////////////////////////////////////////////////
  std::unordered_map<uuids::uuid, std::unique_ptr<Scene>> m_scenes;

  /////////////////////////////////////////////////
  /// @brief Map of all event subscriptions, stored by event type.
  /////////////////////////////////////////////////
  std::unordered_map<EventType, std::shared_ptr<Subscriber>> m_subscriptions;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking a GameContext object.
  ///
  /// @param game_context GameContext object
  /////////////////////////////////////////////////
  SceneManager(const GameContext &game_context);

  /////////////////////////////////////////////////
  /// @brief Container function for configuring the SceneManager from
  /// flatbuffers data
  ///
  /// @param scene_manager_data Flatbuffers SceneManagerData object to configure
  /// from
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureSceneManagerFromData(const SceneManagerData *scene_manager_data);

  /////////////////////////////////////////////////
  /// @brief Add the Subscriber to the subscriptions map.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
      RegisterSubscriber(std::shared_ptr<Subscriber>);

  /////////////////////////////////////////////////
  /// @brief Given multiple subscription data, configure the subscribers for the
  /// SceneManager.
  ///
  /// @param subscriptions A flatbuffers vector of SubscriberData objects.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ConfigureSubscribersFromData(
      const ::flatbuffers::Vector<
          ::flatbuffers::Offset<steamrot::SubscriberData>> *subscriptions);

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
  /// @brief Updates all scennes by calling their various system methods.
  ///
  /// This encapsulation method means that all scenes have their systems/logic
  /// called in the same order.
  /////////////////////////////////////////////////
  void UpdateScenes();

  /////////////////////////////////////////////////
  /// @brief Container function for all other functions required for each update
  /// cycle
  /////////////////////////////////////////////////
  void UpdateSceneManager();

  /////////////////////////////////////////////////
  /// @brief Cause the cascade of events that will add a scene from default
  /// data.
  ///
  /// @param scene_type An enum value representing the type of scene to create.
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  AddSceneFromDefault(const SceneType &scene_type);

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
  /// @brief Returns a constant reference to the subscriptions map. for
  /// inspection
  /////////////////////////////////////////////////
  const std::unordered_map<EventType, std::shared_ptr<Subscriber>> &
  GetSubscriptions() const;

  /////////////////////////////////////////////////
  /// @brief Loads unique scenes that run by themselves,
  ///
  /// @param scene_type Enum pointing to the type of scene to load
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  LoadStandAloneScene(const SceneType &scene_type);

  /////////////////////////////////////////////////
  /// @brief Go through all subscriptions, if active call relevant Logic
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ProcessSubscriptions();

  /////////////////////////////////////////////////
  /// @brief Returns all available Scenes for inspection
  /////////////////////////////////////////////////
  const std::unordered_map<uuids::uuid, std::unique_ptr<Scene>> &
  GetScenes() const;
};

} // namespace steamrot
