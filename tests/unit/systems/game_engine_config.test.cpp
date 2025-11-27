/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for game_engine_config.h
/////////////////////////////////////////////////

#include "game_engine_config.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("GameEngineConfig has test mode values", "[unit][config]") {
  // In test builds, STEAMROT_TEST_MODE is defined
  // so we should see test configuration values

  SECTION("Runtime logic injection is enabled in test mode") {
    REQUIRE(steamrot::config::GameEngineConfig::ENABLE_RUNTIME_LOGIC_INJECTION ==
            true);
  }

  SECTION("Data injection is enabled in test mode") {
    REQUIRE(steamrot::config::GameEngineConfig::ENABLE_DATA_INJECTION == true);
  }

  SECTION("Headless mode is enabled in test mode") {
    REQUIRE(steamrot::config::GameEngineConfig::ENABLE_HEADLESS_MODE == true);
  }

  SECTION("Default environment type is Test in test mode") {
    REQUIRE(steamrot::config::GameEngineConfig::DEFAULT_ENV_TYPE ==
            steamrot::EnvironmentType::Test);
  }
}

TEST_CASE("IsConfigEnabled helper works correctly", "[unit][config]") {
  SECTION("Returns true for enabled options") {
    REQUIRE(steamrot::config::IsConfigEnabled<true>() == true);
  }

  SECTION("Returns false for disabled options") {
    REQUIRE(steamrot::config::IsConfigEnabled<false>() == false);
  }

  SECTION("Works with config values") {
    REQUIRE(steamrot::config::IsConfigEnabled<
            steamrot::config::GameEngineConfig::ENABLE_HEADLESS_MODE>() == true);
  }
}
