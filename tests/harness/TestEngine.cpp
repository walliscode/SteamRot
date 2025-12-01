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
#include "simulation_runner.h"
#include <expected>
#include <iostream>
#include <variant>

namespace steamrot::tests {

/////////////////////////////////////////////////
TestEngine::TestEngine(TestDataConfig *config)
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
std::expected<std::monostate, FailInfo>
TestEngine::ConfigureEngineStateFromData() {

  // configure Engine subscribers from test data
  auto configure_subs_result = ConfigureSubscribersFromData(
      m_test_config->starting_engine_state()->subscriptions());
  if (!configure_subs_result.has_value()) {
    return std::unexpected(configure_subs_result.error());
  }

  // configure GameResources from test data

  // configure SceneManager from test data
  auto configure_result = m_scene_manager.ConfigureSceneManagerFromData(
      m_test_config->starting_engine_state()->scene_manager_data());
  if (!configure_result.has_value()) {
    return std::unexpected(configure_result.error());
  }

  return std::monostate{};
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
  std::vector<SceneData> scene_snapshots;

  // Iterate through all scenes in the SceneManager
  for (auto &scene_pair : m_scene_manager.GetScenes()) {
    Scene &scene = *scene_pair.second;

    // Get SceneInfo which has id and type
    const SceneData scene_info = scene.GetSceneInfo();

    // Create a SceneData snapshot
    SceneData snapshot;
    snapshot.id = scene_info.id;
    snapshot.type = scene_info.type;

    // Get SceneContext to access the EntityMemoryPool
    SceneContext context = scene.GetSceneContext();
    // Copy the entity memory pool from the scene context
    snapshot.entity_memory_pool = context.scene_entities;

    scene_snapshots.push_back(std::move(snapshot));
  }

  // Store the snapshots in the data bank
  m_data_bank[tick] = std::move(scene_snapshots);
}

/////////////////////////////////////////////////
const std::unordered_map<size_t, std::vector<SceneData>> &
TestEngine::GetDataBank() const {
  return m_data_bank;
}

/////////////////////////////////////////////////
void TestEngine::ExecuteSceneLevelLogic() {

  // cycle through scenes from SceneManager
  for (auto &scene_pair : m_scene_manager.GetScenes()) {

    // get scene
    Scene &scene = *scene_pair.second;
    // create SceneContext object, this only needs to live as long as the sim
    // runner
    SceneContext scene_context = scene.GetSceneContext();
    // execute simulation using simulation runner
    auto sim_result = ExecuteSimulation(m_simulation_data, scene_context);
    if (!sim_result.has_value()) {
      // log error
      std::cerr << "Simulation execution failed: " << sim_result.error().message
                << std::endl;
    }
  }
}

} // namespace steamrot::tests
