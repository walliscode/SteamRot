#include <SFML/Graphics.hpp>
#include "Assets.h"
#include "Scene.h"

typedef std::vector<std::shared_ptr<Scene>> SceneList;

class GameEngine
{
	private:

		sf::RenderWindow  m_window; // SFML window
		Assets            m_assets;       // Assets object
		SceneList		 m_scenes;       // List of scenes
	
	public:
		GameEngine(); // Constructor
		void init(); // Initialize the game
		void run(); // Run the game
		void update(SceneList* scenes); // Update the game, update the scenes
		sf::RenderWindow* getWindow(); // Get the window

		//######### Asset Functions #########
		Assets* getAssets(); // Get the assets

		// ######### Scene Functions #########
		

		void addScene(std::shared_ptr<Scene> scene); // Add a scene
		void removeScene(std::shared_ptr<Scene> scene); // Remove a scene
		void activateScene(std::shared_ptr<Scene> scene); // Activate a scene
		void deactivateScene(std::shared_ptr<Scene> scene); // Deactivate a scene
};