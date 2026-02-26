/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the abstract Engine base class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Engine.h"
#include "FailInfo.h"
#include "engine_configuration.h"
#include <expected>
#include <variant>
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
  auto engine_data_result = data_provider.CreateEngineData();
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

  // Start up SceneManager
  auto scene_manager_startup_result = m_scene_manager.StartUp();
  if (!scene_manager_startup_result) {
    return std::unexpected(scene_manager_startup_result.error());
  }

  // Set up AssetManager
  auto asset_manager_startup_result =
      m_engine_resources.asset_manager.Startup();
  if (!asset_manager_startup_result) {
    return std::unexpected(asset_manager_startup_result.error());
  }
  // Pass initial AssetConfig to AssetManager
  auto load_initial_assets_result =
      m_engine_resources.asset_manager.LoadAssetsFromConfig(
          engine_data.initial_asset_config);
  if (!load_initial_assets_result) {
    return std::unexpected(load_initial_assets_result.error());
  }

  // Configure EventHandler input-action registry
  auto input_action_provider_result =
      m_engine_resources.data_access_factory.GetInputActionConfigProvider();
  if (!input_action_provider_result) {
    return std::unexpected(input_action_provider_result.error());
  }

  auto input_action_registry_result =
      input_action_provider_result.value()->CreateInputActionRegistry();
  if (!input_action_registry_result) {
    return std::unexpected(input_action_registry_result.error());
  }

  m_engine_resources.event_handler.SetInputActionRegistry(
      std::move(input_action_registry_result.value()));

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> Engine::RunGame() {

  RunGameLoop();

  return std::monostate{};
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

  // update mouse position in GameContext
  m_game_context.mouse_position =
      sf::Mouse::getPosition(m_engine_resources.game_window);
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

/////////////////////////////////////////////////
std::expected<EngineSnapshot, FailInfo> Engine::CaptureEngineSnapshot() {
  EngineSnapshot snapshot;

  // capture SceneCollectionData snapshot
  auto scene_collection_data_result =
      m_scene_manager.CaptureSceneCollectionData();
  if (!scene_collection_data_result) {
    return std::unexpected(scene_collection_data_result.error());
  }

  // grab all the Scene Data
  snapshot.scene_collection_data =
      std::move(scene_collection_data_result.value());

  // capture global EventBus in snapshot
  snapshot.global_event_bus = m_game_context.event_handler.GetGlobalEventBus();

  return snapshot;
}
} // namespace steamrot
