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

  // Create a vector of logic class enums to test
  std::vector<steamrot::LogicClassEnumFbs> logic_class_enums = {
      steamrot::LogicClassEnumFbs_UIActionLogic,
      steamrot::LogicClassEnumFbs_UICollisionLogic,
      steamrot::LogicClassEnumFbs_UIRenderLogic,
      steamrot::LogicClassEnumFbs_UIStateLogic,
      steamrot::LogicClassEnumFbs_CraftingRenderLogic};

  // For each LogicClassEnumFbs, create a SimulationStepFbs
  std::vector<flatbuffers::Offset<steamrot::SimulationStepFbs>>
      simulation_steps;
  for (const auto &logic_enum : logic_class_enums) {
    auto sim_step = steamrot::CreateSimulationStepFbs(builder, logic_enum);
    simulation_steps.push_back(sim_step);
  }

  // Create SimulationDataFbs
  auto steps_vector = builder.CreateVector(simulation_steps);
  auto fbs_simulation_data = steamrot::CreateSimulationDataFbs(
      builder, steps_vector, builder.CreateString("Test Simulation"));

  // Finish the buffer and get pointer to SimulationDataFbs
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

  REQUIRE(simulation_data.steps.size() == logic_class_enums.size());
  REQUIRE(simulation_data.steps[0].element ==
          steamrot::LogicClassEnum::UIActionLogic);
  REQUIRE(simulation_data.steps[1].element ==
          steamrot::LogicClassEnum::UICollisionLogic);
  REQUIRE(simulation_data.steps[2].element ==
          steamrot::LogicClassEnum::UIRenderLogic);
  REQUIRE(simulation_data.steps[3].element ==
          steamrot::LogicClassEnum::UIStateLogic);
  REQUIRE(simulation_data.steps[4].element ==
          steamrot::LogicClassEnum::CraftingRenderLogic);
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

  // Create a simple simulation step with logic_class_type
  auto sim_step = steamrot::CreateSimulationStepFbs(
      builder, steamrot::LogicClassEnumFbs_UIActionLogic);

  // Create vector of simulation steps
  std::vector<flatbuffers::Offset<steamrot::SimulationStepFbs>> sim_step_vec;
  auto sim_step_offset = builder.CreateVector(&sim_step, 1);

  // create SimulationDataFbs
  auto simulation_data = steamrot::CreateSimulationDataFbs(
      builder, sim_step_offset, builder.CreateString("Simple Simulation"));
  // combine into TestDataFbs
  auto fbs_test_data =
      steamrot::CreateTestDataFbs(builder, test_meta_data, simulation_data, 8);
  ;

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
  REQUIRE(test_data.simulation_data.steps.size() == 1);
  REQUIRE(test_data.simulation_data.steps[0].element ==
          steamrot::LogicClassEnum::UIActionLogic);
  REQUIRE(test_data.simulation_data.description == "Simple Simulation");
  REQUIRE(test_data.number_of_ticks == 8);
}

TEST_CASE(
    "FlatbuffersTestDataProvider::ProviderAllTestData returns all TestData "
    "correctly",
    "[FlatbuffersTestDataProvider]") {
  // Arrange
  std::filesystem::path obj_dir_path =
      std::filesystem::path(__FILE__).parent_path();
  FlatbuffersTestDataProvider provider(obj_dir_path);
  // Act
  auto result = provider.ProvideAllTestData();
  // Assert
  if (!result)
    FAIL(result.error().message);
  const auto &test_data_vec = result.value();
  REQUIRE(test_data_vec.size() == 1);
  REQUIRE(test_data_vec[0].meta_data.test_name ==
          "Test loading json to TestDataFbs");
  REQUIRE(test_data_vec[0].simulation_data.steps.size() == 1);
  REQUIRE(std::holds_alternative<steamrot::FunctionEnum>(
      test_data_vec[0].simulation_data.steps[0].element));
  REQUIRE(std::get<steamrot::FunctionEnum>(
              test_data_vec[0].simulation_data.steps[0].element) ==
          steamrot::FunctionEnum::ProcessUIActionsAndEvents);
}

