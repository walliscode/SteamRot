/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TestEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestEngine.h"
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
  m_target_ticks = m_test_data.number_of_ticks;

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
void TestEngine::TickSceneLogic() {

  // process SimulationRunner for each scene
  for (auto &scene : m_scene_manager.GetScenes()) {

    auto scene_context = scene.second->GetSceneContext();

    // create SimulationRunner instance for each Scene
    SimulationRunner runner(m_test_data.simulation_data, scene_context);

    // execute simulation
    auto simulation_result = runner.ExecuteSimulation();
    if (!simulation_result.has_value()) {
      // [TODO:] handle simulation failure (log, halt, etc.)
    }
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
  m_data_bank[m_current_tick] = std::move(snapshot);
  return std::monostate{};
}
/////////////////////////////////////////////////
const std::map<size_t, EngineSnapshot> &TestEngine::GetDataBank() const {
  return m_data_bank;
}

} // namespace steamrot::tests
