/////////////////////////////////////////////////
/// @file
/// @brief Integration tests for context configuration loading
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ContextConfigurator.h"
#include "ContextDirector.h"
#include "FlatbuffersDataLoader.h"
#include "GameContextBuilder.h"
#include "LogicContextBuilder.h"
#include "PathProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Load context configuration and create builders",
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

  // Create GameContextBuilder from configuration
  auto game_builder_result = configurator.CreateGameContextBuilder();
  REQUIRE(game_builder_result.has_value());

  // Create LogicContextBuilder for each configured scene
  for (const auto *scene_config : *context_data->scene_contexts()) {
    auto logic_builder_result =
        configurator.CreateLogicContextBuilder(scene_config->scene_type());
    REQUIRE(logic_builder_result.has_value());
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
  auto test_builder = configurator.CreateLogicContextBuilder(
      steamrot::SceneType::SceneType_TEST);
  REQUIRE(test_builder.has_value());

  // Verify TITLE scene is configured
  auto title_builder = configurator.CreateLogicContextBuilder(
      steamrot::SceneType::SceneType_TITLE);
  REQUIRE(title_builder.has_value());
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

TEST_CASE("Builder can be used to create context with runtime objects",
          "[integration][context][configuration]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;

  auto context_data_result = loader.ProvideContextData();
  REQUIRE(context_data_result.has_value());

  steamrot::ContextConfigurator configurator(context_data_result.value());

  // Get builder from configurator
  auto builder_result = configurator.CreateGameContextBuilder();
  REQUIRE(builder_result.has_value());

  auto builder = builder_result.value();

  // Add runtime objects to builder
  auto window = std::make_shared<sf::RenderWindow>();
  auto event_handler = std::make_shared<steamrot::EventHandler>();
  auto asset_manager = std::make_shared<steamrot::AssetManager>();
  auto loop_num = std::make_shared<const size_t>(0);

  builder.SetWindow(window)
      .SetEventHandler(event_handler)
      .SetAssetManager(asset_manager)
      .SetLoopNumber(loop_num);

  // Build the context
  auto context_result = builder.Build();
  REQUIRE(context_result.has_value());

  // Verify the context was created with correct values
  REQUIRE(&context_result.value().game_window == window.get());
  REQUIRE(&context_result.value().event_handler == event_handler.get());
  REQUIRE(&context_result.value().asset_manager == asset_manager.get());
}

TEST_CASE("ContextDirector registers and builds contexts from configuration",
          "[integration][context][director]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::ContextDirector::ClearBuilders();

  steamrot::FlatbuffersDataLoader loader;
  auto context_data_result = loader.ProvideContextData();
  REQUIRE(context_data_result.has_value());

  steamrot::ContextConfigurator configurator(context_data_result.value());

  // Create builder from configuration for TITLE scene
  auto builder_result = configurator.CreateLogicContextBuilder(
      steamrot::SceneType::SceneType_TITLE);
  REQUIRE(builder_result.has_value());

  // Register builder with ContextDirector
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::SceneType_TITLE, builder_result.value());

  // Verify builder is registered
  REQUIRE(steamrot::ContextDirector::HasBuilder(
      steamrot::SceneType::SceneType_TITLE));

  // Get builder from director
  auto retrieved_builder =
      steamrot::ContextDirector::GetLogicContextBuilder(
          steamrot::SceneType::SceneType_TITLE);
  REQUIRE(retrieved_builder.has_value());
}

TEST_CASE("ContextDirector can manage multiple scene contexts",
          "[integration][context][director]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::ContextDirector::ClearBuilders();

  steamrot::FlatbuffersDataLoader loader;
  auto context_data_result = loader.ProvideContextData();
  REQUIRE(context_data_result.has_value());

  steamrot::ContextConfigurator configurator(context_data_result.value());

  // Register builders for all configured scenes
  const auto *context_data = context_data_result.value();
  for (const auto *scene_config : *context_data->scene_contexts()) {
    auto builder_result =
        configurator.CreateLogicContextBuilder(scene_config->scene_type());
    REQUIRE(builder_result.has_value());

    steamrot::ContextDirector::RegisterLogicContextBuilder(
        scene_config->scene_type(), builder_result.value());
  }

  // Verify all scenes are registered
  REQUIRE(steamrot::ContextDirector::HasBuilder(
      steamrot::SceneType::SceneType_TEST));
  REQUIRE(steamrot::ContextDirector::HasBuilder(
      steamrot::SceneType::SceneType_TITLE));

  // Verify can retrieve builders for each scene
  auto test_builder = steamrot::ContextDirector::GetLogicContextBuilder(
      steamrot::SceneType::SceneType_TEST);
  REQUIRE(test_builder.has_value());

  auto title_builder = steamrot::ContextDirector::GetLogicContextBuilder(
      steamrot::SceneType::SceneType_TITLE);
  REQUIRE(title_builder.has_value());
}

