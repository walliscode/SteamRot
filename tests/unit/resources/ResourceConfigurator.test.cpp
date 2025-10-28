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
  steamrot::ResourceConfigurator configurator(nullptr, nullptr);
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

  auto scene_resources_result = loader.ProvideSceneResourcesData();
  REQUIRE(scene_resources_result.has_value());

  steamrot::ResourceConfigurator configurator(game_resources_result.value(),
                                             scene_resources_result.value());
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

  auto scene_resources_result = loader.ProvideSceneResourcesData();
  REQUIRE(scene_resources_result.has_value());

  steamrot::ResourceConfigurator configurator(game_resources_result.value(),
                                             scene_resources_result.value());
  steamrot::GameResources resources;
  auto config_result = configurator.ConfigureGameResources(resources);
  REQUIRE(config_result.has_value());

  // Verify environment type was set
  REQUIRE(resources.env_type == steamrot::EnvironmentType::Test);

  // Verify window was created
  REQUIRE(resources.game_window.isOpen());
}

TEST_CASE("ResourceConfigurator configures SceneResources for existing scene",
          "[unit][resources][ResourceConfigurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;

  auto game_resources_result = loader.ProvideGameResourcesData();
  REQUIRE(game_resources_result.has_value());

  auto scene_resources_result = loader.ProvideSceneResourcesData();
  REQUIRE(scene_resources_result.has_value());

  steamrot::ResourceConfigurator configurator(game_resources_result.value(),
                                             scene_resources_result.value());
  steamrot::SceneResources resources;
  auto config_result = configurator.ConfigureSceneResources(
      resources, steamrot::SceneType::SceneType_TEST);
  REQUIRE(config_result.has_value());

  // Verify render texture was created
  REQUIRE(resources.scene_texture.getSize().x > 0);
  REQUIRE(resources.scene_texture.getSize().y > 0);
}

TEST_CASE("ResourceConfigurator fails for non-existent scene type",
          "[unit][resources][ResourceConfigurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  
  // Create minimal scene resources with only TEST scene
  flatbuffers::FlatBufferBuilder fbb;
  auto test_scene = steamrot::CreateSceneResourcesData(
      fbb, steamrot::SceneType::SceneType_TEST, 640, 480);
  std::vector<flatbuffers::Offset<steamrot::SceneResourcesData>> scenes;
  scenes.push_back(test_scene);
  auto scene_vector = fbb.CreateVector(scenes);
  auto scene_collection = steamrot::CreateSceneResourcesCollection(fbb, scene_vector);
  fbb.Finish(scene_collection);

  const steamrot::SceneResourcesCollection *scene_data =
      steamrot::GetSceneResourcesCollection(fbb.GetBufferPointer());

  // Create minimal game resources
  flatbuffers::FlatBufferBuilder fbb2;
  auto window_title = fbb2.CreateString("Test");
  auto env_type = fbb2.CreateString("Test");
  auto game_data = steamrot::CreateGameResourcesData(
      fbb2, 800, 600, window_title, 60, env_type);
  fbb2.Finish(game_data);
  
  const steamrot::GameResourcesData *game_resources =
      steamrot::GetGameResourcesData(fbb2.GetBufferPointer());

  steamrot::ResourceConfigurator configurator(game_resources, scene_data);
  steamrot::SceneResources resources;

  // Try to configure CRAFTING scene which doesn't exist in config
  auto result = configurator.ConfigureSceneResources(
      resources, steamrot::SceneType::SceneType_CRAFTING);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::SceneTypeNotFound);
}

TEST_CASE("ResourceConfigurator fails with null scene data",
          "[unit][resources][ResourceConfigurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;

  auto game_resources_result = loader.ProvideGameResourcesData();
  REQUIRE(game_resources_result.has_value());

  steamrot::ResourceConfigurator configurator(game_resources_result.value(), nullptr);
  steamrot::SceneResources resources;
  auto result = configurator.ConfigureSceneResources(
      resources, steamrot::SceneType::SceneType_TEST);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
}

TEST_CASE("ResourceConfigurator configures resources for multiple scene types",
          "[unit][resources][ResourceConfigurator]") {
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::FlatbuffersDataLoader loader;

  auto game_resources_result = loader.ProvideGameResourcesData();
  REQUIRE(game_resources_result.has_value());

  auto scene_resources_result = loader.ProvideSceneResourcesData();
  REQUIRE(scene_resources_result.has_value());

  steamrot::ResourceConfigurator configurator(game_resources_result.value(),
                                             scene_resources_result.value());

  // Test multiple scene types
  steamrot::SceneResources test_resources;
  auto test_result = configurator.ConfigureSceneResources(
      test_resources, steamrot::SceneType::SceneType_TEST);
  REQUIRE(test_result.has_value());

  steamrot::SceneResources title_resources;
  auto title_result = configurator.ConfigureSceneResources(
      title_resources, steamrot::SceneType::SceneType_TITLE);
  REQUIRE(title_result.has_value());
}
