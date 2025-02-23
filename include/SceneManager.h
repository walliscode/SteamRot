#pragma once
#include "AssetManager.h"
#include "Scene.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

typedef std::map<std::string, std::shared_ptr<Scene>> SceneList;

class SceneManager {
private:
  SceneList m_all_scenes;

  // scenes that continue to calculate, theses may or may not be rendered
  SceneList m_active_scenes;
  SceneList m_inactive_scenes;

  // scenes that can take user events
  SceneList m_interactive_scenes;
  AssetManager m_asset_manager;

  // to_json needs access to private members
  friend void to_json(json &j, const SceneManager &scene_manager);

public:
  SceneManager();
  void AddScene(std::string tag, std::string scene_type, size_t pool_size);
  void RemoveScene(std::string tag);

  // activation and deactivation of scenes refers to whetehre they should
  // continue calculations
  void ActivateScene(std::string tag);
  void DeactivateScene(std::string tag);

  // interactive scenes are those that can take user events. this will probably
  // be done with mouse hovering or with tiling window manager
  void MakeInteractive();
  void MakeNonInteractive();

  SceneList &getAllScenes();
  SceneList &getActiveScenes();
  SceneList &getInactiveScenes();
  SceneList &getInteractiveScenes();

  std::map<std::string, SceneDrawables> ProvideSceneDrawables();

  // Asset manager functions
  AssetManager &getAssetManager();

  void update();
  void passEvent(const std::optional<sf::Event>);
};

void to_json(json &j, const SceneManager &scene_manager);
