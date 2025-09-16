/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for DisplayManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DisplayManager.h"
#include "SceneManager.h"
#include "TestContext.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

TEST_CASE("DisplayManager Initializes with SceneManager", "[DisplayManager]") {

  steamrot::tests::TestContext test_context;
  std::cout << "TestContext initialized successfully" << std::endl;
  steamrot::GameContext &game_context =
      test_context
          .GetGameContext(); // Get the game context from the test context
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};

  std::cout << "SceneManager initialized successfully" << std::endl;
  steamrot::DisplayManager display_manager{game_context.game_window,
                                           scene_manager};

  SUCCEED("DisplayManager initialized successfully");
}

TEST_CASE("DisplayManager Render Cycle", "[DisplayManager]") {
  steamrot::tests::TestContext test_context;
  steamrot::GameContext game_context =
      test_context
          .GetGameContext(); // Get the game context from the test context
  steamrot::SceneManager scene_manager{test_context.GetGameContext()};
  steamrot::DisplayManager display_manager{game_context.game_window,
                                           scene_manager};
  auto result = display_manager.CallRenderCycle();
  if (!result.has_value()) {
    FAIL(result.error().message); // If there's an error, fail the test
  }
}
