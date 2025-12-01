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
#include "TestEngine.h"
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
/// @brief Wrapper function for EntityMemoryPool comparison with
/// EntityCollection
///
/// This function configures an expected EntityMemoryPool from an
/// EntityCollection and compares it with the actual pool. This is a convenience
/// function that handles the EMP setup internally.
///
/// @param actual_memory_pool The actual EntityMemoryPool to test
/// @param expected_collection The EntityCollection to configure expected pool
/// from
/// @param fixture TestFixture containing resources for configuration
/// @param context Test context information for enriched error messages
/// @param expected_to_pass If true, expects pools to match; if false, expects
/// mismatch
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunEntityMemoryPoolComparisonTest(const EntityMemoryPool &actual_memory_pool,
                                  const EntityCollection *expected_collection,
                                  bool expected_to_pass = true);

/////////////////////////////////////////////////
/// @brief Wrapper function for EventBus comparison tests
///
/// This function compares two EventBus instances using the
/// EqualsEventBus matcher, ensuring detailed error messages on
/// mismatch. This is a low-level comparison function - prefer using
/// RunDataStructComparisonTest for higher-level testing.
///
/// @param actual The actual EventBus to test
/// @param expected The expected EventBus to compare against
/// @param context Test context information for enriched error messages
/// @param expected_to_pass If true, expects buses to match; if false, expects
/// mismatch
/////////////////////////////////////////////////
void RunEventBusComparisonTest(const EventBus &actual, const EventBus &expected,
                               bool expected_to_pass = true);

/////////////////////////////////////////////////
/// @brief Primary function for comparing data structures from DataCollection
///
/// DEPRECATED: This function is part of the old TestFixture workflow.
/// For new tests, use RunTestEngineTest which provides tick-based comparison.
///
/// This function orchestrates comparison of all data structures specified
/// in the DataCollection (EntityMemoryPool, EventBus, etc.). It serves as
/// the primary entry point for data structure comparison in the test harness.
///
/// @param expected_to_pass If true, expects data to match; if false, expects
/// mismatch
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunDataStructComparisonTest(bool expected_to_pass = true);

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
