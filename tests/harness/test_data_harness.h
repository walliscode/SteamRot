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

} // namespace steamrot::tests
