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

TEST_CASE("GameEngine initializes correctly", "[GameEngine]") {
  // Create a GameEngine instance
  steamrot::GameEngine game_engine(steamrot::EnvironmentType::Test);

  SUCCEED("GameEngine instance created successfully");
}

TEST_CASE("GameEngine runs for a set number of frames", "[GameEngine]") {
  // set a few different frame counts to test using Catch2's GENERATE
  size_t frame_count = GENERATE(1, 5, 10);
  // Create a GameEngine instance
  steamrot::GameEngine game_engine(steamrot::EnvironmentType::Test);

  // run in simulation mode to limit the number of frames
  game_engine.RunGame(frame_count, true);
  // check that the game loop ran for the specified number of frames
  REQUIRE(game_engine.GetLoopNumber() == frame_count);
  SUCCEED("GameEngine ran for the specified number of frames");
}
