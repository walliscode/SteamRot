/////////////////////////////////////////////////
/// @file
/// @brief Implementation of test data harness utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "test_data_harness.h"
#include "EntityMemoryPoolEqualsMatcher.h"
#include "EventBusEqualsMatcher.h"
#include "FlatbuffersConfigurator.h"
#include "catch2/matchers/catch_matchers.hpp"
#include "conmat.h"
#include "console_output.h"
#include "event_bus_conversion.h"
#include "tick_executor.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <sstream>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Helper to load binary data from file
/////////////////////////////////////////////////
static char *load_binary_data(const std::filesystem::path &file_path) {
  std::ifstream infile;
  infile.open(file_path, std::ios::binary | std::ios::in);
  infile.seekg(0, std::ios::end);
  int length = infile.tellg();
  infile.seekg(0, std::ios::beg);
  char *data = new char[length];
  infile.read(data, length);
  infile.close();
  return data;
}

/////////////////////////////////////////////////
/// @brief Helper to get adjacent data directory from source file path
/////////////////////////////////////////////////
static std::expected<std::filesystem::path, FailInfo>
GetAdjacentDataDirectory(const char *source_file_path) {
  std::filesystem::path source_path = source_file_path;
  std::filesystem::path source_dir = source_path.parent_path();
  std::filesystem::path data_dir = source_dir / "data";

  if (!std::filesystem::exists(data_dir)) {
    std::string error_message =
        std::format("Adjacent data directory not found: {}", data_dir.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }

  if (!std::filesystem::is_directory(data_dir)) {
    std::string error_message = std::format(
        "Adjacent data path is not a directory: {}", data_dir.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }

  return data_dir;
}

/////////////////////////////////////////////////
/// @brief Helper to discover and load test data from a directory
/////////////////////////////////////////////////
static std::expected<std::vector<const TestDataConfig *>, FailInfo>
DiscoverAndLoadFromDirectory(const std::filesystem::path &data_dir) {

  std::vector<const TestDataConfig *> configs;

  // Iterate through directory and find .test_data.bin files
  for (const auto &entry : std::filesystem::directory_iterator(data_dir)) {
    if (entry.is_regular_file()) {
      std::string filename = entry.path().filename().string();

      // Check if filename ends with .test_data.bin
      const std::string suffix = ".test_data.bin";
      if (filename.size() >= suffix.size() &&
          filename.compare(filename.size() - suffix.size(), suffix.size(),
                           suffix) == 0) {

        // Load the binary data
        const TestDataConfig *config =
            GetTestDataConfig(load_binary_data(entry.path()));

        // Verify the data was loaded
        if (!config) {
          std::string error_message = std::format(
              "Failed to parse test data from: {}", entry.path().string());
          return std::unexpected(
              FailInfo(FailMode::FlatbuffersDataNotFound, error_message));
        }

        // Verify required metadata is present
        if (!config->metadata()) {
          std::string error_message = std::format(
              "Test data missing required metadata: {}", entry.path().string());
          return std::unexpected(
              FailInfo(FailMode::FlatbuffersDataNotFound, error_message));
        }

        configs.push_back(config);
      }
    }
  }

  return configs;
}

/////////////////////////////////////////////////
std::expected<std::vector<const TestDataConfig *>, FailInfo>
LoadTestDataConfigsImpl(const char *source_file_path) {

  // Get the adjacent data directory using the provided source file path
  auto data_dir_result = GetAdjacentDataDirectory(source_file_path);
  if (!data_dir_result.has_value()) {
    return std::unexpected(data_dir_result.error());
  }

  return DiscoverAndLoadFromDirectory(data_dir_result.value());
}

/////////////////////////////////////////////////
std::expected<TestFixture, FailInfo>
CreateFixtureFromTestData(const TestDataConfig *config,
                          const SceneType &scene_type) {

  // Validate config
  if (!config) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "TestDataConfig is null"));
  }

  // Create and initialize the fixture with the scene type
  TestFixture fixture(scene_type);

  // Initialize with entity collection if present in start_data_collection
  const EntityCollection *start_entities = nullptr;

  if (config->start_data_collection() &&
      config->start_data_collection()->entity_collection()) {
    start_entities = config->start_data_collection()->entity_collection();
  }

  fixture.Intialize(start_entities);

  // Configure EventBus from start_event_bus if present in start_data_collection
  if (config->start_data_collection() &&
      config->start_data_collection()->event_bus()) {
    auto configure_result =
        event::conversion::ConfigureEventHandlerFromEventBusData(
            config->start_data_collection()->event_bus(),
            fixture.GetGameResources().event_handler);

    if (!configure_result.has_value()) {
      return std::unexpected(configure_result.error());
    }
  }

  return fixture;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> RunEntityMemoryPoolComparisonTest(
    const EntityMemoryPool &actual_memory_pool,
    const EntityCollection *expected_collection, TestFixture &fixture,
    const TestContext &context, bool expected_to_pass) {

  // Configure expected EntityMemoryPool from expected_collection
  EntityMemoryPool expected_pool;
  FlatbuffersConfigurator configurator(
      fixture.GetGameResources().event_handler);

  auto configure_result = configurator.ConfigureEntitiesFromCollection(
      expected_pool, expected_collection);

  if (!configure_result.has_value()) {
    return std::unexpected(configure_result.error());
  }

  // run comparison using matcher
  if (expected_to_pass) {
    // Test expects pools to match
    CHECK_THAT(actual_memory_pool,
               EqualsEntityMemoryPool(expected_pool, context));
  } else {
    //
    CHECK_THAT(actual_memory_pool,
               !EqualsEntityMemoryPool(expected_pool, context));
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
void RunEventBusComparisonTest(const EventBus &actual, const EventBus &expected,
                               const TestContext &context,
                               bool expected_to_pass) {

  if (expected_to_pass) {
    // Test expects event buses to match
    CHECK_THAT(actual, EqualsEventBus(expected, context));
  } else {
    // Test expects event buses to NOT match
    CHECK_THAT(actual, !EqualsEventBus(expected, context));
  }
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunDataStructComparisonTest(const DataCollection *data_collection,
                            TestFixture &fixture, const TestContext &context,
                            bool expected_to_pass) {

  // Validate input
  if (!data_collection) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "DataCollection is null"));
  }

  // set up header for error messages

  std::ostringstream oss;

  oss << conmat::Divider("=", 40) << "\n";
  oss << conmat::Colorize("Data Structure Comparison Tests",
                          conmat::Color::Blue)
      << "\n";
  // test/file name if available
  oss << "\t" << context.FormatTestName() << "\n";
  // tick info if available
  oss << "\t" << context.FormatTickInfo() << "\n";

  // finishing divider
  oss << conmat::Divider("=", 40) << "\n";

  INFO(oss.str());

  // Check for entity collection comparison
  if (data_collection->entity_collection()) {
    // Use the convenience overload that handles EMP setup
    auto emp_comparison_result = RunEntityMemoryPoolComparisonTest(
        fixture.GetEntityManager().GetEntityMemoryPool(),
        data_collection->entity_collection(), fixture, context,
        expected_to_pass);

    if (!emp_comparison_result.has_value()) {
      return std::unexpected(emp_comparison_result.error());
    }
  }

  // Check for event bus comparison
  if (data_collection->event_bus()) {
    // create headers for global event bus comparison
    std::ostringstream eb_oss;
    eb_oss << conmat::Divider("=", 40) << "\n";
    eb_oss << conmat::Colorize("Global Event Bus Comparison",
                               conmat::Color::Blue)
           << "\n";
    INFO(eb_oss.str());

    // Convert EventBusData to EventBus
    auto expected_event_bus_result =
        event::conversion::ConvertEventBusDataToEventBus(
            data_collection->event_bus());

    if (!expected_event_bus_result.has_value()) {
      return std::unexpected(expected_event_bus_result.error());
    }

    EventBus expected_event_bus = expected_event_bus_result.value();

    // Get actual event bus from fixture
    const EventBus &actual_event_bus =
        fixture.GetGameResources().event_handler.GetGlobalEventBus();

    // Run comparison
    RunEventBusComparisonTest(actual_event_bus, expected_event_bus, context,
                              expected_to_pass);
  }

  // check for waiting room event bus comparison
  if (data_collection->waiting_room()) {

    // create headers for waiting room comparison
    std::ostringstream wr_oss;
    wr_oss << conmat::Divider("=", 40) << "\n";
    wr_oss << conmat::Colorize("Waiting Room Event Bus Comparison",
                               conmat::Color::Blue)
           << "\n";
    INFO(wr_oss.str());

    // Convert EventBusData to EventBus
    auto expected_event_bus_result =
        event::conversion::ConvertEventBusDataToEventBus(
            data_collection->waiting_room());

    if (!expected_event_bus_result.has_value()) {
      return std::unexpected(expected_event_bus_result.error());
    }
    EventBus expected_event_bus = expected_event_bus_result.value();

    // Get actual waiting room event bus from fixture
    const EventBus &actual_event_bus =
        fixture.GetGameResources().event_handler.GetWaitingRoomEventBus();

    // Run comparison
    RunEventBusComparisonTest(actual_event_bus, expected_event_bus, context,
                              expected_to_pass);
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunFixtureTest(const TestDataConfig *config) {

  // Create fixture from test data
  auto fixture_result = CreateFixtureFromTestData(config);
  if (!fixture_result.has_value()) {
    console::PrintError("Failed to create fixture from test data");
    return std::unexpected(fixture_result.error());
  }

  TestFixture &fixture = fixture_result.value();

  // Execute the test using tick-based execution
  auto tick_result = ExecuteTickBasedTest(config, fixture);
  if (!tick_result.has_value()) {
    return std::unexpected(tick_result.error());
  }

  // If expected_data_collection is provided, compare results
  if (config->expected_data_collection()) {
    // Build test context from config
    TestContext context;
    if (config->metadata()) {
      if (config->metadata()->test_name()) {
        context.test_name = config->metadata()->test_name()->str();
      }
      if (config->metadata()->description()) {
        context.description = config->metadata()->description()->str();
      }
    }

    // Get expected_to_pass from test metadata (default true)
    bool expected_to_pass = true;
    if (config->metadata()) {
      expected_to_pass = config->metadata()->expected_to_pass();
    }

    // Use RunDataStructComparisonTest with expected_to_pass parameter
    auto comparison_result = RunDataStructComparisonTest(
        config->expected_data_collection(), fixture, context, expected_to_pass);

    if (!comparison_result.has_value()) {
      return std::unexpected(comparison_result.error());
    }
  }

  return std::monostate{};
}

} // namespace steamrot::tests
