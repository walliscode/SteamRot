/////////////////////////////////////////////////
/// @file
/// @brief Level 4 executor for full game loop execution
///
/// This executor provides the highest level of test execution,
/// running complete game loop iterations including all subsystems.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "GameResources.h"
#include "SceneManager.h"
#include "event_test_data_generated.h"
#include "input_test_data_generated.h"
#include <expected>

// Forward declarations
namespace steamrot {
class DisplayManager;
}

namespace steamrot::tests {
class TestFixture;
}

namespace steamrot::tests::execution {

/////////////////////////////////////////////////
/// @brief Configuration for game loop execution
/////////////////////////////////////////////////
struct GameLoopConfig {
  /// Number of loop iterations to execute (default: 1)
  size_t num_iterations = 1;

  /// Run in headless mode without display (default: true for tests)
  bool headless = true;

  /// Event sequence to inject (optional)
  const EventSequence *events = nullptr;

  /// Input sequence to inject (optional)
  const InputSequence *inputs = nullptr;
};

/////////////////////////////////////////////////
/// @brief Execute game loop iterations in headless mode
///
/// Runs complete game loop iterations without display rendering:
/// 1. Update game resources
/// 2. Process event tick start
/// 3. Update scene manager
/// 4. Process event tick end
///
/// @param game_resources Reference to GameResources
/// @param scene_manager Reference to SceneManager
/// @param num_iterations Number of iterations to execute
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteHeadlessGameLoop(GameResources &game_resources,
                        SceneManager &scene_manager,
                        size_t num_iterations = 1);

/////////////////////////////////////////////////
/// @brief Execute game loop iterations with display
///
/// Runs complete game loop iterations including display rendering:
/// 1. Update game resources
/// 2. Process event tick start
/// 3. Update scene manager
/// 4. Call display manager render cycle
/// 5. Process event tick end
///
/// @param game_resources Reference to GameResources
/// @param scene_manager Reference to SceneManager
/// @param display_manager Reference to DisplayManager
/// @param num_iterations Number of iterations to execute
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteFullGameLoop(GameResources &game_resources, SceneManager &scene_manager,
                    DisplayManager &display_manager, size_t num_iterations = 1);

/////////////////////////////////////////////////
/// @brief Execute configured game loop
///
/// Handles event/input injection and runs game loop iterations
/// according to the configuration.
///
/// @param game_resources Reference to GameResources
/// @param scene_manager Reference to SceneManager
/// @param config Configuration specifying how to execute
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteConfiguredGameLoop(GameResources &game_resources,
                          SceneManager &scene_manager,
                          const GameLoopConfig &config);

/////////////////////////////////////////////////
/// @brief Execute game loop using TestFixture
///
/// Creates a lightweight game loop without full GameEngine.
/// Uses TestFixture's GameResources for headless execution.
///
/// @param fixture Reference to the TestFixture
/// @param config Configuration specifying how to execute
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteGameLoopWithFixture(TestFixture &fixture, const GameLoopConfig &config);

} // namespace steamrot::tests::execution
