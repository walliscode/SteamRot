#include "GameEngine.h"

GameEngine::GameEngine()
{
	init(); // kick off the game, loading assets, calling run function e.t.c
}

void GameEngine::init()
{
	// Create a new SFML window
	window.create(sf::VideoMode(800, 600), "SFML window");
}

void GameEngine::run()
{
	// Run the program as long as the window is open
	while (window.isOpen())
	{
		// Check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// Clear the window with green color
		window.clear(sf::Color::Green);

		// End the current frame and display its contents on screen
		window.display();
	}
}

sf::RenderWindow* GameEngine::getWindow()
{
	return &window;
}