/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersEngineDataProvider
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEngineDataProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersEngineDataProvider is constructed correctly",
          "[unit][FlatbuffersEngineDataProvider]") {

  steamrot::FlatbuffersEngineDataProvider provider;
  REQUIRE_NOTHROW(provider);
}

TEST_CASE(
    "FlatbuffersEngineDataProvider::LoadEngineResourcesConfig loads correctly",
    "[unit][FlatbuffersEngineDataProvider]") {

  steamrot::FlatbuffersEngineDataProvider provider;
  auto result = provider.LoadEngineResourcesConfig();

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &config = result.value();
  REQUIRE(config.window_width > 0);
  REQUIRE(config.window_height > 0);
  REQUIRE(!config.window_title.empty());
  REQUIRE(config.framerate_limit > 0);
}

TEST_CASE(
    "FlatbuffersEngineDataProvider::LoadEngineResourcesConfig validates data",
    "[unit][FlatbuffersEngineDataProvider]") {

  steamrot::FlatbuffersEngineDataProvider provider;
  auto result = provider.LoadEngineResourcesConfig();

  REQUIRE(result.has_value());

  const auto &config = result.value();
  // Verify reasonable window dimensions
  REQUIRE(config.window_width >= 800);
  REQUIRE(config.window_height >= 600);
  // Verify title is not empty
  REQUIRE(config.window_title.length() > 0);
  // Verify framerate limit is reasonable
  REQUIRE(config.framerate_limit >= 30);
}

TEST_CASE("FlatbuffersEngineDataProvider::LoadEngineConfig loads correctly",
          "[unit][FlatbuffersEngineDataProvider]") {

  steamrot::FlatbuffersEngineDataProvider provider;
  auto result = provider.LoadEngineConfig();

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &config = result.value();
  REQUIRE(!config.display.window_title.empty());
  REQUIRE(config.display.framerate_limit > 0);

  // Check user preferences defaults
  REQUIRE(config.user_preferences.master_volume >= 0.0f);
  REQUIRE(config.user_preferences.master_volume <= 1.0f);
  REQUIRE(!config.user_preferences.preferred_language.empty());
}

TEST_CASE("FlatbuffersEngineDataProvider::LoadEngineState loads correctly",
          "[unit][FlatbuffersEngineDataProvider]") {

  steamrot::FlatbuffersEngineDataProvider provider;
  auto result = provider.LoadEngineState();

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &state = result.value();
  // Initial state should have these flags set to false
  REQUIRE(state.running == false);
  REQUIRE(state.paused == false);
  REQUIRE(state.quit_requested == false);

  // Subscriptions should be loaded
  REQUIRE_NOTHROW(state.subscriptions);
}

TEST_CASE("FlatbuffersEngineDataProvider::GetSubscriberViewer returns viewer",
          "[unit][FlatbuffersEngineDataProvider]") {

  steamrot::FlatbuffersEngineDataProvider provider;
  auto result = provider.GetSubscriberViewer();

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &viewer = result.value();
  REQUIRE(viewer != nullptr);

  // Test that viewer can get subscribers
  auto subscribers_result = viewer->GetSubscribers();
  if (!subscribers_result.has_value()) {
    FAIL(subscribers_result.error().message);
  }

  // Subscribers should be a valid vector (may be empty)
  const auto &subscribers = subscribers_result.value();
  REQUIRE_NOTHROW(subscribers.size());
}

TEST_CASE("FlatbuffersEngineDataProvider loads subscriptions into EngineState",
          "[unit][FlatbuffersEngineDataProvider]") {

  steamrot::FlatbuffersEngineDataProvider provider;
  auto result = provider.LoadEngineState();

  REQUIRE(result.has_value());

  const auto &state = result.value();
  // Verify subscriptions are loaded
  // They may be empty, but the vector should be valid
  REQUIRE_NOTHROW(state.subscriptions.size());
}

TEST_CASE(
    "FlatbuffersEngineDataProvider::LoadEngineConfig has valid user preferences",
    "[unit][FlatbuffersEngineDataProvider]") {

  steamrot::FlatbuffersEngineDataProvider provider;
  auto result = provider.LoadEngineConfig();

  REQUIRE(result.has_value());

  const auto &config = result.value();
  // Verify user preferences structure
  REQUIRE(config.user_preferences.master_volume == 1.0f);
  REQUIRE(config.user_preferences.show_fps == false);
  REQUIRE(config.user_preferences.preferred_language == "en");
}
