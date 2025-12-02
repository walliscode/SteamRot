/////////////////////////////////////////////////
/// @file
/// @brief Declaration of test harness utilities for orchestrating tests
///
/// The test harness provides:
/// - Wrapper functions for Catch2 generators
/// - TestEngine orchestration
/// - Test execution functions
///
/// For data loading, use test_data_loader.h
/// For data comparison, use test_data_comparison.h
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "test_data_generated.h"
#include <expected>
#include <variant>

namespace steamrot::tests {

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
std::expected<std::monostate, FailInfo> RunTestHarness(TestDataConfig *config);

} // namespace steamrot::tests
