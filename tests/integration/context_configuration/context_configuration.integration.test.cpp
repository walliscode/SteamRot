/////////////////////////////////////////////////
/// @file
/// @brief Integration tests for resource configuration loading
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include "GameResources.h"
#include "resources_configuration.h"
#include "SceneResources.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Load resource configuration and configure resources",
          "[integration][resources][configuration]") {
  steamrot::FlatbuffersDataLoader loader;

  // Load game resources data
  auto game_resources_result = loader.ProvideGameResourcesData();
  REQUIRE(game_resources_result.has_value());

  const steamrot::GameResourcesData *game_data = game_resources_result.value();
  REQUIRE(game_data != nullptr);

  // Verify game resource configuration
  REQUIRE(game_data->window_width() > 0);
  REQUIRE(game_data->window_height() > 0);
  REQUIRE(game_data->window_title() != nullptr);

  // Configure GameResources from configuration
  steamrot::GameResources game_resources;
  auto game_config_result = steamrot::resources::ConfigureGameResources(game_resources, game_data);
  REQUIRE(game_config_result.has_value());

  // Configure SceneResources for test scene
  auto scene_resources_result =
      loader.ProvideSceneResourcesData(steamrot::SceneType::SceneType_TEST);
  REQUIRE(scene_resources_result.has_value());

  steamrot::SceneResources scene_resources;
  auto scene_config_result =
      steamrot::resources::ConfigureSceneResources(scene_resources,
                                        scene_resources_result.value());
  REQUIRE(scene_config_result.has_value());
}

TEST_CASE("Configuration supports all required scene types",
          "[integration][resources][configuration]") {
  steamrot::FlatbuffersDataLoader loader;

  // Verify TEST scene is configured
  auto test_scene_data =
      loader.ProvideSceneResourcesData(steamrot::SceneType::SceneType_TEST);
  REQUIRE(test_scene_data.has_value());

  steamrot::SceneResources test_resources;
  auto test_result = steamrot::resources::ConfigureSceneResources(
      test_resources, test_scene_data.value());
  REQUIRE(test_result.has_value());

  // Verify TITLE scene is configured
  auto title_scene_data =
      loader.ProvideSceneResourcesData(steamrot::SceneType::SceneType_TITLE);
  REQUIRE(title_scene_data.has_value());

  steamrot::SceneResources title_resources;
  auto title_result = steamrot::resources::ConfigureSceneResources(
      title_resources, title_scene_data.value());
  REQUIRE(title_result.has_value());
}

TEST_CASE("Configuration values are properly loaded",
          "[integration][resources][configuration]") {
  steamrot::FlatbuffersDataLoader loader;

  auto game_resources_result = loader.ProvideGameResourcesData();
  REQUIRE(game_resources_result.has_value());

  const steamrot::GameResourcesData *game_data = game_resources_result.value();

  // Verify configuration values are reasonable
  REQUIRE(game_data->window_width() >= 640);
  REQUIRE(game_data->window_height() >= 480);
  REQUIRE(game_data->framerate_limit() > 0);
  REQUIRE(game_data->framerate_limit() <= 240);

  // Verify scene configurations have valid values
  auto test_scene_data =
      loader.ProvideSceneResourcesData(steamrot::SceneType::SceneType_TEST);
  REQUIRE(test_scene_data.has_value());

  const steamrot::SceneResourcesData *scene_data = test_scene_data.value();
  if (scene_data) {
    REQUIRE(scene_data->render_texture_width() > 0);
    REQUIRE(scene_data->render_texture_height() > 0);
  }
}

TEST_CASE("Resources can be configured from resource data",
          "[integration][resources][configuration]") {
  steamrot::FlatbuffersDataLoader loader;

  auto game_resources_result = loader.ProvideGameResourcesData();
  REQUIRE(game_resources_result.has_value());

  // Configure GameResources from resource data
  steamrot::GameResources game_resources;
  auto config_result = steamrot::resources::ConfigureGameResources(game_resources, game_resources_result.value());
  REQUIRE(config_result.has_value());

  // Verify the resources were configured with correct values
  REQUIRE(game_resources.game_window.isOpen());
  
  // Verify default initialization of other resources
  REQUIRE(game_resources.loop_number == 0);
  REQUIRE(game_resources.mouse_position.x == 0);
  REQUIRE(game_resources.mouse_position.y == 0);
}
