/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersSaveDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSaveDataProvider.h"
#include "SaveData.h"
#include "SaveMetadata.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersSaveDataProvider::SaveExists returns false for "
          "non-existent save",
          "[unit][FlatbuffersSaveDataProvider]") {
  steamrot::FlatbuffersSaveDataProvider provider;

  // Test a slot that definitely doesn't exist
  REQUIRE_FALSE(provider.SaveExists(999));
}

TEST_CASE("FlatbuffersSaveDataProvider::SaveExists returns true for existing "
          "save",
          "[unit][FlatbuffersSaveDataProvider]") {
  steamrot::FlatbuffersSaveDataProvider provider;

  // Test slot 0 which should have test data
  // Note: This test assumes save_slot_0.json exists and has been compiled
  REQUIRE(provider.SaveExists(0));
}

TEST_CASE("FlatbuffersSaveDataProvider::GetSaveMetadata returns error for "
          "non-existent save",
          "[unit][FlatbuffersSaveDataProvider]") {
  steamrot::FlatbuffersSaveDataProvider provider;

  auto result = provider.GetSaveMetadata(999);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FileNotFound);
}

TEST_CASE("FlatbuffersSaveDataProvider::GetSaveMetadata returns metadata for "
          "existing save",
          "[unit][FlatbuffersSaveDataProvider]") {
  steamrot::FlatbuffersSaveDataProvider provider;

  auto result = provider.GetSaveMetadata(0);
  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &metadata = result.value();
  REQUIRE(metadata.save_name == "Test Save");
  REQUIRE(metadata.game_version == "1.0.0");
  REQUIRE(metadata.play_time_seconds == 3600);
  REQUIRE(metadata.slot_index == 0);
}

TEST_CASE("FlatbuffersSaveDataProvider::LoadSaveData returns error for "
          "non-existent save",
          "[unit][FlatbuffersSaveDataProvider]") {
  steamrot::FlatbuffersSaveDataProvider provider;

  auto result = provider.LoadSaveData(999);
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FileNotFound);
}

TEST_CASE("FlatbuffersSaveDataProvider::LoadSaveData returns complete save "
          "data",
          "[unit][FlatbuffersSaveDataProvider]") {
  steamrot::FlatbuffersSaveDataProvider provider;

  auto result = provider.LoadSaveData(0);
  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  const auto &save_data = result.value();

  // Check metadata
  REQUIRE(save_data.metadata.save_name == "Test Save");
  REQUIRE(save_data.metadata.game_version == "1.0.0");
  REQUIRE(save_data.metadata.slot_index == 0);

  // Check scene data exists
  REQUIRE(save_data.scene_data != nullptr);
  REQUIRE(save_data.scene_data->scene_info.type ==
          steamrot::SceneType::SceneType_TITLE);

  // Check engine state
  REQUIRE(save_data.engine_state.running == true);
  REQUIRE(save_data.engine_state.paused == false);
  REQUIRE(save_data.engine_state.quit_requested == false);

  // Check scene state
  REQUIRE(save_data.scene_state.active == true);
}
