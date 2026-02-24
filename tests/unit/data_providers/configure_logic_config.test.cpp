/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the logic config configuration logic.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_logic_config.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ConfigureLogicConfig returns unexpected when given null pointer",
          "[ConfigureLogicConfig]") {
  // Arrange
  steamrot::LogicConfig logic_config;
  // Act
  auto result =
      steamrot::data::configure::ConfigureLogicConfig(logic_config, nullptr);
  // Assert
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().message == "LogicConfigFbs pointer is null.");
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}
