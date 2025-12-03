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
#include "SubscriberFactory.h"
#include "core_configuration.h"
#include <vector>

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

/////////////////////////////////////////////////
void Engine::ExecuteEngineLevelLogic() {
  // Check all subscriptions for activation and process
  auto process_subs_result = ProcessSubscriptions();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> Engine::ConfigureSubscribersFromData(
    const ::flatbuffers::Vector<::flatbuffers::Offset<steamrot::SubscriberData>>
        *subscriptions) {

  // call SubscriberFactory to create and register subscribers
  SubscriberFactory subscriber_factory(m_game_context.event_handler);
  ;
  for (auto subscription_data : *subscriptions) {

    auto subscriber_result =
        subscriber_factory.CreateAndRegisterSubscriber(*subscription_data);
    if (!subscriber_result.has_value()) {
      return std::unexpected(subscriber_result.error());
    }
    m_subscriptions.push_back(subscriber_result.value());
  };

  return std::monostate{};
}
/////////////////////////////////////////////////
std::vector<std::shared_ptr<Subscriber>> &Engine::GetSubscriptions() {
  return m_subscriptions;
}
} // namespace steamrot
