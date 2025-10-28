/////////////////////////////////////////////////
/// @file
/// @brief Integration tests for context configuration loading
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ContextConfigurator.h"
#include "FlatbuffersDataLoader.h"
#include "GameResources.h"
#include "PathProvider.h"
#include "SceneResources.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Load context configuration and configure resources",
          "[integration][context][configuration]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;

  // Load context configuration data
  auto context_data_result = loader.ProvideContextData();
  REQUIRE(context_data_result.has_value());

  const steamrot::ContextData *context_data = context_data_result.value();
  REQUIRE(context_data != nullptr);

  // Verify game context configuration
  REQUIRE(context_data->game_context() != nullptr);
  REQUIRE(context_data->game_context()->window_width() > 0);
  REQUIRE(context_data->game_context()->window_height() > 0);
  REQUIRE(context_data->game_context()->window_title() != nullptr);
  REQUIRE(context_data->game_context()->environment_type() != nullptr);

  // Verify scene contexts configuration
  REQUIRE(context_data->scene_contexts() != nullptr);
  REQUIRE(context_data->scene_contexts()->size() > 0);

  // Create configurator
  steamrot::ContextConfigurator configurator(context_data);

  // Configure GameResources from configuration
  steamrot::GameResources game_resources;
  auto game_config_result = configurator.ConfigureGameResources(game_resources);
  REQUIRE(game_config_result.has_value());

  // Configure SceneResources for each configured scene
  for (const auto *scene_config : *context_data->scene_contexts()) {
    steamrot::SceneResources scene_resources;
    auto scene_config_result =
        configurator.ConfigureSceneResources(scene_resources,
                                             scene_config->scene_type());
    REQUIRE(scene_config_result.has_value());
  }
}

TEST_CASE("Configuration supports all required scene types",
          "[integration][context][configuration]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;

  auto context_data_result = loader.ProvideContextData();
  REQUIRE(context_data_result.has_value());

  steamrot::ContextConfigurator configurator(context_data_result.value());

  // Verify TEST scene is configured
  steamrot::SceneResources test_resources;
  auto test_result = configurator.ConfigureSceneResources(
      test_resources, steamrot::SceneType::SceneType_TEST);
  REQUIRE(test_result.has_value());

  // Verify TITLE scene is configured
  steamrot::SceneResources title_resources;
  auto title_result = configurator.ConfigureSceneResources(
      title_resources, steamrot::SceneType::SceneType_TITLE);
  REQUIRE(title_result.has_value());
}

TEST_CASE("Configuration values are properly loaded",
          "[integration][context][configuration]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;

  auto context_data_result = loader.ProvideContextData();
  REQUIRE(context_data_result.has_value());

  const steamrot::ContextData *context_data = context_data_result.value();
  const steamrot::GameContextConfig *game_config = context_data->game_context();

  // Verify configuration values are reasonable
  REQUIRE(game_config->window_width() >= 640);
  REQUIRE(game_config->window_height() >= 480);
  REQUIRE(game_config->framerate_limit() > 0);
  REQUIRE(game_config->framerate_limit() <= 240);

  // Verify scene configurations have valid values
  for (const auto *scene_config : *context_data->scene_contexts()) {
    REQUIRE(scene_config->entity_pool_size() > 0);
    REQUIRE(scene_config->render_texture_width() > 0);
    REQUIRE(scene_config->render_texture_height() > 0);
  }
}

TEST_CASE("Resources can be configured from context data",
          "[integration][context][configuration]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;

  auto context_data_result = loader.ProvideContextData();
  REQUIRE(context_data_result.has_value());

  steamrot::ContextConfigurator configurator(context_data_result.value());

  // Configure GameResources from configurator
  steamrot::GameResources game_resources;
  auto config_result = configurator.ConfigureGameResources(game_resources);
  REQUIRE(config_result.has_value());

  // Verify the resources were configured with correct values
  REQUIRE(game_resources.game_window.isOpen());
  REQUIRE(game_resources.env_type == steamrot::EnvironmentType::Test);
  
  // Verify default initialization of other resources
  REQUIRE(game_resources.loop_number == 0);
  REQUIRE(game_resources.mouse_position.x == 0);
  REQUIRE(game_resources.mouse_position.y == 0);
}
