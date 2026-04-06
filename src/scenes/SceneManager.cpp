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
  auto data_result = data_provider.CreateSceneManagerData();
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
std::expected<std::monostate, FailInfo>
SceneManager::AddScenesFromSceneCollectionData(
    const SceneCollectionData &scene_collection_data) {

  // clear existing scenes and check that it is cleared
  m_scenes.clear();
  if (!m_scenes.empty()) {
    FailInfo fail_info(FailMode::NotImplemented,
                       "Existing scenes were not cleared correctly");
    return std::unexpected(fail_info);
  }

  // create SceneFactory object
  SceneFactory scene_factory(m_game_context);

  // loop through scene collection data and add each scene
  for (auto &scene_data : scene_collection_data) {

    // create and configure scene from data
    auto scene_creation_result =
        scene_factory.CreateSceneFromSceneData(scene_data);
    if (!scene_creation_result.has_value()) {
      return std::unexpected(scene_creation_result.error());
    }

    // add to m_scenes maps
    auto adding_result =
        m_scenes.emplace(scene_creation_result.value()->GetSceneInfo().id,
                         std::move(scene_creation_result.value()));
    if (!adding_result.second) {
      FailInfo fail_info(
          FailMode::NotAddedToMap,
          "Scene with this ID already exists or function failed");
      return std::unexpected(fail_info);
    }
  }
  return std::monostate{};
}
/////////////////////////////////////////////////
std::expected<uuids::uuid, FailInfo> SceneManager::LoadTitleScene() {

  // clear existing scenes
  m_scenes.clear();

  // create title scene
  auto title_result = AddSceneFromDefault(SceneType::TITLE);
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
  auto crafting_result = AddSceneFromDefault(SceneType::CRAFTING);
  if (!crafting_result.has_value())
    return std::unexpected(crafting_result.error());

  // check that there is only one scene in the map and the uuids match
  if (m_scenes.size() != 1) {
    FailInfo fail_info(FailMode::NotAddedToMap,
                       "Crafting scene was not added correctly");
    return std::unexpected(fail_info);
  }
  // return the ID of the crafting scene
  return m_scenes.begin()->first;
}

/////////////////////////////////////////////////
std::expected<uuids::uuid, FailInfo> SceneManager::LoadUIExplorerScene() {
  // clear existing scenes
  m_scenes.clear();

  SceneFactory scene_factory(m_game_context);

  auto scene_result = scene_factory.CreateUIExplorerScene();
  if (!scene_result.has_value()) {
    return std::unexpected(scene_result.error());
  }

  const uuids::uuid id = scene_result.value()->GetSceneInfo().id;

  auto adding_result = m_scenes.emplace(id, std::move(scene_result.value()));
  if (!adding_result.second) {
    return std::unexpected(
        FailInfo{FailMode::NotAddedToMap,
                 "UIExplorerScene was not added correctly"});
  }

  return id;
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

    scene->sCollision();
    scene->sAction();
    scene->sMovement();
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

      switch (subscriber->event_type) {

      // deal with ScenePayload events
      case EventType::SCENE: {

        // check that captured data is of the correct type
        if (!subscriber->captured_payload.has_value() ||
            !std::holds_alternative<ScenePayload>(
                subscriber->captured_payload.value())) {
          return std::unexpected(
              FailInfo{FailMode::VariantTypeMismatch,
                       "Subscriber payload is not of type ScenePayload"});
        }

        ScenePayload &scene_payload_data =
            std::get<ScenePayload>(subscriber->captured_payload.value());

        // deal with different ScenePayload actions
        switch (scene_payload_data.action) {

        // deal with scene changes
        case ScenePayload::SceneAction::CHANGE: {

          // if there is a SceneType Provided but not uuid, load the default
          // scene of that type
          if (!scene_payload_data.optional_scene_id.has_value()) {

            if (!scene_payload_data.optional_scene_type.has_value() ||
                scene_payload_data.optional_scene_type.value() ==
                    SceneType::UNKNOWN) {
              return std::unexpected(
                  FailInfo{FailMode::MissingData,
                           "Scene change event missing valid SceneType"});
            }
            // switch on the SceneType
            switch (scene_payload_data.optional_scene_type.value()) {
              // deal with Title Scene Loading
            case SceneType::TITLE: {
              auto load_scene_result = LoadTitleScene();
              if (!load_scene_result.has_value()) {
                return std::unexpected(load_scene_result.error());
              }
              break;
            }

            case SceneType::CRAFTING: {
              auto load_scene_result = LoadCraftingScene();
              if (!load_scene_result.has_value()) {
                return std::unexpected(load_scene_result.error());
              }
              break;
            }
            default:
              // Other SceneTypes are not errors, just not handled
              break;
            }

            break;
          } // deal with other ScenePayload info here

        default:
          // Other SceneActions are not errors, just not handled
          break;
        }
        }
      }
      default:
        // other EventTypes are not errors, just not handled
        break;
      }
    }
  }

  // function end
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SceneManager::ExecuteSceneManagerLevelLogic() {
  // Process subscriptions and propagate any errors
  auto process_result = ProcessSubscriptions();
  if (!process_result.has_value())
    return std::unexpected(process_result.error());

  // update all scenes
  UpdateScenes();

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<SceneCollectionData, FailInfo>
SceneManager::CaptureSceneCollectionData() const {

  // create SceneCollectionData to return
  SceneCollectionData scene_collection_data;

  // populate SceneCollectionData from current scenes
  for (const auto &pair : m_scenes) {
    const auto &scene = pair.second;
    SceneData scene_data;
    // get SceneInfo
    scene_data.scene_info = scene->GetSceneInfo();

    // ADD OTHER PARTS WHEN REQUIRED

    // get EntityMemoryPool as shared_ptr
    scene_data.entity_transport =
        std::make_shared<EntityMemoryPool>(scene->GetEntityMemoryPool());

    // add to SceneCollectionData
    scene_collection_data.push_back(std::move(scene_data));
  }
  return scene_collection_data;
}
} // namespace steamrot
