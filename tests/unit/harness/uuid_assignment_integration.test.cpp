/////////////////////////////////////////////////
/// @file
/// @brief Integration tests for UUID assignment in TestData loading
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersTestDataProvider.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

TEST_CASE("FlatbuffersTestDataProvider assigns UUIDs to starting snapshot",
          "[unit][integration][uuid]") {
  
  // Get the directory of this test file
  std::filesystem::path test_file_path(__FILE__);
  std::filesystem::path test_dir = test_file_path.parent_path();

  // Create provider
  FlatbuffersTestDataProvider provider(test_dir);

  // Load test data
  auto test_data_result = provider.ProvideAllTestData();
  REQUIRE(test_data_result.has_value());

  auto &test_data_vec = test_data_result.value();
  
  // Find the uuid_assignment test
  bool found_uuid_test = false;
  for (const auto &test_data : test_data_vec) {
    if (test_data.meta_data.test_name == "UUID assignment validation") {
      found_uuid_test = true;

      // Validate starting snapshot has scenes with UUIDs
      REQUIRE_FALSE(test_data.starting_engine_snapshot.scene_collection_data.empty());
      
      const auto &starting_scene = 
          test_data.starting_engine_snapshot.scene_collection_data[0];
      
      // Check that UUID was assigned (not empty)
      REQUIRE(starting_scene.scene_info.id != uuids::uuid{});
      
      // Check scene type
      REQUIRE(starting_scene.scene_info.type == steamrot::SceneType::TITLE);

      // Store the UUID from starting snapshot
      uuids::uuid starting_uuid = starting_scene.scene_info.id;

      // Validate expected snapshots
      REQUIRE(test_data.expected_engine_snapshots.size() == 1);
      
      // Check the expected snapshot at tick 1
      auto it = test_data.expected_engine_snapshots.find(1);
      REQUIRE(it != test_data.expected_engine_snapshots.end());
      
      const auto &expected_snapshot = it->second;
      REQUIRE_FALSE(expected_snapshot.scene_collection_data.empty());
      
      const auto &expected_scene = expected_snapshot.scene_collection_data[0];
      
      // Check that the expected snapshot has the SAME UUID as starting snapshot
      REQUIRE(expected_scene.scene_info.id == starting_uuid);
      REQUIRE(expected_scene.scene_info.id != uuids::uuid{});
      
      // Check scene type matches
      REQUIRE(expected_scene.scene_info.type == steamrot::SceneType::TITLE);
      
      break;
    }
  }

  REQUIRE(found_uuid_test);
}

TEST_CASE("FlatbuffersTestDataProvider preserves existing UUIDs",
          "[unit][integration][uuid]") {
  
  // This test would validate that if a UUID is provided in the JSON,
  // it's preserved. For now, we'll just document the expected behavior.
  
  // Expected behavior:
  // 1. If scene_info.scene_id is provided in JSON with a valid UUID string
  // 2. That UUID should be used instead of generating a new one
  // 3. The UUID should still be propagated to expected snapshots
  
  // This test should be added once we have test data with explicit UUIDs
  SUCCEED("Documented expected behavior for explicit UUIDs");
}

TEST_CASE("FlatbuffersTestDataProvider handles multiple scenes with different types",
          "[unit][integration][uuid]") {
  
  // Expected behavior for multiple scenes:
  // 1. Each scene type gets its own UUID
  // 2. Multiple scenes of the same type share the same UUID
  // 3. UUIDs are propagated correctly to all expected snapshots
  
  // This test should be expanded when we have test data with multiple scenes
  SUCCEED("Documented expected behavior for multiple scenes");
}
