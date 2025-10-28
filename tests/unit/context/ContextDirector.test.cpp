/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for ContextDirector class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ContextDirector.h"
#include "AssetManager.h"
#include "EventHandler.h"
#include "PathProvider.h"
#include "containers.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <catch2/catch_test_macros.hpp>
#include <memory>

/////////////////////////////////////////////////
/// Helper function to create a valid LogicContextBuilder
/////////////////////////////////////////////////
steamrot::LogicContextBuilder CreateValidBuilder() {
  auto entities = std::make_shared<steamrot::EntityMemoryPool>(10);
  auto archetypes = std::make_shared<
      const std::unordered_map<steamrot::ArchetypeID, steamrot::Archetype>>();
  auto texture = std::make_shared<sf::RenderTexture>();
  auto window =
      std::make_shared<sf::RenderWindow>(sf::VideoMode(800, 600), "Test");
  auto assets = std::make_shared<const steamrot::AssetManager>(
      steamrot::PathProvider(steamrot::EnvironmentType::Test));
  auto handler = std::make_shared<steamrot::EventHandler>();
  auto mouse_pos = std::make_shared<const sf::Vector2i>(0, 0);

  steamrot::LogicContextBuilder builder;
  builder.SetSceneEntities(entities)
      .SetArchetypes(archetypes)
      .SetSceneTexture(texture)
      .SetGameWindow(window)
      .SetAssetManager(assets)
      .SetEventHandler(handler)
      .SetMousePosition(mouse_pos);

  return builder;
}

TEST_CASE("ContextDirector::ClearBuilders clears registry",
          "[unit][ContextDirector]") {
  steamrot::ContextDirector::ClearBuilders();
  REQUIRE_FALSE(
      steamrot::ContextDirector::HasBuilder(steamrot::SceneType::TEST));
}

TEST_CASE("ContextDirector::RegisterLogicContextBuilder stores builder",
          "[unit][ContextDirector]") {
  steamrot::ContextDirector::ClearBuilders();

  auto builder = CreateValidBuilder();
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::TEST, builder);

  REQUIRE(steamrot::ContextDirector::HasBuilder(steamrot::SceneType::TEST));
}

TEST_CASE("ContextDirector::HasBuilder returns false for unregistered type",
          "[unit][ContextDirector]") {
  steamrot::ContextDirector::ClearBuilders();

  REQUIRE_FALSE(
      steamrot::ContextDirector::HasBuilder(steamrot::SceneType::TITLE));
}

TEST_CASE("ContextDirector::GetLogicContextBuilder returns error when not "
          "registered",
          "[unit][ContextDirector]") {
  steamrot::ContextDirector::ClearBuilders();

  auto result = steamrot::ContextDirector::GetLogicContextBuilder(
      steamrot::SceneType::CRAFTING);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NonExistentEnumValue);
}

TEST_CASE("ContextDirector::GetLogicContextBuilder returns builder copy",
          "[unit][ContextDirector]") {
  steamrot::ContextDirector::ClearBuilders();

  auto builder = CreateValidBuilder();
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::TEST, builder);

  auto result = steamrot::ContextDirector::GetLogicContextBuilder(
      steamrot::SceneType::TEST);

  REQUIRE(result.has_value());
}

TEST_CASE("ContextDirector::BuildLogicContext builds valid context",
          "[unit][ContextDirector]") {
  steamrot::ContextDirector::ClearBuilders();

  auto builder = CreateValidBuilder();
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::TEST, builder);

  auto context_result =
      steamrot::ContextDirector::BuildLogicContext(steamrot::SceneType::TEST);

  REQUIRE(context_result.has_value());
}

TEST_CASE("ContextDirector::BuildLogicContext returns error when not "
          "registered",
          "[unit][ContextDirector]") {
  steamrot::ContextDirector::ClearBuilders();

  auto result =
      steamrot::ContextDirector::BuildLogicContext(steamrot::SceneType::TITLE);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NonExistentEnumValue);
}

TEST_CASE("ContextDirector can register multiple scene types",
          "[unit][ContextDirector]") {
  steamrot::ContextDirector::ClearBuilders();

  auto builder1 = CreateValidBuilder();
  auto builder2 = CreateValidBuilder();
  auto builder3 = CreateValidBuilder();

  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::TEST, builder1);
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::TITLE, builder2);
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::CRAFTING, builder3);

  REQUIRE(steamrot::ContextDirector::HasBuilder(steamrot::SceneType::TEST));
  REQUIRE(steamrot::ContextDirector::HasBuilder(steamrot::SceneType::TITLE));
  REQUIRE(
      steamrot::ContextDirector::HasBuilder(steamrot::SceneType::CRAFTING));
}

TEST_CASE(
    "ContextDirector::RegisterLogicContextBuilder replaces existing builder",
    "[unit][ContextDirector]") {
  steamrot::ContextDirector::ClearBuilders();

  auto builder1 = CreateValidBuilder();
  auto builder2 = CreateValidBuilder();

  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::TEST, builder1);
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::TEST, builder2);

  REQUIRE(steamrot::ContextDirector::HasBuilder(steamrot::SceneType::TEST));

  auto result = steamrot::ContextDirector::GetLogicContextBuilder(
      steamrot::SceneType::TEST);
  REQUIRE(result.has_value());
}

TEST_CASE("ContextDirector::ClearBuilders removes all registered builders",
          "[unit][ContextDirector]") {
  steamrot::ContextDirector::ClearBuilders();

  auto builder1 = CreateValidBuilder();
  auto builder2 = CreateValidBuilder();

  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::TEST, builder1);
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::TITLE, builder2);

  REQUIRE(steamrot::ContextDirector::HasBuilder(steamrot::SceneType::TEST));
  REQUIRE(steamrot::ContextDirector::HasBuilder(steamrot::SceneType::TITLE));

  steamrot::ContextDirector::ClearBuilders();

  REQUIRE_FALSE(
      steamrot::ContextDirector::HasBuilder(steamrot::SceneType::TEST));
  REQUIRE_FALSE(
      steamrot::ContextDirector::HasBuilder(steamrot::SceneType::TITLE));
}

TEST_CASE("ContextDirector builder copy is independent from registered",
          "[unit][ContextDirector]") {
  steamrot::ContextDirector::ClearBuilders();

  auto builder = CreateValidBuilder();
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::TEST, builder);

  auto builder_copy = steamrot::ContextDirector::GetLogicContextBuilder(
      steamrot::SceneType::TEST);

  REQUIRE(builder_copy.has_value());

  // Building from copy should not affect registered builder
  auto context = builder_copy.value().Build();
  REQUIRE(context.has_value());

  // Should still be able to get builder from registry
  auto builder_copy2 = steamrot::ContextDirector::GetLogicContextBuilder(
      steamrot::SceneType::TEST);
  REQUIRE(builder_copy2.has_value());
}
