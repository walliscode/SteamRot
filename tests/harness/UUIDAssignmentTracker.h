/////////////////////////////////////////////////
/// @file
/// @brief Declaration of UUID assignment utilities for TestData
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "SceneType.h"
#include "uuid.h"
#include <expected>
#include <map>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Helper class to manage UUID assignment across TestData snapshots
///
/// This class tracks and assigns UUIDs to scenes across multiple engine
/// snapshots (starting and expected). It ensures that scenes with the same
/// SceneType get the same UUID across all snapshots in a single TestData.
///
/// Usage:
/// 1. Create UUIDAssignmentTracker
/// 2. Call ProcessStartingSnapshot() with starting snapshot scenes
/// 3. Call GetOrAssignUUID() for each scene in expected snapshots
/////////////////////////////////////////////////
class UUIDAssignmentTracker {
private:
  /////////////////////////////////////////////////
  /// @brief Maps SceneType to assigned UUID
  /////////////////////////////////////////////////
  std::map<SceneType, uuids::uuid> m_scene_type_to_uuid;

public:
  /////////////////////////////////////////////////
  /// @brief Default constructor
  /////////////////////////////////////////////////
  UUIDAssignmentTracker() = default;

  /////////////////////////////////////////////////
  /// @brief Processes a starting snapshot and assigns/records UUIDs
  ///
  /// For each scene in the starting snapshot:
  /// - If scene has a UUID, record it for this SceneType
  /// - If scene has no UUID, generate a new one and record it
  ///
  /// @param scene_type The SceneType of the scene
  /// @param existing_uuid Optional existing UUID from the scene_id field
  /// @return The UUID to use for this scene
  /////////////////////////////////////////////////
  uuids::uuid ProcessStartingSceneUUID(SceneType scene_type,
                                       std::optional<uuids::uuid> existing_uuid);

  /////////////////////////////////////////////////
  /// @brief Gets the assigned UUID for a SceneType, or generates if not found
  ///
  /// Used when processing expected snapshots. If the SceneType was in the
  /// starting snapshot, returns the same UUID. Otherwise generates a new one.
  ///
  /// @param scene_type The SceneType to get UUID for
  /// @return The UUID for this SceneType
  /////////////////////////////////////////////////
  uuids::uuid GetOrAssignUUID(SceneType scene_type);

  /////////////////////////////////////////////////
  /// @brief Checks if a SceneType has been assigned a UUID
  ///
  /// @param scene_type The SceneType to check
  /// @return true if UUID has been assigned, false otherwise
  /////////////////////////////////////////////////
  bool HasUUID(SceneType scene_type) const;

  /////////////////////////////////////////////////
  /// @brief Gets the UUID for a SceneType if it exists
  ///
  /// @param scene_type The SceneType to get UUID for
  /// @return The UUID if found, std::nullopt otherwise
  /////////////////////////////////////////////////
  std::optional<uuids::uuid> GetUUID(SceneType scene_type) const;
};

} // namespace steamrot::tests
