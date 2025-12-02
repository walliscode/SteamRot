/////////////////////////////////////////////////
/// @file
/// @brief Implementation of test data loader utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "test_data_loader.h"
#include <format>
#include <fstream>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Helper to load binary data from file
/////////////////////////////////////////////////
static char *LoadBinaryData(const std::filesystem::path &file_path) {
  std::ifstream infile;
  infile.open(file_path, std::ios::binary | std::ios::in);
  if (!infile.is_open()) {
    return nullptr;
  }
  infile.seekg(0, std::ios::end);
  auto length = infile.tellg();
  if (length <= 0) {
    return nullptr;
  }
  infile.seekg(0, std::ios::beg);
  char *data = new char[static_cast<size_t>(length)];
  infile.read(data, length);
  infile.close();
  return data;
}

/////////////////////////////////////////////////
/// @brief Helper to get adjacent data directory from source file path
/////////////////////////////////////////////////
static std::expected<std::filesystem::path, FailInfo>
GetAdjacentDataDirectory(const char *source_file_path) {
  std::filesystem::path source_path = source_file_path;
  std::filesystem::path source_dir = source_path.parent_path();
  std::filesystem::path data_dir = source_dir / "data";

  if (!std::filesystem::exists(data_dir)) {
    std::string error_message =
        std::format("Adjacent data directory not found: {}", data_dir.string());
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
DiscoverTestDataFiles(const std::filesystem::path &data_dir_path) {

  // Validate directory exists
  if (!std::filesystem::exists(data_dir_path)) {
    std::string error_message =
        std::format("Data directory not found: {}", data_dir_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }

  if (!std::filesystem::is_directory(data_dir_path)) {
    std::string error_message =
        std::format("Path is not a directory: {}", data_dir_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }

  std::vector<std::filesystem::path> file_paths;
  const std::string suffix = ".test_data.bin";

  // Iterate through directory and find .test_data.bin files
  for (const auto &entry : std::filesystem::directory_iterator(data_dir_path)) {
    if (entry.is_regular_file()) {
      std::string filename = entry.path().filename().string();

      // Check if filename ends with .test_data.bin
      if (filename.size() >= suffix.size() &&
          filename.compare(filename.size() - suffix.size(), suffix.size(),
                           suffix) == 0) {
        file_paths.push_back(entry.path());
      }
    }
  }

  return file_paths;
}

/////////////////////////////////////////////////
std::expected<const TestDataConfig *, FailInfo>
LoadTestDataConfigFromFile(const std::filesystem::path &file_path) {

  // Validate file exists
  if (!std::filesystem::exists(file_path)) {
    std::string error_message =
        std::format("Test data file not found: {}", file_path.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }

  // Load binary data
  char *data = LoadBinaryData(file_path);
  if (!data) {
    std::string error_message =
        std::format("Failed to read test data file: {}", file_path.string());
    return std::unexpected(
        FailInfo(FailMode::FlatbuffersDataNotFound, error_message));
  }

  // Parse the FlatBuffers data
  const TestDataConfig *config = GetTestDataConfig(data);
  if (!config) {
    std::string error_message =
        std::format("Failed to parse test data from: {}", file_path.string());
    return std::unexpected(
        FailInfo(FailMode::FlatbuffersDataNotFound, error_message));
  }

  // Verify required metadata is present
  if (!config->metadata()) {
    std::string error_message =
        std::format("Test data missing required metadata: {}",
                    file_path.string());
    return std::unexpected(
        FailInfo(FailMode::FlatbuffersDataNotFound, error_message));
  }

  return config;
}

/////////////////////////////////////////////////
std::expected<std::vector<const TestDataConfig *>, FailInfo>
LoadTestDataConfigsFromPath(const std::filesystem::path &data_dir_path) {

  // Discover test data files
  auto files_result = DiscoverTestDataFiles(data_dir_path);
  if (!files_result.has_value()) {
    return std::unexpected(files_result.error());
  }

  std::vector<const TestDataConfig *> configs;
  configs.reserve(files_result.value().size());

  // Load each file
  for (const auto &file_path : files_result.value()) {
    auto config_result = LoadTestDataConfigFromFile(file_path);
    if (!config_result.has_value()) {
      return std::unexpected(config_result.error());
    }
    configs.push_back(config_result.value());
  }

  return configs;
}

/////////////////////////////////////////////////
std::expected<std::vector<const TestDataConfig *>, FailInfo>
LoadTestDataConfigsImpl(const char *source_file_path) {

  // Get the adjacent data directory using the provided source file path
  auto data_dir_result = GetAdjacentDataDirectory(source_file_path);
  if (!data_dir_result.has_value()) {
    return std::unexpected(data_dir_result.error());
  }

  return LoadTestDataConfigsFromPath(data_dir_result.value());
}

} // namespace steamrot::tests
