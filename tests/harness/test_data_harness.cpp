/////////////////////////////////////////////////
/// @file
/// @brief Implementation of test data harness utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "test_data_harness.h"
#include "EntityMemoryPoolEqualsMatcher.h"
#include "FlatbuffersConfigurator.h"
#include "TestEngine.h"
#include "catch2/matchers/catch_matchers.hpp"
#include "engine_data_generated.h"

#include "test_context.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

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
/// @brief Helper to compare EntityMemoryPool with EngineData from tick snapshot
/////////////////////////////////////////////////
static std::expected<std::monostate, FailInfo> CompareTickSnapshotEntityPool(
    const SceneData &actual_scene_data, const EngineData *expected_engine_state,
    const TestContext &context, bool expected_to_pass) {

  // If no engine state in snapshot, skip comparison
  if (!expected_engine_state) {
    return std::monostate{};
  }

  // Get the scene manager data from engine state
  if (!expected_engine_state->scene_manager_data()) {
    return std::monostate{};
  }

  // Get scene data from scene manager
  const auto *scene_data_list =
      expected_engine_state->scene_manager_data()->scene_data();
  if (!scene_data_list || scene_data_list->size() == 0) {
    return std::monostate{};
  }

  // Find matching scene by type
  for (const auto *expected_scene_data : *scene_data_list) {
    if (!expected_scene_data || !expected_scene_data->entity_collection()) {
      continue;
    }

    // Check if scene types match
    if (expected_scene_data->scene_type() != actual_scene_data.type) {
      continue;
    }

    // Configure expected EntityMemoryPool from EntityCollection
    EntityMemoryPool expected_pool;
    EventHandler temp_handler;
    FlatbuffersConfigurator configurator(temp_handler);

    auto configure_result = configurator.ConfigureEntitiesFromCollection(
        expected_pool, expected_scene_data->entity_collection());

    if (!configure_result.has_value()) {
      return std::unexpected(configure_result.error());
    }

    // Run comparison using matcher
    if (expected_to_pass) {
      CHECK_THAT(actual_scene_data.entity_memory_pool,
                 EqualsEntityMemoryPool(expected_pool, context));
    } else {
      CHECK_THAT(actual_scene_data.entity_memory_pool,
                 !EqualsEntityMemoryPool(expected_pool, context));
    }
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
/// @brief Compare data bank entry with tick snapshot
/////////////////////////////////////////////////
static std::expected<std::monostate, FailInfo> CompareDataBankWithTickSnapshot(
    const std::vector<SceneData> &actual_scene_snapshots,
    const TickSnapshot *tick_snapshot, const TestContext &context,
    bool expected_to_pass) {

  if (!tick_snapshot) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "TickSnapshot is null"));
  }

  // Compare each scene in the data bank with expected engine state
  for (const auto &actual_scene : actual_scene_snapshots) {
    auto result = CompareTickSnapshotEntityPool(
        actual_scene, tick_snapshot->engine_state(), context, expected_to_pass);

    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunTestEngineTest(const TestDataConfig *config) {

  // Validate config
  if (!config) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "TestDataConfig is null"));
  }

  // Create TestEngine - it simulates the Engine based on the config
  TestEngine engine(config);

  // Run the engine simulation using base Engine::RunGame()
  engine.RunGame();

  // Get the data bank output from the engine simulation
  const auto &data_bank = engine.GetDataBank();

  // Determine number of ticks from config for context building
  size_t num_ticks = 1;
  if (config->num_ticks() > 0) {
    num_ticks = config->num_ticks();
  }

  // Build base test context from config
  TestContext base_context;
  bool expected_to_pass = true;

  if (config->metadata()) {
    if (config->metadata()->test_name()) {
      base_context.test_name = config->metadata()->test_name()->str();
    }
    if (config->metadata()->description()) {
      base_context.description = config->metadata()->description()->str();
    }
    expected_to_pass = config->metadata()->expected_to_pass();
  }

  // Compare data bank with tick_snapshots (purely tick-based comparison)
  if (config->tick_snapshots() && config->tick_snapshots()->size() > 0) {
    for (const auto *tick_snapshot : *config->tick_snapshots()) {
      if (!tick_snapshot) {
        continue;
      }

      size_t tick_num = tick_snapshot->tick();

      // Find the corresponding data bank entry
      auto it = data_bank.find(tick_num);
      if (it == data_bank.end()) {
        std::string error_message =
            std::format("No data bank entry found for tick {}", tick_num);
        return std::unexpected(
            FailInfo(FailMode::FlatbuffersDataNotFound, error_message));
      }

      // Build context for this tick
      TestContext tick_context = base_context;
      tick_context.current_tick = tick_num;
      tick_context.total_ticks = num_ticks;
      if (tick_snapshot->description()) {
        tick_context.description = tick_snapshot->description()->str();
      }

      // Compare the data bank entry with the snapshot
      auto result = CompareDataBankWithTickSnapshot(
          it->second, tick_snapshot, tick_context, expected_to_pass);

      if (!result.has_value()) {
        return std::unexpected(result.error());
      }
    }
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
/// DEPRECATED FUNCTIONS
/// These functions depend on TestFixture which no longer exists.
/// Use RunTestEngineTest instead for tick-based testing.
/////////////////////////////////////////////////

std::expected<std::monostate, FailInfo>
RunFixtureTest(const TestDataConfig * /*config*/) {
  return std::unexpected(
      FailInfo(FailMode::NotImplemented,
               "RunFixtureTest is deprecated. Use RunTestEngineTest instead."));
}

} // namespace steamrot::tests
