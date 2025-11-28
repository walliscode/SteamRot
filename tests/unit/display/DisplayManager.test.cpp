/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for DisplayManager class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DisplayManager.h"
#include "PathProvider.h"
#include "SceneManager.h"
#include "TestEngine.h"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

TEST_CASE("DisplayManager Initializes with SceneManager", "[unit][DisplayManager]") {

  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  steamrot::tests::TestEngine test_engine(nullptr);
  auto init_result = test_engine.Initialize();
  REQUIRE(init_result.has_value());
  std::cout << "TestEngine initialized successfully" << std::endl;
  steamrot::GameContext &game_context =
      test_engine
          .GetGameContext(); // Get the game context from the test engine
  steamrot::SceneManager scene_manager{test_engine.GetGameContext()};

  std::cout << "SceneManager initialized successfully" << std::endl;
  steamrot::DisplayManager display_manager{game_context.game_window,
                                           scene_manager};

  SUCCEED("DisplayManager initialized successfully");
}

TEST_CASE("DisplayManager Render Cycle", "[unit][DisplayManager]") {
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  steamrot::tests::TestEngine test_engine(nullptr);
  auto init_result = test_engine.Initialize();
  REQUIRE(init_result.has_value());
  steamrot::GameContext game_context =
      test_engine
          .GetGameContext(); // Get the game context from the test context
  steamrot::SceneManager scene_manager{test_engine.GetGameContext()};
  steamrot::DisplayManager display_manager{game_context.game_window,
                                           scene_manager};
  auto result = display_manager.CallRenderCycle();
  if (!result.has_value()) {
    FAIL(result.error().message); // If there's an error, fail the test
  }
}
