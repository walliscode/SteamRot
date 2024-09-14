#include <catch2/catch_test_macros.hpp>
#include "GameEngine.h"
#include <iostream>

TEST_CASE("Simulation 1 is run", "[Simulations]") {
	// create GameEngine object
	GameEngine game;

	// run simulation with 14 loops
	game.runSimulation(14);

	// check for results in json

	json simulationResults = game.extractJSON("simulations", "test_data");

	REQUIRE(simulationResults["GameEngine"]["loopNumber"] == 14);
	
	// test for required scenes

	std::vector<std::string> scenes = {"mainMenu"};
	
	// check that the SceneManager has the required scenes and not any extra or less
	REQUIRE(simulationResults["SceneManager"]["scenes"].size() == scenes.size());

	// test for specific scenes
	for (auto scene : scenes) {
		REQUIRE(simulationResults["SceneManager"]["scenes"].contains(scene));
	}

	// test for entities/entity data in specific scenes

	REQUIRE(simulationResults["SceneManager"]["scenes"]["mainMenu"]["entities"].size() == 1);

	// test for required fonts
	std::vector<std::string> fonts = { "SourceCodePro-Regular" };

	REQUIRE(simulationResults["AssetManager"]["fonts"].size() == fonts.size());
	for (auto font : fonts) {
		REQUIRE(simulationResults["AssetManager"]["fonts"].contains(font));
	}


}