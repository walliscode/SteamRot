/////////////////////////////////////////////////
/// @file
/// @brief Integration tests for ContextDirector lifecycle management
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ContextConfigurator.h"
#include "ContextDirector.h"
#include "FlatbuffersDataLoader.h"
#include "LogicContextBuilder.h"
#include "PathProvider.h"
#include "TestContext.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ContextDirector full workflow: load config -> register -> build",
          "[integration][ContextDirector]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::ContextDirector::ClearBuilders();

  // Load context configuration
  steamrot::FlatbuffersDataLoader loader;
  auto context_data_result = loader.ProvideContextData();
  REQUIRE(context_data_result.has_value());

  // Create configurator
  steamrot::ContextConfigurator configurator(context_data_result.value());

  // Get builder from configurator for TEST scene
  auto builder_result = configurator.CreateLogicContextBuilder(
      steamrot::SceneType::SceneType_TEST);
  REQUIRE(builder_result.has_value());

  auto builder = builder_result.value();

  // Create test context to get runtime objects
  steamrot::tests::TestContext test_context;
  auto logic_context = test_context.GetLogicContextForTestScene();

  // Configure builder with runtime objects
  builder
      .SetSceneEntities(std::make_shared<steamrot::EntityMemoryPool>(
          logic_context.scene_entities))
      .SetArchetypes(
          std::make_shared<const std::unordered_map<steamrot::ArchetypeID,
                                                     steamrot::Archetype>>(
              logic_context.archetypes))
      .SetSceneTexture(
          std::make_shared<sf::RenderTexture>(logic_context.scene_texture))
      .SetGameWindow(
          std::make_shared<sf::RenderWindow>(logic_context.game_window))
      .SetAssetManager(
          std::make_shared<const steamrot::AssetManager>(
              logic_context.asset_manager))
      .SetEventHandler(
          std::make_shared<steamrot::EventHandler>(logic_context.event_handler))
      .SetMousePosition(std::make_shared<const sf::Vector2i>(
          logic_context.mouse_position));

  // Register builder with ContextDirector
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::SceneType_TEST, builder);

  // Verify registration
  REQUIRE(steamrot::ContextDirector::HasBuilder(
      steamrot::SceneType::SceneType_TEST));

  // Build context from director
  auto context_result =
      steamrot::ContextDirector::BuildLogicContext(steamrot::SceneType::SceneType_TEST);
  REQUIRE(context_result.has_value());

  // Clean up
  steamrot::ContextDirector::ClearBuilders();
}

////////////////////////////////////////////////////////////
TEST_CASE("ContextDirector manages multiple scene types from configuration",
          "[integration][ContextDirector]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::ContextDirector::ClearBuilders();

  // Load context configuration
  steamrot::FlatbuffersDataLoader loader;
  auto context_data_result = loader.ProvideContextData();
  REQUIRE(context_data_result.has_value());

  steamrot::ContextConfigurator configurator(context_data_result.value());

  // Create test context
  steamrot::tests::TestContext test_context;

  // Register builders for multiple scene types
  const std::vector<steamrot::SceneType> scene_types = {
      steamrot::SceneType::SceneType_TEST,
      steamrot::SceneType::SceneType_TITLE};

  for (const auto scene_type : scene_types) {
    // Get builder from configurator
    auto builder_result = configurator.CreateLogicContextBuilder(scene_type);
    REQUIRE(builder_result.has_value());

    auto builder = builder_result.value();

    // Get appropriate logic context
    steamrot::LogicContext logic_context;
    if (scene_type == steamrot::SceneType::SceneType_TEST) {
      logic_context = test_context.GetLogicContextForTestScene();
    } else if (scene_type == steamrot::SceneType::SceneType_TITLE) {
      logic_context = test_context.GetLogicContextForTitleScene();
    }

    // Configure builder with runtime objects
    builder
        .SetSceneEntities(std::make_shared<steamrot::EntityMemoryPool>(
            logic_context.scene_entities))
        .SetArchetypes(
            std::make_shared<const std::unordered_map<steamrot::ArchetypeID,
                                                       steamrot::Archetype>>(
                logic_context.archetypes))
        .SetSceneTexture(
            std::make_shared<sf::RenderTexture>(logic_context.scene_texture))
        .SetGameWindow(
            std::make_shared<sf::RenderWindow>(logic_context.game_window))
        .SetAssetManager(
            std::make_shared<const steamrot::AssetManager>(
                logic_context.asset_manager))
        .SetEventHandler(std::make_shared<steamrot::EventHandler>(
            logic_context.event_handler))
        .SetMousePosition(std::make_shared<const sf::Vector2i>(
            logic_context.mouse_position));

    // Register with director
    steamrot::ContextDirector::RegisterLogicContextBuilder(scene_type, builder);
  }

  // Verify all scene types are registered
  for (const auto scene_type : scene_types) {
    REQUIRE(steamrot::ContextDirector::HasBuilder(scene_type));
  }

  // Build contexts for all scene types
  for (const auto scene_type : scene_types) {
    auto context_result =
        steamrot::ContextDirector::BuildLogicContext(scene_type);
    REQUIRE(context_result.has_value());
  }

  // Clean up
  steamrot::ContextDirector::ClearBuilders();
}

