/////////////////////////////////////////////////
/// @file
/// @brief Declaration of test data harness utilities for loading test
/// configurations
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventHandler.h"
#include "FailInfo.h"
#include "containers.h"
#include "entities_generated.h"
#include "test_data_generated.h"
#include <expected>
#include <variant>
#include <vector>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Load test data configurations from an adjacent data directory
///
/// Internal function that takes the calling source file path to determine
/// the adjacent data directory. Use the LOAD_TEST_DATA_CONFIGS() macro instead.
///
/// @param source_file_path Path to the calling source file (__FILE__)
/// @return Vector of TestDataConfig pointers or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::vector<const TestDataConfig *>, FailInfo>
LoadTestDataConfigsImpl(const char *source_file_path);

/////////////////////////////////////////////////
/// @brief Macro to load test data from adjacent data directory
///
/// This macro uses __FILE__ from the call site to determine the adjacent
/// data/ directory. It automatically:
/// - Discovers all .test_data.bin files in the adjacent data/ directory
/// - Loads them as TestDataConfig objects
/// - Returns a vector suitable for use with Catch2 generators
///
/// The adjacent data/ directory is determined relative to where this macro
/// is invoked. An error is returned if the adjacent data/ directory does not
/// exist.
///
/// @return Vector of TestDataConfig pointers or FailInfo on error
/////////////////////////////////////////////////
#define load_test_data_configs() LoadTestDataConfigsImpl(__FILE__)

/////////////////////////////////////////////////
/// DEPRECATED DECLARATIONS
/// These functions are deprecated. Use RunTestEngineTest instead.
/////////////////////////////////////////////////

// std::expected<std::monostate, FailInfo>
// RunEntityMemoryPoolComparisonTest(...);
// void RunEventBusComparisonTest(...);
// std::expected<std::monostate, FailInfo> RunDataStructComparisonTest(...);

/////////////////////////////////////////////////
/// @brief Wrapper function for data-driven testing with TestEngine
///
/// This function orchestrates a complete test workflow:
/// 1. Creates a TestEngine from test data configuration
/// 2. Runs the TestEngine for the specified number of ticks
/// 3. Pulls out the data bank containing scene state at each tick
/// 4. Compares data bank entries with tick_snapshots from config
///
/// This is the main entry point for data-driven testing using TestEngine.
/// It should be called from TEST_CASE blocks with configs loaded from
/// load_test_data_configs().
///
/// The comparison is purely tick-based - it uses tick_snapshots from the
/// config to validate state at specific ticks. The expected_data_collection
/// is no longer used.
///
/// @param config The test data configuration containing all test parameters
/// @return std::monostate on success, FailInfo on error
///
/// Example usage:
/// @code
/// TEST_CASE("Data-driven test with TestEngine", "[unit]") {
///   auto configs = steamrot::tests::load_test_data_configs();
///   REQUIRE(configs.has_value());
///
///   const auto *config = GENERATE_COPY(from_range(configs.value()));
///
///   auto result = steamrot::tests::RunTestEngineTest(config);
///   REQUIRE(result.has_value());
/// }
/// @endcode
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunTestEngineTest(const TestDataConfig *config);

/////////////////////////////////////////////////
/// @brief DEPRECATED: Wrapper function for data-driven testing with TestFixture
///
/// @deprecated Use RunTestEngineTest instead for purely tick-based testing.
///
/// This function orchestrates a complete test workflow:
/// 1. Creates a TestFixture from test data configuration
/// 2. Executes tick-based test logic (inputs, events, simulation)
/// 3. Compares final state with expected_data_collection if present
///
/// @param config The test data configuration containing all test parameters
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunFixtureTest(const TestDataConfig *config);

} // namespace steamrot::tests
