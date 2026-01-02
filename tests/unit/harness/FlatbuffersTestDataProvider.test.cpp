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

  // create a vector of instances of FunctionEnumWrapper
  std::vector<steamrot::FunctionEnumFbs> function_enums = {
      steamrot::FunctionEnumFbs_ProcessUIActionsAndEvents,
      steamrot::FunctionEnumFbs_ProcessNestedUIActionsAndEvents,
      steamrot::FunctionEnumFbs_ProcessButtonElementActions,
      steamrot::FunctionEnumFbs_ProcessDropDownListElementActions,
      steamrot::FunctionEnumFbs_CheckMouseOverNestedUIElement,
      steamrot::FunctionEnumFbs_UpdateCUserInterfaceVisibilityFromCUIState};

  // for each FunctionEnumFbs, create a FunctionEnumWrapper and then a
  // SimulationStepFbs
  std::vector<flatbuffers::Offset<steamrot::SimulationStepFbs>>
      simulation_steps;
  for (const auto &func_enum : function_enums) {
    auto func_enum_wrapper =
        steamrot::CreateFunctionEnumWrapper(builder, func_enum);
    auto sim_step = steamrot::CreateSimulationStepFbs(
        builder, steamrot::SimulationElementFbs_FunctionEnumWrapper,
        func_enum_wrapper.Union());
    simulation_steps.push_back(sim_step);
  }

  // create SimulationDataFbs
  auto steps_vector = builder.CreateVector(simulation_steps);
  auto fbs_simulation_data = steamrot::CreateSimulationDataFbs(
      builder, steps_vector, builder.CreateString("Test Simulation"));

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

  REQUIRE(simulation_data.steps.size() == function_enums.size());
  REQUIRE(std::holds_alternative<steamrot::FunctionEnum>(
      simulation_data.steps[0].element));
  REQUIRE(std::get<steamrot::FunctionEnum>(simulation_data.steps[0].element) ==
          steamrot::FunctionEnum::ProcessUIActionsAndEvents);
  REQUIRE(std::holds_alternative<steamrot::FunctionEnum>(
      simulation_data.steps[1].element));
  REQUIRE(std::get<steamrot::FunctionEnum>(simulation_data.steps[1].element) ==
          steamrot::FunctionEnum::ProcessNestedUIActionsAndEvents);
  REQUIRE(std::holds_alternative<steamrot::FunctionEnum>(
      simulation_data.steps[2].element));
  REQUIRE(std::get<steamrot::FunctionEnum>(simulation_data.steps[2].element) ==
          steamrot::FunctionEnum::ProcessButtonElementActions);
  REQUIRE(std::holds_alternative<steamrot::FunctionEnum>(
      simulation_data.steps[3].element));
  REQUIRE(std::get<steamrot::FunctionEnum>(simulation_data.steps[3].element) ==
          steamrot::FunctionEnum::ProcessDropDownListElementActions);
  REQUIRE(std::holds_alternative<steamrot::FunctionEnum>(
      simulation_data.steps[4].element));
  REQUIRE(std::get<steamrot::FunctionEnum>(simulation_data.steps[4].element) ==
          steamrot::FunctionEnum::CheckMouseOverNestedUIElement);
  REQUIRE(std::holds_alternative<steamrot::FunctionEnum>(
      simulation_data.steps[5].element));
  REQUIRE(std::get<steamrot::FunctionEnum>(simulation_data.steps[5].element) ==
          steamrot::FunctionEnum::UpdateCUserInterfaceVisibilityFromCUIState);
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