////////////////////////////////////////////////////////////
TEST_CASE("ContextDirector lifecycle: register -> use -> clear -> re-register",
          "[integration][ContextDirector]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::ContextDirector::ClearBuilders();

  steamrot::tests::TestContext test_context;
  auto logic_context = test_context.GetLogicContextForTestScene();

  // Create and configure first builder
  steamrot::LogicContextBuilder builder1;
  builder1
      .SetSceneEntities(std::make_shared<steamrot::EntityMemoryPool>(
          logic_context.scene_entities))
      .SetArchetypes(
          std::make_shared<const std::unordered_map<steamrot::ArchetypeID,
                                                     steamrot::Archetype>>(
              logic_context.archetypes))
      .SetSceneTexture(
          std::make_shared<sf::RenderTexture>(logic_context.scene_texture))
      .SetGameWindow(
          std::make_shared<sf::RenderWindow>(logic_context.game_window))
      .SetAssetManager(
          std::make_shared<const steamrot::AssetManager>(
              logic_context.asset_manager))
      .SetEventHandler(
          std::make_shared<steamrot::EventHandler>(logic_context.event_handler))
      .SetMousePosition(std::make_shared<const sf::Vector2i>(
          logic_context.mouse_position));

  // Register first builder
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::SceneType_TEST, builder1);
  REQUIRE(steamrot::ContextDirector::HasBuilder(
      steamrot::SceneType::SceneType_TEST));

  // Use the builder
  auto context1_result =
      steamrot::ContextDirector::BuildLogicContext(steamrot::SceneType::SceneType_TEST);
  REQUIRE(context1_result.has_value());

  // Clear all builders
  steamrot::ContextDirector::ClearBuilders();
  REQUIRE_FALSE(steamrot::ContextDirector::HasBuilder(
      steamrot::SceneType::SceneType_TEST));

  // Re-register with new builder
  steamrot::LogicContextBuilder builder2;
  builder2
      .SetSceneEntities(std::make_shared<steamrot::EntityMemoryPool>(
          logic_context.scene_entities))
      .SetArchetypes(
          std::make_shared<const std::unordered_map<steamrot::ArchetypeID,
                                                     steamrot::Archetype>>(
              logic_context.archetypes))
      .SetSceneTexture(
          std::make_shared<sf::RenderTexture>(logic_context.scene_texture))
      .SetGameWindow(
          std::make_shared<sf::RenderWindow>(logic_context.game_window))
      .SetAssetManager(
          std::make_shared<const steamrot::AssetManager>(
              logic_context.asset_manager))
      .SetEventHandler(
          std::make_shared<steamrot::EventHandler>(logic_context.event_handler))
      .SetMousePosition(std::make_shared<const sf::Vector2i>(
          logic_context.mouse_position));

  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::SceneType_TEST, builder2);
  REQUIRE(steamrot::ContextDirector::HasBuilder(
      steamrot::SceneType::SceneType_TEST));

  // Use the new builder
  auto context2_result =
      steamrot::ContextDirector::BuildLogicContext(steamrot::SceneType::SceneType_TEST);
  REQUIRE(context2_result.has_value());

  // Clean up
  steamrot::ContextDirector::ClearBuilders();
}

////////////////////////////////////////////////////////////
TEST_CASE("ContextDirector with ContextConfigurator integration",
          "[integration][ContextDirector][ContextConfigurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::ContextDirector::ClearBuilders();

  // Load configuration
  steamrot::FlatbuffersDataLoader loader;
  auto context_data_result = loader.ProvideContextData();
  REQUIRE(context_data_result.has_value());

  steamrot::ContextConfigurator configurator(context_data_result.value());

  // Get all scene types from configuration
  const auto *context_data = context_data_result.value();
  REQUIRE(context_data->scene_contexts() != nullptr);

  steamrot::tests::TestContext test_context;

  // For each scene in configuration, create and register builder
  for (const auto *scene_config : *context_data->scene_contexts()) {
    auto scene_type = scene_config->scene_type();

    // Get builder from configurator
    auto builder_result = configurator.CreateLogicContextBuilder(scene_type);
    REQUIRE(builder_result.has_value());

    auto builder = builder_result.value();

    // Get appropriate logic context
    steamrot::LogicContext logic_context;
    if (scene_type == steamrot::SceneType::SceneType_TEST) {
      logic_context = test_context.GetLogicContextForTestScene();
    } else if (scene_type == steamrot::SceneType::SceneType_TITLE) {
      logic_context = test_context.GetLogicContextForTitleScene();
    } else {
      continue; // Skip unsupported scene types in test
    }

    // Configure builder
    builder
        .SetSceneEntities(std::make_shared<steamrot::EntityMemoryPool>(
            logic_context.scene_entities))
        .SetArchetypes(
            std::make_shared<const std::unordered_map<steamrot::ArchetypeID,
                                                       steamrot::Archetype>>(
                logic_context.archetypes))
        .SetSceneTexture(
            std::make_shared<sf::RenderTexture>(logic_context.scene_texture))
        .SetGameWindow(
            std::make_shared<sf::RenderWindow>(logic_context.game_window))
        .SetAssetManager(
            std::make_shared<const steamrot::AssetManager>(
                logic_context.asset_manager))
        .SetEventHandler(std::make_shared<steamrot::EventHandler>(
            logic_context.event_handler))
        .SetMousePosition(std::make_shared<const sf::Vector2i>(
            logic_context.mouse_position));

    // Register with director
    steamrot::ContextDirector::RegisterLogicContextBuilder(scene_type, builder);

    // Verify registration
    REQUIRE(steamrot::ContextDirector::HasBuilder(scene_type));

    // Verify we can build
    auto context_result =
        steamrot::ContextDirector::BuildLogicContext(scene_type);
    REQUIRE(context_result.has_value());
  }

  // Clean up
  steamrot::ContextDirector::ClearBuilders();
}
