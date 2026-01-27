/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for UUIDAssignmentTracker
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UUIDAssignmentTracker.h"
#include "SceneType.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UUIDAssignmentTracker::ProcessStartingSceneUUID with no existing UUID",
          "[unit][UUIDAssignmentTracker]") {
  steamrot::tests::UUIDAssignmentTracker tracker;

  // Process a scene with no existing UUID
  auto uuid1 = tracker.ProcessStartingSceneUUID(steamrot::SceneType::TITLE,
                                                 std::nullopt);

  // Should generate a valid UUID
  REQUIRE(uuid1 != uuids::uuid{});

  // Should be able to retrieve it
  REQUIRE(tracker.HasUUID(steamrot::SceneType::TITLE));
  auto retrieved_uuid = tracker.GetUUID(steamrot::SceneType::TITLE);
  REQUIRE(retrieved_uuid.has_value());
  REQUIRE(retrieved_uuid.value() == uuid1);
}

TEST_CASE("UUIDAssignmentTracker::ProcessStartingSceneUUID with existing UUID",
          "[unit][UUIDAssignmentTracker]") {
  steamrot::tests::UUIDAssignmentTracker tracker;

  // Create a specific UUID to use
  uuids::uuid_system_generator generator;
  uuids::uuid provided_uuid = generator();

  // Process a scene with an existing UUID
  auto assigned_uuid = tracker.ProcessStartingSceneUUID(
      steamrot::SceneType::CRAFTING, provided_uuid);

  // Should use the provided UUID
  REQUIRE(assigned_uuid == provided_uuid);

  // Should be able to retrieve it
  auto retrieved_uuid = tracker.GetUUID(steamrot::SceneType::CRAFTING);
  REQUIRE(retrieved_uuid.has_value());
  REQUIRE(retrieved_uuid.value() == provided_uuid);
}

TEST_CASE("UUIDAssignmentTracker::ProcessStartingSceneUUID ensures consistency",
          "[unit][UUIDAssignmentTracker]") {
  steamrot::tests::UUIDAssignmentTracker tracker;

  // Process same scene type multiple times
  auto uuid1 = tracker.ProcessStartingSceneUUID(steamrot::SceneType::TEST,
                                                 std::nullopt);
  auto uuid2 = tracker.ProcessStartingSceneUUID(steamrot::SceneType::TEST,
                                                 std::nullopt);

  // Should return the same UUID for the same scene type
  REQUIRE(uuid1 == uuid2);
}

TEST_CASE("UUIDAssignmentTracker::GetOrAssignUUID for existing scene type",
          "[unit][UUIDAssignmentTracker]") {
  steamrot::tests::UUIDAssignmentTracker tracker;

  // First, process a starting scene
  auto original_uuid = tracker.ProcessStartingSceneUUID(
      steamrot::SceneType::TITLE, std::nullopt);

  // Then get the UUID for an expected snapshot
  auto retrieved_uuid = tracker.GetOrAssignUUID(steamrot::SceneType::TITLE);

  // Should return the same UUID
  REQUIRE(retrieved_uuid == original_uuid);
}

TEST_CASE("UUIDAssignmentTracker::GetOrAssignUUID for new scene type",
          "[unit][UUIDAssignmentTracker]") {
  steamrot::tests::UUIDAssignmentTracker tracker;

  // Get UUID for a scene type that wasn't in starting snapshot
  auto uuid = tracker.GetOrAssignUUID(steamrot::SceneType::CRAFTING);

  // Should generate a new valid UUID
  REQUIRE(uuid != uuids::uuid{});

  // Should now be tracked
  REQUIRE(tracker.HasUUID(steamrot::SceneType::CRAFTING));
}

TEST_CASE("UUIDAssignmentTracker::HasUUID returns false for untracked types",
          "[unit][UUIDAssignmentTracker]") {
  steamrot::tests::UUIDAssignmentTracker tracker;

  // Check for a scene type that hasn't been processed
  REQUIRE_FALSE(tracker.HasUUID(steamrot::SceneType::UNKNOWN));
}

TEST_CASE("UUIDAssignmentTracker::GetUUID returns nullopt for untracked types",
          "[unit][UUIDAssignmentTracker]") {
  steamrot::tests::UUIDAssignmentTracker tracker;

  // Try to get UUID for untracked scene type
  auto uuid = tracker.GetUUID(steamrot::SceneType::UNKNOWN);

  // Should return nullopt
  REQUIRE_FALSE(uuid.has_value());
}

TEST_CASE("UUIDAssignmentTracker handles multiple scene types",
          "[unit][UUIDAssignmentTracker]") {
  steamrot::tests::UUIDAssignmentTracker tracker;

  // Process multiple different scene types
  auto uuid_title =
      tracker.ProcessStartingSceneUUID(steamrot::SceneType::TITLE, std::nullopt);
  auto uuid_crafting = tracker.ProcessStartingSceneUUID(
      steamrot::SceneType::CRAFTING, std::nullopt);
  auto uuid_test =
      tracker.ProcessStartingSceneUUID(steamrot::SceneType::TEST, std::nullopt);

  // All should have different UUIDs
  REQUIRE(uuid_title != uuid_crafting);
  REQUIRE(uuid_title != uuid_test);
  REQUIRE(uuid_crafting != uuid_test);

  // All should be retrievable
  REQUIRE(tracker.HasUUID(steamrot::SceneType::TITLE));
  REQUIRE(tracker.HasUUID(steamrot::SceneType::CRAFTING));
  REQUIRE(tracker.HasUUID(steamrot::SceneType::TEST));

  // Retrieved UUIDs should match
  REQUIRE(tracker.GetUUID(steamrot::SceneType::TITLE).value() == uuid_title);
  REQUIRE(tracker.GetUUID(steamrot::SceneType::CRAFTING).value() ==
          uuid_crafting);
  REQUIRE(tracker.GetUUID(steamrot::SceneType::TEST).value() == uuid_test);
}
