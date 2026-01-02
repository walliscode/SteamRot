/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the FlatbuffersTestDataProvider class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersTestDataProvider.h"
#include "FlatbuffersTestDataLoader.h"
#include "SimulationData.h"
#include "TestData.h"
#include "simulation_data_generated.h"
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

steamrot::FunctionEnum
ConvertFbsToFunctionEnum(steamrot::FunctionEnumFbs fbs_function_enum) {
  switch (fbs_function_enum) {

  case steamrot::FunctionEnumFbs::FunctionEnumFbs_ProcessUIActionsAndEvents:
    return steamrot::FunctionEnum::ProcessUIActionsAndEvents;

  case steamrot::FunctionEnumFbs::
      FunctionEnumFbs_ProcessNestedUIActionsAndEvents:
    return steamrot::FunctionEnum::ProcessNestedUIActionsAndEvents;

  case steamrot::FunctionEnumFbs::FunctionEnumFbs_ProcessButtonElementActions:
    return steamrot::FunctionEnum::ProcessButtonElementActions;

  case steamrot::FunctionEnumFbs::
      FunctionEnumFbs_ProcessDropDownListElementActions:
    return steamrot::FunctionEnum::ProcessDropDownListElementActions;

  case steamrot::FunctionEnumFbs::FunctionEnumFbs_CheckMouseOverNestedUIElement:
    return steamrot::FunctionEnum::CheckMouseOverNestedUIElement;

  case steamrot::FunctionEnumFbs::
      FunctionEnumFbs_UpdateCUserInterfaceVisibilityFromCUIState:

    return steamrot::FunctionEnum::UpdateCUserInterfaceVisibilityFromCUIState;

  default:
    return steamrot::FunctionEnum::None;
  }
}

steamrot::LogicClassEnum
ConvertFbsToLogicClassEnum(steamrot::LogicClassEnumFbs fbs_logic_class_enum) {
  switch (fbs_logic_class_enum) {
  case steamrot::LogicClassEnumFbs::LogicClassEnumFbs_None:
    return steamrot::LogicClassEnum::None;
  case steamrot::LogicClassEnumFbs::LogicClassEnumFbs_UIActionLogic:
    return steamrot::LogicClassEnum::UIActionLogic;
  case steamrot::LogicClassEnumFbs::LogicClassEnumFbs_UICollisionLogic:
    return steamrot::LogicClassEnum::UICollisionLogic;
  case steamrot::LogicClassEnumFbs::LogicClassEnumFbs_UIRenderLogic:
    return steamrot::LogicClassEnum::UIRenderLogic;
  case steamrot::LogicClassEnumFbs::LogicClassEnumFbs_UIStateLogic:
    return steamrot::LogicClassEnum::UIStateLogic;
  case steamrot::LogicClassEnumFbs::LogicClassEnumFbs_CraftingRenderLogic:
    return steamrot::LogicClassEnum::CraftingRenderLogic;
  default:
    return steamrot::LogicClassEnum::None;
  }
}
/////////////////////////////////////////////////
std::expected<std::monostate, steamrot::FailInfo>
FlatbuffersTestDataProvider::ConfigureSimulationData(
    steamrot::SimulationData &simulation_data,
    const steamrot::SimulationDataFbs *fbs_simulation_data) const {
  if (fbs_simulation_data == nullptr)
    return std::unexpected(
        steamrot::FailInfo{steamrot::FailMode::FlatbuffersDataNotFound,
                           "Input Flatbuffers SimulationData is null."});

  // for each SimulationStepFbs, create a SimulationStep and add to
  // simulation_data
  for (const auto *fbs_step : *fbs_simulation_data->steps()) {

    steamrot::SimulationElement element;

    // Determine the element type
    if (fbs_step->simulation_element_as_FunctionEnumWrapper()) {

      steamrot::FunctionEnumFbs fbs_function_enum =
          fbs_step->simulation_element_as_FunctionEnumWrapper()->value();

      // assign the converted FunctionEnum to the element variant
      element = ConvertFbsToFunctionEnum(fbs_function_enum);

    } else if (fbs_step->simulation_element_as_LogicClassEnumWrapper()) {

      // assign the converted LogicClassEnum to the element variant
      element = ConvertFbsToLogicClassEnum(
          fbs_step->simulation_element_as_LogicClassEnumWrapper()->value());

    } else {
      return std::unexpected(steamrot::FailInfo{
          steamrot::FailMode::FlatbuffersDataNotFound,
          "SimulationStepFbs has unknown SimulationElement type."});
    }

    // Create SimulationStep and add to simulation_data
    steamrot::SimulationStep step(element);

    simulation_data.steps.emplace_back(step);
  }

  return std::monostate{};
}
