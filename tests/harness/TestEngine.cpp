/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TestEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestEngine.h"
#include "SceneContext.h"
#include "simulation_runner.h"
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
void TestEngine::Run() {
  // Start up the engine
  auto start_up_result = StartUp();
  if (!start_up_result.has_value()) {
    std::cerr << "TestEngine StartUp failed: "
              << start_up_result.error().message << std::endl;
    return;
  }

  // Run the game loop
  RunGameLoop();
}

/////////////////////////////////////////////////
void TestEngine::RunGameLoop() {
  // run through the specified number of loops
  for (size_t i{1}; i <= m_target_ticks; i++) {
    // Update current tick number
    m_current_tick = i;

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
TestEngine &TestEngine::SetTicks(size_t num_ticks) {
  m_target_ticks = num_ticks;
  return *this;
}

/////////////////////////////////////////////////
TestEngine &TestEngine::UseFullScene(SceneType scene_type) {
  m_tick_level = TickLevel::FullEngine;
  m_scene_type = scene_type;
  return *this;
}

/////////////////////////////////////////////////
TestEngine &TestEngine::UseTickLevel(TickLevel level) {
  m_tick_level = level;
  return *this;
}

/////////////////////////////////////////////////
TestEngine &TestEngine::SetSceneType(SceneType scene_type) {
  m_scene_type = scene_type;
  return *this;
}

/////////////////////////////////////////////////
TestEngine &TestEngine::AddFunction(TestFunction func,
                                    const std::string &name) {
  m_custom_functions.push_back(NamedTestFunction{std::move(func), name});
  return *this;
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

    // Execute based on tick level
    switch (m_tick_level) {
    case TickLevel::FullEngine:
    case TickLevel::SceneManager:
    case TickLevel::SceneLogic:
      // For these levels, execute simulation from config
      if (m_simulation_data) {
        auto sim_result = ExecuteSimulation(m_simulation_data, scene_context);
        if (!sim_result.has_value()) {
          std::cerr << "Simulation execution failed: "
                    << sim_result.error().message << std::endl;
        }
      }
      break;

    case TickLevel::Custom:
      // Execute simulation if present
      if (m_simulation_data) {
        auto sim_result = ExecuteSimulation(m_simulation_data, scene_context);
        if (!sim_result.has_value()) {
          std::cerr << "Simulation execution failed: "
                    << sim_result.error().message << std::endl;
        }
      }

      // Execute custom functions
      for (const auto &named_func : m_custom_functions) {
        named_func.function(scene_context);
      }
      break;
    }
  }
}
} // namespace steamrot::tests
