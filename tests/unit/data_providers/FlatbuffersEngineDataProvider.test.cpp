/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersEngineDataProvider
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersEngineDataProvider.h"
#include "events_generated.h"
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
  REQUIRE(config.window_width == 800);
  REQUIRE(config.window_height == 600);
  REQUIRE(config.window_title == "SteamRot");
  REQUIRE(config.framerate_limit == 60);
}

TEST_CASE("FlatbuffersEngineDataProvider::LoadEngineConfig loads correctly",
          "[unit][FlatbuffersEngineDataProvider]") {

  steamrot::FlatbuffersEngineDataProvider provider;
  auto result = provider.LoadEngineConfig();

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &config = result.value();
  REQUIRE(config.display.window_title == "SteamRot");
  REQUIRE(config.display.framerate_limit == 60);
  REQUIRE(config.display.fullscreen == false);
  REQUIRE(config.display.vsync == true);

  // Check user preferences defaults
  REQUIRE(config.user_preferences.master_volume >= 0.0f);
  REQUIRE(config.user_preferences.master_volume <= 1.0f);
  REQUIRE(!config.user_preferences.preferred_language.empty());
  REQUIRE(config.user_preferences.show_fps == false);
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
  REQUIRE(state.subscriptions.size() == 1);
  REQUIRE(state.subscriptions[0]->m_trigger_event_type ==
          steamrot::EventType_EVENT_QUIT_GAME);
}

TEST_CASE("FlatbuffersEngineDataProvider::GetSubscriberViewer returns viewer ",
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

  // check subscribers for size and specific data. the Engine should have a
  // specific set of subscribers
  REQUIRE(subscribers_result.value().size() == 1);
  REQUIRE(subscribers_result.value()[0]->m_trigger_event_type ==
          steamrot::EventType_EVENT_QUIT_GAME);
}
