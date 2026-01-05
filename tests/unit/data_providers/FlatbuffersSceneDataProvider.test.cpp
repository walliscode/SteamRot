/////////////////////////////////////////////////
/// @file
/// @brief Units tests for FlatbuffersSceneDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneDataProvider.h"
#include "FbsSceneData.h"
#include "FlatbuffersDataLoader.h"
#include <catch2/catch_test_macros.hpp>
#include <memory>

TEST_CASE("FlatbuffersSceneDataProvider::ProviderDefaultSceneData returns "
          "error if bad SceneType",
          "[unit][FlatbuffersSceneDataProvider]") {
  steamrot::FlatbuffersSceneDataProvider provider;
  // Use an invalid SceneType
  steamrot::SceneType invalid_scene_type = static_cast<steamrot::SceneType>(-1);
  auto scene_data_get_result =
      provider.ProvideDefaultSceneData(invalid_scene_type);
  REQUIRE(!scene_data_get_result.has_value());
  REQUIRE(scene_data_get_result.error().message ==
          "Invalid SceneType provided");
}

TEST_CASE("FlatbuffersSceneDataProvider::ConvertSceneData returns error if "
          "null pointer",
          "[unit][FlatbuffersSceneDataProvider]") {
  steamrot::FlatbuffersSceneDataProvider provider;
  auto convert_result = provider.ConvertSceneData(nullptr);
  REQUIRE(!convert_result.has_value());
  REQUIRE(convert_result.error().mode ==
          steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("FlatbuffersSceneDataProvider::ConvertSceneData converts "
          "FlatBuffers data to native SceneData",
          "[unit][FlatbuffersSceneDataProvider]") {
  steamrot::FlatbuffersSceneDataProvider provider;
  steamrot::FlatbuffersDataLoader loader;

  // Load FlatBuffers data for Title scene
  auto load_result = loader.ProvideDefaultSceneData(
      steamrot::SceneType::SceneType_TITLE);
  REQUIRE(load_result.has_value());
  const steamrot::SceneDataFbs *fb_data = load_result.value();

  // Convert using the new method
  auto convert_result = provider.ConvertSceneData(fb_data);
  REQUIRE(convert_result.has_value());

  auto &scene_data_ptr = convert_result.value();
  REQUIRE(scene_data_ptr != nullptr);

  // Verify it's the correct type
  steamrot::FbsSceneData *fbs_scene_data =
      dynamic_cast<steamrot::FbsSceneData *>(scene_data_ptr.get());
  REQUIRE(fbs_scene_data != nullptr);

  // Verify entity collection pointer is set
  REQUIRE(fbs_scene_data->entity_collection != nullptr);
}

TEST_CASE("FlatbuffersSceneDataProvider::ProvideDefaultSceneData returns "
          "FbsSceneData for SceneType"
          "[unit][FlatbuffersSceneDataProvider]") {
  steamrot::FlatbuffersSceneDataProvider provider;

  // Test for each SceneType
  for (const auto scene_type : {steamrot::SceneType::SceneType_CRAFTING,
                                steamrot::SceneType::SceneType_TITLE}) {

    auto scene_data_get_result = provider.ProvideDefaultSceneData(scene_type);
    if (!scene_data_get_result.has_value()) {
      FAIL(scene_data_get_result.error().message);
    }
    auto &scene_data_ptr = scene_data_get_result.value();
    REQUIRE(scene_data_ptr != nullptr);
    REQUIRE(scene_data_ptr->scene_info.type == scene_type);

    REQUIRE(dynamic_cast<steamrot::FbsSceneData *>(scene_data_ptr.get()));
  }
}

TEST_CASE("FlatbuffersSceneDataProvider::ProvideDefaultSceneData returns "
          "correct data with Title scene",
          "[unit][FlatbuffersSceneDataProvider]") {
  steamrot::FlatbuffersSceneDataProvider provider;
  auto scene_data_get_result =
      provider.ProvideDefaultSceneData(steamrot::SceneType::SceneType_TITLE);
  if (!scene_data_get_result.has_value()) {
    FAIL(scene_data_get_result.error().message);
  }
  auto &scene_data_ptr = scene_data_get_result.value();
  REQUIRE(scene_data_ptr != nullptr);
  steamrot::FbsSceneData *fbs_scene_data =
      dynamic_cast<steamrot::FbsSceneData *>(scene_data_ptr.get());
  REQUIRE(fbs_scene_data != nullptr);

  // check scene info
  REQUIRE(fbs_scene_data->scene_info.type ==
          steamrot::SceneType::SceneType_TITLE);
  REQUIRE(fbs_scene_data->scene_info.id.is_nil());

  // check pointer to scene data exists
  REQUIRE(fbs_scene_data->entity_collection != nullptr);

  // SPECIFIC DATA CHECKS CAN BE ADDED HERE //
}

TEST_CASE("FlatbuffersSceneDataProvider::ProvideDefaultSceneData returns "
          "correct data with Crafting scene",
          "[unit][FlatbuffersSceneDataProvider]") {
  steamrot::FlatbuffersSceneDataProvider provider;
  auto scene_data_get_result =
      provider.ProvideDefaultSceneData(steamrot::SceneType::SceneType_CRAFTING);
  if (!scene_data_get_result.has_value()) {
    FAIL(scene_data_get_result.error().message);
  }
  auto &scene_data_ptr = scene_data_get_result.value();
  REQUIRE(scene_data_ptr != nullptr);
  steamrot::FbsSceneData *fbs_scene_data =
      dynamic_cast<steamrot::FbsSceneData *>(scene_data_ptr.get());
  REQUIRE(fbs_scene_data != nullptr);
  // check scene info
  REQUIRE(fbs_scene_data->scene_info.type ==
          steamrot::SceneType::SceneType_CRAFTING);
  REQUIRE(fbs_scene_data->scene_info.id.is_nil());
  // check pointer to scene data exists
  REQUIRE(fbs_scene_data->entity_collection != nullptr);
  // SPECIFIC DATA CHECKS CAN BE ADDED HERE //
}
