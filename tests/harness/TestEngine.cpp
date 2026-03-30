/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TestEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestEngine.h"
#include "EventHandler.h"
#include "FailInfo.h"
#include "SimulationRunner.h"
#include <expected>
#include <format>
#include <variant>

namespace steamrot::tests {
/////////////////////////////////////////////////
TestEngine::TestEngine(const TestData &test_data) : m_test_data(test_data) {};

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> TestEngine::StartUp() {
  // call base class StartUp
  auto base_startup_result = Engine::StartUp();
  if (!base_startup_result.has_value()) {
    return std::unexpected(base_startup_result.error());
  }

  // assign TestEngine specific variables from TestData
  // set target ticks
  m_target_ticks = m_test_data.number_of_ticks;

  // load EventBus into EventHandler if available
  if (m_test_data.starting_engine_snapshot.global_event_bus.has_value()) {

    auto &event_bus =
        m_test_data.starting_engine_snapshot.global_event_bus.value();
    auto &event_handler = m_engine_resources.event_handler;
    for (const auto &event : event_bus) {
      event_handler.AddEvent(event);
    }
    event_handler.ProcessWaitingRoomEventBus();
  }

  // load initial scenes: either from default data or from starting snapshot
  if (m_test_data.initial_scene_type.has_value()) {
    auto add_scene_result = m_scene_manager.AddSceneFromDefault(
        m_test_data.initial_scene_type.value());
    if (!add_scene_result.has_value()) {
      return std::unexpected(add_scene_result.error());
    }
  } else {
    auto add_scenes_result = m_scene_manager.AddScenesFromSceneCollectionData(
        m_test_data.starting_engine_snapshot.scene_collection_data);
    if (!add_scenes_result.has_value()) {
      return std::unexpected(add_scenes_result.error());
    }
  }

  // after successful startup, export initial snapshot at tick 0
  auto snapshot_result = StoreEngineSnapShot();
  if (!snapshot_result.has_value()) {
    return std::unexpected(snapshot_result.error());
  }

  return std::monostate{};
}
/////////////////////////////////////////////////
void TestEngine::RunGameLoop() {

  // run through the specified number of loops
  for (size_t i{1}; i <= m_target_ticks; i++) {

    // execute same tick pipeline as GameEngine
    ExecuteTick();

    // capture snapshot after tick
    auto snapshot_result = StoreEngineSnapShot();

    // proceed to next tick if not greater than target
    if (i < m_target_ticks) {
      m_current_tick++;
    }
  }
}

/////////////////////////////////////////////////
void TestEngine::TickSceneManager() {

  // When use_default_logic is set, delegate entirely to the SceneManager:
  // subscriptions then UpdateScenes — identical to production GameEngine behaviour.
  if (m_test_data.simulation_data.use_default_logic) {
    Engine::TickSceneManager();
    return;
  }

  // When use_default_logic is false, skip the default scene logic entirely and
  // only run the explicit SimulationRunner steps — avoiding double-processing.
  for (auto &scene : m_scene_manager.GetScenes()) {

    auto scene_context = scene.second->GetSceneContext();

    // create SimulationRunner instance for each Scene
    SimulationRunner runner(m_test_data.simulation_data, scene_context);

    // execute simulation (error is not propagated — TickSceneManager is void)
    runner.ExecuteSimulation();
  }
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> TestEngine::StoreEngineSnapShot() {

  auto snapshot_result = Engine::CaptureEngineSnapshot();
  if (!snapshot_result) {
    return std::unexpected(snapshot_result.error());
  }
  EngineSnapshot &snapshot = snapshot_result.value();

  // cycle through SceneCollectionData and check the entities are stored as a
  // std::shared_ptr<EntityMemoryPool>
  for (auto &scene_data : snapshot.scene_collection_data) {
    // check entity_transport variant type and pointer
    auto ptr = std::get_if<std::shared_ptr<EntityMemoryPool>>(
        &scene_data.entity_transport);
    if (!ptr) {
      return std::unexpected(
          FailInfo{FailMode::VariantTypeMismatch,
                   std::format("Tick {}: SceneData entity_transport is not a "
                               "shared_ptr<EntityMemoryPool>",
                               m_current_tick)});
    }
    if (!*ptr) {
      return std::unexpected(FailInfo{
          FailMode::NullPointer,
          std::format("Tick {}: SceneData entity_transport shared_ptr is null",
                      m_current_tick)});
    }
    // assign a copy constructed EntityMemoryPool to the variant
    scene_data.entity_transport = *ptr;
  }

  // store snapshot in data bank, this needs moving due to unique_ptr members
  // if data bank is empty then store at tick 0, otherwise use current tick
  if (m_data_bank.empty()) {
    m_data_bank[0] = std::move(snapshot);
  } else {
    m_data_bank[m_current_tick] = std::move(snapshot);
  }
  return std::monostate{};
}
/////////////////////////////////////////////////
const std::map<size_t, EngineSnapshot> &TestEngine::GetDataBank() const {
  return m_data_bank;
}

} // namespace steamrot::tests
