/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the FlatbuffersTestDataProvider class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersTestDataProvider.h"
#include "EventHandler.h"
#include "FlatbuffersTestDataLoader.h"
#include "SimulationData.h"
#include "TestData.h"
#include "UUIDAssignmentTracker.h"
#include "configure_test_engine_snapshot.h"
#include "simulation_data_generated.h"
#include <expected>
#include <filesystem>

/////////////////////////////////////////////////
FlatbuffersTestDataProvider::FlatbuffersTestDataProvider(
    std::filesystem::path obj_dir_path)
    : ITestDataProvider(obj_dir_path) {}

/////////////////////////////////////////////////
std::expected<std::vector<steamrot::TestData>, steamrot::FailInfo>
FlatbuffersTestDataProvider::ProvideAllTestData() const {
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
    test_data_vec.push_back(std::move(test_data_result.value()));
  }

  return test_data_vec;
}

/////////////////////////////////////////////////
std::expected<steamrot::TestData, steamrot::FailInfo>
FlatbuffersTestDataProvider::CreateTestData(
    const steamrot::TestDataFbs *fbs_test_data) const {

  if (!fbs_test_data) {
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

  // Configure SimulationData
  auto simulation_data_result = ConfigureSimulationData(
      test_data.simulation_data, fbs_test_data->simulation_data());
  if (!simulation_data_result)
    return std::unexpected(simulation_data_result.error());

  // Confugure number_of_ticks, this must be present
  if (!fbs_test_data->num_ticks()) {
    return std::unexpected(steamrot::FailInfo{
        steamrot::FailMode::FlatbuffersDataNotFound,
        "TestDataFbs is missing required field: num_ticks."});
  } else {
    test_data.number_of_ticks = fbs_test_data->num_ticks();
  }

  // Create UUID tracker for consistent UUID assignment across snapshots
  steamrot::tests::UUIDAssignmentTracker uuid_tracker;

  // Create a temporary EventHandler for entity importers
  // Note: This is only used during data loading, not during test execution
  steamrot::EventHandler temp_event_handler;

  // Configure starting_engine_snapshot with UUID assignment
  if (fbs_test_data->starting_engine_snapshot()) {
    auto snapshot_result = steamrot::tests::ConfigureEngineSnapshotWithUUIDs(
        test_data.starting_engine_snapshot,
        fbs_test_data->starting_engine_snapshot(), temp_event_handler,
        uuid_tracker, true); // true = is_starting_snapshot

    if (!snapshot_result.has_value()) {
      return std::unexpected(snapshot_result.error());
    }
  }

  // Configure expected_engine_snapshots with UUID propagation
  if (fbs_test_data->expected_engine_snapshots()) {
    for (const auto *tick_snapshot_pair :
         *fbs_test_data->expected_engine_snapshots()) {
      if (!tick_snapshot_pair) {
        continue; // Skip null entries
      }

      // Validate required fields
      if (!tick_snapshot_pair->snapshot()) {
        return std::unexpected(steamrot::FailInfo{
            steamrot::FailMode::FlatbuffersDataNotFound,
            "TickSnapshotPair is missing required field: snapshot"});
      }

      // Get the tick number
      size_t tick = tick_snapshot_pair->tick();

      // Configure the expected snapshot with UUID tracking
      steamrot::EngineSnapshot expected_snapshot;
      auto expected_result = steamrot::tests::ConfigureEngineSnapshotWithUUIDs(
          expected_snapshot, tick_snapshot_pair->snapshot(), temp_event_handler,
          uuid_tracker, false); // false = not starting snapshot

      if (!expected_result.has_value()) {
        return std::unexpected(expected_result.error());
      }

      // Add to the expected snapshots map
      test_data.expected_engine_snapshots[tick] = std::move(expected_snapshot);
    }
  }

  // return the populated TestData instance
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
  if (!fbs_simulation_data)
    return std::unexpected(
        steamrot::FailInfo{steamrot::FailMode::FlatbuffersDataNotFound,
                           "Input Flatbuffers SimulationData is null."});

  // Add the description if it exists
  if (fbs_simulation_data->description()) {
    simulation_data.description = fbs_simulation_data->description()->str();
  }

  // For each SimulationStepFbs, create a SimulationStep and add to
  // simulation_data
  for (const auto *fbs_step : *fbs_simulation_data->steps()) {

    steamrot::SimulationElement element;

    // Check that logic_class_type is set
    if (fbs_step->logic_class_type() == steamrot::LogicClassEnumFbs_None) {
      return std::unexpected(
          steamrot::FailInfo{steamrot::FailMode::FlatbuffersDataNotFound,
                             "SimulationStepFbs has logic_class_type set to None. "
                             "A valid logic class must be specified."});
    }

    // Assign the converted LogicClassEnum to the element
    element = ConvertFbsToLogicClassEnum(fbs_step->logic_class_type());

    // Create SimulationStep and add to simulation_data
    steamrot::SimulationStep step(element);
    simulation_data.steps.emplace_back(step);
  }

  return std::monostate{};
}
