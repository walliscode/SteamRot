/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersSaveDataProvider class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSaveDataProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersSaveDataProvider is constructed correctly",
          "[unit][FlatbuffersSaveDataProvider]") {

  steamrot::FlatbuffersSaveDataProvider provider;
  REQUIRE_NOTHROW(provider);
}

TEST_CASE("FlatbuffersSaveDataProvider::CreateSaveData returns error when no "
          "save file available",
          "[unit][FlatbuffersSaveDataProvider]") {

  // Arrange
  steamrot::FlatbuffersSaveDataProvider provider;

  // Act
  auto result = provider.CreateSaveData();

  // Assert
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FileNotFound);
}

TEST_CASE("FlatbuffersSaveDataProvider::ConfigureSaveData returns error when "
          "no save file available",
          "[unit][FlatbuffersSaveDataProvider]") {

  // Arrange
  steamrot::FlatbuffersSaveDataProvider provider;
  steamrot::SaveData save_data;

  // Act
  auto result = provider.ConfigureSaveData(save_data);

  // Assert
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FileNotFound);
}
