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
/// @brief Wrapper function to run EMP comparison tests
///
/// This function compares two EntityMemoryPool instances using the
/// EqualsEntityMemoryPool matcher, ensuring detailed error messages on
/// mismatch.
///
/// This allows tests to instantiate and manipulate EMPs (e.g., simulate logic)
/// before comparison.
///
/// @param actual The actual EntityMemoryPool to test
/// @param expected The expected EntityMemoryPool to compare against
/// @param expected_to_pass If true, expects pools to match; if false, expects
/// mismatch
/////////////////////////////////////////////////
void RunEntityMemoryPoolComparisonTest(const EntityMemoryPool &actual,
                                       const EntityMemoryPool &expected,
                                       bool expected_to_pass = true);

/////////////////////////////////////////////////
/// @brief Wrapper function to run EMP comparison tests with test metadata
///
/// This function compares two EntityMemoryPool instances using the
/// EqualsEntityMemoryPool matcher with test metadata, ensuring detailed
/// error messages on mismatch that include the test name and description.
///
/// @param actual The actual EntityMemoryPool to test
/// @param expected The expected EntityMemoryPool to compare against
/// @param test_metadata Test metadata string to include in error messages
/// @param expected_to_pass If true, expects pools to match; if false, expects
/// mismatch
/////////////////////////////////////////////////
void RunEntityMemoryPoolComparisonTest(const EntityMemoryPool &actual,
                                       const EntityMemoryPool &expected,
                                       const std::string &test_metadata,
                                       bool expected_to_pass = true);

std::expected<std::monostate, FailInfo>
RunEntityMemoryPoolComparisonTest(const EntityMemoryPool &actual_memory_pool,
                                  const EntityCollection *expected_collection,
                                  TestFixture &fixture,
                                  bool expected_to_pass = true);

/////////////////////////////////////////////////
/// @brief Wrapper function to run EventBus comparison tests
///
/// This function compares two EventBus instances using the
/// EqualsEventBus matcher, ensuring detailed error messages on
/// mismatch.
///
/// @param actual The actual EventBus to test
/// @param expected The expected EventBus to compare against
/// @param expected_to_pass If true, expects buses to match; if false, expects
/// mismatch
/////////////////////////////////////////////////
void RunEventBusComparisonTest(const EventBus &actual, const EventBus &expected,
                               bool expected_to_pass = true);

/////////////////////////////////////////////////
/// @brief Wrapper function to run EventBus comparison tests with test metadata
///
/// This function compares two EventBus instances using the
/// EqualsEventBus matcher with test metadata context, ensuring detailed
/// error messages on mismatch that include the test name and description.
///
/// @param actual The actual EventBus to test
/// @param expected The expected EventBus to compare against
/// @param test_metadata Test metadata string to include in error messages
/// @param expected_to_pass If true, expects buses to match; if false, expects
/// mismatch
/////////////////////////////////////////////////
void RunEventBusComparisonTest(const EventBus &actual, const EventBus &expected,
                               const std::string &test_metadata,
                               bool expected_to_pass = true);

/////////////////////////////////////////////////
/// @brief Create and configure TestFixture from test data configuration
///
/// This function creates a TestFixture and configures it based on the
/// test data configuration. The fixture will have entities populated from
/// start_entity_collection if present.
///
/// @param config The test data configuration
/// @param scene_type The scene type for the fixture (default: SceneType_TEST)
/// @return Configured TestFixture or FailInfo on error
/////////////////////////////////////////////////
std::expected<TestFixture, FailInfo> CreateFixtureFromTestData(
    const TestDataConfig *config,
    const SceneType &scene_type = SceneType::SceneType_TEST);

/////////////////////////////////////////////////
/// @brief Wrapper function for data-driven testing with TestFixture
///
/// This function creates a TestFixture from test data configuration,
/// and if expected_entity_collection is present, compares the fixture's
/// entity state with the expected state.
///
/// This wrapper is designed to be used in TEST_CASE with Catch2 generators.
/// It handles fixture creation and comparison, leaving room for future
/// simulation functionality.
///
/// @param config The test data configuration
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

std::expected<std::monostate, FailInfo>
RunDataStructComparisonTest(const DataCollection *data_collection,
                            TestFixture &fixture, const TestContext &context);

} // namespace steamrot::tests
