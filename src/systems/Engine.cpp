/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the abstract Engine base class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Engine.h"
#include "FlatbuffersDataLoader.h"
#include "FlatbuffersUserPreferencesProvider.h"
#include "core_configuration.h"

namespace steamrot {

/////////////////////////////////////////////////
Engine::Engine()
    : m_game_core(), m_game_context(m_game_core),
      m_scene_manager(m_game_context) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> Engine::StartUp() {
  // Load core data via FlatbuffersDataLoader
  FlatbuffersDataLoader data_loader;

  // Get EngineCoreData from data loader
  // This loads window configuration (size, title, framerate)
  auto engine_core_result = data_loader.ProvideEngineCoreData();
  if (!engine_core_result) {
    return std::unexpected(engine_core_result.error());
  }

  // Use core data to configure GameCore
  auto configure_core_result =
      core::ConfigureGameCore(m_game_core, engine_core_result.value());
  if (!configure_core_result) {
    return std::unexpected(configure_core_result.error());
  }

  // Load default user preferences from default.preferences.bin
  // Both GameEngine and TestEngine load default preferences
  FlatbuffersUserPreferencesProvider preferences_provider;
  auto preferences_result = preferences_provider.LoadPreferences();
  if (!preferences_result.has_value()) {
    return std::unexpected(preferences_result.error());
  }
  m_user_preferences = preferences_result.value();

  // Configure Engine state from data - this is virtual, so derived classes
  // (GameEngine/TestEngine) provide their own data source strategy
  auto configure_engine_result = ConfigureEngineStateFromData();
  if (!configure_engine_result.has_value()) {
    return std::unexpected(configure_engine_result.error());
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
      m_game_core.game_window);

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
