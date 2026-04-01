/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for configuring engine snapshot data
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_engine_snapshot.h"
#include "FlatbuffersSceneDataProvider.h"
#include "configure_scene_manager_data.h"

namespace steamrot::data::configure {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEngineSnapshot(EngineSnapshot &snapshot,
                        const EngineSnapshotFbs *fb_snapshot,
                        EventHandler &event_handler) {

  // Validate input
  if (!fb_snapshot) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "EngineSnapshotFbs is null"});
  }

  // Configure tick_number (optional field)
  // FlatBuffers scalars are always present, but we treat 0 as "not set"
  // to match optional behavior
  if (fb_snapshot->tick_number() > 0) {
    snapshot.tick_number = fb_snapshot->tick_number();
  }
  // // Configure global_event_bus (optional field)
  // if (fb_snapshot->global_event_bus()) {
  //
  //   auto event_bus_result =
  //       event::ConvertEventBusDataFbsToEventBus(fb_snapshot->global_event_bus());
  //   if (!event_bus_result.has_value()) {
  //     return std::unexpected(event_bus_result.error());
  //   }
  //
  //   snapshot.global_event_bus = event_bus_result.value();
  // }
  // Configure scene_manager_data (optional field)
  if (fb_snapshot->scene_manager_data()) {
    SceneManagerData scene_manager_data;
    auto configure_state_result =
        ConfigureSceneManagerState(scene_manager_data.scene_manager_state,
                                   fb_snapshot->scene_manager_data()->state());
    if (!configure_state_result.has_value()) {
      return std::unexpected(configure_state_result.error());
    }
    snapshot.scene_manager_data = scene_manager_data;
  }

  // Configure scene_collection_data (optional field)
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
      FlatbuffersSceneDataProvider scene_provider(event_handler,
                                                  scene_data_fbs);

      // Create and configure the SceneData
      SceneData scene_data;
      auto configure_result = scene_provider.ConfigureSceneData(scene_data);
      if (!configure_result.has_value()) {
        return std::unexpected(configure_result.error());
      }

      // Add the configured SceneData to the collection
      snapshot.scene_collection_data.push_back(std::move(scene_data));
    }
  }

  return std::monostate{};
}

} // namespace steamrot::data::configure
