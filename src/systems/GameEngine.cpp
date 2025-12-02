/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GameEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "GameEngine.h"
#include "FlatbuffersDataLoader.h"
#include "FlatbuffersUserPreferencesProvider.h"

namespace steamrot {

/////////////////////////////////////////////////
GameEngine::GameEngine()
    : m_display_manager(m_game_resources.game_window, m_scene_manager) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
GameEngine::ConfigureEngineStateFromData() {
  // GameEngine loads EngineData from default files
  FlatbuffersDataLoader data_loader;

  auto engine_data_result = data_loader.ProvideEngineData();
  if (!engine_data_result.has_value()) {
    return std::unexpected(engine_data_result.error());
  }
  const EngineData *engine_data = engine_data_result.value();

  // Configure Engine-level subscriptions from EngineData
  // These handle engine-level events like quit game
  if (engine_data->subscriptions()) {
    auto configure_subs_result =
        ConfigureSubscribersFromData(engine_data->subscriptions());
    if (!configure_subs_result.has_value()) {
      return std::unexpected(configure_subs_result.error());
    }
  }

  // Configure SceneManager from EngineData
  // This sets up scene-level subscriptions like scene changes
  if (engine_data->scene_manager_data()) {
    auto configure_result = m_scene_manager.ConfigureSceneManagerFromData(
        engine_data->scene_manager_data());
    if (!configure_result.has_value()) {
      return std::unexpected(configure_result.error());
    }
  }

  // Load user preferences from file
  // GameEngine loads from default.preferences.bin (and user overrides if present)
  FlatbuffersUserPreferencesProvider preferences_provider;
  auto preferences_result = preferences_provider.LoadPreferences();
  if (!preferences_result.has_value()) {
    return std::unexpected(preferences_result.error());
  }
  m_user_preferences = preferences_result.value();

  return std::monostate{};
}

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
