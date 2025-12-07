/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TestEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestEngine.h"
#include "FailInfo.h"
#include "SceneContext.h"
#include "SceneSnapshot.h"
#include "simulation_runner.h"
#include <expected>
#include <iostream>

namespace steamrot::tests {

/////////////////////////////////////////////////
TestEngine::TestEngine(const TestDataConfig *config)
    : Engine(), m_test_config(config) {
  // Extract simulation data from config if present
  if (m_test_config && m_test_config->simulation_data()) {
    m_simulation_data = m_test_config->simulation_data();
  }

  // Extract num_ticks from config if present
  if (m_test_config && m_test_config->num_ticks() > 0) {
    m_target_ticks = m_test_config->num_ticks();
  }
}

/////////////////////////////////////////////////
void TestEngine::RunGameLoop() {

  // run through the specified number of loops
  for (size_t i{1}; i <= m_target_ticks; i++) {

    // execute all the logic required for a tick, this is derived in the Engine
    // class and reflects the desired Game logic
    ExecuteSystemsTick();

    // add data to data bank
    AddToDataBank(i);
  }
}

/////////////////////////////////////////////////
void TestEngine::AddToDataBank(size_t tick) {
  std::vector<SceneSnapshot> scene_snapshots;

  // Iterate through all scenes in the SceneManager
  for (auto &scene_pair : m_scene_manager.GetScenes()) {
    Scene &scene = *scene_pair.second;

    // Get SceneInfo which has id and type
    const SceneInfo scene_info = scene.GetSceneInfo();

    // Get SceneContext to access the EntityMemoryPool
    SceneContext context = scene.GetSceneContext();

    // Create a SceneSnapshot with both metadata and entity pool
    SceneSnapshot snapshot;
    snapshot.scene_info.id = scene_info.id;
    snapshot.scene_info.type = scene_info.type;
    snapshot.entity_memory_pool = context.scene_entities;

    scene_snapshots.push_back(std::move(snapshot));
  }

  // Store the snapshots in the data bank
  m_data_bank[tick] = std::move(scene_snapshots);
}

/////////////////////////////////////////////////
const std::unordered_map<size_t, std::vector<SceneSnapshot>> &
TestEngine::GetDataBank() const {
  return m_data_bank;
}

/////////////////////////////////////////////////
void TestEngine::TickSceneLogic() {
  // Cycle through scenes from SceneManager
  for (auto &scene_pair : m_scene_manager.GetScenes()) {

    // Get scene
    Scene &scene = *scene_pair.second;
    // Create SceneContext object, this only needs to live as long as the sim
    // runner
    SceneContext scene_context = scene.GetSceneContext();

    // If simulation data is available, use it; otherwise fall back to normal
    // scene updates
    if (m_simulation_data) {
      // Execute simulation using simulation runner
      auto sim_result = ExecuteSimulation(m_simulation_data, scene_context);
      if (!sim_result.has_value()) {
        // Log error
        std::cerr << "Simulation execution failed: "
                  << sim_result.error().message << std::endl;
      }
    } else {
      // Fall back to normal scene updates for simple tests
      m_scene_manager.UpdateScenes();
      break; // UpdateScenes handles all scenes, so we can break
    }
  }
}

} // namespace steamrot::tests
