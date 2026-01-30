/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for running the test harness.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "FailInfo.h"
#include "SceneData.h"
#include <expected>
#include <filesystem>
#include <variant>
namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Wrapper function to run all harness tests located in the adjacent
/// directory
///
/// @param current_location The current file location (use __FILE__ macro)
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunHarnessTests(const std::filesystem::path current_location);

/////////////////////////////////////////////////
/// @brief Converts the variant in the SceneData to the wanted type
///
/// @param scene_data SceneData object with variant to convert
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConvertEMPData(SceneData &scene_data);

/////////////////////////////////////////////////
/// @brief Converts EntityCollectionFbs data to EntityMemoryPool for all scenes
/// in an EngineSnapshot
///
/// This helper function converts FlatBuffers entity data to EntityMemoryPool
/// format for comparison purposes in tests. It processes all SceneData objects
/// in the scene_collection_data vector.
///
/// @param snapshot EngineSnapshot containing scene collection to convert
/// @return std::expected with monostate on success, or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConvertAllSceneEntityData(EngineSnapshot &snapshot);

} // namespace steamrot::tests
