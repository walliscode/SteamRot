#include "GameEngine.h"
#include "SceneMainMenu.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;



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

void GameEngine::run(size_t numLoops)
{
	// get size of asssets fonts
	size_t size = m_assets.getFonts().size();
	std::cout << "Size of fonts in GameEngine run: " << size << std::endl;
	// Run the program as long as the window is open
	while (m_window.isOpen())
	{
		// increment the loop number by 1, the tick number is defined at the beginning of the loop
		m_loopNumber++;
		// handle user input
		sUserInput();

		// Clear the window with green color
		m_window.clear(sf::Color::Green);

		// Update all the scenes
		GameEngine::update(m_scenes);

		// End the current frame and display its contents on screen
		m_window.display();

		// statement to test whether to break the loop, must be called at end
		if (numLoops > 0 && m_loopNumber >= numLoops)
		{
			// export data to json, first variable is the directory name, second is the file name
			createJSON("simulations", "test_data");
			break;
		}
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
			for (auto& pair : m_scenes)
			{
				auto& scene = pair.second;

				// add in guard clauses

				// check if scene is currently active
				if (!scene->getActive()) {
					continue;
				}
				// check if the key is in the action map
				if (scene->getActionMap().find(event.key.code) == scene->getActionMap().end())
				{
					continue;
				}

				// determine if the event is a key press or key release
				const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";
				
				scene->doAction(Action(scene->getActionMap().at(event.key.code), actionType));
			}

		}
	}
}

size_t GameEngine::getLoopNumber()
{
	return m_loopNumber;
}

void GameEngine::runSimulation(int loops)
{
	// prevent undefined behaviour of simulation loop
	if (loops <= 0)
	{
		throw std::invalid_argument("The number of loops must be greater than 0");
	}
	else {
		GameEngine::run(loops);
	}
	
}

json GameEngine::toJSON(std::string containerName)
{
	json j; // create a json object
	j[containerName] = {}; // create a json object with the container name as key
	j[containerName]["loopNumber"] = m_loopNumber;
	return j;
}

void GameEngine::createJSON(const std::string& directoryName, const std::string& fileName)
{
	std::string fullFileName = fileName + ".json";
	// define the path to the directory
	fs::path filePath = fs::path(DATA_OUT_DIR)/ directoryName / fullFileName;

	// create the directory if it does not exist
	fs::create_directories(filePath.parent_path());

	// create ofstream object
	std::ofstream jsonFile(filePath);

	// check if the file is open
	if (jsonFile.is_open())
	{
		// create a json object which is an empty array
		
		json jsonList = json::array();

		// here we call various toJSON functions from each class to create a json object
		jsonList.push_back(toJSON("GameEngine"));

	

		// write the json object to the file
		jsonFile << jsonList.dump(4);
		// close the file
		jsonFile.close();
	}
	else {
		throw std::runtime_error("Could not open file");
	}
}
