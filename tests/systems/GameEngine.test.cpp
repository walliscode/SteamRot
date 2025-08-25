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

TEST_CASE("GameEngine initializes correctly", "[GameEngine]") {
  // Create a GameEngine instance
  steamrot::GameEngine game_engine(steamrot::EnvironmentType::Test);

  SUCCEED("GameEngine instance created successfully");
}
