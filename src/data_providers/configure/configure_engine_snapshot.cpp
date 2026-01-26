/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for configuring engine snapshot data
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_engine_snapshot.h"
#include "configure_scene_manager_data.h"
#include "event_bus_conversion.h"

namespace steamrot::data::configure {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEngineSnapshot(EngineSnapshot &snapshot,
                        const EngineSnapshotFbs *fb_snapshot) {

  // Validate input
  if (!fb_snapshot) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound, "EngineSnapshotFbs is null"});
  }

  // Configure tick_number (optional field)
  // FlatBuffers scalars are always present, but we treat 0 as "not set"
  // to match optional behavior
  if (fb_snapshot->tick_number() > 0) {
    snapshot.tick_number = fb_snapshot->tick_number();
  }

  // Configure global_event_bus (optional field)
  if (fb_snapshot->global_event_bus()) {
    auto event_bus_result =
        event::ConvertEventBusDataToEventBus(fb_snapshot->global_event_bus());
    if (!event_bus_result.has_value()) {
      return std::unexpected(event_bus_result.error());
    }
    snapshot.global_event_bus = event_bus_result.value();
  }

  // Configure scene_manager_data (optional field)
  if (fb_snapshot->scene_manager_data()) {
    SceneManagerData scene_manager_data;
    auto configure_state_result = ConfigureSceneManagerState(
        scene_manager_data.scene_manager_state,
        fb_snapshot->scene_manager_data()->state());
    if (!configure_state_result.has_value()) {
      return std::unexpected(configure_state_result.error());
    }
    snapshot.scene_manager_data = scene_manager_data;
  }

  // Configure scene_collection_data (optional field)
  // Note: SceneCollectionData configuration is complex and handled by
  // FlatbuffersSceneDataProvider. For snapshot testing, this is typically
  // not configured via this function but rather directly constructed.
  if (fb_snapshot->scene_collection_data()) {
    // TODO: Implement SceneCollectionData configuration when needed
    // This would require iterating over scenes and creating SceneData objects
    return std::unexpected(
        FailInfo{FailMode::NotImplemented,
                 "SceneCollectionData configuration not yet implemented"});
  }

  return std::monostate{};
}

} // namespace steamrot::data::configure
