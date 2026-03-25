/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the FlatbuffersTestDataProvider class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersTestDataProvider.h"
#include "EventHandler.h"
#include "test_data_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

TEST_CASE("FlatbuffersTestDataProvider constructor sets object directory path "
          "correctly",
          "[unit][FlatbuffersTestDataProvider]") {

  // Arrange
  std::filesystem::path obj_dir_path =
      std::filesystem::path(__FILE__).parent_path();
  steamrot::EventHandler event_handler;

  // Act
  steamrot::FlatbuffersTestDataProvider provider(obj_dir_path, event_handler);

  // Assert
  REQUIRE(provider.GetObjectDirectoryPath() == obj_dir_path);
}

TEST_CASE("FlatbuffersTestDataProvider::ConfigureTestData returns FailInfo on "
          "null input",
          "[unit][FlatbuffersTestDataProvider]") {

  // Arrange
  std::filesystem::path obj_dir_path =
      std::filesystem::path(__FILE__).parent_path();
  steamrot::EventHandler event_handler;
  steamrot::FlatbuffersTestDataProvider provider(obj_dir_path, event_handler);
  steamrot::TestData test_data;

  // Act
  auto result = provider.ConfigureTestData(test_data, nullptr);

  // Assert
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "Input Flatbuffers TestData is null.");
}

TEST_CASE("FlatbuffersTestDataProvider::ConfigureTestData configures TestData "
          "from valid FlatBuffers data",
          "[unit][FlatbuffersTestDataProvider]") {

  // Arrange
  std::filesystem::path obj_dir_path =
      std::filesystem::path(__FILE__).parent_path();
  steamrot::EventHandler event_handler;
  steamrot::FlatbuffersTestDataProvider provider(obj_dir_path, event_handler);

  // Create flatbuffers builder
  flatbuffers::FlatBufferBuilder builder;

  auto meta_offset = steamrot::CreateTestMetadataFbs(
      builder, builder.CreateString("ConfigureTest"));
  auto step = steamrot::CreateSimulationStepFbs(
      builder, steamrot::LogicClassEnumFbs_UICollisionLogic);
  auto steps_offset = builder.CreateVector(&step, 1);
  auto sim_offset = steamrot::CreateSimulationDataFbs(
      builder, steps_offset, builder.CreateString("Collision simulation"));
  auto test_data_offset =
      steamrot::CreateTestDataFbs(builder, meta_offset, sim_offset, 7);
  builder.Finish(test_data_offset);
  const steamrot::TestDataFbs *fbs =
      steamrot::GetTestDataFbs(builder.GetBufferPointer());

  steamrot::TestData test_data;

  // Act
  auto result = provider.ConfigureTestData(test_data, fbs);

  // Assert
  REQUIRE(result.has_value());
  REQUIRE(test_data.meta_data.test_name == "ConfigureTest");
  REQUIRE(test_data.simulation_data.description == "Collision simulation");
  REQUIRE(test_data.simulation_data.steps.size() == 1);
  REQUIRE(test_data.simulation_data.steps[0].element ==
          steamrot::LogicClassEnum::UICollisionLogic);
  REQUIRE(test_data.number_of_ticks == 7);
}

TEST_CASE(
    "FlatbuffersTestDataProvider::CreateAllTestData returns all TestData "
    "correctly",
    "[unit][FlatbuffersTestDataProvider]") {

  // Arrange
  std::filesystem::path obj_dir_path =
      std::filesystem::path(__FILE__).parent_path();
  steamrot::EventHandler event_handler;
  steamrot::FlatbuffersTestDataProvider provider(obj_dir_path, event_handler);

  // Act
  auto result = provider.CreateAllTestData();

  // Assert
  if (!result)
    FAIL(result.error().message);

  const auto &test_data_vec = result.value();
  REQUIRE(test_data_vec.size() == 1);
  REQUIRE(test_data_vec[0].meta_data.test_name ==
          "FlatbuffersTestDataProvider unit test data");
  REQUIRE(test_data_vec[0].simulation_data.steps.size() == 1);
  REQUIRE(test_data_vec[0].simulation_data.steps[0].element ==
          steamrot::LogicClassEnum::UIActionLogic);
  REQUIRE(test_data_vec[0].number_of_ticks == 3);
}

TEST_CASE("FlatbuffersTestDataProvider::CreateAllTestData returns error when "
          "data directory does not exist",
          "[unit][FlatbuffersTestDataProvider]") {

  // Arrange
  std::filesystem::path obj_dir_path =
      std::filesystem::temp_directory_path() / "non_existent_directory";
  steamrot::EventHandler event_handler;
  steamrot::FlatbuffersTestDataProvider provider(obj_dir_path, event_handler);

  // Act
  auto result = provider.CreateAllTestData();

  // Assert
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::DirectoryNotFound);
}
