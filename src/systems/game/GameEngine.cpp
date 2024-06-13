#include "GameEngine.h"


GameEngine::GameEngine(): m_assets()
{
	init(); // kick off the game, loading assets, calling run function e.t.c
}

void GameEngine::init()
{
	// Create a new SFML window
	m_window.create(sf::VideoMode(800, 600), "SFML window");

}

void GameEngine::run()
{
	// Run the program as long as the window is open
	while (m_window.isOpen())
	{
		// Check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				m_window.close();
		}

		// Clear the window with green color
		m_window.clear(sf::Color::Green);

		// End the current frame and display its contents on screen
		m_window.display();
	}
}

void GameEngine::update(SceneList* scenes)
{
	// Loop through all the scenes and update them
	for (auto scene : *scenes)
	{
		if (scene->getActive()) {
			scene->update();
		}
		
	}
}

sf::RenderWindow* GameEngine::getWindow()
{
	return & m_window;
}

Assets* GameEngine::getAssets()
{
	return &m_assets;
}



void GameEngine::addScene(std::shared_ptr<Scene> scene)
{
	m_scenes.push_back(scene);
}

void GameEngine::removeScene(std::shared_ptr<Scene> scene)
{
	// Remove the scene from the list
	m_scenes.erase(std::remove(m_scenes.begin(), m_scenes.end(), scene), m_scenes.end());
}

void GameEngine::activateScene(std::shared_ptr<Scene> scene)
{
	// Activate the scene
	scene->setActive(true);
}

void GameEngine::deactivateScene(std::shared_ptr<Scene> scene)
{
	// Deactivate the scene
	scene->setActive(false);
}