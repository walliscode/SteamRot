#include "SceneManager.h"
#include "uuid.h"
#include <memory>
#include <utility>

namespace steamrot {

/////////////////////////////////////////////////
SceneManager::SceneManager(const GameContext game_contest)
    : m_scene_factory(game_contest), m_scenes(), m_game_context(game_contest) {}

/////////////////////////////////////////////////
void SceneManager::StartUp() {}

/////////////////////////////////////////////////
void SceneManager::AddSceneFromDefault(const SceneType &scene_type) {

  std::unique_ptr<Scene> new_scene =
      m_scene_factory.CreateDefaultScene(scene_type).value();

  // add to m_scenes maps
  m_scenes[new_scene->GetSceneID()] = std::move(new_scene);

  // load default scene assets
  m_game_context.asset_manager.LoadSceneAssets(scene_type);
};

/////////////////////////////////////////////////
uuids::uuid SceneManager::LoadTitleScene() {
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
