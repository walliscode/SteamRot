/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TestEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestEngine.h"
#include "simulation_runner.h"
#include <iostream>

namespace steamrot::tests {

/////////////////////////////////////////////////
TestEngine::TestEngine(const TestDataConfig *config)
    : Engine(), m_test_config(config) {}

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
void TestEngine::AddToDataBank(size_t tick) {}

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
