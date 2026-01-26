/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for configuring engine snapshot data
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
#include "engine_snapshot_generated.h"
#include <expected>

namespace steamrot::data::configure {

/////////////////////////////////////////////////
/// @brief Configures EngineSnapshot from FlatBuffers data.
///
/// This function configures an EngineSnapshot object from FlatBuffers
/// EngineSnapshotFbs data. All fields in EngineSnapshot are optional
/// to support selective testing scenarios.
///
/// @param snapshot EngineSnapshot to configure.
/// @param fb_snapshot FlatBuffers EngineSnapshotFbs data.
/// @param event_handler Reference to EventHandler for entity importers.
/// @return std::monostate on success, FailInfo on error.
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureEngineSnapshot(EngineSnapshot &snapshot,
                        const EngineSnapshotFbs *fb_snapshot,
                        EventHandler &event_handler);

} // namespace steamrot::data::configure
