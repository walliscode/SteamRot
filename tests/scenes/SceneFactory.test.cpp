/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for SceneFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneFactory.h"
#include "CraftingScene.h"
#include "TitleScene.h"
#include "configuration_helpers.h"
#include "containers.h"
#include "scene_helpers.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <catch2/catch_test_macros.hpp>

// create a GameContext object for use by all tests
TEST_CASE("SceneFactory can be constructed without errors", "[SceneFactory]") {

  steamrot::SceneFactory scene_factory(steamrot::tests::create_game_context());
  REQUIRE_NOTHROW(scene_factory);
}

TEST_CASE("SceneFactory can create a TitleScene from default",
          "[SceneFactory]") {
  steamrot::SceneFactory scene_factory(steamrot::tests::create_game_context());

  // define SceneType for the test
  const steamrot::SceneType scene_type = steamrot::SceneType::SceneType_TITLE;

  // create a TitleScene
  auto scene_creation_result = scene_factory.CreateDefaultScene(scene_type);

  if (!scene_creation_result.has_value()) {
    FAIL("Scene creation failed: " + scene_creation_result.error().message);
  }

  // check that the created scene is a TitleScene
  auto title_scene = std::move(scene_creation_result.value());
  REQUIRE(title_scene != nullptr);
  REQUIRE(title_scene->GetSceneInfo().type == scene_type);
  REQUIRE(dynamic_cast<steamrot::TitleScene *>(title_scene.get()));

  // check that the TitleScene entities are initialized correctly
  const steamrot::EntityMemoryPool &entity_memory_pool =
      title_scene->GetEntityMemoryPool();

  steamrot::tests::TestConfigurationOfEMPfromDefaultData(entity_memory_pool,
                                                         scene_type);
}

TEST_CASE("SceneFactory can create a CraftingScene from default",
          "[SceneFactory]") {
  steamrot::SceneFactory scene_factory(steamrot::tests::create_game_context());
  // create a CraftingScene
  auto scene_creation_result =
      scene_factory.CreateDefaultScene(steamrot::SceneType::SceneType_CRAFTING);
  if (!scene_creation_result.has_value()) {
    FAIL("Scene creation failed: " + scene_creation_result.error().message);
  }
  // check that the created scene is a CraftingScene
  auto crafting_scene = std::move(scene_creation_result.value());
  REQUIRE(crafting_scene != nullptr);
  REQUIRE(crafting_scene->GetSceneInfo().type ==
          steamrot::SceneType::SceneType_CRAFTING);
  REQUIRE(dynamic_cast<steamrot::CraftingScene *>(crafting_scene.get()));

  // check that the CraftingScene entities are initialized correctly
  const steamrot::EntityMemoryPool &entity_memory_pool =
      crafting_scene->GetEntityMemoryPool();
  steamrot::tests::TestConfigurationOfEMPfromDefaultData(
      entity_memory_pool, steamrot::SceneType::SceneType_CRAFTING);
}
