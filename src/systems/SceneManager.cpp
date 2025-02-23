#include "SceneManager.h"
#include "SceneMenu.h"
#include <iostream>
#include <memory>

SceneManager::SceneManager()
    : m_all_scenes(), m_active_scenes(), m_inactive_scenes(),
      m_asset_manager() {

  // kick off initial scene(s)
  AddScene("main_menu", "menu", 100);
  std::cout << "main menu added" << std::endl;
}

// add an object from types derived from the Scene type
// for now this will just use string tags to identify the scene type
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

void SceneManager::RemoveScene(std::string tag) {
  m_all_scenes.erase(tag);
  m_active_scenes.erase(tag);
  m_inactive_scenes.erase(tag);
  m_interactive_scenes.erase(tag);
}

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

SceneList &SceneManager::getAllScenes() { return m_all_scenes; }

SceneList &SceneManager::getActiveScenes() { return m_active_scenes; }

SceneList &SceneManager::getInactiveScenes() { return m_inactive_scenes; }

SceneList &SceneManager::getInteractiveScenes() { return m_interactive_scenes; }

// get the scene name and the drawables that are associated with it
// this makes a fresh map each time
std::map<std::string, SceneDrawables> SceneManager::ProvideSceneDrawables() {
  std::map<std::string, SceneDrawables> drawables;
  for (auto &pair : m_active_scenes) {
    drawables[pair.first] = pair.second->sProvideDrawables();
  }
  return drawables;
}

void SceneManager::update() {
  // Loop through all the scenes and update them
  // updating does not mean rendering, it means updating the state of the scene
  for (auto &pair : m_all_scenes) {
    auto &scene = pair.second;
    scene->sUpdate();
  }
}

void SceneManager::passEvent(const std::optional<sf::Event> event) {
  // // events should only be passed to interactive scenes (e.g. with a mouse
  // // hovering over it)
  // for (auto &pair : m_interactiveScenes) {
  //   auto &scene = pair.second;
  //
  //   // some collision clause will be probably need to be added at some point
  //   // check if the key is in the action map
  //   std::string key_code = event->getIf<sf::Event::KeyPressed>()->;
  //   if (scene->getActionMap().find(event.key.code) ==
  //       scene->getActionMap().end()) {
  //     continue;
  //   }
  //
  //   // determine if the event is a key press or key release
  //   const std::string actionType =
  //       (event.type == sf::Event::KeyPressed) ? "START" : "END";
  //
  //   scene->doAction(
  //       Action(scene->getActionMap().at(event.key.code), actionType));
  // }
}

void SceneManager::MakeInteractive() {
  // pass through mouse location from eventual dashboard and copy active scene
  // to interactive scene
}

void SceneManager::MakeNonInteractive() {
  // remove active scene from interactive scenes
}

// json functions that nlohmann needs to convert the class to json
void to_json(json &j, const SceneManager &scene_manager) {
  j = json{
      {"type", "SceneManager"},
      {"m_all_scenes", {{"size", scene_manager.m_all_scenes.size()}}},
      {"m_active_scenes", {{"size", scene_manager.m_active_scenes.size()}}},
      {"m_inactive_scenes", {{"size", scene_manager.m_inactive_scenes.size()}}},
      {"m_interactive_scenes",
       {{"size", scene_manager.m_interactive_scenes.size()}}}};
};
