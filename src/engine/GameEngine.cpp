/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GameEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "GameEngine.h"
#include "EventPayload.h"
#include "FailInfo.h"
#include <expected>
#include <variant>

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
    auto tick_result = ExecuteTick();
    if (!tick_result.has_value()) {
      m_loop_error = tick_result.error();
      break;
    }
    // increment loop number
    m_engine_resources.loop_number++;
  }
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> GameEngine::TickSceneLogic() {

  ///// LEFT BLANK INTENTIONALLY /////
  /// For GameEngine, all scene logic is handled by SceneManager internally
  /// This is to provide granularity for the TestEngine to simulate scene logic

  return std::monostate{};
}

/////////////////////////////////////////////////
void GameEngine::TickRendering() {
  // Let DisplayManager handle rendering
  auto call_render_cycle_result = m_display_manager.CallRenderCycle();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> GameEngine::ProcessSubscriptions() {
  // cycle through all subscribers and process active ones
  for (const auto &subscriber : m_engine_state.subscriptions) {

    // only process active subscribers
    if (subscriber->m_active) {

      // switch on the EventType
      switch (subscriber->event_type) {

        // deal with SYSTEM level events
      case EventType::SYSTEM: {

        // check filter data is SystemPayload
        if (!std::holds_alternative<SystemPayload>(
                subscriber->filter_payload)) {
          // filter data is not SystemPayload, skip
          continue;
        }
        SystemPayload &filter_data =
            std::get<SystemPayload>(subscriber->filter_payload);

        // switch on the SystemAction
        switch (filter_data.action) {
        case SystemPayload::SystemAction::QUIT: {

          // close the window to quit the game
          m_engine_resources.game_window.close();
          break;
        }

        // deal with other system actions here as needed
        default:
          break;
        }
      }
      // deal with other event types here as needed
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
