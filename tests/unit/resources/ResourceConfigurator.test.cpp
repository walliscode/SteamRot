/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for ResourceConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ResourceConfigurator.h"
#include "FlatbuffersDataLoader.h"
#include "GameResources.h"
#include "PathProvider.h"
#include "SceneResources.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ResourceConfigurator constructor with null game data",
          "[unit][resources][ResourceConfigurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::ResourceConfigurator configurator(nullptr);
  steamrot::GameResources resources;

  auto result = configurator.ConfigureGameResources(resources);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("ResourceConfigurator loads game resources data successfully",
          "[unit][resources][ResourceConfigurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;

  auto game_resources_result = loader.ProvideGameResourcesData();
  REQUIRE(game_resources_result.has_value());

  steamrot::ResourceConfigurator configurator(game_resources_result.value());
  steamrot::GameResources resources;
  auto config_result = configurator.ConfigureGameResources(resources);
  REQUIRE(config_result.has_value());
}

TEST_CASE("ResourceConfigurator configures GameResources with environment type",
          "[unit][resources][ResourceConfigurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;

  auto game_resources_result = loader.ProvideGameResourcesData();
  REQUIRE(game_resources_result.has_value());

  steamrot::ResourceConfigurator configurator(game_resources_result.value());
  steamrot::GameResources resources;
  auto config_result = configurator.ConfigureGameResources(resources);
  REQUIRE(config_result.has_value());

  // Verify environment type was set
  REQUIRE(resources.env_type == steamrot::EnvironmentType::Test);

  // Verify window was created
  REQUIRE(resources.game_window.isOpen());
}

TEST_CASE("ResourceConfigurator configures SceneResources from scene data",
          "[unit][resources][ResourceConfigurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;

  auto game_resources_result = loader.ProvideGameResourcesData();
  REQUIRE(game_resources_result.has_value());

  auto scene_resources_result = loader.ProvideSceneResourcesData(
      steamrot::SceneType::SceneType_TEST);
  REQUIRE(scene_resources_result.has_value());

  steamrot::ResourceConfigurator configurator(game_resources_result.value());
  steamrot::SceneResources resources;
  auto config_result = configurator.ConfigureSceneResources(
      resources, scene_resources_result.value());
  REQUIRE(config_result.has_value());

  // Verify render texture was created
  REQUIRE(resources.scene_texture.getSize().x > 0);
  REQUIRE(resources.scene_texture.getSize().y > 0);
}

TEST_CASE("ResourceConfigurator uses defaults when scene data is null",
          "[unit][resources][ResourceConfigurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;

  auto game_resources_result = loader.ProvideGameResourcesData();
  REQUIRE(game_resources_result.has_value());

  steamrot::ResourceConfigurator configurator(game_resources_result.value());
  steamrot::SceneResources resources;
  
  // Configure with null scene data - should use defaults
  auto result = configurator.ConfigureSceneResources(resources, nullptr);
  REQUIRE(result.has_value());
  
  // Verify default dimensions were used (800x600)
  REQUIRE(resources.scene_texture.getSize().x == 800);
  REQUIRE(resources.scene_texture.getSize().y == 600);
}

TEST_CASE("ResourceConfigurator configures resources for multiple scene types",
          "[unit][resources][ResourceConfigurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;

  auto game_resources_result = loader.ProvideGameResourcesData();
  REQUIRE(game_resources_result.has_value());

  steamrot::ResourceConfigurator configurator(game_resources_result.value());

  // Test multiple scene types
  auto test_scene_data = loader.ProvideSceneResourcesData(
      steamrot::SceneType::SceneType_TEST);
  REQUIRE(test_scene_data.has_value());
  
  steamrot::SceneResources test_resources;
  auto test_result = configurator.ConfigureSceneResources(
      test_resources, test_scene_data.value());
  REQUIRE(test_result.has_value());

  auto title_scene_data = loader.ProvideSceneResourcesData(
      steamrot::SceneType::SceneType_TITLE);
  REQUIRE(title_scene_data.has_value());
  
  steamrot::SceneResources title_resources;
  auto title_result = configurator.ConfigureSceneResources(
      title_resources, title_scene_data.value());
  REQUIRE(title_result.has_value());
}

TEST_CASE("ResourceConfigurator with custom scene resource data",
          "[unit][resources][ResourceConfigurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  
  // Create custom scene resources
  flatbuffers::FlatBufferBuilder fbb;
  auto scene_data = steamrot::CreateSceneResourcesData(fbb, 1024, 768);
  fbb.Finish(scene_data);
  
  const steamrot::SceneResourcesData *custom_scene_data =
      flatbuffers::GetRoot<steamrot::SceneResourcesData>(fbb.GetBufferPointer());

  // Create minimal game resources
  flatbuffers::FlatBufferBuilder fbb2;
  auto window_title = fbb2.CreateString("Test");
  auto env_type = fbb2.CreateString("Test");
  auto game_data = steamrot::CreateGameResourcesData(
      fbb2, 800, 600, window_title, 60, env_type);
  fbb2.Finish(game_data);
  
  const steamrot::GameResourcesData *game_resources =
      flatbuffers::GetRoot<steamrot::GameResourcesData>(fbb2.GetBufferPointer());

  steamrot::ResourceConfigurator configurator(game_resources);
  steamrot::SceneResources resources;

  auto result = configurator.ConfigureSceneResources(resources, custom_scene_data);
  REQUIRE(result.has_value());
  REQUIRE(resources.scene_texture.getSize().x == 1024);
  REQUIRE(resources.scene_texture.getSize().y == 768);
}

