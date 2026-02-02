/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for running the test harness.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "harness_runner.h"
#include "EngineSnapshotEqualsMatcher.h"
#include "EntityTransportVariant.h"
#include "EventHandler.h"
#include "FailInfo.h"
#include "FlatbuffersTestDataProvider.h"
#include "TestEngine.h"
#include "add_uuids.h"
#include "containers.h"
#include "entities_generated.h"
#include "test_context.h"
#include <catch2/catch_test_macros.hpp>
#include <format>
#include <variant>

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunHarnessTests(const std::filesystem::path current_location) {

  // check if the adjacent data directory exists
  std::filesystem::path data_dir = current_location / "data";
  if (!std::filesystem::exists(data_dir) |
      !std::filesystem::is_directory(data_dir)) {
    return std::unexpected(FailInfo{
        FailMode::DirectoryNotFound,
        std::format("No adjacent 'data' directory found at {}",
                    data_dir.string()),
    });
  }

  // Create an EventHandler for loading test data
  EventHandler event_handler;

  // instantiate the FlatbuffersTestDataProvider with the object directory path
  FlatbuffersTestDataProvider data_provider(current_location, event_handler);

  // get all test data
  auto test_data_result = data_provider.ProvideAllTestData();
  if (!test_data_result) {
    return std::unexpected(test_data_result.error());
  }

  // check if any test data was found
  auto &test_data_vec = test_data_result.value();
  if (test_data_vec.empty()) {
    return std::unexpected(FailInfo{
        FailMode::None,
        "No test data found in the 'data' directory.",
    });
  }

  // modify data
  for (auto &test_data : test_data_vec) {

    auto add_uuids_result = add_uuids(test_data);
    if (!add_uuids_result) {
      return std::unexpected(add_uuids_result.error());
    }
  }
  // cycle through all test data and run tests
  for (auto &test_data : test_data_vec) {

    // create TestEngine instance
    TestEngine test_engine{test_data};

    // start up the test engine
    auto start_up_result = test_engine.StartUp();
    if (!start_up_result) {
      return std::unexpected(start_up_result.error());
    }
    // run the game
    auto run_result = test_engine.RunGame();
    if (!run_result) {
      return std::unexpected(run_result.error());
    }

    // convert all entity transport variants in the test data to
    // EntityMemoryPool
    auto convert_result =
        ConvertAllEntityTransportVariantsInTestData(test_data);

    if (!convert_result) {
      return std::unexpected(convert_result.error());
    }

    // compare the snapshots from TestEngine against expected snapshots
    auto comparison_result = RunSnapshotComparisons(test_engine, test_data);
    if (!comparison_result) {
      return std::unexpected(comparison_result.error());
    }
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConvertEntityTransportVariant(SceneData &scene_data) {

  if (!std::holds_alternative<const EntityCollectionFbs *>(
          scene_data.entity_transport)) {
    return std::unexpected(FailInfo{
        FailMode::VariantTypeMismatch,
        "Invalid variant type in SceneData, "
        "EntityCollectionFbs pointer is required",
    });
  }
  // check the SceneData entity configurator is not null
  if (scene_data.entity_configurator == nullptr) {
    return std::unexpected(FailInfo{
        FailMode::NullPointer,
        "SceneData entity_configurator is null.",
    });
  }

  // create fresh EntityMemoryPool
  EntityMemoryPool emp;
  // configure the EMP from the SceneData entity configurator
  auto configure_result =
      scene_data.entity_configurator->ConfigureEntityMemoryPoolFromSource(
          emp, scene_data.entity_transport);

  // assign the configured EMP back to the SceneData
  if (!configure_result) {
    return std::unexpected(configure_result.error());
  }

  scene_data.entity_transport = std::move(emp);

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConvertAllEntityTransportVariants(EngineSnapshot &engine_snapshot) {

  for (auto &scene_data : engine_snapshot.scene_collection_data) {
    auto convert_result = ConvertEntityTransportVariant(scene_data);
    if (!convert_result) {
      return std::unexpected(convert_result.error());
    }
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConvertAllEntityTransportVariantsInTestData(TestData &test_data) {
  // convert starting snapshot
  auto convert_starting_result =
      ConvertAllEntityTransportVariants(test_data.starting_engine_snapshot);
  if (!convert_starting_result) {
    return std::unexpected(convert_starting_result.error());
  }

  // convert all expected snapshots
  for (auto &[tick, engine_snapshot] : test_data.expected_engine_snapshots) {
    auto convert_expected_result =
        ConvertAllEntityTransportVariants(engine_snapshot);
    if (!convert_expected_result) {
      return std::unexpected(convert_expected_result.error());
    }
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
CompareEngineSnapshots(const EngineSnapshot &actual,
                       const EngineSnapshot &expected,
                       const std::string &test_name, uint32_t tick) {

  // Create TestContext for enriched error messages
  TestContext context;
  context.test_name = test_name;
  context.current_tick = tick;
  context.total_ticks = tick; // Set to current tick as total

  REQUIRE_THAT(actual, EqualsEngineSnapshot(expected, context));

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunSnapshotComparisons(const TestEngine &test_engine,
                       const TestData &test_data) {

  const auto &data_bank = test_engine.GetDataBank();
  const auto &expected_snapshots = test_data.expected_engine_snapshots;

  // Extract test name from metadata
  std::string test_name = test_data.meta_data.test_name;

  // Iterate through all expected snapshots
  for (const auto &[expected_tick, expected_snapshot] : expected_snapshots) {

    // Check if the actual snapshot exists for this tick
    auto actual_it = data_bank.find(expected_tick);
    if (actual_it == data_bank.end()) {
      return std::unexpected(FailInfo{
          FailMode::None,
          std::format("Test '{}': Expected snapshot at tick {} not found in "
                      "TestEngine data bank",
                      test_name, expected_tick),
      });
    }

    const EngineSnapshot &actual_snapshot = actual_it->second;

    // Compare the snapshots
    auto comparison_result = CompareEngineSnapshots(
        actual_snapshot, expected_snapshot, test_name, expected_tick);

    if (!comparison_result.has_value()) {
      return std::unexpected(comparison_result.error());
    }
  }

  return std::monostate{};
}
} // namespace steamrot::tests
