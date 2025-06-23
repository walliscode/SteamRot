#include "SceneManager.h"
#include "SceneType.h"
#include "actions_generated.h"
#include "magic_enum/magic_enum.hpp"
#include "uuid.h"
#include <iostream>
#include <memory>
#include <optional>
#include <utility>

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
  // print the scene ID
  std::cout << "Scene ID: " << new_scene->GetSceneID() << std::endl;
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

/////////////////////////////////////////////////
uuids::uuid SceneManager::LoadCraftingScene() {
  // clear existing scenes
  m_scenes.clear();
  std::cout << "Cleared existing scenes." << std::endl;
  // create crafting scene
  AddSceneFromDefault(SceneType::crafting, 100);
  std::cout << "Added crafting scene." << std::endl;
  // return the ID of the crafting scene
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

/////////////////////////////////////////////////
void SceneManager::UpdateActions() {
  // Loop through all the scenes and update their actions
  for (auto &pair : m_scenes) {
    auto &scene = pair.second;
    scene->sAction();
  }
  // Process actions after updating all scenes
  SceneManager::ProcessActions();
}
/////////////////////////////////////////////////
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

/////////////////////////////////////////////////
const std::pair<ActionNames, SceneDataPackage>
SceneManager::ScrapeSceneForActions() const {
  ActionNames action_name{0};
  SceneDataPackage scene_data_package;
  // Loop through all the scenes and scrape them for actions
  for (const auto &pair : m_scenes) {
    const auto &scene = pair.second;
    // get scene action
    action_name = scene->GetSceneAction();

    if (action_name != 0) {
      std::cout << "Action detected in Scene: "
                << magic_enum::enum_name(action_name) << std::endl;
      // get scene data package
      scene_data_package = scene->GetSceneDataPackage();
      if (scene_data_package.new_scene_type == std::nullopt) {
        std::cout << "scene type needed in scene data package" << std::endl;
      }
      return std::make_pair(action_name, scene_data_package);
    }
  }
  return {action_name, scene_data_package};
}

/////////////////////////////////////////////////
void SceneManager::ProcessActions() {
  // reset actions and data
  m_scene_manager_action = ActionNames{0};
  m_scene_manager_data_package = SceneManagerDataPackage{};
  // scrape all scenes
  const std::pair<ActionNames, SceneDataPackage> action_and_data =
      ScrapeSceneForActions();

  ActionNames action_name = action_and_data.first;
  SceneDataPackage scene_data_package = action_and_data.second;

  switch (action_name) {
  case ActionNames_ACTION_CHANGE_SCENE: {

    std::cout << "SceneManager: Processing action to change scene."
              << std::endl;

    if (scene_data_package.new_scene_type == std::nullopt) {
      std::cout << "Scene type needed in data package" << std::endl;
    }
    // if the scene type is Title or Crafting then we signal the GameEngine
    // (this a temporary solution)
    if (scene_data_package.new_scene_type == SceneType::title ||
        scene_data_package.new_scene_type == SceneType::crafting) {
      std::cout << "found releventa Scene Type" << std::endl;
      // set the scene manager action to change scene
      m_scene_manager_action = action_name;
      // set the scene manager data package variables
      m_scene_manager_data_package.new_scene_type =
          scene_data_package.new_scene_type;
    }

  default:
    // no action, do nothing
    break;
  }
  }
}

/////////////////////////////////////////////////
const ActionNames &SceneManager::GetSceneManagerAction() const {
  return m_scene_manager_action;
}

/////////////////////////////////////////////////
const SceneManagerDataPackage &
SceneManager::GetSceneManagerDataPackage() const {
  return m_scene_manager_data_package;
}

} // namespace steamrot
