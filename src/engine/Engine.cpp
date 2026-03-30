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

  if (m_loop_error.has_value())
    return std::unexpected(m_loop_error.value());

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> Engine::ExecuteTick() {
  OnTickBegin();

  auto tick_events_result = TickEvents();
  if (!tick_events_result.has_value())
    return std::unexpected(tick_events_result.error());

  auto engine_logic_result = TickEngineLogic();
  if (!engine_logic_result.has_value())
    return std::unexpected(engine_logic_result.error());

  auto scene_manager_result = TickSceneManager();
  if (!scene_manager_result.has_value())
    return std::unexpected(scene_manager_result.error());

  auto tick_rendering_result = TickRendering();
  if (!tick_rendering_result.has_value())
    return std::unexpected(tick_rendering_result.error());

  OnTickEnd();

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> Engine::TickEvents() {
  // Let EventHandler process SFML events and update bus and subscribers
  return m_game_context.event_handler.ExecuteEventHandlerLevelLogic(
      m_engine_resources.game_window);
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> Engine::TickEngineLogic() {
  // Process engine-level subscriptions and propagate any errors
  auto process_subs_result = ProcessSubscriptions();
  if (!process_subs_result.has_value())
    return std::unexpected(process_subs_result.error());

  // update mouse position in GameContext
  m_game_context.mouse_position =
      sf::Mouse::getPosition(m_engine_resources.game_window);

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> Engine::TickSceneManager() {
   // Delegate all SceneManager work to the SceneManager itself:
  // ProcessSubscriptions (e.g. scene-change events) then UpdateScenes.
  auto scene_manager_result = m_scene_manager.ExecuteSceneManagerLevelLogic();
  if (!scene_manager_result.has_value())
    return std::unexpected(scene_manager_result.error());

  return std::monostate{};
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
