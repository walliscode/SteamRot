/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GameEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "GameEngine.h"
#include <expected>
#include <iostream>

namespace steamrot {

/////////////////////////////////////////////////
GameEngine::GameEngine()
    : m_display_manager(m_engine_resources.game_window, m_scene_manager) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> GameEngine::StartUp() {
  // Call base StartUp() which loads common engine data
  auto base_startup_result = Engine::StartUp();
  if (!base_startup_result.has_value()) {
    return std::unexpected(base_startup_result.error());
  }

  // GameEngine-specific: Load the title scene to start the game
  // This does NOT automatically load save game data - user must select a save
  auto load_title_result = m_scene_manager.LoadTitleScene();
  if (!load_title_result.has_value()) {
    return std::unexpected(load_title_result.error());
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
void GameEngine::RunGameLoop() {
  m_engine_state.running = true;
  // main game loop
  while (m_engine_state.running && m_engine_resources.game_window.isOpen()) {
    // execute a single tick of all systems
    ExecuteTick();
    // increment loop number
    m_engine_resources.loop_number++;
  }
}

/////////////////////////////////////////////////
void GameEngine::TickSceneLogic() {

  ///// LEFT BLANK INTENTIONALLY /////
  /// For GameEngine, all scene logic is handled by SceneManager internally
  /// This is to provide granularity for the TestEngine to simulate scene logic
}

/////////////////////////////////////////////////
void GameEngine::TickRendering() {
  // Let DisplayManager handle rendering
  auto call_render_cycle_result = m_display_manager.CallRenderCycle();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> GameEngine::ProcessSubscriptions() {
  std::cout << "[DEBUG] GameEngine::ProcessSubscriptions - Checking "
            << m_engine_state.subscriptions.size() << " engine subscriptions"
            << std::endl;
  
  // cycle through all subscribers and process active ones
  for (const auto &subscriber : m_engine_state.subscriptions) {

    // only process active subscribers
    if (subscriber->m_active) {

      std::cout << "[DEBUG] Processing ACTIVE subscription for event type: "
                << EnumNameEventType(subscriber->m_trigger_event_type)
                << std::endl;
      // switch on the EventType
      switch (subscriber->m_trigger_event_type) {
      case EventType::QUIT_GAME: {
        std::cout << "[DEBUG] *** QUIT_GAME event detected! Closing window! ***"
                  << std::endl;
        // close the window to quit the game
        m_engine_resources.game_window.close();
        break;
      }
      default:
        std::cout << "[DEBUG] Event type has no handler in GameEngine" << std::endl;
        break;
      }

      // FINALLY set the subscriber to inactive
      subscriber->m_active = false;
    }
  }
  
  std::cout << "[DEBUG] GameEngine::ProcessSubscriptions - Complete" << std::endl;
  return std::monostate{};
}

} // namespace steamrot
