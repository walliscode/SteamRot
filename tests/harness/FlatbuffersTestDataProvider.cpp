/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the FlatbuffersTestDataProvider class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersTestDataProvider.h"
#include "TestData.h"
#include <expected>
#include <filesystem>

/////////////////////////////////////////////////
std::expected<std::vector<steamrot::TestData>, steamrot::FailInfo>
FlatbuffersTestDataProvider::ProviderAllTestData() const {

  // check for data directory, return error if not found
  auto get_data_dir_result = GetAdjacentDataDirectoryPath();
  if (!get_data_dir_result)
    return std::unexpected(get_data_dir_result.error());

  // get data directory path
  std::filesystem::path data_dir_path = get_data_dir_result.value();
  // provide return vector
  std::vector<steamrot::TestData> return_vector;

  // iterate over all the test data files
  for (const auto &file : std::filesystem::directory_iterator(data_dir_path)) {

    //
  }
  return return_vector;
}
