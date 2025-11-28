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

  return std::monostate{};
}

/////////////////////////////////////////////////
void TestEngine::ExecuteTick() {
  switch (m_mode) {
  case Mode::Custom:
    ExecuteCustomTick();
    break;
  case Mode::Standard:
    ExecuteStandardTick();
    break;
  }
}

/////////////////////////////////////////////////
void TestEngine::ExecuteCustomTick() {
  // Process event buses
  events::ProcessEventBusCycle(m_game_resources.event_handler);

  // Execute custom steps in order
  SceneContext &ctx = GetSceneContext();
  for (auto &step : m_custom_steps) {
    step->Execute(ctx);
  }
}

/////////////////////////////////////////////////
void TestEngine::ExecuteStandardTick() {
  // Process event buses (same as GameEngine)
  events::ProcessEventBusCycle(m_game_resources.event_handler);

  // In standard mode, we would execute scene logic
  // For now, this is a placeholder - full implementation would
  // mirror GameEngine's SceneManager::UpdateScenes()
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
  m_mode = Mode::Custom;
  m_custom_steps.push_back(std::make_unique<FunctionStep>(func, name));
  return *this;
}

/////////////////////////////////////////////////
TestEngine &TestEngine::UseFullScene(SceneType scene_type) {
  m_mode = Mode::Standard;
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
TestEngine::Mode TestEngine::GetMode() const { return m_mode; }

} // namespace steamrot::tests
