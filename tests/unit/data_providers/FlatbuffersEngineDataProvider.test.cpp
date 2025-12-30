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

TEST_CASE("FlatbuffersEngineDataProvider::LoadEngineData loads correctly",
          "[unit][FlatbuffersEngineDataProvider]") {
  steamrot::FlatbuffersEngineDataProvider provider;
  auto result = provider.LoadEngineData();
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  const auto &data = result.value();
  // Check EngineResourcesConfig
  const auto &resources_config = data.engine_resources_config;
  REQUIRE(resources_config.window_width == 800);
  REQUIRE(resources_config.window_height == 600);
  REQUIRE(resources_config.window_title == "SteamRot");
  REQUIRE(resources_config.framerate_limit == 60);

  // Check EngineConfig
  const auto &engine_config = data.engine_config;
  REQUIRE(engine_config.display.window_title == "SteamRot");
  REQUIRE(engine_config.display.framerate_limit == 60);
  REQUIRE(engine_config.display.fullscreen == false);
  REQUIRE(engine_config.display.vsync == true);

  // Check EngineState
  const auto &engine_state = data.engine_state;
  REQUIRE(engine_state.running == false);
  REQUIRE(engine_state.paused == false);
  REQUIRE(engine_state.quit_requested == false);
  REQUIRE(engine_state.subscriptions.size() == 1);
  REQUIRE(engine_state.subscriptions[0]->m_trigger_event_type ==
          steamrot::EventType_EVENT_QUIT_GAME);

  // Check AssetConfig
  const auto &asset_config = data.initial_asset_config;
  REQUIRE(asset_config.fonts.size() == 2);

  std::vector<std::string> expected_font_names = {
      "DaddyTimeMonoNerdFont-Regular", "Roboto-Regular"};

  for (size_t i = 0; i < asset_config.fonts.size(); ++i) {
    REQUIRE(asset_config.fonts[i].name == expected_font_names[i]);
  }

  // verify styles
  REQUIRE(asset_config.ui_styles.size() == 1);
}
