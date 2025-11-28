/////////////////////////////////////////////////
/// @file
/// @brief Declaration of path-based test data loader utilities
///
/// Provides alternative test data loading that accepts explicit
/// paths, enabling CMake-configured test templates.
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
#include <filesystem>
#include <vector>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Load test data configurations from a specified directory path
///
/// This function loads all .test_data.bin files from the given directory.
/// Unlike load_test_data_configs() which uses __FILE__ to find adjacent
/// data directories, this function accepts an explicit path.
///
/// This is primarily used by CMake-configured test templates where the
/// data directory path is known at configure time.
///
/// @param data_dir_path Path to the directory containing .test_data.bin files
/// @return Vector of TestDataConfig pointers or FailInfo on error
///
/// Example usage:
/// @code
/// // In CMake-generated test file:
/// constexpr const char* TEST_DATA_DIR = "@TEST_DATA_DIR@";
/// auto configs = LoadTestDataConfigsFromPath(TEST_DATA_DIR);
/// @endcode
/////////////////////////////////////////////////
std::expected<std::vector<const TestDataConfig *>, FailInfo>
LoadTestDataConfigsFromPath(const std::filesystem::path &data_dir_path);

/////////////////////////////////////////////////
/// @brief Discover .test_data.bin files in a directory
///
/// Returns a list of paths to all .test_data.bin files found in the
/// specified directory (non-recursive).
///
/// @param data_dir_path Path to the directory to search
/// @return Vector of paths to test data files, or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::vector<std::filesystem::path>, FailInfo>
DiscoverTestDataFiles(const std::filesystem::path &data_dir_path);

/////////////////////////////////////////////////
/// @brief Load a single test data configuration from a file path
///
/// @param file_path Path to the .test_data.bin file
/// @return Pointer to TestDataConfig, or FailInfo on error
/////////////////////////////////////////////////
std::expected<const TestDataConfig *, FailInfo>
LoadTestDataConfigFromFile(const std::filesystem::path &file_path);

} // namespace steamrot::tests
