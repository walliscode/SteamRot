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
#include <cstdint>
#include <expected>
#include <filesystem>
#include <string>
#include <variant>
namespace steamrot::tests {

// Forward declarations
class TestEngine;

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

/////////////////////////////////////////////////
/// @brief Compare an actual EngineSnapshot against an expected EngineSnapshot
///
/// @param actual The actual EngineSnapshot from TestEngine
/// @param expected The expected EngineSnapshot from TestData
/// @param test_name Optional test name for error reporting
/// @param tick Optional tick number for error reporting
/// @return std::monostate on match, FailInfo on mismatch
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
CompareEngineSnapshots(const EngineSnapshot &actual,
                       const EngineSnapshot &expected,
                       const std::string &test_name = "",
                       uint32_t tick = 0);

/////////////////////////////////////////////////
/// @brief Run all snapshot comparisons for a single test
///
/// @param test_engine The TestEngine instance that has run the simulation
/// @param test_data The TestData with expected snapshots
/// @return std::monostate on success, FailInfo on any failure
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunSnapshotComparisons(const TestEngine &test_engine,
                       const TestData &test_data);
} // namespace steamrot::tests
