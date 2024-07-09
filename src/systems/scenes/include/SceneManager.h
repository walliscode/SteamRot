#pragma once
#include <iostream>
#include "Scene.h"
#include <map>
#include <SFML/Graphics.hpp>
#include "AssetManager.h"




typedef std::map<std::string, std::shared_ptr<Scene>> SceneList;

class SceneManager
{
private:
	SceneList m_scenes;
	GameEngine& m_game;
	AssetManager m_assetManager;
	
public:
	SceneManager(GameEngine& game);
	void addScene(std::string tag, std::shared_ptr<Scene> scene);
	void removeScene(std::string tag);
	void activateScene(std::shared_ptr<Scene> scene);
	void deactivateScene(std::shared_ptr<Scene> scene);
	SceneList& getScenes();

	void update();
	void passEvent(sf::Event& event); // pass event from game engine to scene manager to scenes

	AssetManager& getAssetManager();
};