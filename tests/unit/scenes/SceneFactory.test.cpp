/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for SceneFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneFactory.h"
#include "CraftingScene.h"
#include "TestFixture.h"
#include "TitleScene.h"
#include "scene_test_helpers.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <catch2/catch_test_macros.hpp>

// create a GameContext object for use by all tests
TEST_CASE("SceneFactory can be constructed without errors",
          "[unit][SceneFactory]") {
  steamrot::tests::TestFixture test_fixture;
  test_fixture.Intialize();
  steamrot::ISceneFactory scene_factory;
  REQUIRE_NOTHROW(scene_factory);
}

TEST_CASE("SceneFactory can create a TitleScene from default",
          "[unit][SceneFactory]") {
  steamrot::tests::TestFixture test_fixture;
  test_fixture.Intialize();
  steamrot::ISceneFactory scene_factory;

  // define SceneType for the test
  const steamrot::SceneType scene_type = steamrot::SceneType::SceneType_TITLE;

  // create a TitleScene
  auto scene_creation_result = scene_factory.CreateDefaultScene(
      scene_type, test_fixture.GetGameContext());

  if (!scene_creation_result.has_value()) {
    FAIL("Scene creation failed: " + scene_creation_result.error().message);
  }

  // check that the created scene is a TitleScene
  auto title_scene = std::move(scene_creation_result.value());
  REQUIRE(title_scene != nullptr);
  REQUIRE(title_scene->GetSceneInfo().type == scene_type);
  REQUIRE(dynamic_cast<steamrot::TitleScene *>(title_scene.get()));

  // check that the TitleScene is configured correctly
  steamrot::tests::CheckDefaultSceneConfiguration(*title_scene);
}

TEST_CASE("SceneFactory can create a CraftingScene from default",
          "[unit][SceneFactory]") {
  steamrot::tests::TestFixture test_fixture;
  test_fixture.Intialize();
  steamrot::ISceneFactory scene_factory;
  // create a CraftingScene
  auto scene_creation_result = scene_factory.CreateDefaultScene(
      steamrot::SceneType::SceneType_CRAFTING, test_fixture.GetGameContext());
  if (!scene_creation_result.has_value()) {
    FAIL("Scene creation failed: " + scene_creation_result.error().message);
  }
  // check that the created scene is a CraftingScene
  auto crafting_scene = std::move(scene_creation_result.value());
  REQUIRE(crafting_scene != nullptr);
  REQUIRE(crafting_scene->GetSceneInfo().type ==
          steamrot::SceneType::SceneType_CRAFTING);
  REQUIRE(dynamic_cast<steamrot::CraftingScene *>(crafting_scene.get()));

  // check that the CraftingScene is configured correctly
  steamrot::tests::CheckDefaultSceneConfiguration(*crafting_scene);
}
