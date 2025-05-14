#include "SceneManager.h"
#include <memory>

namespace steamrot {
///////////////////////////////////////////////////////////
SceneManager::SceneManager()
    : m_scene_factory(), m_all_scenes(), m_active_scenes(), m_inactive_scenes(),
      m_asset_manager() {}

///////////////////////////////////////////////////////////
void SceneManager::StartUp() {

  // create initial scene
  AddScene("main_menu", "menu", 100);
}
///////////////////////////////////////////////////////////
void SceneManager::AddScene(std::string name, std::string scene_type,
                            const size_t pool_size) {

  // shared pointer is used as Scene can be in multiple maps
  std::shared_ptr<Scene> new_scene =
      m_scene_factory.CreateScene(name, scene_type);

  // add to relevant maps
  m_all_scenes.insert({name, new_scene});
  m_active_scenes.insert({name, new_scene});
};

///////////////////////////////////////////////////////////
void SceneManager::RemoveScene(std::string tag) {
  m_all_scenes.erase(tag);
  m_active_scenes.erase(tag);
  m_inactive_scenes.erase(tag);
  m_interactive_scenes.erase(tag);
}

///////////////////////////////////////////////////////////
void SceneManager::ActivateScene(std::string tag) {
  // find scene in m_allScenes
  std::shared_ptr<Scene> scene = m_all_scenes[tag];
  // activate scene
  scene->SetActive(true);
  // add to active scenes
  m_active_scenes[tag] = scene;
  // remove from inactive scenes
  m_inactive_scenes.erase(tag);
}

///////////////////////////////////////////////////////////
void SceneManager::DeactivateScene(std::string tag) {
  // find scene in m_allScenes
  std::shared_ptr<Scene> scene = m_all_scenes[tag];
  // inactivate scene
  scene->SetActive(false);
  // add to inactive scenes
  m_inactive_scenes[tag] = scene;
  // remove from active scenes and interactive scenes
  m_active_scenes.erase(tag);
  m_interactive_scenes.erase(tag);
}

///////////////////////////////////////////////////////////
TexturesPackage SceneManager::ProvideTexturesPackage() {

  TexturesPackage textures_package;
  return textures_package;
}

///////////////////////////////////////////////////////////
void SceneManager::UpdateScenes() {
  // Loop through all the scenes and update them
  // updating does not mean rendering, it means updating the state of the scene
  for (auto &pair : m_all_scenes) {
    auto &scene = pair.second;
    scene->sMovement();

    // add further systems here
  }
}

///////////////////////////////////////////////////////////
void SceneManager::MakeInteractive() {
  // pass through mouse location from eventual dashboard and copy active scene
  // to interactive scene
}

///////////////////////////////////////////////////////////
void SceneManager::MakeNonInteractive() {
  // remove active scene from interactive scenes
}

//////////////////////////////////////////////////////////
void to_json(json &j, const SceneManager &scene_manager) {
  j = json{
      {"type", "SceneManager"},
      {"m_all_scenes",
       {{"size", scene_manager.m_all_scenes.size()},
        {"scenes", json::array()}}},
      {"m_active_scenes", {{"size", scene_manager.m_active_scenes.size()}}},
      {"m_inactive_scenes", {{"size", scene_manager.m_inactive_scenes.size()}}},
      {"m_interactive_scenes",
       {{"size", scene_manager.m_interactive_scenes.size()}}}};

  // for (auto &pair : scene_manager.m_all_scenes) {
  //
  //   j["m_all_scenes"]["scenes"].push_back(pair.first);
  // }
};
} // namespace steamrot
