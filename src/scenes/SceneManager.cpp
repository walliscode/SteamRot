/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the SceneManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneManager.h"
#include "ISceneManagerDataProvider.h"
#include "SceneFactory.h"
#include <expected>
#include <iostream>
#include <variant>

namespace steamrot {

/////////////////////////////////////////////////
SceneManager::SceneManager(const GameContext &game_context)
    : m_scenes(), m_game_context(game_context) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> SceneManager::StartUp() {

  // create data provider for SceneManager configuration
  auto data_provider_result =
      m_game_context.engine_resources.data_access_factory
          .GetSceneManagerDataProvider();
  if (!data_provider_result) {
    return std::unexpected(data_provider_result.error());
  }
  ISceneManagerDataProvider &data_provider = *data_provider_result.value();

  // get SceneManager data from data provider
  auto data_result = data_provider.ProvideSceneManagerData();
  if (!data_result.has_value()) {
    return std::unexpected(data_result.error());
  }
  // assign SceneManagerState from data provider
  m_scene_manager_state = data_result.value().scene_manager_state;

  // Register subscriptions with EventHandler
  for (auto &subscriber : m_scene_manager_state.subscriptions) {
    auto register_result =
        m_game_context.event_handler.RegisterSubscriber(subscriber);
    if (!register_result.has_value()) {
      return std::unexpected(register_result.error());
    }
  }
  return std::monostate{};
}
/////////////////////////////////////////////////
const std::unordered_map<uuids::uuid, std::unique_ptr<Scene>> &
SceneManager::GetScenes() const {
  return m_scenes;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromDefault(const SceneType &scene_type) {

  // create SceneFactory object
  SceneFactory scene_factory(m_game_context);

  // create and configure scene
  auto scene_creation_result = scene_factory.CreateSceneFromDefault(scene_type);
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

  return std::monostate{};
};

/////////////////////////////////////////////////
std::expected<uuids::uuid, FailInfo> SceneManager::LoadTitleScene() {

  // clear existing scenes
  m_scenes.clear();

  std::cout << "Loading Title Scene..." << std::endl;
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

  std::cout << "Loading Crafting Scene..." << std::endl;
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

    scene->sAction();
    // scene->sMovement();
    scene->sCollision();
    scene->sRender();

    // add further systems here
  }
}
/////////////////////////////////////////////////
const std::vector<std::shared_ptr<Subscriber>> &
SceneManager::GetSubscriptions() const {
  return m_scene_manager_state.subscriptions;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> SceneManager::ProcessSubscriptions() {

  for (auto &subscriber : m_scene_manager_state.subscriptions) {
    // only process active subscribers
    if (subscriber->m_active) {

      switch (subscriber->m_trigger_event_type) {
      case EventType::EventType_EVENT_CHANGE_SCENE: {

        // make sure the data type is correct - use received event data
        if (!subscriber->m_received_event_data.has_value() ||
            !std::holds_alternative<SceneChangePacket>(
                subscriber->m_received_event_data.value())) {

          // just continue if data is not valid, will need to log at some point
          //[TODO: logging here]

          // still need to set subscriber to inactive
          subscriber->m_active = false;
          continue;
        }
        // get SceneChangeData from received event data
        const SceneChangePacket &scene_change_data =
            std::get<SceneChangePacket>(
                subscriber->m_received_event_data.value());

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
    }
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
void SceneManager::ExecuteSceneManagerLevelLogic() {
  // process subscriptions, [TODO: handle potential failure]
  auto process_result = ProcessSubscriptions();

  // update all scenes
  UpdateScenes();
}
} // namespace steamrot
