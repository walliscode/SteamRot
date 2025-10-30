/////////////////////////////////////////////////
/// @file
/// @brief Declaration of test data harness utilities for loading test configurations
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "TestFixture.h"
#include "containers.h"
#include "simulation_generated.h"
#include "test_data_generated.h"
#include <expected>
#include <string>
#include <vector>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Load all test data configurations from an adjacent data directory
///
/// This is the primary function for loading test data. It automatically:
/// - Discovers all .test_data.bin files in the adjacent data/ directory
/// - Loads them as TestDataConfig objects
/// - Returns a vector suitable for use with Catch2 generators
///
/// The adjacent data/ directory is determined using the __FILE__ macro.
/// An error is returned if the adjacent data/ directory does not exist.
///
/// @return Vector of TestDataConfig pointers or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::vector<const TestDataConfig *>, FailInfo>
load_test_data_configs();

/////////////////////////////////////////////////
/// @brief Load test data configurations from a specific subdirectory
///
/// Loads test data from tests/<subdirectory>/data/ directory.
/// This is useful when you need to load test data from a specific test directory.
///
/// @param subdirectory Test subdirectory name (e.g., "entity", "components")
/// @return Vector of TestDataConfig pointers or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::vector<const TestDataConfig *>, FailInfo>
load_test_data_configs(const std::string &subdirectory);

/////////////////////////////////////////////////
/// @brief Top-level wrapper to run tests based on TestDataConfig contents
///
/// This wrapper examines the TestDataConfig and dispatches to appropriate
/// test functions based on what data is present. It provides a unified
/// entry point for data-driven testing.
///
/// Currently supports:
/// - Entity Memory Pool comparison tests (start_entity_collection + expected_entity_collection)
///
/// Future extensions can add support for:
/// - Event sequence tests
/// - UI configuration tests
/// - Logic execution tests
/// - etc.
///
/// @param config The test data configuration to process
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
run_test_data_config(const TestDataConfig *config);

/////////////////////////////////////////////////
/// @brief Wrapper function to run EMP comparison tests
///
/// This function compares two EntityMemoryPool instances using the
/// EqualsEntityMemoryPool matcher, ensuring detailed error messages on mismatch.
///
/// This allows tests to instantiate and manipulate EMPs (e.g., simulate logic)
/// before comparison.
///
/// @param actual The actual EntityMemoryPool to test
/// @param expected The expected EntityMemoryPool to compare against
/////////////////////////////////////////////////
void run_entity_memory_pool_comparison_test(const EntityMemoryPool &actual,
                                            const EntityMemoryPool &expected);

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
std::expected<TestFixture, FailInfo>
create_fixture_from_test_data(const TestDataConfig *config,
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
///   auto result = steamrot::tests::run_fixture_test(config);
///   REQUIRE(result.has_value());
/// }
/// @endcode
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
run_fixture_test(const TestDataConfig *config);

} // namespace steamrot::tests
