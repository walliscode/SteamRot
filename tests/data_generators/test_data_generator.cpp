/////////////////////////////////////////////////
/// @file
/// @brief Implementation of test data generator utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "test_data_generator.h"
#include "TestDataLoader.h"
#include <filesystem>
#include <format>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Helper function to get the adjacent data directory path
///
/// Uses __FILE__ macro to determine the current source file location
/// and constructs the path to the adjacent data/ directory.
///
/// @return Path to adjacent data directory or FailInfo on error
/////////////////////////////////////////////////
static std::expected<std::filesystem::path, FailInfo>
get_adjacent_data_directory() {
  // Get the directory containing this source file
  std::filesystem::path source_file_path = __FILE__;
  std::filesystem::path source_dir = source_file_path.parent_path();
  
  // Construct path to adjacent data directory
  std::filesystem::path data_dir = source_dir / "data";
  
  // Check if data directory exists
  if (!std::filesystem::exists(data_dir)) {
    std::string error_message = std::format(
        "Adjacent data directory not found: {}", data_dir.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }
  
  if (!std::filesystem::is_directory(data_dir)) {
    std::string error_message = std::format(
        "Adjacent data path is not a directory: {}", data_dir.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }
  
  return data_dir;
}

/////////////////////////////////////////////////
std::expected<std::vector<std::filesystem::path>, FailInfo>
discover_test_data_json_files() {
  
  // Get the adjacent data directory
  auto data_dir_result = get_adjacent_data_directory();
  if (!data_dir_result.has_value()) {
    return std::unexpected(data_dir_result.error());
  }
  
  const auto &data_dir = data_dir_result.value();
  std::vector<std::filesystem::path> json_files;

  // Iterate through directory and find .test_data.json files
  for (const auto &entry : std::filesystem::directory_iterator(data_dir)) {
    if (entry.is_regular_file()) {
      std::string filename = entry.path().filename().string();
      
      // Check if filename ends with .test_data.json
      const std::string suffix = ".test_data.json";
      if (filename.size() >= suffix.size() &&
          filename.compare(filename.size() - suffix.size(), suffix.size(),
                          suffix) == 0) {
        json_files.push_back(entry.path());
      }
    }
  }

  return json_files;
}

/////////////////////////////////////////////////
std::expected<std::vector<const TestDataConfig *>, FailInfo>
load_test_data_for_generator() {

  // Get the adjacent data directory path
  auto data_dir_result = get_adjacent_data_directory();
  if (!data_dir_result.has_value()) {
    return std::unexpected(data_dir_result.error());
  }

  // Create TestDataLoader instance
  TestDataLoader loader;

  // Discover all test data files using "data_generators" subdirectory
  auto test_names_result = loader.DiscoverTestDataFiles("data_generators");
  
  if (!test_names_result.has_value()) {
    return std::unexpected(test_names_result.error());
  }

  const auto &test_names = test_names_result.value();

  // Load all test data configurations
  auto configs_result = loader.LoadMultipleTestData(test_names, "data_generators");
  
  if (!configs_result.has_value()) {
    return std::unexpected(configs_result.error());
  }

  return configs_result.value();
}

/////////////////////////////////////////////////
std::expected<std::vector<std::string>, FailInfo>
get_test_names_for_generator() {

  // Get the adjacent data directory to ensure it exists
  auto data_dir_result = get_adjacent_data_directory();
  if (!data_dir_result.has_value()) {
    return std::unexpected(data_dir_result.error());
  }

  // Create TestDataLoader instance
  TestDataLoader loader;

  // Discover and return test names using "data_generators" subdirectory
  return loader.DiscoverTestDataFiles("data_generators");
}

} // namespace steamrot::tests
