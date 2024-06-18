#include <catch2/catch_test_macros.hpp>
#include "TestScene.h"
#include "GameEngine.h"

TEST_CASE("Scene is created with an entity manager", "[Scene]") {

	GameEngine game;
	TestScene TestScene("SceneTest", 10, game);

	REQUIRE(TestScene.getActive() == true); // check that it intialises with active scene
}