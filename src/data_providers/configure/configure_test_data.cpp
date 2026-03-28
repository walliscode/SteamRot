/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions to configure test data
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_test_data.h"
#include "configure_engine_snapshot.h"
#include "scene_type_conversion.h"

namespace steamrot::data::configure {

namespace {

/////////////////////////////////////////////////
/// @brief Converts a FlatBuffers LogicClassEnumFbs to a LogicClassEnum.
///
/// @param fbs_logic_class_enum The FlatBuffers enum value to convert.
/// @return The corresponding LogicClassEnum value.
/////////////////////////////////////////////////
LogicClassEnum
ConvertFbsToLogicClassEnum(LogicClassEnumFbs fbs_logic_class_enum) {
  switch (fbs_logic_class_enum) {
  case LogicClassEnumFbs::LogicClassEnumFbs_None:
    return LogicClassEnum::None;
  case LogicClassEnumFbs::LogicClassEnumFbs_UIActionLogic:
    return LogicClassEnum::UIActionLogic;
  case LogicClassEnumFbs::LogicClassEnumFbs_UICollisionLogic:
    return LogicClassEnum::UICollisionLogic;
  case LogicClassEnumFbs::LogicClassEnumFbs_UIRenderLogic:
    return LogicClassEnum::UIRenderLogic;
  case LogicClassEnumFbs::LogicClassEnumFbs_UIStateLogic:
    return LogicClassEnum::UIStateLogic;
  case LogicClassEnumFbs::LogicClassEnumFbs_CraftingRenderLogic:
    return LogicClassEnum::CraftingRenderLogic;
  default:
    return LogicClassEnum::None;
  }
}

} // namespace

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureTestMetaData(TestMetaData &test_meta_data,
                      const TestMetadataFbs *fbs_test_meta_data) {

  if (fbs_test_meta_data == nullptr) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "Input Flatbuffers TestMetaData is null."});
  }

  // add required field: test_name
  if (!fbs_test_meta_data->test_name()) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
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
std::expected<std::monostate, FailInfo>
ConfigureSimulationData(SimulationData &simulation_data,
                        const SimulationDataFbs *fbs_simulation_data) {

  if (!fbs_simulation_data)
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "Input Flatbuffers SimulationData is null."});

  // Add the description if it exists
  if (fbs_simulation_data->description()) {
    simulation_data.description = fbs_simulation_data->description()->str();
  }

  // For each SimulationStepFbs, create a SimulationStep and add to
  // simulation_data
  for (const auto *fbs_step : *fbs_simulation_data->steps()) {

    SimulationElement element;

    // Check that logic_class_type is set
    if (fbs_step->logic_class_type() == LogicClassEnumFbs_None) {
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "SimulationStepFbs has logic_class_type set to None. "
                   "A valid logic class must be specified."});
    }

    // Assign the converted LogicClassEnum to the element
    element = ConvertFbsToLogicClassEnum(fbs_step->logic_class_type());

    // Create SimulationStep and add to simulation_data
    SimulationStep step(element);
    simulation_data.steps.emplace_back(step);
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureExpectedEngineSnapshots(
    std::map<size_t, EngineSnapshot> &expected_snapshots,
    const flatbuffers::Vector<flatbuffers::Offset<TickSnapshotPairFbs>>
        *fbs_tick_snapshot_pairs,
    EventHandler &event_handler) {

  if (!fbs_tick_snapshot_pairs) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "Input Flatbuffers tick-snapshot pairs vector is null."});
  }

  // Clear any existing snapshots
  expected_snapshots.clear();

  // Iterate through each tick-snapshot pair
  for (const auto *fbs_pair : *fbs_tick_snapshot_pairs) {
    if (!fbs_pair) {
      continue; // Skip null entries
    }

    // Get the tick number
    size_t tick = fbs_pair->tick();

    // Create and configure the EngineSnapshot
    EngineSnapshot snapshot;
    if (fbs_pair->snapshot()) {
      auto snapshot_result = ConfigureEngineSnapshot(
          snapshot, fbs_pair->snapshot(), event_handler);
      if (!snapshot_result) {
        return std::unexpected(snapshot_result.error());
      }
    }

    // Add to the map
    expected_snapshots[tick] = std::move(snapshot);
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureTestData(TestData &test_data, const TestDataFbs *fbs_test_data,
                  EventHandler &event_handler) {

  if (!fbs_test_data) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "Input Flatbuffers TestData is null."});
  }

  // Configure TestMetaData
  auto meta_data_result =
      ConfigureTestMetaData(test_data.meta_data, fbs_test_data->meta_data());
  if (!meta_data_result)
    return std::unexpected(meta_data_result.error());

  // Configure SimulationData (optional).
  // When absent, test_data.simulation_data remains default-constructed
  // (empty steps vector), which means no simulation logic will be executed.
  if (fbs_test_data->simulation_data()) {
    auto simulation_data_result = ConfigureSimulationData(
        test_data.simulation_data, fbs_test_data->simulation_data());
    if (!simulation_data_result)
      return std::unexpected(simulation_data_result.error());
  }

  // Configure number_of_ticks, this must be present
  if (!fbs_test_data->num_ticks()) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "TestDataFbs is missing required field: num_ticks."});
  } else {
    test_data.number_of_ticks = fbs_test_data->num_ticks();
  }

  // Configure starting_engine_snapshot (optional)
  if (fbs_test_data->starting_engine_snapshot()) {
    auto snapshot_result =
        ConfigureEngineSnapshot(test_data.starting_engine_snapshot,
                                fbs_test_data->starting_engine_snapshot(),
                                event_handler);
    if (!snapshot_result)
      return std::unexpected(snapshot_result.error());
  }

  // Configure expected_engine_snapshots (optional)
  if (fbs_test_data->expected_engine_snapshots()) {
    auto expected_snapshots_result = ConfigureExpectedEngineSnapshots(
        test_data.expected_engine_snapshots,
        fbs_test_data->expected_engine_snapshots(), event_handler);
    if (!expected_snapshots_result)
      return std::unexpected(expected_snapshots_result.error());
  }

  // Configure initial_scene_type (optional).
  // UNKNOWN is the FlatBuffers default and means "use starting_engine_snapshot".
  if (fbs_test_data->initial_scene_type() != SceneTypeFbs_UNKNOWN) {
    auto conversion_result =
        ConvertSceneTypeFbsToSceneType(fbs_test_data->initial_scene_type());
    if (!conversion_result.has_value())
      return std::unexpected(conversion_result.error());
    test_data.initial_scene_type = conversion_result.value();
  }

  return std::monostate{};
}

} // namespace steamrot::data::configure
