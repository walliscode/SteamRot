/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for SceneManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneManager.h"

#include "scene_helpers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SceneManager is constructed without any errors", "[SceneManager]") {

  steamrot::SceneManager scene_manager{steamrot::tests::create_game_context()};
  REQUIRE_NOTHROW(scene_manager);
}

TEST_CASE("SceneManager's AddSceneFromDefault creates a configured TitleScene",
          "[SceneManager]") {
  steamrot::SceneManager scene_manager{steamrot::tests::create_game_context()};

  auto title_result =
      scene_manager.AddSceneFromDefault(steamrot::SceneType::SceneType_TITLE);
  if (!title_result.has_value()) {
    FAIL("Failed to create test scene from default: " +
         title_result.error().message);
  }
}

TEST_CASE(
    "SceneManager's AddSceneFromDefault creates a configured CraftingScene",
    "[SceneManager]") {
  steamrot::SceneManager scene_manager{steamrot::tests::create_game_context()};
  auto crafting_result = scene_manager.AddSceneFromDefault(
      steamrot::SceneType::SceneType_CRAFTING);
  if (!crafting_result.has_value()) {
    FAIL("Failed to create test scene from default: " +
         crafting_result.error().message);
  }
}
