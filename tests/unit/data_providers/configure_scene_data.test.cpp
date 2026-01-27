/////////////////////////////////////////////////
/// @file
/// @brief unit tests for configure_scene_data functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_scene_data.h"
#include "SceneInfo.h"
#include "SceneResourcesConfig.h"
#include "scene_info_generated.h"
#include "scene_resources_config_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <cstdint>

TEST_CASE("ConfigureSceneInfo handles nullptr Flatbuffers data gracefully",
          "[unit][configure_scene_data]") {
  steamrot::SceneInfo scene_info;
  const steamrot::SceneInfoFbs *null_data = nullptr;
  auto result =
      steamrot::data::configure::ConfigureSceneInfo(scene_info, null_data);
  REQUIRE(!result.has_value());

  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "SceneInfoFbs is null");
}

TEST_CASE("ConfigureSceneInfo correctly configures SceneInfo with different "
          "scene types",
          "[unit][configure_scene_data]") {
  // Test multiple scene types to verify correct configuration
  for (const auto scene_type : {steamrot::SceneType::TITLE,
                                steamrot::SceneType::CRAFTING}) {

    flatbuffers::FlatBufferBuilder builder;
    flatbuffers::Offset<steamrot::SceneInfoFbs> scene_info_offset =
        steamrot::CreateSceneInfoFbs(builder, 0, scene_type);
    builder.Finish(scene_info_offset);

    const steamrot::SceneInfoFbs *scene_info_fbs =
        steamrot::GetSceneInfoFbs(builder.GetBufferPointer());

    steamrot::SceneInfo scene_info;
    auto result = steamrot::data::configure::ConfigureSceneInfo(scene_info,
                                                                scene_info_fbs);
    // Should succeed with the given type
    REQUIRE(result.has_value());
    REQUIRE(scene_info.type == scene_type);
  }
}

TEST_CASE("ConfigureSceneInfo correctly configures SceneInfo with valid UUID",
          "[unit][configure_scene_data]") {
  // Create FlatBuffers data
  flatbuffers::FlatBufferBuilder builder;
  std::string valid_uuid = "550e8400-e29b-41d4-a716-446655440000";
  flatbuffers::Offset<flatbuffers::String> scene_id_offset =
      builder.CreateString(valid_uuid);
  flatbuffers::Offset<steamrot::SceneInfoFbs> scene_info_offset =
      steamrot::CreateSceneInfoFbs(builder, scene_id_offset,
                                   steamrot::SceneType::TITLE);
  builder.Finish(scene_info_offset);

  const steamrot::SceneInfoFbs *scene_info_fbs =
      steamrot::GetSceneInfoFbs(builder.GetBufferPointer());

  REQUIRE(scene_info_fbs != nullptr);

  steamrot::SceneInfo scene_info;
  auto result =
      steamrot::data::configure::ConfigureSceneInfo(scene_info, scene_info_fbs);

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  REQUIRE(scene_info.type == steamrot::SceneType::TITLE);
  REQUIRE(!scene_info.id.is_nil());
  REQUIRE(scene_info.id == uuids::uuid::from_string(valid_uuid).value());
}

TEST_CASE("ConfigureSceneInfo correctly configures SceneInfo without UUID",
          "[unit][configure_scene_data]") {
  // Create FlatBuffers data without scene_id
  flatbuffers::FlatBufferBuilder builder;
  flatbuffers::Offset<steamrot::SceneInfoFbs> scene_info_offset =
      steamrot::CreateSceneInfoFbs(builder, 0,
                                   steamrot::SceneType::CRAFTING);
  builder.Finish(scene_info_offset);

  const steamrot::SceneInfoFbs *scene_info_fbs =
      steamrot::GetSceneInfoFbs(builder.GetBufferPointer());

  REQUIRE(scene_info_fbs != nullptr);

  steamrot::SceneInfo scene_info;
  auto result =
      steamrot::data::configure::ConfigureSceneInfo(scene_info, scene_info_fbs);

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  REQUIRE(scene_info.type == steamrot::SceneType::CRAFTING);
  // UUID should be nil since it wasn't provided
  REQUIRE(scene_info.id.is_nil());
}

TEST_CASE("ConfigureSceneInfo handles invalid UUID string",
          "[unit][configure_scene_data]") {
  // Create FlatBuffers data with invalid UUID
  flatbuffers::FlatBufferBuilder builder;
  std::string invalid_uuid = "not-a-valid-uuid";
  flatbuffers::Offset<flatbuffers::String> scene_id_offset =
      builder.CreateString(invalid_uuid);
  flatbuffers::Offset<steamrot::SceneInfoFbs> scene_info_offset =
      steamrot::CreateSceneInfoFbs(builder, scene_id_offset,
                                   steamrot::SceneType::TITLE);
  builder.Finish(scene_info_offset);

  const steamrot::SceneInfoFbs *scene_info_fbs =
      steamrot::GetSceneInfoFbs(builder.GetBufferPointer());

  steamrot::SceneInfo scene_info;
  auto result =
      steamrot::data::configure::ConfigureSceneInfo(scene_info, scene_info_fbs);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::InvalidUUID);
  REQUIRE(result.error().message ==
          "Scene ID in SceneInfoFbs is not a valid UUID string");
}

TEST_CASE(
    "ConfigureSceneResourcesConfig handles nullptr Flatbuffers data gracefully",
    "[unit][configure_scene_data]") {
  steamrot::SceneResourcesConfig config;
  const steamrot::SceneResourcesConfigFbs *null_data = nullptr;
  auto result = steamrot::data::configure::ConfigureSceneResourcesConfig(
      config, null_data);
  REQUIRE(!result.has_value());

  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "SceneResourcesConfigFbs is null");
}

TEST_CASE("ConfigureSceneResourcesConfig correctly configures "
          "SceneResourcesConfig",
          "[unit][configure_scene_data]") {
  // Create FlatBuffers data
  flatbuffers::FlatBufferBuilder builder;
  flatbuffers::Offset<steamrot::SceneResourcesConfigFbs> config_offset =
      steamrot::CreateSceneResourcesConfigFbs(builder, 1080, 1958);
  builder.Finish(config_offset);

  const steamrot::SceneResourcesConfigFbs *config_fbs =
      steamrot::GetSceneResourcesConfigFbs(builder.GetBufferPointer());

  REQUIRE(config_fbs != nullptr);

  steamrot::SceneResourcesConfig config;
  auto result = steamrot::data::configure::ConfigureSceneResourcesConfig(
      config, config_fbs);

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  REQUIRE(config.texture_width == 1958);
  REQUIRE(config.texture_height == 1080);
}

TEST_CASE("ConfigureSceneResourcesConfig handles zero dimensions",
          "[unit][configure_scene_data]") {
  // Create FlatBuffers data with zero dimensions
  flatbuffers::FlatBufferBuilder builder;
  int32_t zero_width = 0;
  int32_t zero_height = 0;
  flatbuffers::Offset<steamrot::SceneResourcesConfigFbs> config_offset =
      steamrot::CreateSceneResourcesConfigFbs(builder, zero_height, zero_width);
  builder.Finish(config_offset);

  const steamrot::SceneResourcesConfigFbs *config_fbs =
      steamrot::GetSceneResourcesConfigFbs(builder.GetBufferPointer());

  REQUIRE(config_fbs != nullptr);

  steamrot::SceneResourcesConfig config;
  auto result = steamrot::data::configure::ConfigureSceneResourcesConfig(
      config, config_fbs);

  // zero dimensions will register as null value, so this will error out
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message ==
          "Texture width is missing in SceneResourcesConfigFbs");
}
