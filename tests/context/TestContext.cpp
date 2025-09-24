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
#include "PathProvider.h"
#include "scene_types_generated.h"
#include <iostream>

namespace steamrot::tests {

/////////////////////////////////////////////////
TestContext::TestContext(const SceneType scene_type)
    : render_window(sf::VideoMode({900, 600}), "SteamRot Test Window"),
      event_handler(), archetype_manager(scene_entities) {

  // load default assets into the asset manager
  auto load_result = asset_manager.LoadDefaultAssets();
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
    ConfigureLogicContextForTestScene();
    break;
  case SceneType::SceneType_TITLE:
    ConfigureLogicContextForTitleScene();
    break;
  case SceneType::SceneType_CRAFTING:
    ConfigureLogicContextForCraftingScene();
    break;
  default:
    throw std::runtime_error("Unsupported scene type for TestContext");
  }
  std::cout << "LogicContext for scene configured" << std::endl;
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
const LogicContext &TestContext::GetLogicContextForTestScene() const {

  if (!logic_context_for_test_scene) {
    throw std::runtime_error("LogicContext for Test Scene not configured. Call "
                             "ConfigureLogicContextForTestScene() first.");
  }
  return *logic_context_for_test_scene;
}
/////////////////////////////////////////////////
const LogicContext &TestContext::GetLogicContextForTitleScene() const {
  if (!logic_context_for_title_scene) {
    throw std::runtime_error(
        "LogicContext for Title Scene not configured. Call "
        "ConfigureLogicContextForTitleScene() first.");
  }
  return *logic_context_for_title_scene;
}

/////////////////////////////////////////////////
const LogicContext &TestContext::GetLogicContextForCraftingScene() const {
  if (!logic_context_for_crafting_scene) {
    throw std::runtime_error(
        "LogicContext for Crafting Scene not configured. Call "
        "ConfigureLogicContextForCraftingScene() first.");
  }
  return *logic_context_for_crafting_scene;
}

/////////////////////////////////////////////////
void TestContext::ConfigureGameContext() {
  game_context_ptr =
      std::make_unique<GameContext>(render_window, event_handler, loop_number,
                                    asset_manager, EnvironmentType::Test);
}

/////////////////////////////////////////////////
void TestContext::ConfigureLogicContextForTestScene() {
  // Configure the EntityMemoryPool for the test scene
  FlatbuffersConfigurator configurator(event_handler);

  std::cout << "Configuring entities for Test Scene" << std::endl;
  auto configure_result = configurator.ConfigureEntitiesFromDefaultData(
      scene_entities, SceneType::SceneType_TEST);
  std::cout << "Entities configured for Test Scene" << std::endl;
  // check the configuration was successful
  if (!configure_result.has_value()) {
    // handle the error (for example, log it)
    const FailInfo &error = configure_result.error();
    // For testing purposes, we can print the error message
    std::cerr << "Error configuring entities: " << error.message << std::endl;
  }
  // generate all archetypes for the test scene
  auto archetype_result = archetype_manager.GenerateAllArchetypes();
  if (!archetype_result.has_value()) {
    const FailInfo &error = archetype_result.error();
    std::cerr << "Error generating archetypes: " << error.message << std::endl;
  }
  // create pointer to the logic context for the test scene
  logic_context_for_test_scene = std::make_unique<LogicContext>(
      LogicContext{scene_entities, archetype_manager.GetArchetypes(),
                   render_texture, render_window, asset_manager, event_handler,
                   game_context_ptr->mouse_position});
}

/////////////////////////////////////////////////
void TestContext::ConfigureLogicContextForTitleScene() {
  // Configure the EntityMemoryPool for the title scene
  FlatbuffersConfigurator configurator(event_handler);
  auto configure_result = configurator.ConfigureEntitiesFromDefaultData(
      scene_entities, SceneType::SceneType_TITLE);
  // check the configuration was successful
  if (!configure_result.has_value()) {
    // handle the error (for example, log it)
    const FailInfo &error = configure_result.error();
    // For testing purposes, we can print the error message
    std::cerr << "Error configuring entities: " << error.message << std::endl;
  }
  // generate all archetypes for the title scene
  auto archetype_result = archetype_manager.GenerateAllArchetypes();
  if (!archetype_result.has_value()) {
    const FailInfo &error = archetype_result.error();
    std::cerr << "Error generating archetypes: " << error.message << std::endl;
  }
  // create pointer to the logic context for the title scene
  logic_context_for_title_scene = std::make_unique<LogicContext>(
      LogicContext{scene_entities, archetype_manager.GetArchetypes(),
                   render_texture, render_window, asset_manager, event_handler,
                   game_context_ptr->mouse_position});
}

/////////////////////////////////////////////////
void TestContext::ConfigureLogicContextForCraftingScene() {
  // Configure the EntityMemoryPool for the crafting scene
  FlatbuffersConfigurator configurator(event_handler);
  auto configure_result = configurator.ConfigureEntitiesFromDefaultData(
      scene_entities, SceneType::SceneType_CRAFTING);
  // check the configuration was successful
  if (!configure_result.has_value()) {
    // handle the error (for example, log it)
    const FailInfo &error = configure_result.error();
    // For testing purposes, we can print the error message
    std::cerr << "Error configuring entities: " << error.message << std::endl;
  }
  // generate all archetypes for the crafting scene
  auto archetype_result = archetype_manager.GenerateAllArchetypes();
  if (!archetype_result.has_value()) {
    const FailInfo &error = archetype_result.error();
    std::cerr << "Error generating archetypes: " << error.message << std::endl;
  }
  // create pointer to the logic context for the crafting scene
  logic_context_for_crafting_scene = std::make_unique<LogicContext>(
      LogicContext{scene_entities, archetype_manager.GetArchetypes(),
                   render_texture, render_window, asset_manager, event_handler,
                   game_context_ptr->mouse_position});
}
} // namespace steamrot::tests
