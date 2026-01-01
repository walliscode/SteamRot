/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the ITestDataProvider class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ITestDataProvider.h"
#include "FailInfo.h"
#include <expected>

/////////////////////////////////////////////////
std::expected<std::filesystem::path, steamrot::FailInfo>
ITestDataProvider::GetAdjacentDataDirectoryPath() const {
  // get the current working directory
  std::filesystem::path current_path = std::filesystem::current_path();

  // construct the path to the "data" directory
  std::filesystem::path data_dir_path = current_path / "data";

  // Check if the "data" directory exists
  if (std::filesystem::exists(data_dir_path) &&
      std::filesystem::is_directory(data_dir_path)) {

    // Return the path if found
    return data_dir_path;

  } else {

    return std::unexpected(steamrot::FailInfo(
        steamrot::FailMode::DirectoryNotFound,
        "Data directory not found at path: " + data_dir_path.string()));
  }
}
