/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TestEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestEngine.h"
#include <expected>

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

    // capture snapshot after tick execution
    auto capture_result = CaptureSnapShot(i);

    // proceed to next tick if not greater than target
    if (i < m_target_ticks) {
      m_current_tick++;
    }
  }
}

/////////////////////////////////////////////////
void TestEngine::TickSceneLogic() {}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
TestEngine::CaptureSnapShot(size_t tick) {
  EngineSnapshot snapshot;
  // Set tick number for context
  snapshot.tick_number = tick;

  // Capture scene entity state
  const auto &scenes = m_scene_manager.GetScenes();
  std::unordered_map<uuids::uuid, EntityMemoryPool> scene_map;

  for (const auto &[scene_uuid, scene_ptr] : scenes) {
    if (scene_ptr && scene_ptr->GetActive()) {
      // Get the EntityMemoryPool from the scene
      const EntityMemoryPool &scene_pool =
          scene_ptr->GetEntityManager().GetEntityMemoryPool();

      // Deep copy the pool with scene UUID as key
      scene_map[scene_uuid] = scene_pool;
    }
  }

  if (!scene_map.empty()) {
    snapshot.scene_snapshots = std::move(scene_map);
  }

  // Capture global event bus state
  snapshot.global_event_bus =
      m_engine_resources.event_handler.GetGlobalEventBus();

  // Store the complete snapshot in the data bank
  m_data_bank[tick] = std::move(snapshot);

  return std::monostate{};
}
/////////////////////////////////////////////////
const std::map<size_t, EngineSnapshot> &TestEngine::GetDataBank() const {
  return m_data_bank;
}

} // namespace steamrot::tests
