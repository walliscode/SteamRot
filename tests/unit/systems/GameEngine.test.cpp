/////////////////////////////////////////////////
/// @file
/// @brief Units tests for GameEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GameEngine.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Mouse.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("GameEngine initializes correctly", "[unit][GameEngine]") {
  steamrot::GameEngine game_engine;

  SUCCEED("GameEngine initialized correctly");
}

TEST_CASE("GameEngine::ConfigureSubscribersFromData fails on null data",
          "[unit][GameEngine]") {
  steamrot::GameEngine game_engine;
  // Attempt to configure subscribers from null data
  auto configure_result = game_engine.ConfigureSubscribersFromData(nullptr);
  // Check that the configuration failed
  REQUIRE(!configure_result.has_value());
  REQUIRE(configure_result.error().mode == steamrot::FailMode::NullPointer);
  REQUIRE(game_engine.GetSubscriptions().empty());
}
