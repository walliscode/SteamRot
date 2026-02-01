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

#include "EngineSnapshot.h"
#include "FailInfo.h"
#include "SceneData.h"
#include "TestData.h"
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
std::expected<std::monostate, FailInfo>
ConvertEntityTransportVariant(SceneData &scene_data);

/////////////////////////////////////////////////
/// @brief Converts all EngineSnapshot SceneData variants to the wanted type
///
/// @param engine_snapshot EngineSnapshot object with SceneData variants to
/// convert
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConvertAllEntityTransportVariants(EngineSnapshot &engine_snapshot);

/////////////////////////////////////////////////
/// @brief Converts all EngineSnapshot SceneData variants to the wanted type
///
/// @param test_data TestData object with EngineSnapshot variants to convert
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConvertAllEntityTransportVariantsInTestData(TestData &test_data);
} // namespace steamrot::tests
