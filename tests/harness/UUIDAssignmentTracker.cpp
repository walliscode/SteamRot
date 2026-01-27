/////////////////////////////////////////////////
/// @file
/// @brief Implementation of UUID assignment utilities for TestData
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UUIDAssignmentTracker.h"
#include "uuid.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
uuids::uuid UUIDAssignmentTracker::ProcessStartingSceneUUID(
    SceneType scene_type, std::optional<uuids::uuid> existing_uuid) {

  // If UUID already exists for this scene, use it
  if (existing_uuid.has_value()) {
    m_scene_type_to_uuid[scene_type] = existing_uuid.value();
    return existing_uuid.value();
  }

  // Check if we already assigned a UUID to this SceneType
  auto it = m_scene_type_to_uuid.find(scene_type);
  if (it != m_scene_type_to_uuid.end()) {
    return it->second;
  }

  // Generate a new UUID for this SceneType
  uuids::uuid_system_generator generator;
  uuids::uuid new_uuid = generator();
  m_scene_type_to_uuid[scene_type] = new_uuid;
  return new_uuid;
}

/////////////////////////////////////////////////
uuids::uuid
UUIDAssignmentTracker::GetOrAssignUUID(SceneType scene_type) {

  // Check if UUID already exists for this SceneType
  auto it = m_scene_type_to_uuid.find(scene_type);
  if (it != m_scene_type_to_uuid.end()) {
    return it->second;
  }

  // Generate a new UUID if not found
  uuids::uuid_system_generator generator;
  uuids::uuid new_uuid = generator();
  m_scene_type_to_uuid[scene_type] = new_uuid;
  return new_uuid;
}

/////////////////////////////////////////////////
bool UUIDAssignmentTracker::HasUUID(SceneType scene_type) const {
  return m_scene_type_to_uuid.find(scene_type) != m_scene_type_to_uuid.end();
}

/////////////////////////////////////////////////
std::optional<uuids::uuid>
UUIDAssignmentTracker::GetUUID(SceneType scene_type) const {
  auto it = m_scene_type_to_uuid.find(scene_type);
  if (it != m_scene_type_to_uuid.end()) {
    return it->second;
  }
  return std::nullopt;
}

} // namespace steamrot::tests
