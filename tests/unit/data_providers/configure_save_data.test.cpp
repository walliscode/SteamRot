/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for configure_save_data functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_save_data.h"
#include "SaveData.h"
#include "SaveMetaData.h"
#include "save_data_generated.h"
#include "uuid.h"
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
// ConfigureSaveMetaData tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureSaveMetaData fails with null data",
          "[unit][configure_save_data]") {
  steamrot::SaveMetaData meta_data;

  auto result =
      steamrot::data::configure::ConfigureSaveMetaData(meta_data, nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "Null SaveMetaDataFbs pointer");
}

TEST_CASE("ConfigureSaveMetaData fails when save_name is missing",
          "[unit][configure_save_data]") {
  steamrot::SaveMetaData meta_data;

  // Create flatbuffers data without save_name
  flatbuffers::FlatBufferBuilder builder;
  auto meta_data_fbs_offset = steamrot::CreateSaveMetaDataFbs(builder);
  builder.Finish(meta_data_fbs_offset);
  const steamrot::SaveMetaDataFbs *meta_data_fbs =
      flatbuffers::GetRoot<steamrot::SaveMetaDataFbs>(
          builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureSaveMetaData(
      meta_data, meta_data_fbs);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message ==
          "SaveMetaDataFbs missing required field: save_name");
}

TEST_CASE("ConfigureSaveMetaData fails when file_id is missing",
          "[unit][configure_save_data]") {
  steamrot::SaveMetaData meta_data;

  // Create flatbuffers data with save_name but no file_id
  flatbuffers::FlatBufferBuilder builder;
  auto save_name_offset = builder.CreateString("Test Save");
  auto meta_data_fbs_offset =
      steamrot::CreateSaveMetaDataFbs(builder, save_name_offset);
  builder.Finish(meta_data_fbs_offset);
  const steamrot::SaveMetaDataFbs *meta_data_fbs =
      flatbuffers::GetRoot<steamrot::SaveMetaDataFbs>(
          builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureSaveMetaData(
      meta_data, meta_data_fbs);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message ==
          "SaveMetaDataFbs missing required field: file_id");
}

TEST_CASE("ConfigureSaveMetaData fails when file_id is not a valid UUID",
          "[unit][configure_save_data]") {
  steamrot::SaveMetaData meta_data;

  // Create flatbuffers data with invalid UUID
  flatbuffers::FlatBufferBuilder builder;
  auto save_name_offset = builder.CreateString("Test Save");
  auto file_id_offset = builder.CreateString("not-a-valid-uuid");
  auto meta_data_fbs_offset = steamrot::CreateSaveMetaDataFbs(
      builder, save_name_offset, file_id_offset);
  builder.Finish(meta_data_fbs_offset);
  const steamrot::SaveMetaDataFbs *meta_data_fbs =
      flatbuffers::GetRoot<steamrot::SaveMetaDataFbs>(
          builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureSaveMetaData(
      meta_data, meta_data_fbs);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message ==
          "SaveMetaDataFbs file_id is not a valid UUID string");
}

TEST_CASE("ConfigureSaveMetaData populates from valid data",
          "[unit][configure_save_data]") {
  steamrot::SaveMetaData meta_data;

  // Create valid flatbuffers data
  uuids::uuid test_uuid = uuids::uuid_system_generator{}();
  flatbuffers::FlatBufferBuilder builder;
  auto save_name_offset = builder.CreateString("Test Save");
  auto file_id_offset = builder.CreateString(uuids::to_string(test_uuid));
  auto meta_data_fbs_offset = steamrot::CreateSaveMetaDataFbs(
      builder, save_name_offset, file_id_offset);
  builder.Finish(meta_data_fbs_offset);
  const steamrot::SaveMetaDataFbs *meta_data_fbs =
      flatbuffers::GetRoot<steamrot::SaveMetaDataFbs>(
          builder.GetBufferPointer());

  auto result = steamrot::data::configure::ConfigureSaveMetaData(
      meta_data, meta_data_fbs);

  REQUIRE(result.has_value());
  REQUIRE(meta_data.save_name == "Test Save");
  REQUIRE(meta_data.file_id == test_uuid);
}

/////////////////////////////////////////////////
// ConfigureSaveData tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureSaveData fails with null data",
          "[unit][configure_save_data]") {
  steamrot::SaveData save_data;

  auto result = steamrot::data::configure::ConfigureSaveData(save_data, nullptr);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "Null SaveDataFbs pointer");
}

TEST_CASE("ConfigureSaveData fails when save_meta_data is missing",
          "[unit][configure_save_data]") {
  steamrot::SaveData save_data;

  // Create flatbuffers data without save_meta_data
  flatbuffers::FlatBufferBuilder builder;
  auto save_data_fbs_offset = steamrot::CreateSaveDataFbs(builder);
  builder.Finish(save_data_fbs_offset);
  const steamrot::SaveDataFbs *save_data_fbs =
      flatbuffers::GetRoot<steamrot::SaveDataFbs>(builder.GetBufferPointer());

  auto result =
      steamrot::data::configure::ConfigureSaveData(save_data, save_data_fbs);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message ==
          "SaveDataFbs missing required field: save_meta_data");
}

TEST_CASE("ConfigureSaveData populates from valid data",
          "[unit][configure_save_data]") {
  steamrot::SaveData save_data;

  // Create valid flatbuffers data
  uuids::uuid test_uuid = uuids::uuid_system_generator{}();
  flatbuffers::FlatBufferBuilder builder;
  
  // Create SaveMetaDataFbs
  auto save_name_offset = builder.CreateString("Test Save");
  auto file_id_offset = builder.CreateString(uuids::to_string(test_uuid));
  auto meta_data_fbs_offset = steamrot::CreateSaveMetaDataFbs(
      builder, save_name_offset, file_id_offset);
  
  // Create SaveDataFbs with the meta_data
  auto save_data_fbs_offset =
      steamrot::CreateSaveDataFbs(builder, meta_data_fbs_offset);
  builder.Finish(save_data_fbs_offset);
  const steamrot::SaveDataFbs *save_data_fbs =
      flatbuffers::GetRoot<steamrot::SaveDataFbs>(builder.GetBufferPointer());

  auto result =
      steamrot::data::configure::ConfigureSaveData(save_data, save_data_fbs);

  REQUIRE(result.has_value());
  REQUIRE(save_data.meta_data.save_name == "Test Save");
  REQUIRE(save_data.meta_data.file_id == test_uuid);
}
