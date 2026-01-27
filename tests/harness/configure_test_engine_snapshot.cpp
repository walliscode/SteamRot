/////////////////////////////////////////////////
/// @file
/// @brief Implementation of configuration functions for TestData engine snapshots
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_test_engine_snapshot.h"
#include "FlatbuffersSceneDataProvider.h"
#include "configure_engine_snapshot.h"
#include "configure_scene_manager_data.h"
#include "event_bus_conversion.h"
#include "scene_type_conversion.h"
#include <iostream>

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, steamrot::FailInfo>
ConfigureEngineSnapshotWithUUIDs(steamrot::EngineSnapshot &snapshot,
                                 const steamrot::EngineSnapshotFbs *fb_snapshot,
                                 steamrot::EventHandler &event_handler,
                                 UUIDAssignmentTracker &uuid_tracker,
                                 bool is_starting_snapshot) {

  // Validate input
  if (!fb_snapshot) {
    return std::unexpected(steamrot::FailInfo{
        steamrot::FailMode::FlatbuffersDataNotFound, "EngineSnapshotFbs is null"});
  }

  // Configure tick_number (optional field)
  if (fb_snapshot->tick_number() > 0) {
    snapshot.tick_number = fb_snapshot->tick_number();
  }

  // Configure global_event_bus (optional field)
  if (fb_snapshot->global_event_bus()) {
    auto event_bus_result = steamrot::event::ConvertEventBusDataToEventBus(
        fb_snapshot->global_event_bus());
    if (!event_bus_result.has_value()) {
      return std::unexpected(event_bus_result.error());
    }
    snapshot.global_event_bus = event_bus_result.value();
  }

  // Configure scene_manager_data (optional field)
  if (fb_snapshot->scene_manager_data()) {
    steamrot::SceneManagerData scene_manager_data;
    auto configure_state_result =
        steamrot::data::configure::ConfigureSceneManagerState(
            scene_manager_data.scene_manager_state,
            fb_snapshot->scene_manager_data()->state());
    if (!configure_state_result.has_value()) {
      return std::unexpected(configure_state_result.error());
    }
    snapshot.scene_manager_data = scene_manager_data;
  }

  // Configure scene_collection_data with UUID assignment
  if (fb_snapshot->scene_collection_data() &&
      fb_snapshot->scene_collection_data()->scene_data()) {
    // Clear any existing scene data
    snapshot.scene_collection_data.clear();

    // Iterate over each SceneDataFbs in the collection
    for (const auto *scene_data_fbs :
         *fb_snapshot->scene_collection_data()->scene_data()) {
      if (!scene_data_fbs) {
        continue; // Skip null entries
      }

      // Create a FlatbuffersSceneDataProvider for this scene
      steamrot::FlatbuffersSceneDataProvider scene_provider(event_handler,
                                                            scene_data_fbs);

      // Create and configure the SceneData
      steamrot::SceneData scene_data;
      auto configure_result = scene_provider.ConfigureSceneData(scene_data);
      if (!configure_result.has_value()) {
        return std::unexpected(configure_result.error());
      }

      // Get the scene type from configured SceneData
      steamrot::SceneType scene_type = scene_data.scene_info.type;

      // Handle UUID assignment based on snapshot type
      if (is_starting_snapshot) {
        // For starting snapshot: process and possibly assign UUID
        std::optional<uuids::uuid> existing_uuid;

        // Check if scene already has a UUID (it would be set by
        // ConfigureSceneData)
        if (scene_data.scene_info.id != uuids::uuid{}) {
          existing_uuid = scene_data.scene_info.id;
        }

        // Process through tracker to ensure consistency
        uuids::uuid assigned_uuid =
            uuid_tracker.ProcessStartingSceneUUID(scene_type, existing_uuid);

        // Assign the UUID to the scene
        scene_data.scene_info.id = assigned_uuid;

      } else {
        // For expected snapshots: use UUID from starting snapshot
        // If scene in expected snapshot has a UUID, validate it matches
        uuids::uuid expected_uuid = uuid_tracker.GetOrAssignUUID(scene_type);

        // If the scene data has a UUID and it doesn't match, that's an error
        if (scene_data.scene_info.id != uuids::uuid{} &&
            scene_data.scene_info.id != expected_uuid) {
          return std::unexpected(steamrot::FailInfo{
              steamrot::FailMode::InvalidUUID,
              "Expected snapshot scene UUID doesn't match starting "
              "snapshot UUID for same SceneType"});
        }

        // Assign the UUID from starting snapshot
        scene_data.scene_info.id = expected_uuid;
      }

      // Add the configured SceneData to the collection
      snapshot.scene_collection_data.push_back(std::move(scene_data));
    }
  }

  return std::monostate{};
}

} // namespace steamrot::tests
