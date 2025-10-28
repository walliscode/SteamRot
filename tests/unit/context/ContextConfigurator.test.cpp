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
  steamrot::ContextConfigurator configurator(nullptr);
  steamrot::GameResources resources;

  auto result = configurator.ConfigureGameResources(resources);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
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

TEST_CASE("ContextConfigurator fails with missing game context config",
          "[unit][context][ContextConfigurator]") {
  // Create a minimal ContextData without game_context
  flatbuffers::FlatBufferBuilder fbb;
  auto context_data = steamrot::CreateContextData(fbb);
  fbb.Finish(context_data);

  const steamrot::ContextData *data =
      steamrot::GetContextData(fbb.GetBufferPointer());

  steamrot::ContextConfigurator configurator(data);
  steamrot::GameResources resources;
  auto result = configurator.ConfigureGameResources(resources);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingRequiredField);
}

TEST_CASE("ContextConfigurator parses environment type correctly",
          "[unit][context][ContextConfigurator]") {
  flatbuffers::FlatBufferBuilder fbb;

  // Test "Test" environment type
  auto env_type_test = fbb.CreateString("Test");
  auto window_title = fbb.CreateString("Test Window");
  auto game_config = steamrot::CreateGameContextConfig(
      fbb, 800, 600, window_title, 60, env_type_test);
  auto context_data = steamrot::CreateContextData(fbb, game_config);
  fbb.Finish(context_data);

  const steamrot::ContextData *data =
      steamrot::GetContextData(fbb.GetBufferPointer());

  steamrot::ContextConfigurator configurator(data);
  steamrot::GameResources resources;
  auto result = configurator.ConfigureGameResources(resources);
  REQUIRE(result.has_value());
  REQUIRE(resources.env_type == steamrot::EnvironmentType::Test);
}

TEST_CASE("ContextConfigurator fails with invalid environment type",
          "[unit][context][ContextConfigurator]") {
  flatbuffers::FlatBufferBuilder fbb;

  // Test invalid environment type
  auto env_type_invalid = fbb.CreateString("InvalidType");
  auto window_title = fbb.CreateString("Test Window");
  auto game_config = steamrot::CreateGameContextConfig(
      fbb, 800, 600, window_title, 60, env_type_invalid);
  auto context_data = steamrot::CreateContextData(fbb, game_config);
  fbb.Finish(context_data);

  const steamrot::ContextData *data =
      steamrot::GetContextData(fbb.GetBufferPointer());

  steamrot::ContextConfigurator configurator(data);
  steamrot::GameResources resources;
  auto result = configurator.ConfigureGameResources(resources);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NonExistentEnumValue);
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

TEST_CASE("ContextConfigurator fails for non-existent scene type",
          "[unit][context][ContextConfigurator]") {
  flatbuffers::FlatBufferBuilder fbb;

  auto env_type = fbb.CreateString("Test");
  auto window_title = fbb.CreateString("Test");
  auto game_config = steamrot::CreateGameContextConfig(
      fbb, 800, 600, window_title, 60, env_type);
  
  // Create scene context for TEST only
  auto test_scene = steamrot::CreateSceneContextConfig(
      fbb, steamrot::SceneType::SceneType_TEST, 100, 640, 480);
  std::vector<flatbuffers::Offset<steamrot::SceneContextConfig>> scenes;
  scenes.push_back(test_scene);
  auto scene_contexts = fbb.CreateVector(scenes);
  
  auto context_data = steamrot::CreateContextData(fbb, game_config, scene_contexts);
  fbb.Finish(context_data);

  const steamrot::ContextData *data =
      steamrot::GetContextData(fbb.GetBufferPointer());

  steamrot::ContextConfigurator configurator(data);
  steamrot::SceneResources resources;
  
  // Try to configure CRAFTING scene which doesn't exist in config
  auto result = configurator.ConfigureSceneResources(
      resources, steamrot::SceneType::SceneType_CRAFTING);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::SceneTypeNotFound);
}

TEST_CASE("ContextConfigurator fails with missing scene contexts",
          "[unit][context][ContextConfigurator]") {
  flatbuffers::FlatBufferBuilder fbb;

  auto env_type = fbb.CreateString("Test");
  auto window_title = fbb.CreateString("Test");
  auto game_config = steamrot::CreateGameContextConfig(
      fbb, 800, 600, window_title, 60, env_type);
  // No scene_contexts provided
  auto context_data = steamrot::CreateContextData(fbb, game_config);
  fbb.Finish(context_data);

  const steamrot::ContextData *data =
      steamrot::GetContextData(fbb.GetBufferPointer());

  steamrot::ContextConfigurator configurator(data);
  steamrot::SceneResources resources;
  auto result = configurator.ConfigureSceneResources(
      resources, steamrot::SceneType::SceneType_TEST);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::MissingRequiredField);
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
