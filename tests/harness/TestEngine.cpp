/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TestEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestEngine.h"

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
const std::unordered_map<size_t, std::vector<SceneInfo>> &
TestEngine::GetDataBank() const {
  return m_data_bank;
}
} // namespace steamrot::tests
