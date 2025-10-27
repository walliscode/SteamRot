/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the ContextDirector class
/////////////////////////////////////////////////

#include "ContextDirector.h"
#include "LogicContextBuilder.h"
#include "PathProvider.h"
#include "TestContext.h"
#include "TestContextDirector.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ContextDirector::RegisterLogicContextBuilder registers a builder",
          "[unit][ContextDirector]") {
  // Clear any existing builders
  steamrot::ContextDirector::ClearBuilders();

  // Create a builder
  steamrot::LogicContextBuilder builder;

  // Register it
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::SceneType_TITLE, builder);

  // Verify it was registered
  REQUIRE(steamrot::ContextDirector::HasBuilder(
      steamrot::SceneType::SceneType_TITLE));

  // Clean up
  steamrot::ContextDirector::ClearBuilders();
}

////////////////////////////////////////////////////////////
TEST_CASE("ContextDirector::HasBuilder returns false for unregistered scene",
          "[unit][ContextDirector]") {
  // Clear any existing builders
  steamrot::ContextDirector::ClearBuilders();

  // Check for a scene that hasn't been registered
  REQUIRE_FALSE(steamrot::ContextDirector::HasBuilder(
      steamrot::SceneType::SceneType_CRAFTING));
}

////////////////////////////////////////////////////////////
TEST_CASE("ContextDirector::GetLogicContextBuilder returns registered builder",
          "[unit][ContextDirector]") {
  // Clear any existing builders
  steamrot::ContextDirector::ClearBuilders();

  // Create and register a builder
  steamrot::LogicContextBuilder builder;
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::SceneType_TEST, builder);

  // Get the builder
  auto result = steamrot::ContextDirector::GetLogicContextBuilder(
      steamrot::SceneType::SceneType_TEST);

  REQUIRE(result.has_value());

  // Clean up
  steamrot::ContextDirector::ClearBuilders();
}

////////////////////////////////////////////////////////////
TEST_CASE(
    "ContextDirector::GetLogicContextBuilder returns error for missing builder",
    "[unit][ContextDirector]") {
  // Clear any existing builders
  steamrot::ContextDirector::ClearBuilders();

  // Try to get a builder that doesn't exist
  auto result = steamrot::ContextDirector::GetLogicContextBuilder(
      steamrot::SceneType::SceneType_CRAFTING);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NotFound);
}

////////////////////////////////////////////////////////////
TEST_CASE("ContextDirector::ClearBuilders removes all registered builders",
          "[unit][ContextDirector]") {
  // Clear any existing builders
  steamrot::ContextDirector::ClearBuilders();

  // Register multiple builders
  steamrot::LogicContextBuilder builder1;
  steamrot::LogicContextBuilder builder2;

  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::SceneType_TITLE, builder1);
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::SceneType_CRAFTING, builder2);

  // Verify they're registered
  REQUIRE(steamrot::ContextDirector::HasBuilder(
      steamrot::SceneType::SceneType_TITLE));
  REQUIRE(steamrot::ContextDirector::HasBuilder(
      steamrot::SceneType::SceneType_CRAFTING));

  // Clear all builders
  steamrot::ContextDirector::ClearBuilders();

  // Verify they're gone
  REQUIRE_FALSE(steamrot::ContextDirector::HasBuilder(
      steamrot::SceneType::SceneType_TITLE));
  REQUIRE_FALSE(steamrot::ContextDirector::HasBuilder(
      steamrot::SceneType::SceneType_CRAFTING));
}

////////////////////////////////////////////////////////////
TEST_CASE("ContextDirector::BuildLogicContext builds context from registered "
          "builder",
          "[unit][ContextDirector]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};

  // Clear any existing builders
  steamrot::ContextDirector::ClearBuilders();

  // Get a fully configured builder from TestContextDirector
  auto builder = steamrot::tests::TestContextDirector::GetLogicContextBuilder();

  // Register the builder
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::SceneType_TEST, builder);

  // Build the context
  auto result = steamrot::ContextDirector::BuildLogicContext(
      steamrot::SceneType::SceneType_TEST);

  REQUIRE(result.has_value());

  // Clean up
  steamrot::ContextDirector::ClearBuilders();
  steamrot::tests::TestContextDirector::Reset();
}

////////////////////////////////////////////////////////////
TEST_CASE("ContextDirector::BuildLogicContext returns error for missing builder",
          "[unit][ContextDirector]") {
  // Clear any existing builders
  steamrot::ContextDirector::ClearBuilders();

  // Try to build a context for an unregistered scene
  auto result = steamrot::ContextDirector::BuildLogicContext(
      steamrot::SceneType::SceneType_TITLE);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NotFound);
}

////////////////////////////////////////////////////////////
TEST_CASE("ContextDirector can register multiple scene types",
          "[unit][ContextDirector]") {
  // Clear any existing builders
  steamrot::ContextDirector::ClearBuilders();

  // Create and register builders for different scenes
  steamrot::LogicContextBuilder builder1;
  steamrot::LogicContextBuilder builder2;
  steamrot::LogicContextBuilder builder3;

  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::SceneType_TEST, builder1);
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::SceneType_TITLE, builder2);
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::SceneType_CRAFTING, builder3);

  // Verify all are registered
  REQUIRE(steamrot::ContextDirector::HasBuilder(
      steamrot::SceneType::SceneType_TEST));
  REQUIRE(steamrot::ContextDirector::HasBuilder(
      steamrot::SceneType::SceneType_TITLE));
  REQUIRE(steamrot::ContextDirector::HasBuilder(
      steamrot::SceneType::SceneType_CRAFTING));

  // Clean up
  steamrot::ContextDirector::ClearBuilders();
}

////////////////////////////////////////////////////////////
TEST_CASE("ContextDirector::RegisterLogicContextBuilder overwrites existing "
          "builder",
          "[unit][ContextDirector]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};

  // Clear any existing builders
  steamrot::ContextDirector::ClearBuilders();

  // Register first builder (empty)
  steamrot::LogicContextBuilder builder1;
  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::SceneType_TEST, builder1);

  // Register second builder for the same scene type (fully configured)
  auto builder2 = steamrot::tests::TestContextDirector::GetLogicContextBuilder();

  steamrot::ContextDirector::RegisterLogicContextBuilder(
      steamrot::SceneType::SceneType_TEST, builder2);

  // Should still have one builder (the second one)
  REQUIRE(steamrot::ContextDirector::HasBuilder(
      steamrot::SceneType::SceneType_TEST));

  // Should be able to build successfully (proving it's the second builder)
  auto result = steamrot::ContextDirector::BuildLogicContext(
      steamrot::SceneType::SceneType_TEST);
  REQUIRE(result.has_value());

  // Clean up
  steamrot::ContextDirector::ClearBuilders();
  steamrot::tests::TestContextDirector::Reset();
}
