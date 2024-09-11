#include "SceneManager.h"
#include "SceneMainMenu.h"


SceneManager::SceneManager(GameEngine& game) : 
	m_allScenes(), 
	m_activeScenes(), 
	m_inactiveScenes(), 
	m_game(game),
	m_assetManager()
{

	// kick off initial scene(s)
		
}


void SceneManager::addScene(std::string tag, std::shared_ptr<Scene> scene)
{
	// add to all scenes
	m_allScenes[tag] = scene;

	// as default behaviour, add to inactive scenes
	m_inactiveScenes[tag] = scene;

	// load the assets for the scene
	m_assetManager.loadSceneAssets(tag);
}

void SceneManager::removeScene(std::string tag)
{
	m_allScenes.erase(tag);
	m_activeScenes.erase(tag);
	m_inactiveScenes.erase(tag);
	m_interactiveScenes.erase(tag);
}

void SceneManager::activateScene(std::string tag)
{
	// find scene in m_allScenes
	std::shared_ptr<Scene> scene = m_allScenes[tag];
	// activate scene
	scene->setActive(true);
	// add to active scenes
	m_activeScenes[tag] = scene;
	// remove from inactive scenes
	m_inactiveScenes.erase(tag);

}

void SceneManager::deactivateScene(std::string tag)
{
	// find scene in m_allScenes
	std::shared_ptr<Scene> scene = m_allScenes[tag];
	// inactivate scene
	scene->setActive(false);
	// add to inactive scenes
	m_inactiveScenes[tag] = scene;
	// remove from active scenes and interactive scenes
	m_activeScenes.erase(tag);
	m_interactiveScenes.erase(tag);
}

SceneList& SceneManager::getAllScenes()
{
	return m_allScenes;
}

SceneList& SceneManager::getActiveScenes()
{
	return m_activeScenes;
}

SceneList& SceneManager::getInactiveScenes()
{
	return m_inactiveScenes;
}

SceneList& SceneManager::getInteractiveScenes()
{
	return m_interactiveScenes;
}

void SceneManager::update()
{
	// Loop through all the scenes and update them
	// updating does not mean rendering, it means updating the state of the scene
	for (auto& pair : m_allScenes)
	{
		auto& scene = pair.second;
		scene->update();

	}
}

void SceneManager::passEvent(sf::Event& event)
{
	// events should only be passed to interactive scenes (e.g. with a mouse hovering over it)
	for (auto& pair : m_interactiveScenes)
	{
		auto& scene = pair.second;

		// some collision clause will be probably need to be added at some point
		// check if the key is in the action map
		if (scene->getActionMap().find(event.key.code) == scene->getActionMap().end())
		{
			continue;
		}

		// determine if the event is a key press or key release
		const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";

		scene->doAction(Action(scene->getActionMap().at(event.key.code), actionType));
	}
}

void SceneManager::makeInteractive()
{
	// pass through mouse location from eventual dashboard and copy active scene to interactive scene
}
 
void SceneManager::makeNonInteractive()
{
	// remove active scene from interactive scenes

}

json SceneManager::toJSON()
{
	// create json object
	json j;

	// for Scene in m_Scenes, return the tag name and whether the ptr is null or not
	for (auto& pair : m_allScenes)
	{
		bool isNotNull = pair.second != nullptr; // shared ptrs have a bool operator that returns true if the ptr is not null
		j[pair.first] = isNotNull;
	}

	return j;
}

AssetManager& SceneManager::getAssetManager()
{
	return m_assetManager;
}