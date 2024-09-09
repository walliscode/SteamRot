#include <catch2/catch_test_macros.hpp>
#include "TestScene.h"
#include "GameEngine.h"

TEST_CASE("SceneManager is created", "[SceneManager]") {
	GameEngine game;
	SceneManager& sceneManager = game.getSceneManager();

	REQUIRE(sceneManager.getAllScenes().size() == 1); // Check that the scene list is not empty
	REQUIRE(sceneManager.getActiveScenes().size() == 0); // Check that the active scene list is empty
	REQUIRE(sceneManager.getInactiveScenes().size() == 1); // Check that the inactive scene list is not empty
	REQUIRE(sceneManager.getInteractiveScenes().size() == 0); // Check that the interactive scene list is empty
	// check functions of SceneManager
	sceneManager.addScene("testScene", std::make_shared<TestScene>("testScene", 10, game));
	REQUIRE(sceneManager.getAllScenes().size() == 2); // Check that the scene list has increased by 1

	sceneManager.removeScene("testScene");
	REQUIRE(sceneManager.getAllScenes().size() == 1); // Check that the scene list has decreased by 1

	sceneManager.activateScene("mainMenu");
	REQUIRE(sceneManager.getAllScenes().size() == 1); // Check that the all scenes list has not changed
	REQUIRE(sceneManager.getActiveScenes().size() == 1); // Check that the active scene list has increased by 1
	REQUIRE(sceneManager.getInactiveScenes().size() == 0); // Check that the inactive scene list has decreased by 1
	REQUIRE(sceneManager.getInteractiveScenes().size() == 0); // Check that the interactive scene list has not changed


	sceneManager.deactivateScene("mainMenu");
	REQUIRE(sceneManager.getAllScenes().size() == 1); // Check that the scene list has not changed
	REQUIRE(sceneManager.getActiveScenes().size() == 0); // Check that the active scene list has decreased by 1
	REQUIRE(sceneManager.getInactiveScenes().size() == 1); // Check that the inactive scene list has increased by 1
	REQUIRE(sceneManager.getInteractiveScenes().size() == 0); // Check that the interactive scene list has not changed



}