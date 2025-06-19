#include "SceneManager.h"
#include "SceneType.h"
#include "uuid.h"
#include <iostream>
#include <memory>

namespace steamrot {

/////////////////////////////////////////////////
SceneManager::SceneManager(const GameContext game_contest)
    : m_scene_factory(game_contest), m_scenes(), m_game_context(game_contest) {}

/////////////////////////////////////////////////
void SceneManager::StartUp() {}

/////////////////////////////////////////////////
void SceneManager::AddSceneFromDefault(const SceneType &scene_type,
                                       const size_t pool_size) {

  std::unique_ptr<Scene> new_scene = m_scene_factory.CreateScene(scene_type);
  std::cout << "Created new scene of type: "
            << magic_enum::enum_name(scene_type) << std::endl;
  // add to m_scenes maps
  m_scenes[new_scene->GetSceneID()] = std::move(new_scene);
  std::cout << "Added scene to m_scenes map." << std::endl;

  // load default scene assets
  m_game_context.asset_manager.LoadSceneAssets(scene_type);
  std::cout << "Loaded default assets for scene type: "
            << magic_enum::enum_name(scene_type) << std::endl;
};

/////////////////////////////////////////////////
uuids::uuid SceneManager::LoadTitleScene() {
  // clear existing scenes
  m_scenes.clear();
  std::cout << "Cleared existing scenes." << std::endl;
  // create title scene
  AddSceneFromDefault(SceneType::title, 100);
  std::cout << "Added title scene." << std::endl;

  // return the ID of the title scene
  return m_scenes.begin()->first;
}

/**
 * -------------------------------------------------------
 */
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

/**
 * -------------------------------------------------------
 */
void SceneManager::UpdateScenes() {
  // Loop through all the scenes and update them
  // updating does not mean rendering, it means updating the state of the
  // scene
  for (auto &pair : m_scenes) {
    auto &scene = pair.second;
    scene->sMovement();
    scene->sCollision();
    scene->sRender();

    // add further systems here
  }
}

/**
 * ======================= End of SceneManager =======================
 */

} // namespace steamrot
