#include <SFML/Graphics.hpp>
#include "Assets.h"
#include "Scene.h"
#include <map>


typedef std::map<std::string, std::shared_ptr<Scene>> SceneList;


class GameEngine
{
	private:

		sf::RenderWindow  m_window; // SFML window
		Assets            m_assets;       // Assets object
		SceneList		  m_scenes;       // A map of scenes with a custom key
	
	public:
		GameEngine(); // Constructor
		void init(); // Initialize the game
		void run(); // Run the game
		void update(SceneList& scenes); // Update the game, update the scenes
		sf::RenderWindow* getWindow(); // Get the window

		//######### Asset Functions #########
		Assets* getAssets(); // Get the assets

		// ######### Scene Functions #########
		

		void addScene(std::string tag, std::shared_ptr<Scene> scene); // Add a scene
		void removeScene(std::string tag); // Remove a scene
		void activateScene(std::shared_ptr<Scene> scene); // Activate a scene
		void deactivateScene(std::shared_ptr<Scene> scene); // Deactivate a scene

		const SceneList& getScenes(); // Get the scenes
};