/////////////////////////////////////////////////
/// @file
/// @brief Implementation of TestDataLoader class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TestDataLoader.h"
#include "FailInfo.h"
#include "test_data_generated.h"
#include <filesystem>
#include <format>
#include <fstream>

namespace steamrot::tests {

/////////////////////////////////////////////////
TestDataLoader::TestDataLoader() : m_path_provider(EnvironmentType::Test) {}

/////////////////////////////////////////////////
char *TestDataLoader::LoadBinaryData(const std::filesystem::path &file_path) const {
  // open file in binary mode
  std::ifstream infile;
  infile.open(file_path, std::ios::binary | std::ios::in);
  infile.seekg(0, std::ios::end);
  int length = infile.tellg();
  infile.seekg(0, std::ios::beg);
  char *data = new char[length];
  infile.read(data, length);
  infile.close();
  return data;
}

/////////////////////////////////////////////////
std::filesystem::path
TestDataLoader::GetTestDataPath(const std::string &test_name,
                                const std::string &subdirectory) const {
  // Get the data directory (already returns tests/data in Test environment)
  auto data_dir_result = m_path_provider.GetDataDirectory();
  if (!data_dir_result.has_value()) {
    return std::filesystem::path();
  }

  std::filesystem::path base_path = data_dir_result.value();

  // Add subdirectory if provided
  if (!subdirectory.empty()) {
    base_path /= subdirectory;
  }

  // Construct the full path with .test_data.bin extension
  return base_path / (test_name + ".test_data.bin");
}

/////////////////////////////////////////////////
std::expected<const TestDataConfig *, FailInfo>
TestDataLoader::LoadTestData(const std::string &test_name,
                             const std::string &subdirectory) const {

  // Get the full path to the test data file
  std::filesystem::path test_data_path = GetTestDataPath(test_name, subdirectory);

  // Check if the file exists
  if (!std::filesystem::exists(test_data_path)) {
    std::string error_message =
        std::format("Test data file not found: {}", test_data_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }

  // Load the binary data
  const steamrot::TestDataConfig *test_data_config =
      GetTestDataConfig(LoadBinaryData(test_data_path));

  // Verify the data was loaded
  if (!test_data_config) {
    std::string error_message = std::format(
        "Failed to parse test data from: {}", test_data_path.string());
    return std::unexpected(
        FailInfo(FailMode::FlatbuffersDataNotFound, error_message));
  }

  // Verify required metadata is present
  if (!test_data_config->metadata()) {
    std::string error_message =
        std::format("Test data missing required metadata: {}",
                    test_data_path.string());
    return std::unexpected(
        FailInfo(FailMode::FlatbuffersDataNotFound, error_message));
  }

  return test_data_config;
}

/////////////////////////////////////////////////
std::expected<std::vector<std::string>, FailInfo>
TestDataLoader::DiscoverTestDataFiles(const std::string &subdirectory) const {

  // Get the data directory (already returns tests/data in Test environment)
  auto data_dir_result = m_path_provider.GetDataDirectory();
  if (!data_dir_result.has_value()) {
    return std::unexpected(data_dir_result.error());
  }

  std::filesystem::path search_path = data_dir_result.value();

  // Add subdirectory if provided
  if (!subdirectory.empty()) {
    search_path /= subdirectory;
  }

  // Check if directory exists
  if (!std::filesystem::exists(search_path)) {
    std::string error_message =
        std::format("Test data directory not found: {}", search_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }

  std::vector<std::string> test_names;

  // Iterate through directory and find .test_data.bin files
  for (const auto &entry : std::filesystem::directory_iterator(search_path)) {
    if (entry.is_regular_file()) {
      std::string filename = entry.path().filename().string();
      
      // Check if filename ends with .test_data.bin
      const std::string suffix = ".test_data.bin";
      if (filename.size() >= suffix.size() &&
          filename.compare(filename.size() - suffix.size(), suffix.size(),
                          suffix) == 0) {
        // Extract the test name (remove .test_data.bin extension)
        std::string test_name =
            filename.substr(0, filename.size() - suffix.size());
        test_names.push_back(test_name);
      }
    }
  }

  return test_names;
}

/////////////////////////////////////////////////
std::expected<std::vector<const TestDataConfig *>, FailInfo>
TestDataLoader::LoadMultipleTestData(
    const std::vector<std::string> &test_names,
    const std::string &subdirectory) const {

  std::vector<const TestDataConfig *> test_configs;

  for (const auto &test_name : test_names) {
    auto test_data_result = LoadTestData(test_name, subdirectory);
    
    // If any load fails, return the error
    if (!test_data_result.has_value()) {
      return std::unexpected(test_data_result.error());
    }
    
    test_configs.push_back(test_data_result.value());
  }

  return test_configs;
}

} // namespace steamrot::tests
