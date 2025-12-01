/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for resource configuration free functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "resources_configuration.h"
#include "FlatbuffersDataLoader.h"
#include "GameResources.h"
#include "SceneResources.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConfigureGameResources with null game data",
          "[unit][resources][resources_configuration]") {
  steamrot::GameResources resources;

  auto result = steamrot::resources::ConfigureGameResources(resources, nullptr);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("ConfigureGameResources loads game resources data successfully",
          "[unit][resources][resources_configuration]") {
  steamrot::FlatbuffersDataLoader loader;

  auto game_resources_result = loader.ProvideGameResourcesData();
  REQUIRE(game_resources_result.has_value());

  steamrot::GameResources resources;
  auto config_result = steamrot::resources::ConfigureGameResources(resources, game_resources_result.value());
  REQUIRE(config_result.has_value());
}

TEST_CASE("ConfigureGameResources configures GameResources with environment type",
          "[unit][resources][resources_configuration]") {
  steamrot::FlatbuffersDataLoader loader;

  auto game_resources_result = loader.ProvideGameResourcesData();
  REQUIRE(game_resources_result.has_value());

  steamrot::GameResources resources;
  auto config_result = steamrot::resources::ConfigureGameResources(resources, game_resources_result.value());
  REQUIRE(config_result.has_value());

  // Verify window was created
  REQUIRE(resources.game_window.isOpen());
}

TEST_CASE("ConfigureSceneResources from scene data",
          "[unit][resources][resources_configuration]") {
  steamrot::FlatbuffersDataLoader loader;

  auto scene_resources_result =
      loader.ProvideSceneResourcesData(steamrot::SceneType::SceneType_TEST);
  REQUIRE(scene_resources_result.has_value());

  steamrot::SceneResources resources;
  auto config_result = steamrot::resources::ConfigureSceneResources(
      resources, scene_resources_result.value());
  REQUIRE(config_result.has_value());

  // Verify render texture was created
  REQUIRE(resources.scene_texture.getSize().x > 0);
  REQUIRE(resources.scene_texture.getSize().y > 0);
}

TEST_CASE("ConfigureSceneResources uses defaults when scene data is null",
          "[unit][resources][resources_configuration]") {
  steamrot::SceneResources resources;

  // Configure with null scene data - should use defaults
  auto result = steamrot::resources::ConfigureSceneResources(resources, nullptr);
  REQUIRE(result.has_value());

  // Verify default dimensions were used (800x600)
  REQUIRE(resources.scene_texture.getSize().x == 800);
  REQUIRE(resources.scene_texture.getSize().y == 600);
}

TEST_CASE("ConfigureSceneResources configures resources for multiple scene types",
          "[unit][resources][resources_configuration]") {
  steamrot::FlatbuffersDataLoader loader;

  // Test multiple scene types
  auto test_scene_data =
      loader.ProvideSceneResourcesData(steamrot::SceneType::SceneType_TEST);
  REQUIRE(test_scene_data.has_value());

  steamrot::SceneResources test_resources;
  auto test_result = steamrot::resources::ConfigureSceneResources(
      test_resources, test_scene_data.value());
  REQUIRE(test_result.has_value());

  auto title_scene_data =
      loader.ProvideSceneResourcesData(steamrot::SceneType::SceneType_TITLE);
  REQUIRE(title_scene_data.has_value());

  steamrot::SceneResources title_resources;
  auto title_result = steamrot::resources::ConfigureSceneResources(
      title_resources, title_scene_data.value());
  REQUIRE(title_result.has_value());
}

TEST_CASE("ConfigureSceneResources with custom scene resource data",
          "[unit][resources][resources_configuration]") {

  // Create custom scene resources
  flatbuffers::FlatBufferBuilder fbb;
  auto scene_data = steamrot::CreateSceneResourcesData(fbb, 1024, 768);
  fbb.Finish(scene_data);

  const steamrot::SceneResourcesData *custom_scene_data =
      flatbuffers::GetRoot<steamrot::SceneResourcesData>(
          fbb.GetBufferPointer());

  steamrot::SceneResources resources;

  auto result =
      steamrot::resources::ConfigureSceneResources(resources, custom_scene_data);
  REQUIRE(result.has_value());
  REQUIRE(resources.scene_texture.getSize().x == 1024);
  REQUIRE(resources.scene_texture.getSize().y == 768);
}
