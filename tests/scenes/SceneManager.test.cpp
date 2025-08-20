/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for SceneManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneManager.h"

#include "GameContext.h"
#include "TestContext.h"

#include "asset_helpers.h"
#include "scene_types_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SceneManager is constructed without any errors", "[SceneManager]") {

  steamrot::tests::TestContext test_context;
  steamrot::GameContext game_context =
      test_context
          .GetGameContext(); // Get the game context from the test context
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
  REQUIRE_NOTHROW(scene_manager);
}

TEST_CASE("SceneManager's AddSceneFromDefault creates a configured TitleScene",
          "[SceneManager]") {

  steamrot::tests::TestContext test_context;

  steamrot::GameContext game_context =
      test_context
          .GetGameContext(); // Get the game context from the test context
  steamrot::SceneManager scene_manager{game_context};

  // define the scene type
  const steamrot::SceneType scene_type = steamrot::SceneType::SceneType_TITLE;

  // add the scene from default data
  auto title_result = scene_manager.AddSceneFromDefault(scene_type);

  if (!title_result.has_value()) {
    FAIL("Failed to create test scene from default: " +
         title_result.error().message);
  }

  // check AssetManger config
  steamrot::tests::check_asset_configuration(scene_type,
                                             game_context.asset_manager);
}

TEST_CASE(
    "SceneManager's AddSceneFromDefault creates a configured CraftingScene",
    "[SceneManager]") {

  steamrot::tests::TestContext test_context;
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};

  // define the scene type
  const steamrot::SceneType scene_type =
      steamrot::SceneType::SceneType_CRAFTING;
  // add the scene from default data
  auto crafting_result = scene_manager.AddSceneFromDefault(scene_type);
  if (!crafting_result.has_value()) {
    FAIL("Failed to create test scene from default: " +
         crafting_result.error().message);
  }
  // check AssetManger config
  steamrot::tests::check_asset_configuration(
      scene_type, test_context.GetGameContext().asset_manager);
}

TEST_CASE("SceneManager LoadTitleScene returns monostate", "[SceneManager]") {

  steamrot::tests::TestContext test_context;
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
  auto load_title_result = scene_manager.LoadTitleScene();

  if (!load_title_result.has_value()) {
    FAIL("Failed to load title scene: " + load_title_result.error().message);
  }

  // check AssetManger config
  steamrot::tests::check_asset_configuration(
      steamrot::SceneType::SceneType_TITLE,
      test_context.GetGameContext().asset_manager);
}

TEST_CASE("SceneManager LoadCraftingScene returns monostate",
          "[SceneManager]") {

  steamrot::tests::TestContext test_context;
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
  auto load_crafting_result = scene_manager.LoadCraftingScene();
  if (!load_crafting_result.has_value()) {
    FAIL("Failed to load crafting scene: " +
         load_crafting_result.error().message);
  }

  // check AssetManger config
  steamrot::tests::check_asset_configuration(
      steamrot::SceneType::SceneType_CRAFTING,
      test_context.GetGameContext().asset_manager);
}
