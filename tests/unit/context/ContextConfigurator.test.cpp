/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for ContextConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ContextConfigurator.h"
#include "FlatbuffersDataLoader.h"
#include "GameResources.h"
#include "PathProvider.h"
#include "SceneResources.h"
#include "context_data_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ContextConfigurator constructor with null config",
          "[unit][context][ContextConfigurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::ContextConfigurator configurator(nullptr);
  steamrot::GameResources resources;

  // ContextConfigurator now loads resource data files independently,
  // so it should succeed even with null ContextData
  auto result = configurator.ConfigureGameResources(resources);
  REQUIRE(result.has_value());
}

TEST_CASE("ContextConfigurator loads context data successfully",
          "[unit][context][ContextConfigurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;

  auto context_data_result = loader.ProvideContextData();
  REQUIRE(context_data_result.has_value());

  const steamrot::ContextData *context_data = context_data_result.value();
  REQUIRE(context_data != nullptr);

  steamrot::ContextConfigurator configurator(context_data);
  steamrot::GameResources resources;
  auto config_result = configurator.ConfigureGameResources(resources);
  REQUIRE(config_result.has_value());
}

TEST_CASE("ContextConfigurator configures GameResources with environment type",
          "[unit][context][ContextConfigurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;

  auto context_data_result = loader.ProvideContextData();
  REQUIRE(context_data_result.has_value());

  steamrot::ContextConfigurator configurator(context_data_result.value());
  steamrot::GameResources resources;
  auto config_result = configurator.ConfigureGameResources(resources);
  REQUIRE(config_result.has_value());

  // Verify environment type was set
  REQUIRE(resources.env_type == steamrot::EnvironmentType::Test);

  // Verify window was created
  REQUIRE(resources.game_window.isOpen());
}

TEST_CASE("ContextConfigurator configures SceneResources for existing scene",
          "[unit][context][ContextConfigurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;

  auto context_data_result = loader.ProvideContextData();
  REQUIRE(context_data_result.has_value());

  steamrot::ContextConfigurator configurator(context_data_result.value());
  steamrot::SceneResources resources;
  auto config_result = configurator.ConfigureSceneResources(
      resources, steamrot::SceneType::SceneType_TEST);
  REQUIRE(config_result.has_value());

  // Verify render texture was created
  REQUIRE(resources.scene_texture.getSize().x > 0);
  REQUIRE(resources.scene_texture.getSize().y > 0);
}

TEST_CASE("ContextConfigurator configures resources for multiple scene types",
          "[unit][context][ContextConfigurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;

  auto context_data_result = loader.ProvideContextData();
  REQUIRE(context_data_result.has_value());

  steamrot::ContextConfigurator configurator(context_data_result.value());

  // Test multiple scene types if they exist in test data
  steamrot::SceneResources test_resources;
  auto test_result = configurator.ConfigureSceneResources(
      test_resources, steamrot::SceneType::SceneType_TEST);
  REQUIRE(test_result.has_value());

  steamrot::SceneResources title_resources;
  auto title_result = configurator.ConfigureSceneResources(
      title_resources, steamrot::SceneType::SceneType_TITLE);
  REQUIRE(title_result.has_value());
}
