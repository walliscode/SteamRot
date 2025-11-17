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
#include "TestFixture.h"
#include "containers.h"
#include "entities_generated.h"
#include "test_context.h"
#include "test_data_generated.h"
#include <expected>
#include <string>
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
/// @brief Create and configure TestFixture from test data configuration
///
/// This function creates a TestFixture and configures it based on the
/// test data configuration. The fixture will have entities populated from
/// start_data_collection if present.
///
/// This is called at the beginning of test execution, before any ticks run.
///
/// @param config The test data configuration
/// @param scene_type The scene type for the fixture (default: SceneType_TEST)
/// @return Configured TestFixture or FailInfo on error
/////////////////////////////////////////////////
std::expected<TestFixture, FailInfo> CreateFixtureFromTestData(
    const TestDataConfig *config,
    const SceneType &scene_type = SceneType::SceneType_TEST);

/////////////////////////////////////////////////
/// @brief Wrapper function for EntityMemoryPool comparison tests
///
/// This function compares two EntityMemoryPool instances using the
/// EqualsEntityMemoryPool matcher, ensuring detailed error messages on
/// mismatch. This is a low-level comparison function - prefer using
/// RunDataStructComparisonTest for higher-level testing.
///
/// @param actual The actual EntityMemoryPool to test
/// @param expected The expected EntityMemoryPool to compare against
/// @param context Test context information for enriched error messages
/// @param expected_to_pass If true, expects pools to match; if false, expects
/// mismatch
/////////////////////////////////////////////////
void RunEntityMemoryPoolComparisonTest(const EntityMemoryPool &actual,
                                       const EntityMemoryPool &expected,
                                       const TestContext &context,
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
                               const TestContext &context,
                               bool expected_to_pass = true);

/////////////////////////////////////////////////
/// @brief Primary function for comparing data structures from DataCollection
///
/// This function orchestrates comparison of all data structures specified
/// in the DataCollection (EntityMemoryPool, EventBus, etc.). It serves as
/// the primary entry point for data structure comparison in the test harness.
///
/// The function compares actual state from the fixture against expected state
/// from the data_collection parameter. Each data structure is compared if
/// present in the collection.
///
/// This function is used by both:
/// - RunFixtureTest (for final state comparison after all ticks)
/// - CompareTickSnapshot (for intermediate state comparison during ticks)
///
/// @param data_collection Expected data structure states for comparison
/// @param fixture TestFixture containing actual state to compare
/// @param context Test context information for enriched error messages
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunDataStructComparisonTest(const DataCollection *data_collection,
                            TestFixture &fixture, const TestContext &context);

/////////////////////////////////////////////////
/// @brief Wrapper function for data-driven testing with TestFixture
///
/// This function orchestrates a complete test workflow:
/// 1. Creates a TestFixture from test data configuration
/// 2. Executes tick-based test logic (inputs, events, simulation)
/// 3. Compares final state with expected_data_collection if present
///
/// This is the top-level function for data-driven testing. It should be
/// called from TEST_CASE blocks with configs loaded from load_test_data_configs().
///
/// The function respects the expected_to_pass flag in test metadata, allowing
/// tests to verify both successful matches and expected mismatches.
///
/// @param config The test data configuration containing all test parameters
/// @return std::monostate on success, FailInfo on error
///
/// Example usage:
/// @code
/// TEST_CASE("Data-driven test with fixture", "[unit]") {
///   auto configs = steamrot::tests::load_test_data_configs();
///   REQUIRE(configs.has_value());
///
///   const auto *config = GENERATE_COPY(from_range(configs.value()));
///
///   auto result = steamrot::tests::RunFixtureTest(config);
///   REQUIRE(result.has_value());
/// }
/// @endcode
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunFixtureTest(const TestDataConfig *config);

} // namespace steamrot::tests
