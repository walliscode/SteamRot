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
#include "test_data_generated.h"
#include <expected>
#include <functional>
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
/// @brief Setup TestFixture from test data configuration
///
/// This function creates and initializes a TestFixture based on the
/// start_entity_collection from the test data configuration.
/// The fixture's entities will be configured according to the test data.
///
/// @param config The test data configuration containing entity setup
/// @param scene_type The scene type for the fixture (default: SceneType_TEST)
/// @return Initialized TestFixture or FailInfo on error
/////////////////////////////////////////////////
std::expected<TestFixture, FailInfo>
setup_fixture_from_test_data(const TestDataConfig *config,
                             const SceneType &scene_type = SceneType::SceneType_TEST);

/////////////////////////////////////////////////
/// @brief Container function for running tests with TestFixture
///
/// This is the main integration function that:
/// 1. Loads test data from adjacent data directory
/// 2. For each test configuration:
///    a. Creates TestFixture with resources configured from test data
///    b. Calls the simulation function to manipulate the fixture
///    c. Compares final state with expected state from test data
///
/// The simulation function receives a reference to the TestFixture and can:
/// - Access and modify entities via fixture.GetEntityManager()
/// - Run logic simulations
/// - Trigger events
/// - Modify game/scene resources
///
/// After simulation, the function automatically compares the resulting
/// EntityMemoryPool with the expected state from test data.
///
/// @param simulation_fn Function to run simulations on the fixture
/// @return std::monostate on success, FailInfo on error
///
/// Example usage:
/// @code
/// auto result = run_test_with_fixture([](TestFixture& fixture) {
///   // Get entity manager and simulate logic
///   auto& entity_manager = fixture.GetEntityManager();
///   auto& pool = entity_manager.GetEntityMemoryPool();
///   
///   // Perform simulations...
///   // Modify entities, run logic, etc.
/// });
/// REQUIRE(result.has_value());
/// @endcode
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
run_test_with_fixture(std::function<void(TestFixture&)> simulation_fn);

/////////////////////////////////////////////////
/// @brief Run a single test configuration with TestFixture
///
/// This function runs a test for a single configuration:
/// 1. Creates TestFixture from test data
/// 2. Calls the simulation function
/// 3. Compares result with expected state
///
/// @param config The test data configuration
/// @param simulation_fn Function to run simulations on the fixture
/// @param scene_type The scene type for the fixture (default: SceneType_TEST)
/// @return std::monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
run_single_test_with_fixture(const TestDataConfig *config,
                             std::function<void(TestFixture&)> simulation_fn,
                             const SceneType &scene_type = SceneType::SceneType_TEST);

} // namespace steamrot::tests
