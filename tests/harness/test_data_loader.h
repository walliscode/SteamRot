/////////////////////////////////////////////////
/// @file
/// @brief Declaration of test data loader utilities
///
/// Provides all test data loading functionality for the test harness.
/// This includes:
/// - Loading test data from explicit paths (for CMake templates)
/// - Loading test data from adjacent directories (using __FILE__ macro)
/// - Discovering and loading .test_data.bin files
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

} // namespace steamrot::tests
