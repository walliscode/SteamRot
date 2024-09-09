#include <SFML/Graphics.hpp>
#include "Scene.h"
#include <map>
#include "SceneManager.h"


typedef std::map<std::string, std::shared_ptr<Scene>> SceneList;


class GameEngine
{
	private:

		sf::RenderWindow  m_window; // SFML window
		SceneManager      m_sceneManager; // Scene manager object
		SceneList		  m_scenes;       // A map of scenes with a custom key
		size_t 			  m_loopNumber = 0;   // Number of loops the game has run

		void sUserInput(); // System: User input
	
	public:
		GameEngine(); // Constructor
		void init(); // Initialize the game
		void run(size_t numLoops = 0); // Run the game, numLoops is used specifically for the simulation function
		void update(); // calls the SceneManager update function. This may be uncessary separation but will leave for no
		sf::RenderWindow* getWindow(); // Get the window


		//######### Simulation Functions #########
		size_t getLoopNumber(); // Get the loop number
		void runSimulation(int loops); // Run the game for a given number of loops
		json toJSON(); // convert parts of the GameEngine object to JSON, take a string as a container name for the json
		void createJSON(const std::string& directoryName, const std::string& fileName); // Create a JSON file from parts specificed by toJSON functions from each class. This enables multiple jsons to be created with different structures
		json extractJSON(const std::string& directoryName, const std::string& fileName); // given a json file location, turn file into json object 
		//######### Asset Functions #########
		Assets& getAssets(); // Get the assets

		//######### Scene Management Functions #########
		SceneManager& getSceneManager(); // Get the scene manager

};