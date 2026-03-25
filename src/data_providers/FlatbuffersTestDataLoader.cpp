/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the FlatbuffersTestDataLoader class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersTestDataLoader.h"
#include <format>

namespace steamrot {

/////////////////////////////////////////////////
FlatbuffersTestDataLoader::FlatbuffersTestDataLoader(
    std::filesystem::path obj_dir_path)
    : m_object_directory_path(obj_dir_path) {}

/////////////////////////////////////////////////
std::expected<std::filesystem::path, FailInfo>
FlatbuffersTestDataLoader::GetAdjacentDataDirectoryPath() const {

  // construct the path to the "data" directory
  std::filesystem::path data_dir_path = m_object_directory_path / "data";

  // Check if the "data" directory exists
  if (std::filesystem::exists(data_dir_path) &&
      std::filesystem::is_directory(data_dir_path)) {

    // Return the path if found
    return data_dir_path;

  } else {

    return std::unexpected(FailInfo(
        FailMode::DirectoryNotFound,
        "Data directory not found at path: " + data_dir_path.string()));
  }
}

/////////////////////////////////////////////////
std::expected<std::vector<TestDataFbs *>, FailInfo>
FlatbuffersTestDataLoader::LoadTestDataFbs() const {

  // Get the data directory path
  auto data_dir_result = GetAdjacentDataDirectoryPath();
  if (!data_dir_result.has_value()) {
    return std::unexpected(data_dir_result.error());
  }

  std::filesystem::path data_dir_path = data_dir_result.value();

  // return vector of TestDataFbs pointers
  std::vector<TestDataFbs *> test_data_vector;

  // Iterate over files in the data directory
  for (const auto &entry : std::filesystem::directory_iterator(data_dir_path)) {

    // Check for .bin files
    if (entry.path().string().ends_with(".test_data.bin")) {

      // Load the binary data
      char *binary_data = LoadBinaryData(entry.path());

      // Get the TestDataFbs pointer
      const TestDataFbs *test_data_fbs = GetTestDataFbs(binary_data);
      if (test_data_fbs) {
        test_data_vector.push_back(
            const_cast<TestDataFbs *>(test_data_fbs));
      } else {
        std::string error_message = std::format(
            "TestDataFbs pointer is null for file: {}", entry.path().string());
        return std::unexpected(FailInfo(
            FailMode::FlatbuffersDataNotFound, error_message));
      }
    }
  }
  return test_data_vector;
}

/////////////////////////////////////////////////
std::filesystem::path
FlatbuffersTestDataLoader::GetObjectDirectoryPath() const {
  return m_object_directory_path;
}

} // namespace steamrot
