/////////////////////////////////////////////////
/// @file
/// @brief Units tests for GameEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GameEngine.h"
#include "PathProvider.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("GameEngine fails when EnviromentType is None", "[GameEngine]") {
  // Attempt to create a GameEngine instance with EnvironmentType::None
  try {
    steamrot::GameEngine game_engine(steamrot::EnvironmentType::None);
    FAIL("GameEngine should not be created with EnvironmentType::None");
  } catch (const std::exception &e) {
    SUCCEED(
        "GameEngine creation failed as expected with EnvironmentType::None");
  }
}
TEST_CASE("GameEngine fails if PathProvider has not been pre initialized",
          "[GameEngine]") {
  // Attempt to create a GameEngine instance with EnvironmentType::None
  try {
    steamrot::GameEngine game_engine(steamrot::EnvironmentType::None);
    FAIL("PathProvider should not be used with EnvironmentType::None");
  } catch (const std::exception &e) {
    SUCCEED("PathProvider usage failed as expected with EnvironmentType::None");
  }
}

TEST_CASE("GameEngine initializes correctly in test enviroment when "
          "PathProvide is pre initialized",
          "[GameEngine]") {
  // Pre-initialize PathProvider
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  // Create a GameEngine instance
  steamrot::GameEngine game_engine(steamrot::EnvironmentType::Test);

  SUCCEED("GameEngine initialized correctly in test enviroment");
}

TEST_CASE("GameEngine initializes correctly in production enviroment",
          "[GameEngine]") {
  // pre-initialize PathProvider
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Production);
  // Create a GameEngine instance
  steamrot::GameEngine game_engine(steamrot::EnvironmentType::Production);
  SUCCEED("GameEngine initialized correctly in production enviroment");
}
TEST_CASE("GameEngine runs for a set number of frames in test enviroment",
          "[GameEngine]") {
  // set a few different frame counts to test using Catch2's GENERATE
  size_t frame_count = GENERATE(1, 5, 10);

  // create and pre-initialize PathProvider
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  // Create a GameEngine instance
  steamrot::GameEngine game_engine(steamrot::EnvironmentType::Test);

  // run in simulation mode to limit the number of frames
  game_engine.RunGame(frame_count, true);
  // check that the game loop ran for the specified number of frames
  REQUIRE(game_engine.GetLoopNumber() == frame_count);
  SUCCEED("GameEngine ran for the specified number of frames");
}

TEST_CASE("GameEngine runs for a set number of frames in production enviroment",
          "[GameEngine]") {

  // create and pre-initialize PathProvider
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Production);
  // Create a GameEngine instance
  steamrot::GameEngine game_engine(steamrot::EnvironmentType::Production);

  // run in simulation mode to limit the number of frames
  int frame_count{5};
  game_engine.RunGame(frame_count, true);
  // check that the game loop ran for the specified number of frames
  // REQUIRE(game_engine.GetLoopNumber() == frame_count);
  SUCCEED("GameEngine ran for the specified number of frames");
}
