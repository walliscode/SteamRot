/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GameEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "GameEngine.h"
#include "FlatbuffersUserPreferencesProvider.h"
#include "provider_factory.h"

namespace steamrot {

/////////////////////////////////////////////////
GameEngine::GameEngine()
    : m_display_manager(m_engine_resources.game_window, m_scene_manager) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> GameEngine::StartUp() {
  // Call base StartUp() which loads:
  // - EngineCoreData (window config)
  // - Default user preferences from default.preferences.bin
  // - Calls ConfigureEngineStateFromData() (subscriptions, scene manager)
  auto base_startup_result = Engine::StartUp();
  if (!base_startup_result.has_value()) {
    return std::unexpected(base_startup_result.error());
  }

  // GameEngine-specific: Check for saved user preferences
  // If user has saved preferences, load them to override defaults
  FlatbuffersUserPreferencesProvider preferences_provider;
  if (preferences_provider.HasUserPreferences()) {
    auto saved_prefs_result = preferences_provider.LoadPreferences();
    if (saved_prefs_result.has_value()) {
      m_engine_config.user_preferences = saved_prefs_result.value();
    }
    // If loading saved preferences fails, continue with defaults
    // (already loaded by Engine::StartUp)
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
std::expected<std::monostate, FailInfo>
GameEngine::ConfigureEngineStateFromData() {
  // GameEngine loads EngineData from default files via provider
  IGameConfigProvider &config_provider = GetGameConfigProvider();

  auto engine_data_result = config_provider.LoadEngineConfig();
  if (!engine_data_result.has_value()) {
    return std::unexpected(engine_data_result.error());
  }
  const EngineDataFbs *engine_data = engine_data_result.value();

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

  return std::monostate{};
}

/////////////////////////////////////////////////
void GameEngine::TickSceneLogic() {
  // Update all scenes via SceneManager
  m_scene_manager.UpdateScenes();
}

/////////////////////////////////////////////////
void GameEngine::TickRendering() {
  // Let DisplayManager handle rendering
  auto call_render_cycle_result = m_display_manager.CallRenderCycle();
}

/////////////////////////////////////////////////
void GameEngine::RunGameLoop() {
  m_engine_state.running = true;
  // main game loop
  while (m_engine_state.running && m_engine_resources.game_window.isOpen()) {
    // execute a single tick of all systems
    ExecuteSystemsTick();
    // increment loop number
    m_engine_resources.loop_number++;
  }
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> GameEngine::ProcessSubscriptions() {
  // cycle through all subscribers and process active ones
  for (const auto &subscriber : m_engine_state.subscriptions) {

    // only process active subscribers
    if (subscriber->m_active) {

      // switch on the EventType
      switch (subscriber->m_trigger_event_type) {
      case EventType::EventType_EVENT_QUIT_GAME: {
        // close the window to quit the game
        m_engine_resources.game_window.close();
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
