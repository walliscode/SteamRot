#include "SceneManager.h"
#include "FailInfo.h"
#include "Scene.h"
#include "SceneFactory.h"
#include "uuid.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <expected>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
SceneManager::SceneManager(const GameContext game_contest)
    : m_scenes(), m_game_context(game_contest) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromDefault(const SceneType &scene_type) {

  // create SceneFactory object
  SceneFactory scene_factory(m_game_context);

  auto scene_creation_result = scene_factory.CreateDefaultScene(scene_type);
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

    scene->sAction();
    scene->sMovement();
    scene->sCollision();
    scene->sRender();

    // add further systems here
  }
}
} // namespace steamrot
