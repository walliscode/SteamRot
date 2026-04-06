/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GameEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "GameEngine.h"
#include "EventPayload.h"
#include "EventType.h"
#include "FailInfo.h"
#include "Subscriber.h"
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

  // Register a window-resize subscriber so that ProcessSubscriptions() can
  // forward the new size to the SceneManager.
  auto resize_subscriber = std::make_shared<Subscriber>();
  resize_subscriber->event_type = EventType::SYSTEM;
  resize_subscriber->filter_payload =
      SystemPayload{SystemPayload::SystemAction::RESIZE};
  m_engine_state.subscriptions.push_back(resize_subscriber);
  auto register_result =
      m_game_context.event_handler.RegisterSubscriber(resize_subscriber);
  if (!register_result.has_value()) {
    return std::unexpected(register_result.error());
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
std::expected<std::monostate, FailInfo> GameEngine::TickRendering() {
  // Let DisplayManager handle rendering
  return m_display_manager.CallRenderCycle();
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

        case SystemPayload::SystemAction::RESIZE: {
          // Extract the new size from the captured event payload and forward
          // it to the SceneManager so each scene's resize strategy fires.
          if (subscriber->captured_payload.has_value()) {
            if (const auto *resize_payload = std::get_if<SystemPayload>(
                    &subscriber->captured_payload.value())) {
              if (resize_payload->optional_resize_size.has_value()) {
                m_scene_manager.HandleResize(
                    *resize_payload->optional_resize_size);
              }
            }
          }
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
