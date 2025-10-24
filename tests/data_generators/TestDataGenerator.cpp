/////////////////////////////////////////////////
/// @file
/// @brief Implementation of TestDataGenerator utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestDataGenerator.h"
#include "PathProvider.h"
#include "TestDataLoader.h"
#include <filesystem>
#include <format>

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::vector<std::filesystem::path>, FailInfo>
DiscoverTestDataJsonFiles(const std::filesystem::path &directory_path) {

  // Check if directory exists
  if (!std::filesystem::exists(directory_path)) {
    std::string error_message = std::format(
        "Directory not found: {}", directory_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }

  if (!std::filesystem::is_directory(directory_path)) {
    std::string error_message = std::format(
        "Path is not a directory: {}", directory_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }

  std::vector<std::filesystem::path> json_files;

  // Iterate through directory and find .test_data.json files
  for (const auto &entry : std::filesystem::directory_iterator(directory_path)) {
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
LoadTestDataForGenerator(const std::string &subdirectory) {

  // Create TestDataLoader instance
  TestDataLoader loader;

  // Discover all test data files (this finds .test_data.bin files)
  auto test_names_result = loader.DiscoverTestDataFiles(subdirectory);
  
  if (!test_names_result.has_value()) {
    return std::unexpected(test_names_result.error());
  }

  const auto &test_names = test_names_result.value();

  // Load all test data configurations
  auto configs_result = loader.LoadMultipleTestData(test_names, subdirectory);
  
  if (!configs_result.has_value()) {
    return std::unexpected(configs_result.error());
  }

  return configs_result.value();
}

/////////////////////////////////////////////////
std::expected<std::vector<std::string>, FailInfo>
GetTestNamesForGenerator(const std::string &subdirectory) {

  // Create TestDataLoader instance
  TestDataLoader loader;

  // Discover and return test names
  return loader.DiscoverTestDataFiles(subdirectory);
}

} // namespace steamrot::tests
