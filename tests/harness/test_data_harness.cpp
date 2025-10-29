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
std::expected<std::monostate, FailInfo>
run_test_data_config(const TestDataConfig *config) {

  // Validate config is not null
  if (!config) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "TestDataConfig is null"));
  }

  // Validate required metadata is present
  if (!config->metadata()) {
    return std::unexpected(
        FailInfo(FailMode::FlatbuffersDataNotFound,
                 "TestDataConfig missing required metadata"));
  }

  // Check for entity comparison test data
  // If both start_entity_collection and expected_entity_collection are present,
  // this is an entity comparison test that should be handled by
  // run_entity_memory_pool_comparison_test
  if (config->start_entity_collection() ||
      config->expected_entity_collection()) {
    // Entity comparison tests should be dispatched to
    // run_entity_memory_pool_comparison_test. This wrapper doesn't execute
    // those tests directly but validates that the data is present.

    // Verify expected_entity_collection is present if start is present
    if (config->start_entity_collection() &&
        !config->expected_entity_collection()) {
      return std::unexpected(FailInfo(FailMode::FlatbuffersDataNotFound,
                                      "start_entity_collection provided but "
                                      "expected_entity_collection is missing"));
    }

    // Data is present and valid for entity comparison testing
    return std::monostate{};
  }

  // Future: Add checks for other data types here
  // if (config->event_data()) { ... }
  // if (config->ui_data()) { ... }
  // if (config->logic_data()) { ... }

  // No testable data found - this is not necessarily an error,
  // the config might just contain metadata
  return std::monostate{};
}

/////////////////////////////////////////////////
void run_entity_memory_pool_comparison_test(const EntityMemoryPool &actual,
                                            const EntityMemoryPool &expected) {
  // Compare the pools using matcher
  REQUIRE_THAT(actual, EqualsEntityMemoryPool(expected));
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
  TestFixture fixture(scene_type);
  fixture.Intialize();

  // If start_entity_collection is provided, configure entities from it
  if (config->start_entity_collection()) {
    const EntityCollection *entity_collection = config->start_entity_collection();
    
    // Use FlatbuffersConfigurator to populate entities from test data
    FlatbuffersConfigurator configurator(fixture.GetGameResources().event_handler);
    
    auto configure_result = configurator.ConfigureEntitiesFromCollection(
        fixture.GetEntityManager().GetEntityMemoryPool(),
        entity_collection);
    
    if (!configure_result.has_value()) {
      return std::unexpected(configure_result.error());
    }

    // Regenerate archetypes after configuring entities
    auto archetype_result = fixture.GetEntityManager().GenerateAllArchetypes();
    if (!archetype_result.has_value()) {
      return std::unexpected(archetype_result.error());
    }
  }

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

  // If expected_entity_collection is provided, compare results
  if (config->expected_entity_collection()) {
    const EntityCollection *expected_collection = config->expected_entity_collection();
    
    // Create an expected EntityMemoryPool
    EntityMemoryPool expected_pool;
    
    // Configure expected pool from test data
    FlatbuffersConfigurator configurator(fixture.GetGameResources().event_handler);
    auto configure_result = configurator.ConfigureEntitiesFromCollection(
        expected_pool, expected_collection);
    
    if (!configure_result.has_value()) {
      return std::unexpected(configure_result.error());
    }

    // Compare actual vs expected
    const EntityMemoryPool &actual_pool = 
        fixture.GetEntityManager().GetEntityMemoryPool();
    
    run_entity_memory_pool_comparison_test(actual_pool, expected_pool);
  }

  return std::monostate{};
}

} // namespace steamrot::tests
