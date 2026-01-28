/////////////////////////////////////////////////
/// @file
/// @brief unit tests for add_uuids functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "add_uuids.h"
#include "FailInfo.h"
#include "SceneData.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("add_uuids returns unexpected on empty SceneCollectionData in "
          "starting_engine_snapshot",
          "[add_uuids]") {

  steamrot::TestData data;
  auto result = steamrot::tests::add_uuids(data);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().message ==
          "Starting engine snapshot has no scene data");
  REQUIRE(result.error().mode == steamrot::FailMode::MissingData);
}

TEST_CASE(
    "add_uuids returns unexpected if SceneCollectionData lengths do not match",
    "[add_uuids]") {
  // set up test data
  steamrot::TestData data;

  // add one scene to starting_engine_snapshot and two to
  // expected_engine_snapshots
  steamrot::SceneData scene_data;
  steamrot::SceneData scene_data_2;

  data.starting_engine_snapshot.scene_collection_data.push_back(
      std::move(scene_data));
  data.expected_engine_snapshots[1].scene_collection_data.push_back(
      std::move(scene_data));
  data.expected_engine_snapshots[1].scene_collection_data.push_back(
      std::move(scene_data_2));

  auto result = steamrot::tests::add_uuids(data);
  REQUIRE(!result.has_value());
  REQUIRE(result.error().message ==
          "SceneCollectionData length mismatch between starting snapshot and "
          "expected snapshot at tick 1");
}

TEST_CASE("add_uuids returns monostate on valid TestData", "[add_uuids]") {
  // set up test data
  steamrot::TestData data;
  // add one scene to starting_engine_snapshot and one to
  // expected_engine_snapshots
  steamrot::SceneData scene_data;
  data.starting_engine_snapshot.scene_collection_data.push_back(
      std::move(scene_data));
  data.expected_engine_snapshots[1].scene_collection_data.push_back(
      steamrot::SceneData{});
  auto result = steamrot::tests::add_uuids(data);
  REQUIRE(result.has_value());
}

TEST_CASE("add_uuids adds matching uuids across snapshots", "[add_uuids]") {
  // set up test data
  steamrot::TestData data;

  // set up starting snapshot with three scenes
  steamrot::SceneData scene_one;
  steamrot::SceneData scene_two;
  steamrot::SceneData scene_three;
  REQUIRE(scene_one.scene_info.id.is_nil());
  REQUIRE(scene_two.scene_info.id.is_nil());
  REQUIRE(scene_three.scene_info.id.is_nil());

  data.starting_engine_snapshot.scene_collection_data.push_back(
      std::move(scene_one));
  data.starting_engine_snapshot.scene_collection_data.push_back(
      std::move(scene_two));
  data.starting_engine_snapshot.scene_collection_data.push_back(
      std::move(scene_three));

  // set up expected snapshhot at tick 1 with three scenes
  steamrot::SceneData expected_scene_one;
  steamrot::SceneData expected_scene_two;
  steamrot::SceneData expected_scene_three;
  REQUIRE(expected_scene_one.scene_info.id.is_nil());
  REQUIRE(expected_scene_two.scene_info.id.is_nil());
  REQUIRE(expected_scene_three.scene_info.id.is_nil());
  data.expected_engine_snapshots[1].scene_collection_data.push_back(
      std::move(expected_scene_one));
  data.expected_engine_snapshots[1].scene_collection_data.push_back(
      std::move(expected_scene_two));
  data.expected_engine_snapshots[1].scene_collection_data.push_back(
      std::move(expected_scene_three));

  // set up expected snapshhot at tick 2 with three scenes
  steamrot::SceneData expected_scene_one_tick_2;
  steamrot::SceneData expected_scene_two_tick_2;
  steamrot::SceneData expected_scene_three_tick_2;
  REQUIRE(expected_scene_one_tick_2.scene_info.id.is_nil());
  REQUIRE(expected_scene_two_tick_2.scene_info.id.is_nil());
  REQUIRE(expected_scene_three_tick_2.scene_info.id.is_nil());
  data.expected_engine_snapshots[2].scene_collection_data.push_back(
      std::move(expected_scene_one_tick_2));
  data.expected_engine_snapshots[2].scene_collection_data.push_back(
      std::move(expected_scene_two_tick_2));
  data.expected_engine_snapshots[2].scene_collection_data.push_back(
      std::move(expected_scene_three_tick_2));

  // call add_uuids
  auto result = steamrot::tests::add_uuids(data);

  // verify success
  for (const auto &snapshot_pair : data.expected_engine_snapshots) {

    // cycle through each scene and verify uuids match starting snapshot
    const auto &snapshot = snapshot_pair.second;

    for (size_t i = 0; i < snapshot.scene_collection_data.size(); ++i) {

      const auto &starting_scene =
          data.starting_engine_snapshot.scene_collection_data[i];

      const auto &expected_scene = snapshot.scene_collection_data[i];

      REQUIRE(!starting_scene.scene_info.id.is_nil());
      REQUIRE(!expected_scene.scene_info.id.is_nil());
      REQUIRE(starting_scene.scene_info.id == expected_scene.scene_info.id);

      // check that uuids are unique across scenes
      for (size_t j = i + 1; j < snapshot.scene_collection_data.size(); ++j) {
        const auto &other_expected_scene = snapshot.scene_collection_data[j];
        REQUIRE(expected_scene.scene_info.id !=
                other_expected_scene.scene_info.id);
      }
    }
  }
}
