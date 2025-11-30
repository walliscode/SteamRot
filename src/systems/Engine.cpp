/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the abstract Engine base class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Engine.h"
#include "FlatbuffersDataLoader.h"
#include "resources_configuration.h"

namespace steamrot {

/////////////////////////////////////////////////
Engine::Engine()
    : m_game_context(m_game_resources), m_scene_manager(m_game_context) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> Engine::StartUp() {
  // Load resource data via FlatbuffersDataLoader
  FlatbuffersDataLoader data_loader;

  // Get GameResourcesData from data loader
  auto engine_resource_result = data_loader.ProvideGameResourcesData();
  if (!engine_resource_result) {
    return std::unexpected(engine_resource_result.error());
  }

  // Use resource data to configure GameResources
  auto configure_resources_result = resources::ConfigureGameResources(
      m_game_resources, engine_resource_result.value());
  if (!configure_resources_result) {
    return std::unexpected(configure_resources_result.error());
  }

  auto load_engine_data_result = data_loader.ProvideEngineData();
  if (!load_engine_data_result) {
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
void Engine::RunGame() {

  auto start_up_result = StartUp();
  RunGameLoop();
}

/////////////////////////////////////////////////
void Engine::ExecuteSystemsTick() {

  // let EventHandler process sfml events and update bus and subscribers
  m_game_context.event_handler.ExecuteEventHandlerLevelLogic(
      m_game_resources.game_window);

  // Update Engine level logic
  ExecuteEngineLevelLogic();

  // Update SceneManager level logic, such as any subscriptions it owns. It
  // does not update scenes.
  m_scene_manager.ExecuteSceneManagerLevelLogic();

  // Update Scene Level Logic, this is configurable per engine type
  ExecuteSceneLevelLogic();

  // Update DisplayManager level logic, this is configurable per engine type
  ExecuteDisplayManagerTick();
}
} // namespace steamrot
