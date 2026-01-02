/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the FlatbuffersTestDataProvider class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersTestDataProvider.h"
#include "test_data_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

TEST_CASE(
    "FlatbuffersTestsData Provider constructor sets object directory path "
    "correctly",
    "[FlatbuffersTestDataProvider]") {
  // Arrange
  std::filesystem::path obj_dir_path(__FILE__);
  // Act
  FlatbuffersTestDataProvider provider(obj_dir_path);
  // Assert
  REQUIRE(provider.GetObjectDirectoryPath() == obj_dir_path);
}

TEST_CASE("FlatbuffersTestDataProvider::CreateTestData returns FailInfo on "
          "null input",
          "[FlatbuffersTestDataProvider]") {
  // Arrange
  std::filesystem::path obj_dir_path(__FILE__);
  FlatbuffersTestDataProvider provider(obj_dir_path.parent_path());

  // Act
  auto result = provider.CreateTestData(nullptr);
  // Assert
  REQUIRE(!result);
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "Input Flatbuffers TestData is null.");
}
TEST_CASE(
    "FlatbuffersTestDataProvider::ConfigureTestMetaData returns FailInfo on "
    "null input",
    "[FlatbuffersTestDataProvider]") {
  // Arrange
  std::filesystem::path obj_dir_path(__FILE__);
  FlatbuffersTestDataProvider provider(obj_dir_path.parent_path());
  steamrot::TestMetaData test_meta_data;
  // Act
  auto result = provider.ConfigureTestMetaData(test_meta_data, nullptr);
  // Assert
  REQUIRE(!result);
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "Input Flatbuffers TestMetaData is null.");
}

TEST_CASE(
    "FlatbuffersTestDataProvider::ConfigureSimulationData returns FailInfo on "
    "null input",
    "[FlatbuffersTestDataProvider]") {
  // Arrange
  std::filesystem::path obj_dir_path(__FILE__);
  FlatbuffersTestDataProvider provider(obj_dir_path.parent_path());
  steamrot::SimulationData simulation_data;
  // Act
  auto result = provider.ConfigureSimulationData(simulation_data, nullptr);
  // Assert
  REQUIRE(!result);
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message ==
          "Input Flatbuffers SimulationData is null.");
}

TEST_CASE("FlatbuffersTestDataProvider::ConfigureSimulationData configures "
          "SimulationData correctly",
          "[FlatbuffersTestDataProvider]") {

  // Arrange
  std::filesystem::path obj_dir_path(__FILE__);
  FlatbuffersTestDataProvider provider(obj_dir_path.parent_path());

  // Create flatbuffers builder
  flatbuffers::FlatBufferBuilder builder;

  // create simulation data in flatbuffers

  auto fbs_simulation_data = steamrot::CreateSimulationDataFbs(builder);

  // finish the buffer and get pointer to SimulationDataFbs
  builder.Finish(fbs_simulation_data);
  const steamrot::SimulationDataFbs *fbs_simulation_data_ptr =
      steamrot::GetSimulationDataFbs(builder.GetBufferPointer());
  steamrot::SimulationData simulation_data;
  // Act
  auto result = provider.ConfigureSimulationData(simulation_data,
                                                 fbs_simulation_data_ptr);
  // Assert
  if (!result)
    FAIL(result.error().message);
}
TEST_CASE(
    "FlatbuffersTestDataProvider::CreateTestData returns TestData on valid "
    "input",
    "[FlatbuffersTestDataProvider]") {

  // Arrange
  std::filesystem::path obj_dir_path(__FILE__);
  FlatbuffersTestDataProvider provider(obj_dir_path.parent_path());

  // Create flatbuffers builder
  flatbuffers::FlatBufferBuilder builder;

  // create test meta data
  auto test_meta_data =
      steamrot::CreateTestMetadataFbs(builder, builder.CreateString("Test1"));

  // combine into TestDataFbs
  auto fbs_test_data = steamrot::CreateTestDataFbs(builder, test_meta_data);

  // finish the buffer and get pointer to TestDataFbs
  builder.Finish(fbs_test_data);
  const steamrot::TestDataFbs *fbs_test_data_ptr =
      steamrot::GetTestDataFbs(builder.GetBufferPointer());

  // Act
  auto result = provider.CreateTestData(fbs_test_data_ptr);

  // Assert
  if (!result)
    FAIL(result.error().message);

  const steamrot::TestData &test_data = result.value();
  REQUIRE(test_data.meta_data.test_name == "Test1");
}
