/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the FlatbuffersTestDataProvider class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersTestDataProvider.h"
#include "FlatbuffersTestDataLoader.h"
#include "TestData.h"
#include <expected>
#include <filesystem>

/////////////////////////////////////////////////
FlatbuffersTestDataProvider::FlatbuffersTestDataProvider(
    std::filesystem::path obj_dir_path)
    : ITestDataProvider(obj_dir_path) {}

/////////////////////////////////////////////////
std::expected<std::vector<steamrot::TestData>, steamrot::FailInfo>
FlatbuffersTestDataProvider::ProviderAllTestData() const {
  // Instantiate the FlatbuffersTestDataLoader and pass the object directory
  // path
  FlatbuffersTestDataLoader data_loader(object_directory_path);

  // create vector to hold TestData instances
  std::vector<steamrot::TestData> test_data_vec;

  // Load all TestDataFbs
  auto fbs_test_data_result = data_loader.LoadTestDataFbs();
  if (!fbs_test_data_result) {
    return std::unexpected(fbs_test_data_result.error());
  }
  const auto &fbs_test_data_vec = fbs_test_data_result.value();

  // Convert each TestDataFbs to TestData
  for (const auto *fbs_test_data : fbs_test_data_vec) {
    auto test_data_result = CreateTestData(fbs_test_data);
    if (!test_data_result) {
      return std::unexpected(test_data_result.error());
    }
    test_data_vec.push_back(test_data_result.value());
  }

  return test_data_vec;
}

/////////////////////////////////////////////////
std::expected<steamrot::TestData, steamrot::FailInfo>
FlatbuffersTestDataProvider::CreateTestData(
    const steamrot::TestDataFbs *fbs_test_data) const {

  if (fbs_test_data == nullptr) {

    return std::unexpected(
        steamrot::FailInfo{steamrot::FailMode::FlatbuffersDataNotFound,
                           "Input Flatbuffers TestData is "
                           "null."});
  }
  // Create and populate the TestData instance
  steamrot::TestData test_data;

  // Configure TestMetaData
  auto meta_data_result =
      ConfigureTestMetaData(test_data.meta_data, fbs_test_data->meta_data());
  if (!meta_data_result)
    return std::unexpected(meta_data_result.error());

  return test_data;
}

/////////////////////////////////////////////////
std::expected<std::monostate, steamrot::FailInfo>
FlatbuffersTestDataProvider::ConfigureTestMetaData(
    steamrot::TestMetaData &test_meta_data,
    const steamrot::TestMetadataFbs *fbs_test_meta_data) const {
  if (fbs_test_meta_data == nullptr) {
    return std::unexpected(
        steamrot::FailInfo{steamrot::FailMode::FlatbuffersDataNotFound,
                           "Input Flatbuffers TestMetaData is null."});
  }

  // add required field: test_name
  if (!fbs_test_meta_data->test_name()) {
    return std::unexpected(steamrot::FailInfo{
        steamrot::FailMode::FlatbuffersDataNotFound,
        "TestMetaDataFbs is missing required field: test_name."});
  } else {
    test_meta_data.test_name = fbs_test_meta_data->test_name()->str();
  }

  // add optional field: description
  if (fbs_test_meta_data->test_description()) {
    test_meta_data.test_description =
        fbs_test_meta_data->test_description()->str();
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, steamrot::FailInfo>
FlatbuffersTestDataProvider::ConfigureSimulationData(
    steamrot::SimulationData &simulation_data,
    const steamrot::SimulationDataFbs *fbs_simulation_data) const {
  if (fbs_simulation_data == nullptr) {
    return std::unexpected(
        steamrot::FailInfo{steamrot::FailMode::FlatbuffersDataNotFound,
                           "Input Flatbuffers SimulationData is null."});
  }
  // Add configuration for SimulationData fields here as needed
  return std::monostate{};
}
