////////////////////////////////////////////////
/// @file
/// @brief Implementation of TestContext object
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestContext.h"
#include "FlatbuffersConfigurator.h"
#include "GameContext.h"
#include "GameResources.h"
#include "PathProvider.h"
#include "SceneContext.h"
#include "SceneResources.h"
#include "scene_change_packet_generated.h"
#include <iostream>

namespace steamrot::tests {

/////////////////////////////////////////////////
TestContext::TestContext(const SceneType scene_type)
    : entity_manager(game_resources.event_handler) {

  // Initialize GameResources
  game_resources.game_window.create(sf::VideoMode({900, 600}),
                                    "SteamRot Test Window");
  game_resources.env_type = EnvironmentType::Test;

  // load default assets into the asset manager
  auto load_result = game_resources.asset_manager.LoadDefaultAssets();
  if (!load_result.has_value()) {
    const FailInfo &error = load_result.error();
    std::cerr << "Error loading default assets: " << error.message << std::endl;
  }

  std::cout << "Default assets loaded into AssetManager" << std::endl;
  // configure the game context
  ConfigureGameContext();
  std::cout << "GameContext configured" << std::endl;
  switch (scene_type) {
  case SceneType::SceneType_TEST:
    ConfigureSceneContextForTestScene();
    break;
  case SceneType::SceneType_TITLE:
    ConfigureSceneContextForTitleScene();
    break;
  case SceneType::SceneType_CRAFTING:
    ConfigureSceneContextForCraftingScene();
    break;
  default:
    throw std::runtime_error("Unsupported scene type for TestContext");
  }
  std::cout << "SceneContext for scene configured" << std::endl;
}

/////////////////////////////////////////////////
GameContext &TestContext::GetGameContext() const {
  if (!game_context_ptr) {
    // configure the game context if it hasn't been already

    throw std::runtime_error(
        "GameContext not configured. Call ConfigureGameContext() first.");
  }
  return *game_context_ptr;
}

/////////////////////////////////////////////////
const SceneContext &TestContext::GetSceneContextForTestScene() const {

  if (!scene_context_for_test_scene) {
    throw std::runtime_error("SceneContext for Test Scene not configured. Call "
                             "ConfigureSceneContextForTestScene() first.");
  }
  return *scene_context_for_test_scene;
}
/////////////////////////////////////////////////
const SceneContext &TestContext::GetSceneContextForTitleScene() const {
  if (!scene_context_for_title_scene) {
    throw std::runtime_error(
        "SceneContext for Title Scene not configured. Call "
        "ConfigureSceneContextForTitleScene() first.");
  }
  return *scene_context_for_title_scene;
}

/////////////////////////////////////////////////
const SceneContext &TestContext::GetSceneContextForCraftingScene() const {
  if (!scene_context_for_crafting_scene) {
    throw std::runtime_error(
        "SceneContext for Crafting Scene not configured. Call "
        "ConfigureSceneContextForCraftingScene() first.");
  }
  return *scene_context_for_crafting_scene;
}

/////////////////////////////////////////////////
void TestContext::ConfigureGameContext() {
  game_context_ptr = std::make_unique<GameContext>(game_resources);
}

/////////////////////////////////////////////////
void TestContext::ConfigureSceneContextForTestScene() {
  // Configure the EntityMemoryPool for the test scene
  FlatbuffersConfigurator configurator(game_resources.event_handler);

  std::cout << "Configuring entities for Test Scene" << std::endl;
  auto configure_result = configurator.ConfigureEntitiesFromDefaultData(
      entity_manager.GetEntityMemoryPool(), SceneType::SceneType_TEST);
  std::cout << "Entities configured for Test Scene" << std::endl;
  // check the configuration was successful
  if (!configure_result.has_value()) {
    // handle the error (for example, log it)
    const FailInfo &error = configure_result.error();
    // For testing purposes, we can print the error message
    std::cerr << "Error configuring entities: " << error.message << std::endl;
  }
  // generate all archetypes for the test scene
  auto archetype_result = entity_manager.GenerateAllArchetypes();
  if (!archetype_result.has_value()) {
    const FailInfo &error = archetype_result.error();
    std::cerr << "Error generating archetypes: " << error.message << std::endl;
  }
  // create pointer to the scene context for the test scene
  scene_context_for_test_scene = std::make_unique<SceneContext>(
      scene_resources, game_resources, entity_manager);
}

/////////////////////////////////////////////////
void TestContext::ConfigureSceneContextForTitleScene() {
  // Configure the EntityMemoryPool for the title scene
  FlatbuffersConfigurator configurator(game_resources.event_handler);
  auto configure_result = configurator.ConfigureEntitiesFromDefaultData(
      entity_manager.GetEntityMemoryPool(), SceneType::SceneType_TITLE);
  // check the configuration was successful
  if (!configure_result.has_value()) {
    // handle the error (for example, log it)
    const FailInfo &error = configure_result.error();
    // For testing purposes, we can print the error message
    std::cerr << "Error configuring entities: " << error.message << std::endl;
  }
  // generate all archetypes for the title scene
  auto archetype_result = entity_manager.GenerateAllArchetypes();
  if (!archetype_result.has_value()) {
    const FailInfo &error = archetype_result.error();
    std::cerr << "Error generating archetypes: " << error.message << std::endl;
  }
  // create pointer to the scene context for the title scene
  scene_context_for_title_scene = std::make_unique<SceneContext>(
      scene_resources, game_resources, entity_manager);
}

/////////////////////////////////////////////////
void TestContext::ConfigureSceneContextForCraftingScene() {
  // Configure the EntityMemoryPool for the crafting scene
  FlatbuffersConfigurator configurator(game_resources.event_handler);
  auto configure_result = configurator.ConfigureEntitiesFromDefaultData(
      entity_manager.GetEntityMemoryPool(), SceneType::SceneType_CRAFTING);
  // check the configuration was successful
  if (!configure_result.has_value()) {
    // handle the error (for example, log it)
    const FailInfo &error = configure_result.error();
    // For testing purposes, we can print the error message
    std::cerr << "Error configuring entities: " << error.message << std::endl;
  }
  // generate all archetypes for the crafting scene
  auto archetype_result = entity_manager.GenerateAllArchetypes();
  if (!archetype_result.has_value()) {
    const FailInfo &error = archetype_result.error();
    std::cerr << "Error generating archetypes: " << error.message << std::endl;
  }
  // create pointer to the scene context for the crafting scene
  scene_context_for_crafting_scene = std::make_unique<SceneContext>(
      scene_resources, game_resources, entity_manager);
}
} // namespace steamrot::tests
