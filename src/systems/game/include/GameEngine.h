#include <SFML/Graphics.hpp>
#include "Assets.h"
#include "Scene.h"
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

typedef std::map<std::string, std::shared_ptr<Scene>> SceneList;


class GameEngine
{
	private:

		sf::RenderWindow  m_window; // SFML window
		Assets            m_assets;       // Assets object
		SceneList		  m_scenes;       // A map of scenes with a custom key
		size_t 			  m_loopNumber = 0;   // Number of loops the game has run
		void sUserInput(); // System: User input
	
	public:
		GameEngine(); // Constructor
		void init(); // Initialize the game
		void run(size_t numLoops = 0); // Run the game, numLoops is used specifically for the simulation function
		void update(SceneList& scenes); // Update the game, update the scenes
		sf::RenderWindow* getWindow(); // Get the window

		//######### Simulation Functions #########
		size_t getLoopNumber(); // Get the loop number
		void runSimulation(int loops); // Run the game for a given number of loops
		json toJSON(std::string containerName); // convert parts of the GameEngine object to JSON, take a string as a container name for the json

		//######### Asset Functions #########
		Assets& getAssets(); // Get the assets

		// ######### Scene Functions #########
		

		void addScene(std::string tag, std::shared_ptr<Scene> scene); // Add a scene
		void removeScene(std::string tag); // Remove a scene
		void activateScene(std::shared_ptr<Scene> scene); // Activate a scene
		void deactivateScene(std::shared_ptr<Scene> scene); // Deactivate a scene

		const SceneList& getScenes(); // Get the scenes
};