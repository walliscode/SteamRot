#include <SFML/Graphics.hpp>

class GameEngine
{
	private:
		sf::RenderWindow  window; // SFML window
	
	public:
		GameEngine(); // Constructor
		void init(); // Initialize the game
		void run(); // Run the game
		sf::RenderWindow* getWindow(); // Get the window
		
};