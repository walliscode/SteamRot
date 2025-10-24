/////////////////////////////////////////////////
/// @file
/// @brief Declaration of TestDataGenerator utilities for Catch2 generators
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
#include <string>
#include <vector>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Find all .test_data.json files in a directory
///
/// This function searches for test data JSON files in the specified directory.
/// It looks for files matching the pattern "*.test_data.json".
///
/// @param directory_path Path to the directory to search
/// @return Vector of file paths or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::vector<std::filesystem::path>, FailInfo>
DiscoverTestDataJsonFiles(const std::filesystem::path &directory_path);

/////////////////////////////////////////////////
/// @brief Load TestDataConfig objects from discovered JSON files
///
/// This function discovers all .test_data.json files in the given directory,
/// loads the corresponding .test_data.bin files, and returns TestDataConfig
/// objects suitable for use with Catch2 generators.
///
/// Note: The function loads .test_data.bin files (compiled from JSON) since
/// those are what TestDataLoader works with.
///
/// @param subdirectory Test executable subdirectory (e.g., "entity", "components")
/// @return Vector of TestDataConfig pointers or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::vector<const TestDataConfig *>, FailInfo>
LoadTestDataForGenerator(const std::string &subdirectory);

/////////////////////////////////////////////////
/// @brief Helper function to get test names from a directory for generators
///
/// This function discovers test data files and returns just the test names
/// (without extensions), which is useful for Catch2's from_range generator.
///
/// @param subdirectory Test executable subdirectory (e.g., "entity", "components")
/// @return Vector of test names or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::vector<std::string>, FailInfo>
GetTestNamesForGenerator(const std::string &subdirectory);

} // namespace steamrot::tests
