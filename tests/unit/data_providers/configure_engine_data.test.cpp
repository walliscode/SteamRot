/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for configure_engine_data functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_engine_data.h"
#include "EngineConfig.h"
#include "EngineResourcesConfig.h"
#include "EngineState.h"
#include "EventPayload.h"
#include "FlatbuffersDataLoader.h"
#include <catch2/catch_test_macros.hpp>
#include <variant>

/////////////////////////////////////////////////
// ConfigureEngineResourcesConfig tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureEngineResourcesConfig fails with null data",
          "[unit][configure_engine_data]") {
  steamrot::EngineResourcesConfig config;

  auto result = steamrot::data::configure::ConfigureEngineResourcesConfig(
      config, nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureEngineResourcesConfig populates from valid data",
          "[unit][configure_engine_data]") {
  // Load real flatbuffers data
  steamrot::FlatbuffersDataLoader loader;
  auto engine_data_result = loader.ProvideEngineDataFbs();
  REQUIRE(engine_data_result.has_value());

  const auto *engine_data = engine_data_result.value();
  const auto *resources_config_data = engine_data->engine_resources_config();
  REQUIRE(resources_config_data != nullptr);

  steamrot::EngineResourcesConfig config;
  auto result = steamrot::data::configure::ConfigureEngineResourcesConfig(
      config, resources_config_data);

  REQUIRE(result.has_value());
  REQUIRE(config.window_width == 800);
  REQUIRE(config.window_height == 600);
  REQUIRE(config.window_title == "SteamRot");
  REQUIRE(config.framerate_limit == 60);
}

/////////////////////////////////////////////////
// ConfigureEngineConfig tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureEngineConfig fails with null data",
          "[unit][configure_engine_data]") {
  steamrot::EngineConfig config;

  auto result =
      steamrot::data::configure::ConfigureEngineConfig(config, nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureEngineConfig populates from valid data",
          "[unit][configure_engine_data]") {
  // Load real flatbuffers data
  steamrot::FlatbuffersDataLoader loader;
  auto engine_data_result = loader.ProvideEngineDataFbs();
  REQUIRE(engine_data_result.has_value());

  const auto *engine_data = engine_data_result.value();
  const auto *engine_config_data = engine_data->engine_config();
  REQUIRE(engine_config_data != nullptr);

  steamrot::EngineConfig config;
  auto result = steamrot::data::configure::ConfigureEngineConfig(
      config, engine_config_data);

  REQUIRE(result.has_value());
  REQUIRE(config.display.window_title == "SteamRot");
  REQUIRE(config.display.framerate_limit == 60);
  REQUIRE(config.display.fullscreen == false);
  REQUIRE(config.display.vsync == true);
  REQUIRE(config.user_preferences.master_volume == 1.0f);
  REQUIRE(config.user_preferences.show_fps == false);
  REQUIRE(config.user_preferences.preferred_language == "en");
}

/////////////////////////////////////////////////
// ConfigureEngineState tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureEngineState fails with null data",
          "[unit][configure_engine_data]") {
  steamrot::EngineState state;

  auto result = steamrot::data::configure::ConfigureEngineState(state, nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("ConfigureEngineState populates from valid data",
          "[unit][configure_engine_data]") {
  // Load real flatbuffers data
  steamrot::FlatbuffersDataLoader loader;
  auto engine_data_result = loader.ProvideEngineDataFbs();
  REQUIRE(engine_data_result.has_value());

  const auto *engine_data = engine_data_result.value();
  const auto *engine_state_data = engine_data->engine_state();
  REQUIRE(engine_state_data != nullptr);

  steamrot::EngineState state;
  auto result =
      steamrot::data::configure::ConfigureEngineState(state, engine_state_data);

  REQUIRE(result.has_value());
  REQUIRE(state.running == false);
  REQUIRE(state.paused == false);
  REQUIRE(state.quit_requested == false);
  REQUIRE(state.subscriptions.size() == 1);
  REQUIRE(state.subscriptions[0]->event_type == steamrot::EventType::SYSTEM);
  REQUIRE(std::holds_alternative<steamrot::SystemPayload>(
      state.subscriptions[0]->filter_payload));
  REQUIRE(
      std::get<steamrot::SystemPayload>(state.subscriptions[0]->filter_payload)
          .action == steamrot::SystemPayload::SystemAction::QUIT);
}
