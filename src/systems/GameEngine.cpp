
/// Headers
/////////////////////////////////////////////////

#include "GameEngine.h"

namespace steamrot {

/////////////////////////////////////////////////
GameEngine::GameEngine(const PathProvider &path_provider)
    : Engine(path_provider),
      m_display_manager(m_game_resources.game_window, m_scene_manager) {}

/////////////////////////////////////////////////
void GameEngine::ExecuteSceneLevelLogic() {

  // update all scenes via SceneManager
  m_scene_manager.UpdateScenes();
}

/////////////////////////////////////////////////
void GameEngine::ExecuteDisplayManagerTick() {
  // let DisplayManager handle rendering
  auto call_render_cycle_result = m_display_manager.CallRenderCycle();
}

/////////////////////////////////////////////////
void GameEngine::RunGameLoop() {
  m_running = true;
  // main game loop
  while (m_running && m_game_resources.game_window.isOpen()) {
    // execute a single tick of all systems
    ExecuteSystemsTick();
    // increment loop number
    m_game_resources.loop_number++;
  }
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> GameEngine::ProcessSubscriptions() {
  // cycle through all subscribers and process active ones
  for (const auto &subscriber : m_subscriptions) {

    // only process active subscribers
    if (subscriber->m_active) {

      // switch on the EventType
      switch (subscriber->m_trigger_event_type) {
      case EventType::EventType_EVENT_QUIT_GAME: {
        // close the window to quit the game
        m_game_resources.game_window.close();
        break;
      }
      default:
        break;
      }

      // FINALLY set the subscriber to inactive
      subscriber->m_active = false;
    }
  }
  return std::monostate{};
}
} // namespace steamrot
