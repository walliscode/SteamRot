/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersEngineDataProvider
/////////////////////////////////////////////////

#include "FlatbuffersEngineDataProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersEngineDataProvider loads engine resources config",
          "[unit][data_providers][FlatbuffersEngineDataProvider]") {
  steamrot::FlatbuffersEngineDataProvider provider;

  auto result = provider.LoadEngineResourcesConfig();
  REQUIRE(result.has_value());

  const auto &data = result.value();
  REQUIRE(data.window_width > 0);
  REQUIRE(data.window_height > 0);
  REQUIRE(!data.window_title.empty());
  REQUIRE(data.framerate_limit > 0);
}

TEST_CASE("FlatbuffersEngineDataProvider returns mutable native struct",
          "[unit][data_providers][FlatbuffersEngineDataProvider]") {
  steamrot::FlatbuffersEngineDataProvider provider;

  auto result = provider.LoadEngineResourcesConfig();
  REQUIRE(result.has_value());

  // Should be able to mutate the result
  auto data = result.value();
  data.window_width = 1920;
  data.window_title = "Modified";
  REQUIRE(data.window_width == 1920);
  REQUIRE(data.window_title == "Modified");
}

TEST_CASE("FlatbuffersEngineDataProvider loads engine config",
          "[unit][data_providers][FlatbuffersEngineDataProvider]") {
  steamrot::FlatbuffersEngineDataProvider provider;

  auto result = provider.LoadEngineConfig();
  REQUIRE(result.has_value());

  const auto &config = result.value();
  REQUIRE(!config.display.window_title.empty());
  REQUIRE(config.display.framerate_limit > 0);
  REQUIRE(config.user_preferences.master_volume >= 0.0f);
  REQUIRE(config.user_preferences.master_volume <= 1.0f);
  REQUIRE(!config.user_preferences.preferred_language.empty());
}

TEST_CASE("FlatbuffersEngineDataProvider loads engine state",
          "[unit][data_providers][FlatbuffersEngineDataProvider]") {
  steamrot::FlatbuffersEngineDataProvider provider;

  auto result = provider.LoadEngineState();
  REQUIRE(result.has_value());

  const auto &state = result.value();
  // Initial state should have flags set to false
  REQUIRE(state.running == false);
  REQUIRE(state.paused == false);
  REQUIRE(state.quit_requested == false);
}

TEST_CASE("FlatbuffersEngineDataProvider provides subscriber viewer",
          "[unit][data_providers][FlatbuffersEngineDataProvider]") {
  steamrot::FlatbuffersEngineDataProvider provider;

  // Should be able to get viewer from the provider
  auto viewer_result = provider.GetSubscriptions();
  REQUIRE(viewer_result.has_value());

  // Should be able to call GetSubscriberConfigs through the viewer
  const auto &viewer = viewer_result.value();
  auto configs_result = viewer->GetSubscriberConfigs();
  REQUIRE(configs_result.has_value());
}
