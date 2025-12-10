/////////////////////////////////////////////////
/// @file
/// @brief Implementation of test data comparison utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "test_data_comparison.h"
#include "SceneSnapshot.h"
#include "catch2/matchers/catch_matchers.hpp"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
CompareTickSnapshotEntityPool(const SceneInfo &actual_scene_data,
                              const EngineDataFbs *expected_engine_state,
                              const TestContext &context,
                              bool expected_to_pass) {

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
std::expected<std::monostate, FailInfo> CompareDataBankWithTickSnapshot(
    const std::vector<SceneSnapshot> &actual_scene_snapshots,
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

} // namespace steamrot::tests
