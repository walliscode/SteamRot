#pragma once
#include <iostream>
#include "Scene.h"
#include <map>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

typedef std::map<std::string, std::shared_ptr<Scene>> SceneList;

class SceneManager
{
private:
	SceneList m_allScenes; // all scenes that exist and are being updated
	SceneList m_activeScenes; // all scenes that are currently being rendered
	SceneList m_inactiveScenes; // all scenes that are not currently being rendered

	// the wording is very similar to inactive, so we need to be careful
	SceneList m_interactiveScenes; // all scenes that are interactive
	GameEngine& m_game;
	
public:
	SceneManager(GameEngine& game);
	void addScene(std::string tag, std::shared_ptr<Scene> scene);
	void removeScene(std::string tag);
	void activateScene(std::string tag);
	void deactivateScene(std::string tag);

	void makeInteractive(std::string tag); // make a scene interactive so that events can be passed
	void makeNonInteractive(std::string tag); // make a scene non-interactive so that events are not passed

	SceneList& getAllScenes();
	SceneList& getActiveScenes();
	SceneList& getInactiveScenes();
	SceneList& getInteractiveScenes();

	void update();
	void passEvent(sf::Event& event); // pass event from game engine to scene manager to scenes

	/// Testing and simulation functions
	json toJSON(); // convert parts of the SceneManager class to json
};