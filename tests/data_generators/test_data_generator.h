/////////////////////////////////////////////////
/// @file
/// @brief Declaration of test data generator utilities for Catch2 generators
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
/// @brief Find all .test_data.json files in the adjacent data directory
///
/// This function searches for test data JSON files in the data/ directory
/// adjacent to the calling source file. It looks for files matching the
/// pattern "*.test_data.json". The function uses __FILE__ to determine
/// the source file location and expects a data/ subdirectory to exist.
///
/// @return Vector of file paths or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::vector<std::filesystem::path>, FailInfo>
discover_test_data_json_files();

/////////////////////////////////////////////////
/// @brief Load TestDataConfig objects from the adjacent data directory
///
/// This function discovers all .test_data.json files in the adjacent data/
/// directory, loads the corresponding .test_data.bin files, and returns
/// TestDataConfig objects suitable for use with Catch2 generators.
///
/// Note: The function loads .test_data.bin files (compiled from JSON) since
/// those are what TestDataLoader works with. An adjacent data/ directory
/// must exist or the function will return an error.
///
/// @return Vector of TestDataConfig pointers or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::vector<const TestDataConfig *>, FailInfo>
load_test_data_for_generator();

/////////////////////////////////////////////////
/// @brief Get test names from the adjacent data directory for generators
///
/// This function discovers test data files in the adjacent data/ directory
/// and returns just the test names (without extensions), which is useful
/// for Catch2's from_range generator. An adjacent data/ directory must
/// exist or the function will return an error.
///
/// @return Vector of test names or FailInfo on error
/////////////////////////////////////////////////
std::expected<std::vector<std::string>, FailInfo>
get_test_names_for_generator();

} // namespace steamrot::tests
