/////////////////////////////////////////////////
/// @file
/// @brief Units tests for FlatbuffersSceneDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "FlatbuffersSceneDataProvider.h"
#include "TestFixture.h"
#include "flatbuffers/flatbuffers.h"
#include "scene_data_generated.h"
#include "scene_info_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

TEST_CASE("FlatbuffersSceneDataProvider::CreateSceneData creates SceneData "
          "for Title scene",
          "[unit][FlatbuffersSceneDataProvider]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  steamrot::FlatbuffersSceneDataProvider provider(
      fixture.GetGameContext().event_handler);

  auto scene_data_result =
      provider.CreateSceneData(steamrot::SceneType::SceneType_TITLE);
  if (!scene_data_result.has_value()) {
    FAIL(scene_data_result.error().message);
  }

  const steamrot::SceneData &scene_data = scene_data_result.value();
  REQUIRE(scene_data.scene_info.type == steamrot::SceneType::SceneType_TITLE);
  REQUIRE(scene_data.entity_transport != nullptr);
}

TEST_CASE("FlatbuffersSceneDataProvider::CreateSceneData creates SceneData "
          "for Crafting scene",
          "[unit][FlatbuffersSceneDataProvider]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  steamrot::FlatbuffersSceneDataProvider provider(
      fixture.GetGameContext().event_handler);

  auto scene_data_result =
      provider.CreateSceneData(steamrot::SceneType::SceneType_CRAFTING);
  if (!scene_data_result.has_value()) {
    FAIL(scene_data_result.error().message);
  }

  const steamrot::SceneData &scene_data = scene_data_result.value();
  REQUIRE(scene_data.scene_info.type ==
          steamrot::SceneType::SceneType_CRAFTING);
  REQUIRE(scene_data.entity_transport != nullptr);
}

TEST_CASE("FlatbuffersSceneDataProvider::ConfigureSceneData handles null "
          "SceneDataFbs",
          "[unit][FlatbuffersSceneDataProvider]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  steamrot::FlatbuffersSceneDataProvider provider(
      fixture.GetGameContext().event_handler);

  steamrot::SceneData scene_data;
  auto result = provider.ConfigureSceneData(scene_data);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::NullPointer);
  REQUIRE(result.error().message == "FlatBuffers SceneDataFbs pointer is null");
}

TEST_CASE(
    "FlatbuffersSceneDataProvider constructor with SceneDataFbs pointer works",
    "[unit][FlatbuffersSceneDataProvider]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();

  // Create test FlatBuffers data
  flatbuffers::FlatBufferBuilder builder;

  // Create SceneInfo
  flatbuffers::Offset<steamrot::SceneInfoFbs> scene_info_offset =
      steamrot::CreateSceneInfoFbs(builder,
                                   steamrot::SceneType::SceneType_TITLE);

  // Create empty entity collection
  std::vector<flatbuffers::Offset<steamrot::EntityDataFbs>> entities;
  flatbuffers::Offset<
      flatbuffers::Vector<flatbuffers::Offset<steamrot::EntityDataFbs>>>
      entities_offset = builder.CreateVector(entities);
  flatbuffers::Offset<steamrot::EntityCollectionFbs> entity_collection_offset =
      steamrot::CreateEntityCollectionFbs(builder, 5, entities_offset);

  // Create SceneDataFbs
  flatbuffers::Offset<steamrot::SceneDataFbs> scene_data_offset =
      steamrot::CreateSceneDataFbs(builder, scene_info_offset, 0, 0,
                                   entity_collection_offset);
  builder.Finish(scene_data_offset);

  const steamrot::SceneDataFbs *scene_data_fbs =
      steamrot::GetSceneDataFbs(builder.GetBufferPointer());

  REQUIRE(scene_data_fbs != nullptr);

  // Create provider with SceneDataFbs
  steamrot::FlatbuffersSceneDataProvider provider(
      fixture.GetGameContext().event_handler, scene_data_fbs);

  // Configure SceneData
  steamrot::SceneData scene_data;
  auto result = provider.ConfigureSceneData(scene_data);

  if (!result.has_value()) {
    FAIL(result.error().message);
  }

  REQUIRE(scene_data.scene_info.type == steamrot::SceneType::SceneType_TITLE);
  REQUIRE(scene_data.entity_transport != nullptr);
}
