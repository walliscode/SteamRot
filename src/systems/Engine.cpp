/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the abstract Engine base class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Engine.h"

namespace steamrot {

/////////////////////////////////////////////////
Engine::Engine()
    : m_game_context(m_game_resources), m_scene_manager(m_game_context) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> Engine::StartUp() {
  // Derived classes implement ConfigureFromData()
  auto configure_result = ConfigureEngineFromData();

  // Propagate any configuration errors
  if (!configure_result.has_value()) {
    return std::unexpected(configure_result.error());
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
void Engine::Run() {

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

  // Update SceneManager level logic, such as any subscriptions it owns. It does
  // not update scenes yet.
  m_scene_manager.ExecuteSceneManagerLevelLogic();

  // Update Scene Level Logic, this is configurable per engine type
  ExecuteSceneLevelLogic();

  // Update DisplayManager level logic, this is configurable per engine type
  ExecuteDisplayManagerTick();
}
} // namespace steamrot
