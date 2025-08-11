/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for SceneFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SceneFactory.h"
#include "CraftingScene.h"
#include "GameContext.h"
#include "TitleScene.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <catch2/catch_test_macros.hpp>

// create a GameContext object for use by all tests
sf::RenderWindow window(sf::VideoMode({800, 600}), "Test Window");
steamrot::EventHandler event_handler;
sf::Vector2i mouse_position(0, 0);
size_t loop_number = 0;
steamrot::AssetManager asset_manager;
steamrot::DataManager data_manager;

steamrot::GameContext game_context(window, event_handler, mouse_position,
                                   loop_number, asset_manager, data_manager);

TEST_CASE("SceneFactory can be constructed without errors", "[SceneFactory]") {
  steamrot::SceneFactory scene_factory(game_context);
  REQUIRE_NOTHROW(scene_factory);
}

TEST_CASE("SceneFactory can create a TitleScene", "[SceneFactory]") {
  steamrot::SceneFactory scene_factory(game_context);
  // create a TitleScene
  auto scene_creation_result =
      scene_factory.CreateDefaultScene(steamrot::SceneType::SceneType_TITLE);

  if (!scene_creation_result.has_value()) {
    FAIL("Scene creation failed: " + scene_creation_result.error().message);
  }

  // check that the created scene is a TitleScene
  auto title_scene = std::move(scene_creation_result.value());
  REQUIRE(title_scene != nullptr);
  REQUIRE(title_scene->GetSceneType() == steamrot::SceneType::SceneType_TITLE);
  REQUIRE(dynamic_cast<steamrot::TitleScene *>(title_scene.get()));
}

TEST_CASE("SceneFactory can create a CraftingScene", "[SceneFactory]") {
  steamrot::SceneFactory scene_factory(game_context);
  // create a CraftingScene
  auto scene_creation_result =
      scene_factory.CreateDefaultScene(steamrot::SceneType::SceneType_CRAFTING);
  if (!scene_creation_result.has_value()) {
    FAIL("Scene creation failed: " + scene_creation_result.error().message);
  }
  // check that the created scene is a CraftingScene
  auto crafting_scene = std::move(scene_creation_result.value());
  REQUIRE(crafting_scene != nullptr);
  REQUIRE(crafting_scene->GetSceneType() ==
          steamrot::SceneType::SceneType_CRAFTING);
  REQUIRE(dynamic_cast<steamrot::CraftingScene *>(crafting_scene.get()));
}
