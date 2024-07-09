#include <catch2/catch_test_macros.hpp>
#include "TestScene.h"
#include "GameEngine.h"

TEST_CASE("Scene is created with an entity manager", "[Scene]") {

	GameEngine game;
	TestScene TestScene("SceneTest", 10, game, game.getSceneManager());

	REQUIRE(TestScene.getActive() == true); // check that it intialises with active scene
	REQUIRE(TestScene.getActionMap().size() == 1); // check that it intialises with the scene with included actions from json
	REQUIRE(TestScene.getActionMap().at(73) == "UP"); // check that the correct key and value are in the action map
}