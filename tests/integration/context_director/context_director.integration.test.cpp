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
#include "TestContextDirector.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ContextDirector full workflow: register -> build",
          "[integration][ContextDirector]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::ContextDirector::ClearBuilders();

  // Get a fully configured builder from TestContextDirector
  auto builder = steamrot::tests::TestContextDirector::GetLogicContextBuilder();

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
  steamrot::tests::TestContextDirector::Reset();
}

////////////////////////////////////////////////////////////
TEST_CASE("ContextDirector manages multiple scene types",
          "[integration][ContextDirector]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::ContextDirector::ClearBuilders();

  // Register builders for multiple scene types
  const std::vector<steamrot::SceneType> scene_types = {
      steamrot::SceneType::SceneType_TEST,
      steamrot::SceneType::SceneType_TITLE};

  for (const auto scene_type : scene_types) {
    // Get builder from TestContextDirector
    auto builder = steamrot::tests::TestContextDirector::GetLogicContextBuilder();

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
  steamrot::tests::TestContextDirector::Reset();
}

////////////////////////////////////////////////////////////
TEST_CASE("ContextDirector lifecycle: register -> use -> clear -> re-register",
          "[integration][ContextDirector]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::ContextDirector::ClearBuilders();

  // Create and configure first builder
  auto builder1 = steamrot::tests::TestContextDirector::GetLogicContextBuilder();

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
  auto builder2 = steamrot::tests::TestContextDirector::GetLogicContextBuilder();

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
  steamrot::tests::TestContextDirector::Reset();
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

  // For each scene in configuration, create and register builder
  const auto *context_data = context_data_result.value();
  REQUIRE(context_data->scene_contexts() != nullptr);

  for (const auto *scene_config : *context_data->scene_contexts()) {
    auto scene_type = scene_config->scene_type();

    // Get builder from TestContextDirector
    auto builder = steamrot::tests::TestContextDirector::GetLogicContextBuilder();

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
  steamrot::tests::TestContextDirector::Reset();
}
