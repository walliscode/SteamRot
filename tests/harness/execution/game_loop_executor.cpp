/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Level 4 GameLoop executor
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "game_loop_executor.h"
#include "DisplayManager.h"
#include "TestFixture.h"
#include "event_processing.h"
#include "game_loop.h"
#include "scene_manager_executor.h"

namespace steamrot::tests::execution {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteHeadlessGameLoop(GameResources &game_resources,
                        SceneManager &scene_manager, size_t num_iterations) {

  for (size_t iteration = 0; iteration < num_iterations; ++iteration) {
    // Use the extracted game_loop function for headless iteration
    game_loop::ExecuteHeadlessIteration(game_resources, scene_manager);
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteFullGameLoop(GameResources &game_resources, SceneManager &scene_manager,
                    DisplayManager &display_manager, size_t num_iterations) {

  for (size_t iteration = 0; iteration < num_iterations; ++iteration) {
    // Use the extracted game_loop function for full iteration
    game_loop::ExecuteFullIteration(game_resources, scene_manager,
                                    display_manager);
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteConfiguredGameLoop(GameResources &game_resources,
                          SceneManager &scene_manager,
                          const GameLoopConfig &config) {

  for (size_t iteration = 0; iteration < config.num_iterations; ++iteration) {

    // TODO: Inject events for this iteration if config.events is provided
    // This would use the event simulation infrastructure

    // TODO: Inject inputs for this iteration if config.inputs is provided
    // This would use the input simulation infrastructure

    if (config.headless) {
      // Execute headless iteration
      game_loop::ExecuteHeadlessIteration(game_resources, scene_manager);
    } else {
      // For non-headless mode, caller needs to provide DisplayManager
      // Fall back to headless for now
      game_loop::ExecuteHeadlessIteration(game_resources, scene_manager);
    }
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteGameLoopWithFixture(TestFixture &fixture, const GameLoopConfig &config) {

  // TestFixture provides GameResources but doesn't have SceneManager
  // For Level 4 testing with TestFixture, we simulate the game loop pattern
  for (size_t iteration = 0; iteration < config.num_iterations; ++iteration) {

    // TODO: Inject events for this iteration if config.events is provided

    // TODO: Inject inputs for this iteration if config.inputs is provided

    // 1. Update game resources
    game_loop::UpdateGameResources(fixture.GetGameResources());

    // 2. Process event tick start
    events::processing::ProcessEventTickStart(
        fixture.GetGameResources().event_handler, nullptr);

    // Note: TestFixture doesn't have a SceneManager, so we skip scene updates
    // Tests using Level 4 with actual GameEngine should use
    // ExecuteHeadlessGameLoop or ExecuteConfiguredGameLoop directly

    // 3. Process event tick end
    events::processing::ProcessEventTickEnd(
        fixture.GetGameResources().event_handler);
  }

  return std::monostate{};
}

} // namespace steamrot::tests::execution
