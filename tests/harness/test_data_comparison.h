/////////////////////////////////////////////////
/// @file
/// @brief Declaration of test data comparison utilities
///
/// Provides data comparison functions for the test harness.
/// These functions compare actual data (from TestEngine data bank)
/// with expected data (from tick snapshots in test configuration).
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "SceneSnapshot.h"
#include "test_context.h"
#include "test_data_generated.h"
#include <expected>
#include <variant>
#include <vector>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Compare EntityMemoryPool with EngineState from tick snapshot
///
/// Compares a scene's entity memory pool against the expected state
/// from an EngineData snapshot. This function:
/// 1. Finds the matching scene by type in the expected engine state
/// 2. Configures an expected EntityMemoryPool from the snapshot
/// 3. Uses the EntityMemoryPoolEqualsMatcher for comparison
///
/// @param actual_scene_snapshot The actual scene snapshot containing entity
/// pool
/// @param expected_engine_state The expected engine state from tick snapshot
/// @param context Test context for enhanced error messages
/// @param expected_to_pass Whether the comparison is expected to pass
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
CompareTickSnapshotEntityPool(const SceneSnapshot &actual_scene_snapshot,
                              const EngineStateFbs *expected_engine_state,
                              const TestContext &context,
                              bool expected_to_pass);

/////////////////////////////////////////////////
/// @brief Compare data bank entry with tick snapshot
///
/// Compares all scenes in a data bank entry with the expected state
/// from a tick snapshot. Iterates through each scene and calls
/// CompareTickSnapshotEntityPool for the entity pool comparison.
///
/// @param actual_scene_snapshots Vector of actual scene snapshots from data
/// bank
/// @param tick_snapshot The tick snapshot containing expected state
/// @param context Test context for enhanced error messages
/// @param expected_to_pass Whether the comparison is expected to pass
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> CompareDataBankWithTickSnapshot(
    const std::vector<SceneSnapshot> &actual_scene_snapshots,
    const TickSnapshot *tick_snapshot, const TestContext &context,
    bool expected_to_pass);

} // namespace steamrot::tests