TEST_CASE("FlatbuffersTestDataProvider::ConfigureSimulationData fails when "
          "both function and logic class are set",
          "[FlatbuffersTestDataProvider]") {
  // Arrange
  std::filesystem::path obj_dir_path(__FILE__);
  FlatbuffersTestDataProvider provider(obj_dir_path.parent_path());

  flatbuffers::FlatBufferBuilder builder;

  // Create a step with BOTH function_type and logic_class_type set (invalid)
  auto sim_step = steamrot::CreateSimulationStepFbs(
      builder, steamrot::FunctionEnumFbs_ProcessUIActionsAndEvents,
      steamrot::LogicClassEnumFbs_UIActionLogic);

  auto sim_step_offset = builder.CreateVector(&sim_step, 1);
  auto fbs_simulation_data =
      steamrot::CreateSimulationDataFbs(builder, sim_step_offset);

  builder.Finish(fbs_simulation_data);
  const steamrot::SimulationDataFbs *fbs_simulation_data_ptr =
      steamrot::GetSimulationDataFbs(builder.GetBufferPointer());

  steamrot::SimulationData simulation_data;

  // Act
  auto result = provider.ConfigureSimulationData(simulation_data,
                                                 fbs_simulation_data_ptr);

  // Assert
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message.find(
              "both function_type and logic_class_type") != std::string::npos);
}

TEST_CASE("FlatbuffersTestDataProvider::ConfigureSimulationData fails when "
          "neither function nor logic class is set",
          "[FlatbuffersTestDataProvider]") {
  // Arrange
  std::filesystem::path obj_dir_path(__FILE__);
  FlatbuffersTestDataProvider provider(obj_dir_path.parent_path());

  flatbuffers::FlatBufferBuilder builder;

  // Create a step with NEITHER set (both None - invalid)
  auto sim_step = steamrot::CreateSimulationStepFbs(builder);

  auto sim_step_offset = builder.CreateVector(&sim_step, 1);
  auto fbs_simulation_data =
      steamrot::CreateSimulationDataFbs(builder, sim_step_offset);

  builder.Finish(fbs_simulation_data);
  const steamrot::SimulationDataFbs *fbs_simulation_data_ptr =
      steamrot::GetSimulationDataFbs(builder.GetBufferPointer());

  steamrot::SimulationData simulation_data;

  // Act
  auto result = provider.ConfigureSimulationData(simulation_data,
                                                 fbs_simulation_data_ptr);

  // Assert
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message.find(
              "neither function_type nor logic_class_type") !=
          std::string::npos);
}

TEST_CASE("FlatbuffersTestDataProvider::ConfigureSimulationData configures "
          "LogicClass steps correctly",
          "[FlatbuffersTestDataProvider]") {
  // Arrange
  std::filesystem::path obj_dir_path(__FILE__);
  FlatbuffersTestDataProvider provider(obj_dir_path.parent_path());

  flatbuffers::FlatBufferBuilder builder;

  // Create steps with logic_class_type
  std::vector<flatbuffers::Offset<steamrot::SimulationStepFbs>>
      simulation_steps;

  auto step1 = steamrot::CreateSimulationStepFbs(
      builder, steamrot::FunctionEnumFbs_None,
      steamrot::LogicClassEnumFbs_UIActionLogic);
  simulation_steps.push_back(step1);

  auto step2 = steamrot::CreateSimulationStepFbs(
      builder, steamrot::FunctionEnumFbs_None,
      steamrot::LogicClassEnumFbs_UICollisionLogic);
  simulation_steps.push_back(step2);

  auto steps_vector = builder.CreateVector(simulation_steps);
  auto fbs_simulation_data =
      steamrot::CreateSimulationDataFbs(builder, steps_vector);

  builder.Finish(fbs_simulation_data);
  const steamrot::SimulationDataFbs *fbs_simulation_data_ptr =
      steamrot::GetSimulationDataFbs(builder.GetBufferPointer());

  steamrot::SimulationData simulation_data;

  // Act
  auto result = provider.ConfigureSimulationData(simulation_data,
                                                 fbs_simulation_data_ptr);

  // Assert
  REQUIRE(result.has_value());
  REQUIRE(simulation_data.steps.size() == 2);
  REQUIRE(std::holds_alternative<steamrot::LogicClassEnum>(
      simulation_data.steps[0].element));
  REQUIRE(
      std::get<steamrot::LogicClassEnum>(simulation_data.steps[0].element) ==
      steamrot::LogicClassEnum::UIActionLogic);
  REQUIRE(std::holds_alternative<steamrot::LogicClassEnum>(
      simulation_data.steps[1].element));
  REQUIRE(
      std::get<steamrot::LogicClassEnum>(simulation_data.steps[1].element) ==
      steamrot::LogicClassEnum::UICollisionLogic);
}
