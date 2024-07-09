#include "GameEngine.h"
#include "SceneMainMenu.h"


GameEngine::GameEngine() :m_sceneManager(*this)
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
		// handle user input
		sUserInput();

		// Clear the window with green color
		m_window.clear(sf::Color::Green);

		// Update all the scenes
		GameEngine::update();

		// End the current frame and display its contents on screen
		m_window.display();
	}
}

void GameEngine::update()
{
	// call the update function of the scene manager. SceneManager name space is used to avoid ambiguity
	m_sceneManager.SceneManager::update();
}

sf::RenderWindow* GameEngine::getWindow()
{
	return & m_window;
}



void GameEngine::sUserInput()
{
	// Check all the window's events that were triggered since the last iteration of the loop
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		// "close requested" event: we close the window
		if (event.type == sf::Event::Closed)
			m_window.close();

		// Check for key use 
		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{
			// pass the event to the scene manager if it is a key press or key release
			m_sceneManager.passEvent(event);


		}
	}
}

SceneManager& GameEngine::getSceneManager()
{
	return m_sceneManager;

}