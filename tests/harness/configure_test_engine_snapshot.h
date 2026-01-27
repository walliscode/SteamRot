/////////////////////////////////////////////////
/// @file
/// @brief Declaration of configuration functions for TestData engine snapshots
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EngineSnapshot.h"
#include "EventHandler.h"
#include "FailInfo.h"
#include "UUIDAssignmentTracker.h"
#include "engine_snapshot_generated.h"
#include <expected>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Configures EngineSnapshot from FlatBuffers with UUID assignment
///
/// This function configures an EngineSnapshot for test data, ensuring that
/// scene UUIDs are properly assigned and tracked across multiple snapshots.
///
/// @param snapshot EngineSnapshot to configure
/// @param fb_snapshot FlatBuffers EngineSnapshotFbs data
/// @param event_handler Reference to EventHandler for entity importers
/// @param uuid_tracker UUID assignment tracker for consistent UUIDs
/// @param is_starting_snapshot true if this is the starting snapshot,
///        false for expected snapshots
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEngineSnapshotWithUUIDs(EngineSnapshot &snapshot,
                                 const EngineSnapshotFbs *fb_snapshot,
                                 EventHandler &event_handler,
                                 UUIDAssignmentTracker &uuid_tracker,
                                 bool is_starting_snapshot);

} // namespace steamrot::tests
