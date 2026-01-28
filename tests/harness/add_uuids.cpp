/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions to add uuids to TestData
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "add_uuids.h"
#include "uuid.h"
#include <format>

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> add_uuids(TestData &data) {

  // if starting engine snapshot has no scene data, return error
  if (data.starting_engine_snapshot.scene_collection_data.empty())
    return std::unexpected(FailInfo{
        FailMode::MissingData, "Starting engine snapshot has no scene data"});

  // for each expected engine snapshot, the lengths of scene_collection_data
  // must match the starting engine snapshot
  for (const auto &[tick, snapshot] : data.expected_engine_snapshots) {
    if (snapshot.scene_collection_data.size() !=
        data.starting_engine_snapshot.scene_collection_data.size()) {
      return std::unexpected(FailInfo{
          FailMode::BadValue, std::format("SceneCollectionData length mismatch "
                                          "between starting snapshot and "
                                          "expected snapshot at tick {}",
                                          tick)});
    }
  }

  // generate UUIDs for each scene in starting snapshot, will need positional
  // information for this
  for (size_t i = 0;
       i < data.starting_engine_snapshot.scene_collection_data.size(); ++i) {

    // get reference to scene
    auto &scene = data.starting_engine_snapshot.scene_collection_data[i];

    // assign new uuid/ overwrite existing
    scene.scene_info.id = uuids::uuid_system_generator{}();

    // cycle through each expected snapshot and assign same uuid to
    // corresponding scene
    for (auto &[tick, snapshot] : data.expected_engine_snapshots) {
      auto &expected_scene = snapshot.scene_collection_data[i];
      expected_scene.scene_info.id = scene.scene_info.id;
    }
  }

  // If all checks pass, return success
  return std::monostate{};
}
} // namespace steamrot::tests
