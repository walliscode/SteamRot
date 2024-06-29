#include <catch2/catch_test_macros.hpp>
#include "GameEngine.h"
#include "TestScene.h"
#include <iostream>


TEST_CASE("Game is Run", "[GameEngine]") {
	GameEngine game;

	sf::RenderWindow* window = game.getWindow();
	
	REQUIRE(window != nullptr);  // Check that the window is created
	REQUIRE(window->isOpen());   // Check that the window is open


	std::cout << "********************** Assets Tests *********************" << std::endl;
	// Add all assets tests here
	Assets& assets = game.getAssets();

	const std::map<std::string, sf::Font>& fonts = assets.getFonts();
	REQUIRE(fonts.size() > 0);  // Check that the assets object has fonts
	REQUIRE(fonts.find("SourceCodePro-Regular") != fonts.end());  // Check that the assets object has the SourceCodePro-Regular font


	std::cout << "********************** Scene Management Tests *********************" << std::endl;

	const SceneList& testScenes = game.getScenes();

	std::cout << "Checking that the scene list is empty" << std::endl;
	REQUIRE(testScenes.size() == 1);  // Check that the scene list is empty

	std::cout << "Creating a Test Scene\n";
	std::shared_ptr<TestScene> testScene = std::make_shared<TestScene>("SceneTest", 10, game);
	std::cout << "Adding the Test Scene to the Game Engine\n";
	game.addScene("SceneTest", testScene);
	std::cout << "Checking that the scene list has one scene\n";
	REQUIRE(testScenes.size() == 2);  // Check that the scene list has one scene

	std::cout << "Checking that the Test Scene is active\n";
	REQUIRE(testScene->getActive() == true);  // Check that the Test Scene is active

	std::cout << "Deactivating the Test Scene\n";
	game.deactivateScene(testScene);
	std::cout << "Checking that the Test Scene is not active\n";
	REQUIRE(testScene->getActive() == false);  // Check that the Test Scene is not active

	std::cout << "Activating the Test Scene\n";
	game.activateScene(testScene);
	std::cout << "Checking that the Test Scene is active\n";
	REQUIRE(testScene->getActive() == true);  // Check that the Test Scene is active

	std::cout << "**********************Game Engine Scene Management Test END*********************" << "\n";
}
