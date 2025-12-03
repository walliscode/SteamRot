/////////////////////////////////////////////////
/// @file
/// @brief Integration tests for core configuration loading
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersDataLoader.h"
#include "GameCore.h"
#include "SceneCore.h"
#include "core_configuration.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Load core configuration and configure cores",
          "[integration][core][configuration]") {
  steamrot::FlatbuffersDataLoader loader;

  // Load engine core data
  auto engine_core_result = loader.ProvideEngineCoreData();
  REQUIRE(engine_core_result.has_value());

  const steamrot::EngineCoreData *engine_data = engine_core_result.value();
  REQUIRE(engine_data != nullptr);

  // Verify engine core configuration
  REQUIRE(engine_data->window_width() > 0);
  REQUIRE(engine_data->window_height() > 0);
  REQUIRE(engine_data->window_title() != nullptr);

  // Configure GameCore from configuration
  steamrot::GameCore game_core;
  auto game_config_result = steamrot::core::ConfigureGameCore(game_core, engine_data);
  REQUIRE(game_config_result.has_value());

  // Configure SceneCore for test scene
  auto scene_core_result =
      loader.ProvideSceneCoreData(steamrot::SceneType::SceneType_TEST);
  REQUIRE(scene_core_result.has_value());

  steamrot::SceneCore scene_core;
  auto scene_config_result =
      steamrot::core::ConfigureSceneCore(scene_core,
                                         scene_core_result.value());
  REQUIRE(scene_config_result.has_value());
}

TEST_CASE("Configuration supports all required scene types",
          "[integration][core][configuration]") {
  steamrot::FlatbuffersDataLoader loader;

  // Verify TEST scene is configured
  auto test_scene_data =
      loader.ProvideSceneCoreData(steamrot::SceneType::SceneType_TEST);
  REQUIRE(test_scene_data.has_value());

  steamrot::SceneCore test_core;
  auto test_result = steamrot::core::ConfigureSceneCore(
      test_core, test_scene_data.value());
  REQUIRE(test_result.has_value());

  // Verify TITLE scene is configured
  auto title_scene_data =
      loader.ProvideSceneCoreData(steamrot::SceneType::SceneType_TITLE);
  REQUIRE(title_scene_data.has_value());

  steamrot::SceneCore title_core;
  auto title_result = steamrot::core::ConfigureSceneCore(
      title_core, title_scene_data.value());
  REQUIRE(title_result.has_value());
}

TEST_CASE("Configuration values are properly loaded",
          "[integration][core][configuration]") {
  steamrot::FlatbuffersDataLoader loader;

  auto engine_core_result = loader.ProvideEngineCoreData();
  REQUIRE(engine_core_result.has_value());

  const steamrot::EngineCoreData *engine_data = engine_core_result.value();

  // Verify configuration values are reasonable
  REQUIRE(engine_data->window_width() >= 640);
  REQUIRE(engine_data->window_height() >= 480);
  REQUIRE(engine_data->framerate_limit() > 0);
  REQUIRE(engine_data->framerate_limit() <= 240);

  // Verify scene configurations have valid values
  auto test_scene_data =
      loader.ProvideSceneCoreData(steamrot::SceneType::SceneType_TEST);
  REQUIRE(test_scene_data.has_value());

  const steamrot::SceneCoreData *scene_data = test_scene_data.value();
  if (scene_data) {
    REQUIRE(scene_data->render_texture_width() > 0);
    REQUIRE(scene_data->render_texture_height() > 0);
  }
}

TEST_CASE("Cores can be configured from core data",
          "[integration][core][configuration]") {
  steamrot::FlatbuffersDataLoader loader;

  auto engine_core_result = loader.ProvideEngineCoreData();
  REQUIRE(engine_core_result.has_value());

  // Configure GameCore from core data
  steamrot::GameCore game_core;
  auto config_result = steamrot::core::ConfigureGameCore(game_core, engine_core_result.value());
  REQUIRE(config_result.has_value());

  // Verify the core was configured with correct values
  REQUIRE(game_core.game_window.isOpen());
  
  // Verify default initialization of other core members
  REQUIRE(game_core.loop_number == 1);
  REQUIRE(game_core.mouse_position.x == 0);
  REQUIRE(game_core.mouse_position.y == 0);
}
