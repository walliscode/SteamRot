#include <catch2/catch_test_macros.hpp>
#include "TestScene.h"
#include "GameEngine.h"

TEST_CASE("SceneManager is created", "[SceneManager]") {
	GameEngine game;
	SceneManager& sceneManager = game.getSceneManager();

	REQUIRE(sceneManager.getScenes().size() == 1); // Check that the scene list is empty
}