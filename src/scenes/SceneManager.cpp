#include "SceneManager.h"
#include "FailInfo.h"
#include "SceneFactory.h"
#include "uuid.h"
#include <expected>
#include <memory>
#include <utility>
#include <variant>

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
      m_scenes.emplace(scene_creation_result.value()->GetSceneID(),
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
TexturesPackage SceneManager::ProvideTexturesPackage() {
  // create textures package object
  TexturesPackage textures_package;

  // cycle through desired scenes
  // TODO: pass along required IDs from display manager for picking Scenes
  for (auto &pair : m_scenes) {
    // get scene
    auto &scene = pair.second;

    // add created scene texture to texture map
    textures_package.AddTexture(scene->GetSceneID(), scene->GetRenderTexture());
  }
  return textures_package;
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
