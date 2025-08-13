#include "SceneManager.h"
#include "FlatbuffersConfigurator.h"
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

  // // load default scene assets
  // m_game_context.asset_manager.LoadSceneAssets(scene_type);

  return std::monostate{};
};

/////////////////////////////////////////////////
std::expected<uuids::uuid, FailInfo> SceneManager::LoadTitleScene() {

  // clear existing scenes
  m_scenes.clear();

  // create title scene
  AddSceneFromDefault(SceneType::SceneType_TITLE);

  // return the ID of the title scene
  return m_scenes.begin()->first;
}

/////////////////////////////////////////////////
uuids::uuid SceneManager::LoadCraftingScene() {
  // clear existing scenes
  m_scenes.clear();

  // create crafting scene
  AddSceneFromDefault(SceneType::SceneType_CRAFTING);

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
