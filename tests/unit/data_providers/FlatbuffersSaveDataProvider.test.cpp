/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersSaveDataProvider class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSaveDataProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE(
    "FlatbuffersSaveDataProvider::ConfigureSaveMetaData handles null input",
    "[FlatbuffersSaveDataProvider]") {

  // Arrange
  steamrot::FlatbuffersSaveDataProvider provider;
  steamrot::SaveMetaData save_meta_data;

  // Act
  auto result = provider.ConfigureSaveMetaData(save_meta_data, nullptr);

  // Assert
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "Null SaveMetaDataFbs pointer");
}

TEST_CASE(
    "FlatbuffersSaveDataProvider::ConfigureSaveMetaData handles no save name",
    "[FlatbuffersSaveDataProvider]") {

  // Arrange
  steamrot::FlatbuffersSaveDataProvider provider;
  steamrot::SaveMetaData save_meta_data;

  // Create flatbuffers data
  flatbuffers::FlatBufferBuilder builder;
  auto save_meta_data_fbs_offset = steamrot::CreateSaveMetaDataFbs(builder);
  builder.Finish(save_meta_data_fbs_offset);
  const steamrot::SaveMetaDataFbs *save_meta_data_fbs =
      flatbuffers::GetRoot<steamrot::SaveMetaDataFbs>(
          builder.GetBufferPointer());
  // Act
  auto result =
      provider.ConfigureSaveMetaData(save_meta_data, save_meta_data_fbs);

  // Assert
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message ==
          "SaveMetaDataFbs missing required field: save_name");
}

TEST_CASE("FlatbuffersSaveDataProvider::ConfigureSaveMetaData handles no or "
          "incorrect uuid",
          "[FlatbuffersSaveDataProvider]") {
  // Arrange
  steamrot::FlatbuffersSaveDataProvider provider;
  steamrot::SaveMetaData save_meta_data;

  // Create flatbuffers data with save name but no UUID
  flatbuffers::FlatBufferBuilder builder;
  auto save_name_offset = builder.CreateString("Test Save");
  auto uuid_offset = builder.CreateString("");

  auto save_meta_data_fbs_offset =
      steamrot::CreateSaveMetaDataFbs(builder, uuid_offset, save_name_offset);
  builder.Finish(save_meta_data_fbs_offset);
  const steamrot::SaveMetaDataFbs *save_meta_data_fbs =
      flatbuffers::GetRoot<steamrot::SaveMetaDataFbs>(
          builder.GetBufferPointer());

  // Act
  auto result =
      provider.ConfigureSaveMetaData(save_meta_data, save_meta_data_fbs);

  // Assert
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message ==
          "SaveMetaDataFbs file_id is not a valid UUID string");
}

TEST_CASE("FlatbuffersSaveDataProvider::ConfigureSaveMetaData handles valid "
          "input",
          "[FlatbuffersSaveDataProvider]") {
  // Arrange
  steamrot::FlatbuffersSaveDataProvider provider;
  steamrot::SaveMetaData save_meta_data;

  // Create flatbuffers data with save name and valid UUID
  flatbuffers::FlatBufferBuilder builder;
  auto save_name_offset = builder.CreateString("Test Save");
  auto uuid_offset =
      builder.CreateString("123e4567-e89b-12d3-a456-426614174000");

  auto save_meta_data_fbs_offset =
      steamrot::CreateSaveMetaDataFbs(builder, uuid_offset, save_name_offset);
  builder.Finish(save_meta_data_fbs_offset);
  const steamrot::SaveMetaDataFbs *save_meta_data_fbs =
      flatbuffers::GetRoot<steamrot::SaveMetaDataFbs>(
          builder.GetBufferPointer());

  // Act
  auto result =
      provider.ConfigureSaveMetaData(save_meta_data, save_meta_data_fbs);

  // Assert
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  REQUIRE(save_meta_data.save_name == "Test Save");
  REQUIRE(
      save_meta_data.file_id ==
      uuids::uuid::from_string("123e4567-e89b-12d3-a456-426614174000").value());
}
