/////////////////////////////////////////////////
/// @file
/// @brief Implementation of test data harness utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "test_data_harness.h"
#include "FlatbuffersConfigurator.h"
#include "PathProvider.h"
#include "entity_memory_pool_matchers.h"
#include "tick_executor.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <format>
#include <fstream>

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
/// @brief Helper to get adjacent data directory using __FILE__
/////////////////////////////////////////////////
static std::expected<std::filesystem::path, FailInfo>
get_adjacent_data_directory() {
  std::filesystem::path source_file_path = __FILE__;
  std::filesystem::path source_dir = source_file_path.parent_path();
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
discover_and_load_from_directory(const std::filesystem::path &data_dir) {

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
load_test_data_configs() {

  // Get the adjacent data directory
  auto data_dir_result = get_adjacent_data_directory();
  if (!data_dir_result.has_value()) {
    return std::unexpected(data_dir_result.error());
  }

  return discover_and_load_from_directory(data_dir_result.value());
}

/////////////////////////////////////////////////
std::expected<std::vector<const TestDataConfig *>, FailInfo>
load_test_data_configs(const std::string &subdirectory) {

  // Use PathProvider to get the base tests directory
  PathProvider path_provider(EnvironmentType::Test);
  auto data_dir_result = path_provider.GetDataDirectory();
  if (!data_dir_result.has_value()) {
    return std::unexpected(data_dir_result.error());
  }

  // Base path is tests/
  std::filesystem::path tests_path = data_dir_result.value().parent_path();

  // Construct path to subdirectory data
  std::filesystem::path target_dir;
  if (!subdirectory.empty()) {
    target_dir = tests_path / subdirectory / "data";
  } else {
    target_dir = data_dir_result.value();
  }

  // Check if directory exists
  if (!std::filesystem::exists(target_dir)) {
    std::string error_message =
        std::format("Test data directory not found: {}", target_dir.string());
    return std::unexpected(FailInfo(FailMode::FileNotFound, error_message));
  }

  return discover_and_load_from_directory(target_dir);
}

/////////////////////////////////////////////////
void run_entity_memory_pool_comparison_test(const EntityMemoryPool &actual,
                                            const EntityMemoryPool &expected,
                                            bool expected_to_pass) {
  // Create matcher
  auto matcher = EqualsEntityMemoryPool(expected);
  
  if (expected_to_pass) {
    // Test expects pools to match - use REQUIRE_THAT
    REQUIRE_THAT(actual, matcher);
  } else {
    // Test expects pools to NOT match - verify mismatch
    bool pools_match = matcher.match(actual);
    if (pools_match) {
      FAIL("Expected pools to be different, but they matched");
    }
  }
}

/////////////////////////////////////////////////
void run_entity_memory_pool_comparison_test(const EntityMemoryPool &actual,
                                            const EntityMemoryPool &expected,
                                            const std::string &test_metadata,
                                            bool expected_to_pass) {
  // Create matcher with metadata
  auto matcher = EqualsEntityMemoryPool(expected, test_metadata);
  
  if (expected_to_pass) {
    // Test expects pools to match - use REQUIRE_THAT
    REQUIRE_THAT(actual, matcher);
  } else {
    // Test expects pools to NOT match - verify mismatch
    bool pools_match = matcher.match(actual);
    if (pools_match) {
      std::string error_msg = "Expected pools to be different, but they matched";
      if (!test_metadata.empty()) {
        error_msg += " [" + test_metadata + "]";
      }
      FAIL(error_msg);
    }
  }
}

/////////////////////////////////////////////////
std::expected<TestFixture, FailInfo>
create_fixture_from_test_data(const TestDataConfig *config,
                              const SceneType &scene_type) {

  // Validate config
  if (!config) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "TestDataConfig is null"));
  }

  // Create and initialize the fixture with the scene type
  // Pass entity_collection to Initialize so it configures entities from test
  // data instead of loading default scene data
  TestFixture fixture(scene_type);
  fixture.Intialize(config->start_entity_collection());

  return fixture;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
run_fixture_test(const TestDataConfig *config) {

  // Create fixture from test data
  auto fixture_result = create_fixture_from_test_data(config);
  if (!fixture_result.has_value()) {
    return std::unexpected(fixture_result.error());
  }

  TestFixture &fixture = fixture_result.value();

  // Execute the test using tick-based execution
  // This will process inputs, events, and simulation steps on a tick-by-tick
  // basis
  auto tick_result = execute_tick_based_test(config, fixture);
  if (!tick_result.has_value()) {
    return std::unexpected(tick_result.error());
  }

  // If expected_entity_collection is provided, compare results
  if (config->expected_entity_collection()) {
    const EntityCollection *expected_collection =
        config->expected_entity_collection();

    // Create an expected EntityMemoryPool
    EntityMemoryPool expected_pool;

    // Configure expected pool from test data
    FlatbuffersConfigurator configurator(
        fixture.GetGameResources().event_handler);
    auto configure_result = configurator.ConfigureEntitiesFromCollection(
        expected_pool, expected_collection);

    if (!configure_result.has_value()) {
      return std::unexpected(configure_result.error());
    }

    // Compare actual vs expected
    const EntityMemoryPool &actual_pool =
        fixture.GetEntityManager().GetEntityMemoryPool();

    // Build test metadata string from config
    std::string test_metadata;
    bool expected_to_pass = true; // default value
    
    if (config->metadata()) {
      if (config->metadata()->test_name()) {
        test_metadata +=
            "Test: " + std::string(config->metadata()->test_name()->str());
      }
      if (config->metadata()->description()) {
        test_metadata += ", Description: " +
                         std::string(config->metadata()->description()->str());
      }
      // Get expected_to_pass from metadata
      expected_to_pass = config->metadata()->expected_to_pass();
    }

    // Use overload with metadata if available, otherwise use simple version
    if (!test_metadata.empty()) {
      run_entity_memory_pool_comparison_test(actual_pool, expected_pool,
                                             test_metadata, expected_to_pass);
    } else {
      run_entity_memory_pool_comparison_test(actual_pool, expected_pool,
                                             expected_to_pass);
    }
  }

  return std::monostate{};
}

} // namespace steamrot::tests
