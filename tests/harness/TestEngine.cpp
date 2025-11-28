/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TestEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestEngine.h"
#include "FlatbuffersDataLoader.h"
#include "console_output.h"
#include "event_handler_tick.h"
#include "resources_configuration.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
TestEngine::TestEngine(const TestDataConfig *config)
    : Engine(EnvironmentType::Test), m_test_config(config),
      m_scene_manager(m_game_context),
      m_display_manager(m_game_resources.game_window, m_scene_manager),
      m_entity_manager(m_game_resources.event_handler) {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> TestEngine::Initialize() {
  return StartUp();
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
TestEngine::Initialize(const EntityCollection *entity_collection) {
  // Configure game resources
  FlatbuffersDataLoader loader;
  auto game_data_result = loader.ProvideGameResourcesData();

  if (!game_data_result.has_value()) {
    return std::unexpected(game_data_result.error());
  }

  auto config_result = resources::ConfigureGameResources(
      m_game_resources, game_data_result.value());

  if (!config_result.has_value()) {
    return std::unexpected(config_result.error());
  }

  m_game_resources.env_type = EnvironmentType::Test;

  // Load default assets
  auto load_result = m_game_resources.asset_manager.LoadDefaultAssets();
  if (!load_result.has_value()) {
    return std::unexpected(load_result.error());
  }

  // Configure scene resources
  auto scene_data_result = loader.ProvideSceneResourcesData(m_scene_type);
  if (scene_data_result.has_value()) {
    auto scene_config_result = resources::ConfigureSceneResources(
        m_scene_resources, scene_data_result.value());
    if (!scene_config_result.has_value()) {
      return std::unexpected(scene_config_result.error());
    }
  }

  // Configure entities from provided collection
  if (entity_collection) {
    FlatbuffersConfigurator configurator(m_game_resources.event_handler);
    auto configure_result = configurator.ConfigureEntitiesFromCollection(
        m_entity_manager.GetEntityMemoryPool(), entity_collection);
    if (!configure_result.has_value()) {
      return std::unexpected(configure_result.error());
    }

    // Generate archetypes
    auto archetype_result = m_entity_manager.GenerateAllArchetypes();
    if (!archetype_result.has_value()) {
      return std::unexpected(archetype_result.error());
    }
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> TestEngine::ConfigureFromData() {
  // Configure game resources
  FlatbuffersDataLoader loader;
  auto game_data_result = loader.ProvideGameResourcesData();

  if (!game_data_result.has_value()) {
    return std::unexpected(game_data_result.error());
  }

  auto config_result = resources::ConfigureGameResources(
      m_game_resources, game_data_result.value());

  if (!config_result.has_value()) {
    return std::unexpected(config_result.error());
  }

  m_game_resources.env_type = EnvironmentType::Test;

  // Load default assets
  auto load_result = m_game_resources.asset_manager.LoadDefaultAssets();
  if (!load_result.has_value()) {
    return std::unexpected(load_result.error());
  }

  // For FullEngine/SceneManager/SceneLogic levels, configure SceneManager
  if (m_tick_level != TickLevel::Custom) {
    // Configure the SceneManager from data
    auto scene_manager_data_result = loader.ProvideSceneManagerData();
    if (scene_manager_data_result.has_value()) {
      auto sm_configure_result = m_scene_manager.ConfigureSceneManagerFromData(
          scene_manager_data_result.value());
      if (!sm_configure_result.has_value()) {
        return std::unexpected(sm_configure_result.error());
      }
    }

    // Load the scene
    auto load_scene_result = m_scene_manager.LoadStandAloneScene(m_scene_type);
    if (!load_scene_result.has_value()) {
      return std::unexpected(load_scene_result.error());
    }
  }

  // For Custom level, configure scene resources and entities directly
  if (m_tick_level == TickLevel::Custom) {
    // Configure scene resources
    auto scene_data_result = loader.ProvideSceneResourcesData(m_scene_type);
    if (scene_data_result.has_value()) {
      auto scene_config_result = resources::ConfigureSceneResources(
          m_scene_resources, scene_data_result.value());
      if (!scene_config_result.has_value()) {
        return std::unexpected(scene_config_result.error());
      }
    }

    // Configure entities from test data
    if (m_test_config) {
      TestDataSource source(m_test_config);
      const EntityCollection *entity_collection = source.GetEntityCollection();

      if (entity_collection) {
        FlatbuffersConfigurator configurator(m_game_resources.event_handler);
        auto configure_result = configurator.ConfigureEntitiesFromCollection(
            m_entity_manager.GetEntityMemoryPool(), entity_collection);
        if (!configure_result.has_value()) {
          return std::unexpected(configure_result.error());
        }

        // Generate archetypes
        auto archetype_result = m_entity_manager.GenerateAllArchetypes();
        if (!archetype_result.has_value()) {
          return std::unexpected(archetype_result.error());
        }
      }
    }
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
void TestEngine::ExecuteTick() {
  switch (m_tick_level) {
  case TickLevel::FullEngine:
    TickFullEngine();
    break;
  case TickLevel::SceneManager:
    TickSceneManagerLevel();
    break;
  case TickLevel::SceneLogic:
    TickSceneLogicLevel();
    break;
  case TickLevel::Custom:
    TickCustomLevel();
    break;
  }
}

/////////////////////////////////////////////////
void TestEngine::TickFullEngine() {
  // Mirrors GameEngine::UpdateSystems() EXACTLY

  // Update Game Resources (mouse position, etc.)
  m_game_resources.mouse_position =
      sf::Mouse::getPosition(m_game_resources.game_window);

  // Preload Events (external events to waiting room)
  m_game_resources.event_handler.PreloadEvents(m_game_resources.game_window);

  // Process Waiting Room Event Bus into Global Event Bus
  m_game_resources.event_handler.ProcessWaitingRoomEventBus();

  // Update Subscribers from Global Event Bus
  m_game_resources.event_handler.UpateSubscribersFromGlobalEventBus();

  // Update Scenes (SceneManager level)
  m_scene_manager.UpdateSceneManager();

  // Call Render Cycle
  m_display_manager.CallRenderCycle();

  // Tick the Global Event Bus
  m_game_resources.event_handler.TickGlobalEventBus();
}

/////////////////////////////////////////////////
void TestEngine::TickSceneManagerLevel() {
  // Process event buses first
  events::ProcessEventBusCycle(m_game_resources.event_handler);

  // Mirrors SceneManager::UpdateSceneManager() EXACTLY
  m_scene_manager.UpdateSceneManager();
}

/////////////////////////////////////////////////
void TestEngine::TickSceneLogicLevel() {
  // Process event buses first
  events::ProcessEventBusCycle(m_game_resources.event_handler);

  // Run scene logic directly (sAction, sCollision, sRender)
  // This uses SceneManager::UpdateScenes() which calls each scene's logic
  m_scene_manager.UpdateScenes();
}

/////////////////////////////////////////////////
void TestEngine::TickCustomLevel() {
  // Process event buses
  events::ProcessEventBusCycle(m_game_resources.event_handler);

  // Execute custom steps in order
  SceneContext &ctx = GetSceneContext();
  for (auto &step : m_custom_steps) {
    step->Execute(ctx);
  }
}

/////////////////////////////////////////////////
bool TestEngine::ShouldContinueRunning() const {
  if (m_target_ticks > 0) {
    return m_current_tick < m_target_ticks;
  }
  return true;
}

/////////////////////////////////////////////////
void TestEngine::OnPostTick() {
  Engine::OnPostTick();
  m_current_tick++;
}

/////////////////////////////////////////////////
TestEngine &TestEngine::AddFunction(std::function<void(SceneContext &)> func,
                                    const std::string &name) {
  m_tick_level = TickLevel::Custom;
  m_custom_steps.push_back(std::make_unique<FunctionStep>(func, name));
  return *this;
}

/////////////////////////////////////////////////
TestEngine &TestEngine::UseTickLevel(TickLevel level) {
  m_tick_level = level;
  return *this;
}

/////////////////////////////////////////////////
TestEngine &TestEngine::UseFullScene(SceneType scene_type) {
  m_tick_level = TickLevel::FullEngine;
  m_scene_type = scene_type;
  return *this;
}

/////////////////////////////////////////////////
TestEngine &TestEngine::SetSceneType(SceneType scene_type) {
  m_scene_type = scene_type;
  return *this;
}

/////////////////////////////////////////////////
TestEngine &TestEngine::SetTicks(size_t ticks) {
  m_target_ticks = ticks;
  return *this;
}

/////////////////////////////////////////////////
EntityMemoryPool &TestEngine::GetEntityPool() {
  return m_entity_manager.GetEntityMemoryPool();
}

/////////////////////////////////////////////////
const EntityMemoryPool &TestEngine::GetEntityPool() const {
  return m_entity_manager.GetEntityMemoryPool();
}

/////////////////////////////////////////////////
EntityManager &TestEngine::GetEntityManager() { return m_entity_manager; }

/////////////////////////////////////////////////
SceneContext &TestEngine::GetSceneContext() {
  if (!m_scene_context) {
    m_scene_context = std::make_unique<SceneContext>(
        m_scene_resources, m_game_resources, m_entity_manager);
  }
  return *m_scene_context;
}

/////////////////////////////////////////////////
size_t TestEngine::GetCurrentTick() const { return m_current_tick; }

/////////////////////////////////////////////////
TickLevel TestEngine::GetTickLevel() const { return m_tick_level; }

/////////////////////////////////////////////////
SceneManager &TestEngine::GetSceneManager() { return m_scene_manager; }

/////////////////////////////////////////////////
const SceneManager &TestEngine::GetSceneManager() const {
  return m_scene_manager;
}

} // namespace steamrot::tests
