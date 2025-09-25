/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the SceneManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneManager.h"
#include "EventPacket.h"
#include "FailInfo.h"
#include "Scene.h"
#include "SceneFactory.h"
#include "Subscriber.h"
#include "SubscriberFactory.h"
#include "events_generated.h"
#include "scene_types_generated.h"
#include "uuid.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <expected>
#include <memory>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
SceneManager::SceneManager(const GameContext &game_context)
    : m_scenes(), m_game_context(game_context) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SceneManager::ConfigureSceneManagerFromData(
    const SceneManagerData *scene_manager_data) {
  if (!scene_manager_data) {
    FailInfo fail_info(FailMode::NullPointer,
                       "SceneManagerData is a null pointer");
    return std::unexpected(fail_info);
  }
  // configure Subscribers from data
  auto configure_result =
      ConfigureSubscribersFromData(scene_manager_data->subscriptions());
  if (!configure_result.has_value()) {
    return std::unexpected(configure_result.error());
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
const std::unordered_map<uuids::uuid, std::unique_ptr<Scene>> &
SceneManager::GetScenes() const {
  return m_scenes;
}

/////////////////////////////////////////////////
void SceneManager::UpdateSceneManager() {

  // process subscriptions, [TODO: handle potential failure]
  auto process_result = ProcessSubscriptions();

  // update all scenes
  UpdateScenes();
}
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromDefault(const SceneType &scene_type) {

  // create SceneFactory object
  SceneFactory scene_factory;

  auto scene_creation_result =
      scene_factory.CreateDefaultScene(scene_type, m_game_context);
  if (!scene_creation_result.has_value()) {
    return std::unexpected(scene_creation_result.error());
  }
  // add to m_scenes maps
  auto adding_result =
      m_scenes.emplace(scene_creation_result.value()->GetSceneInfo().id,
                       std::move(scene_creation_result.value()));

  if (!adding_result.second) {
    FailInfo fail_info(FailMode::NotAddedToMap,
                       "Scene with this ID already exists or function failed");
    return std::unexpected(fail_info);
  }

  // load default scene assets
  auto load_asset_result =
      m_game_context.asset_manager.LoadSceneAssets(scene_type);

  if (!load_asset_result.has_value()) {
    return std::unexpected(load_asset_result.error());
  }

  return std::monostate{};
};

/////////////////////////////////////////////////
std::expected<uuids::uuid, FailInfo> SceneManager::LoadTitleScene() {

  // clear existing scenes
  m_scenes.clear();

  // create title scene
  auto title_result = AddSceneFromDefault(SceneType::SceneType_TITLE);
  if (!title_result.has_value())
    return std::unexpected(title_result.error());

  // check that there is only one scene in the map and the uuids match
  if (m_scenes.size() != 1) {
    FailInfo fail_info(FailMode::NotAddedToMap,
                       "Title scene was not added correctly");
    return std::unexpected(fail_info);
  }

  // return the ID of the title scene
  return m_scenes.begin()->first;
}

/////////////////////////////////////////////////
std::expected<uuids::uuid, FailInfo> SceneManager::LoadCraftingScene() {
  // clear existing scenes
  m_scenes.clear();

  // create crafting scene
  auto crafting_result = AddSceneFromDefault(SceneType::SceneType_CRAFTING);
  if (!crafting_result.has_value())
    return std::unexpected(crafting_result.error());

  // check that there is only one scene in the map and the uuids match
  // return the ID of the crafting scene
  return m_scenes.begin()->first;
}

/////////////////////////////////////////////////
const std::expected<
    std::unordered_map<uuids::uuid, std::reference_wrapper<sf::RenderTexture>>,
    FailInfo>
SceneManager::ProvideTextures(std::vector<uuids::uuid> &scene_ids) const {

  // create a map of textures to return
  std::unordered_map<uuids::uuid, std::reference_wrapper<sf::RenderTexture>>
      texture_map;
  // loop through the scene IDs and get the textures from the scenes
  for (const auto &scene_id : scene_ids) {
    auto scene_it = m_scenes.find(scene_id);
    if (scene_it != m_scenes.end()) {
      texture_map.emplace(scene_id,
                          std::ref(scene_it->second->GetRenderTexture()));
    } else {
      FailInfo fail_info(FailMode::NotAddedToMap,
                         "Scene ID not found in SceneManager");
      return std::unexpected(fail_info);
    }
  }
  return texture_map;
}

/////////////////////////////////////////////////
const std::expected<std::vector<SceneInfo>, FailInfo>
SceneManager::ProvideAvailableSceneInfo() const {
  std::vector<SceneInfo> scene_info_list;
  for (const auto &pair : m_scenes) {
    scene_info_list.push_back(pair.second->GetSceneInfo());
  }
  return scene_info_list;
}

/////////////////////////////////////////////////
void SceneManager::UpdateScenes() {
  // Loop through all the scenes and update them
  // updating does not mean rendering, it means updating the state of the
  // scene
  for (auto &pair : m_scenes) {
    auto &scene = pair.second;

    // scene->sAction();
    // scene->sMovement();
    scene->sCollision();
    scene->sRender();

    // add further systems here
  }
}
/////////////////////////////////////////////////
const std::unordered_map<EventType, std::shared_ptr<Subscriber>> &
SceneManager::GetSubscriptions() const {
  return m_subscriptions;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SceneManager::RegisterSubscriber(std::shared_ptr<Subscriber> subscriber) {
  // add in guard statements and potential fail modes
  if (!subscriber) {
    FailInfo fail_info(FailMode::NullPointer, "Subscriber is a null pointer");
    return std::unexpected(fail_info);
  }

  // attempt to add the subscriber to the map, fail if duplicate
  auto result = m_subscriptions.emplace(subscriber->GetEventType(), subscriber);
  if (!result.second) {
    FailInfo fail_info(
        FailMode::NotAddedToMap,
        "Subscriber for this event type already exists in the SceneManager");
    return std::unexpected(fail_info);
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SceneManager::ConfigureSubscribersFromData(
    const ::flatbuffers::Vector<::flatbuffers::Offset<steamrot::SubscriberData>>
        *subscriptions) {

  if (!subscriptions) {
    FailInfo fail_info(FailMode::NullPointer,
                       "Subscriptions data is a null pointer");
    return std::unexpected(fail_info);
  }

  // set up SubscriberFactory
  SubscriberFactory subscriber_factory(m_game_context.event_handler);
  // loop through the SubscriberData and create subscribers and register them
  for (const auto &subscription : *subscriptions) {

    // create and register subscriber with EventHandler
    auto create_result = subscriber_factory.CreateAndRegisterSubscriber(
        subscription->event_type_data());
    if (!create_result.has_value()) {
      return std::unexpected(create_result.error());
    }
    // register produced Subscriber with SceneManager
    auto register_result = RegisterSubscriber(create_result.value());
    if (!register_result.has_value()) {
      return std::unexpected(register_result.error());
    }
  }

  return std::monostate();
}
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SceneManager::LoadStandAloneScene(const SceneType &scene_type) {
  // clear existing scenes
  m_scenes.clear();

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> SceneManager::ProcessSubscriptions() {

  for (const auto &[event_type, subscriber] : m_subscriptions) {

    // only process active subscribers
    if (subscriber->IsActive()) {

      // get the event data
      const EventData &event_data = subscriber->GetEventData();

      switch (event_type) {
      case EventType::EventType_EVENT_CHANGE_SCENE: {

        // make sure the data type is correct
        if (!std::holds_alternative<SceneChangeData>(event_data)) {
          FailInfo fail_info(FailMode::ParameterOutOfBounds,
                             "EventData type does not match EventType");
          return std::unexpected(fail_info);
        }
        // case to SceneChangeData
        const SceneChangeData &scene_change_data =
            std::get<SceneChangeData>(event_data);

        // check for scene type
        switch (scene_change_data.second) {

          // deal with Title Scene Loading
        case SceneType_TITLE: {
          auto load_scene_result = LoadTitleScene();
          if (!load_scene_result.has_value()) {
            return std::unexpected(load_scene_result.error());
          }
          break;
        }

        case SceneType_CRAFTING: {
          auto load_scene_result = LoadCraftingScene();
          if (!load_scene_result.has_value()) {
            return std::unexpected(load_scene_result.error());
          }
          break;
        }
        default:
          break;
        }

        break;
      }
      default:
        break;
      }
      // FINAL PART: set subscriber to inactive after processing.
      auto set_inactive_result = subscriber->SetInactive();
    }
  }
  return std::monostate{};
}
} // namespace steamrot
