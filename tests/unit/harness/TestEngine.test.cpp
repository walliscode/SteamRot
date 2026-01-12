/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the TestEngine class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestEngine.h"
#include "TestData.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TestEngine initialises with a TestData object", "[TestEngine]") {
  steamrot::TestData test_data;
  steamrot::tests::TestEngine engine(test_data);
  SUCCEED("TestEngine initialised successfully");
}

TEST_CASE("TestEngine::StartUp assigns variables from TestData",
          "[TestEngine]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 5;

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto run_game_result = engine.RunGame();
  if (!run_game_result.has_value()) {
    FAIL("TestEngine::RunGame failed to start: " +
         run_game_result.error().message);
  }

  // Assert
  REQUIRE(engine.GetTargetTicks() == 5);
}

TEST_CASE("TestEngine::GetDataBank returns empty map initially",
          "[unit][TestEngine]") {
  steamrot::TestData test_data;
  steamrot::tests::TestEngine engine(test_data);

  const auto &data_bank = engine.GetDataBank();
  REQUIRE(data_bank.empty());
}

TEST_CASE("TestEngine::RunGame executes specified number of ticks",
          "[unit][TestEngine]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 4;
  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());
  // Assert
  REQUIRE(engine.GetCurrentTick() == 4);
}

TEST_CASE("TestEngine::RunGame captures snapshots in data bank",
          "[unit][TestEngine]") {
  // Arrange
  steamrot::TestData test_data;
  test_data.number_of_ticks = 3;

  // Act
  steamrot::tests::TestEngine engine(test_data);
  auto result = engine.RunGame();
  REQUIRE(result.has_value());

  // Assert
  const auto &data_bank = engine.GetDataBank();
  REQUIRE(data_bank.size() == 3);
  REQUIRE(data_bank.find(1) != data_bank.end());
  REQUIRE(data_bank.find(2) != data_bank.end());
  REQUIRE(data_bank.find(3) != data_bank.end());
  REQUIRE(data_bank.find(4) == data_bank.end());
}
