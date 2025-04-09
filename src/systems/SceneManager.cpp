#include "SceneManager.h"
#include "SceneMenu.h"
#include <iostream>
#include <memory>

///////////////////////////////////////////////////////////
SceneManager::SceneManager()
    : m_all_scenes(), m_active_scenes(), m_inactive_scenes(),
      m_asset_manager() {

  // kick off initial scene(s)
  AddScene("main_menu", "menu", 100);
  std::cout << "main menu added" << std::endl;
}

///////////////////////////////////////////////////////////
void SceneManager::AddScene(std::string name, std::string scene_type,
                            const size_t pool_size) {

  // create a new scene object, if scene type does not exist throw a runtime
  // error and exit early
  std::shared_ptr<Scene> new_scene = nullptr;
  if (scene_type == "menu") {
    new_scene = std::make_shared<SceneMenu>(name, pool_size);
  }

  else {

    throw std::runtime_error("Scene type not found");
  }

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
  scene->setActive(true);
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
  scene->setActive(false);
  // add to inactive scenes
  m_inactive_scenes[tag] = scene;
  // remove from active scenes and interactive scenes
  m_active_scenes.erase(tag);
  m_interactive_scenes.erase(tag);
}
// get the scene name and the drawables that are associated with it
// this makes a fresh map each time
std::map<std::string, SceneDrawables> SceneManager::ProvideSceneDrawables() {
  std::map<std::string, SceneDrawables> drawables;
  for (auto &pair : m_active_scenes) {
    drawables[pair.first] = pair.second->sProvideDrawables();
  }
  return drawables;
}

///////////////////////////////////////////////////////////
void SceneManager::UpdateScenes() {
  // Loop through all the scenes and update them
  // updating does not mean rendering, it means updating the state of the scene
  for (auto &pair : m_all_scenes) {
    auto &scene = pair.second;
    scene->sUpdate();
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
