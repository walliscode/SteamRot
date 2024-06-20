#include "GameEngine.h"
#include "SceneMainMenu.h"


GameEngine::GameEngine(): m_assets()
{
	init(); // kick off the game, loading assets, calling run function e.t.c
}

void GameEngine::init()
{
	// Create a new SFML window
	m_window.create(sf::VideoMode(800, 600), "SFML window");

	// get size of asssets fonts
	size_t size = m_assets.getFonts().size();
	std::cout << "Size of fonts: " << size << std::endl;

	// kick off initial scene
	addScene("mainMenu", std::make_shared<SceneMainMenu>("mainMenu", 10, *this));

}

void GameEngine::run()
{
	// get size of asssets fonts
	size_t size = m_assets.getFonts().size();
	std::cout << "Size of fonts in GameEngine run: " << size << std::endl;
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

		// Update all the scenes
		GameEngine::update(m_scenes);

		// End the current frame and display its contents on screen
		m_window.display();
	}
}

void GameEngine::update(SceneList& scenes)
{
	// Loop through all the scenes and update them only if m_active is true
	for (auto& pair : scenes)
	{
		auto& scene = pair.second;
		if (scene->getActive()) {
			scene->update();
		}
		
	}
}

sf::RenderWindow* GameEngine::getWindow()
{
	return & m_window;
}

Assets& GameEngine::getAssets()
{
	return m_assets;
}



void GameEngine::addScene(std::string tag, std::shared_ptr<Scene> scene)
{	
	m_scenes.insert({ tag, scene });

}

void GameEngine::removeScene(std::string tag)
{
	// Remove the scene
	m_scenes.erase(tag);
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

const SceneList& GameEngine::getScenes()
{
	return m_scenes;
}