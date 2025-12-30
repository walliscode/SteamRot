/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the abstract Engine base class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Engine.h"
#include "engine_configuration.h"
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
Engine::Engine()
    : m_engine_resources(), m_game_context(m_engine_resources),
      m_scene_manager(m_game_context) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> Engine::StartUp() {
  // Load all engine data via provider interface
  auto data_provider_result =
      m_engine_resources.data_access_factory.GetEngineDataProvider();

  if (!data_provider_result) {
    return std::unexpected(data_provider_result.error());
  }
  IEngineDataProvider &data_provider = *data_provider_result.value();

  // Load EngineData
  auto engine_data_result = data_provider.LoadEngineData();
  if (!engine_data_result) {
    return std::unexpected(engine_data_result.error());
  }
  const EngineData &engine_data = engine_data_result.value();

  // Configure EngineResources
  auto configure_resources_result = engine::ConfigureEngineResources(
      m_engine_resources, engine_data.engine_resources_config);
  if (!configure_resources_result) {
    return std::unexpected(configure_resources_result.error());
  }

  // Load EngineConfig
  m_engine_config = engine_data.engine_config;

  // Load EngineState
  m_engine_state = engine_data.engine_state;

  // Register subscriptions with EventHandler
  for (auto &subscriber : m_engine_state.subscriptions) {
    auto register_result =
        m_game_context.event_handler.RegisterSubscriber(subscriber);
    if (!register_result.has_value()) {
      return std::unexpected(register_result.error());
    }
  }

  // Pass initial AssetConfig to AssetManager
  auto load_initial_assets_result = m_engine_resources.asset_manager.LoadAssets(
      engine_data.initial_asset_config);
  if (!load_initial_assets_result) {
    return std::unexpected(load_initial_assets_result.error());
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
