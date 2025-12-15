/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the abstract Engine base class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Engine.h"
#include "engine_configuration.h"
#include "provider_factory.h"
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
Engine::Engine()
    : m_engine_resources(), m_game_context(m_engine_resources),
      m_scene_manager(m_game_context) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> Engine::StartUp() {
  // Load all engine data via provider interface
  IEngineDataProvider &data_provider = GetEngineDataProvider();

  // Load EngineResources configuration
  auto resources_config_result = data_provider.LoadEngineResourcesConfig();
  if (!resources_config_result) {
    return std::unexpected(resources_config_result.error());
  }

  // Configure EngineResources
  auto configure_resources_result = engine::ConfigureEngineResources(
      m_engine_resources, resources_config_result.value());
  if (!configure_resources_result) {
    return std::unexpected(configure_resources_result.error());
  }

  // Load EngineConfig
  auto engine_config_result = data_provider.LoadEngineConfig();
  if (!engine_config_result) {
    return std::unexpected(engine_config_result.error());
  }
  // assign straight to member
  m_engine_config = engine_config_result.value();

  // Load EngineState
  auto engine_state_result = data_provider.LoadEngineState();
  if (!engine_state_result) {
    return std::unexpected(engine_state_result.error());
  }
  m_engine_state = engine_state_result.value();

  // Register subscriptions with EventHandler
  for (auto &subscriber : m_engine_state.subscriptions) {
    auto register_result =
        m_game_context.event_handler.RegisterSubscriber(subscriber);
    if (!register_result.has_value()) {
      return std::unexpected(register_result.error());
    }
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
void Engine::RunGame() {

  auto start_up_result = StartUp();
  RunGameLoop();
}

/////////////////////////////////////////////////
void Engine::ExecuteTick() {
  OnTickBegin();
  TickEvents();
  TickEngineLogic();
  TickSceneManager();
  TickSceneLogic();
  TickRendering();
  OnTickEnd();
}

/////////////////////////////////////////////////
void Engine::TickEvents() {
  // Let EventHandler process SFML events and update bus and subscribers
  m_game_context.event_handler.ExecuteEventHandlerLevelLogic(
      m_engine_resources.game_window);
}

/////////////////////////////////////////////////
void Engine::TickEngineLogic() {
  // Check all subscriptions for activation and process
  auto process_subs_result = ProcessSubscriptions();
}

/////////////////////////////////////////////////
void Engine::TickSceneManager() {
  // Update SceneManager level logic, such as any subscriptions it owns.
  // It does not update scenes (that's done in TickSceneLogic).
  m_scene_manager.ExecuteSceneManagerLevelLogic();
}

/////////////////////////////////////////////////
std::vector<std::shared_ptr<Subscriber>> &Engine::GetSubscriptions() {
  return m_engine_state.subscriptions;
}
} // namespace steamrot
