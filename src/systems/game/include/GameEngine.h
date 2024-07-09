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

		void sUserInput(); // System: User input
	
	public:
		GameEngine(); // Constructor
		void init(); // Initialize the game
		void run(); // Run the game
		void update(); // Update the game, update the scenes
		sf::RenderWindow* getWindow(); // Get the window

		//######### Scene Management Functions #########
		SceneManager& getSceneManager(); // Get the scene manager

};