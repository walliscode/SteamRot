/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the abstract Engine base class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Engine.h"
#include "SubscriberFactory.h"
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

  // Configure Subscribers of EngineState from loaded data
  // [TODO: Add SUbscriber configuration to EngineState data]

  return std::monostate{};
}

/////////////////////////////////////////////////
void Engine::RunGame() {

  auto start_up_result = StartUp();
  RunGameLoop();
}

/////////////////////////////////////////////////
void Engine::ExecuteSystemsTick() {

  // Use the new unified tick pipeline
  ExecuteTick();
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
std::expected<std::monostate, FailInfo> Engine::ConfigureSubscribersFromData(
    const ::flatbuffers::Vector<
        ::flatbuffers::Offset<steamrot::SubscriberConfigFbs>> *subscriptions) {

  // call subscriber_factory to create and register subscribers
  for (auto subscription_data : *subscriptions) {

    auto subscriber_result =
        subscriber_factory::CreateAndRegisterSubscriber(*subscription_data,
                                                        m_game_context.event_handler);
    if (!subscriber_result.has_value()) {
      return std::unexpected(subscriber_result.error());
    }
    m_engine_state.subscriptions.push_back(subscriber_result.value());
  };

  return std::monostate{};
}
/////////////////////////////////////////////////
std::vector<std::shared_ptr<Subscriber>> &Engine::GetSubscriptions() {
  return m_engine_state.subscriptions;
}
} // namespace steamrot
