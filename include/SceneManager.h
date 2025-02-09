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
  SceneList m_allScenes;    // all scenes that exist and are being updated
  SceneList m_activeScenes; // all scenes that are currently being rendered
  SceneList
      m_inactiveScenes; // all scenes that are not currently being rendered

  // the wording is very similar to inactive, so we need to be careful
  SceneList m_interactiveScenes; // all scenes that are interactive

  AssetManager m_asset_manager; // responsible for loading all assets of the
  // game
  //  (scene by scene)

public:
  SceneManager();
  void addScene(std::string tag, size_t poolSize);
  void removeScene(std::string tag);
  void activateScene(std::string tag);
  void deactivateScene(std::string tag);

  void
  makeInteractive(); // make a scene interactive so that events can be passed
  void makeNonInteractive(); // make a scene non-interactive so that events are
                             // not passed

  SceneList &getAllScenes();
  SceneList &getActiveScenes();
  SceneList &getInactiveScenes();
  SceneList &getInteractiveScenes();

  // Asset manager functions
  AssetManager &getAssetManager();

  void update();
  void passEvent(const std::optional<sf::Event>);

  /// Testing and simulation functions
  json toJSON(); // convert parts of the SceneManager class to json
};
