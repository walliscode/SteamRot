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
  }
}

/////////////////////////////////////////////////
void TestEngine::TickSceneLogic() {}

} // namespace steamrot::tests
