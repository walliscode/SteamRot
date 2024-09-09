#include <catch2/catch_test_macros.hpp>
#include "GameEngine.h"
#include "TestScene.h"
#include <iostream>



TEST_CASE("Game is Run", "[GameEngine]") {
	GameEngine game;

	sf::RenderWindow* window = game.getWindow();
	
	REQUIRE(window != nullptr);  // Check that the window is created
	REQUIRE(window->isOpen());   // Check that the window is open


	//std::cout << "********************** Assets Tests *********************" << std::endl;
	//// Add all assets tests here
	//Assets& assets = game.getAssets();

	//const std::map<std::string, sf::Font>& fonts = assets.getFonts();
	//REQUIRE(fonts.size() > 0);  // Check that the assets object has fonts
	//REQUIRE(fonts.find("SourceCodePro-Regular") != fonts.end());  // Check that the assets object has the SourceCodePro-Regular font


	std::cout << "********************** Scene Management Tests *********************" << std::endl;

	SceneManager& sceneManager = game.getSceneManager();
	SceneList& testScenes = sceneManager.getAllScenes();

	std::cout << "Checking that the scene list is empty" << std::endl;
	REQUIRE(testScenes.size() == 1);  // Check that the scene list is empty

	std::cout << "Creating a Test Scene\n";
	std::shared_ptr<TestScene> testScene = std::make_shared<TestScene>("SceneTest", 10, game, game.getSceneManager());
	std::cout << "Adding the Test Scene to the Game Engine\n";
	sceneManager.addScene("SceneTest", testScene);
	std::cout << "Checking that the scene list has one scene\n";
	REQUIRE(testScenes.size() == 2);  // Check that the scene list has one scene

	std::cout << "Checking that the Test Scene is active\n";
	REQUIRE(testScene->getActive() == true);  // Check that the Test Scene is active

	std::cout << "Deactivating the Test Scene\n";
	sceneManager.deactivateScene("SceneTest");
	std::cout << "Checking that the Test Scene is not active\n";
	REQUIRE(testScene->getActive() == false);  // Check that the Test Scene is not active

	std::cout << "Activating the Test Scene\n";
	sceneManager.activateScene("SceneTest");
	std::cout << "Checking that the Test Scene is active\n";
	REQUIRE(testScene->getActive() == true);  // Check that the Test Scene is active

	std::cout << "**********************Game Engine Scene Management Test END*********************" << "\n";
}

TEST_CASE("Simulation is run","[GameEngine]"){

	std::cout << "********************** Simulation Tests *********************" << std::endl;
	GameEngine game;

	std::cout << "Running the simulation for 0 loops\n";
	REQUIRE_THROWS(game.runSimulation(0)); // Check that the simulation does not run if loops = 0
	REQUIRE(game.getLoopNumber() == 0);
	std::cout << "Running the simulation for -5 loops\n";
	REQUIRE_THROWS(game.runSimulation(-5)); // Check that the simulation does not run if loops < 0
	REQUIRE(game.getLoopNumber() == 0);

	std::cout << "Running the simulation for 10 loops\n";
	game.runSimulation(10);
	REQUIRE(game.getLoopNumber() == 10); // Check that the simulation runs for 10 loops

	std::cout << "Creating a JSON file and checking contents\n";
	game.runSimulation(24);
	json sim_json = game.extractJSON("simulations","test_data");
	REQUIRE(sim_json["GameEngine"]["loopNumber"] == 24); // Check that the loop number is 24


	std::cout << "********************** Simulation Tests END *********************" << std::endl;
	
}