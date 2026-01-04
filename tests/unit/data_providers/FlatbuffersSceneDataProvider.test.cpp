/////////////////////////////////////////////////
/// @file
/// @brief Units tests for FlatbuffersSceneDataProvider.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersSceneDataProvider.h"
#include "FbsSceneData.h"
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
