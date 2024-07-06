#include "SceneManager.h"
#include "SceneMainMenu.h"


SceneManager::SceneManager(GameEngine& game) : m_scenes(), m_game(game)
{

	// kick off initial scene(s)
		addScene("mainMenu", std::make_shared<SceneMainMenu>("mainMenu", 10, game));
}


void SceneManager::addScene(std::string tag, std::shared_ptr<Scene> scene)
{
	m_scenes[tag] = scene;
}

void SceneManager::removeScene(std::string tag)
{
	m_scenes.erase(tag);
}

void SceneManager::activateScene(std::shared_ptr<Scene> scene)
{
	scene->setActive(true);
}

void SceneManager::deactivateScene(std::shared_ptr<Scene> scene)
{
	scene->setActive(false);
}

SceneList& SceneManager::getScenes()
{
	return m_scenes;
}

void SceneManager::update()
{
	// Loop through all the scenes and update them only if m_active is true
	for (auto& pair : m_scenes)
	{
		auto& scene = pair.second;
		if (scene->getActive()) {
			scene->update();
		}

	}
}

void SceneManager::passEvent(sf::Event& event)
{
	for (auto& pair : m_scenes)
	{
		auto& scene = pair.second;

		// add in guard clauses

		// check if scene is currently active
		if (!scene->getActive()) {
			continue;
		}
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

