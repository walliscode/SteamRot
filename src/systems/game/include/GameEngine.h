#include <SFML/Graphics.hpp>
#include "Assets.h"

class GameEngine
{
	private:
		sf::RenderWindow  m_window; // SFML window
		Assets            m_assets;       // Assets object
	
	public:
		GameEngine(); // Constructor
		void init(); // Initialize the game
		void run(); // Run the game
		sf::RenderWindow* getWindow(); // Get the window
		Assets* getAssets(); // Get the assets

		
};