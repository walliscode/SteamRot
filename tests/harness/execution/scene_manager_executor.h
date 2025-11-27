/////////////////////////////////////////////////
/// @file
/// @brief Level 3 executor for SceneManager update execution
///
/// This executor provides SceneManager-level test execution, allowing
/// full SceneManager update cycles including subscription processing
/// and scene updates.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "GameResources.h"
#include "SceneManager.h"
#include "event_sequence_generated.h"
#include "input_sequence_generated.h"
#include "scene_type_generated.h"
#include <expected>

// Forward declaration to avoid circular dependency
namespace steamrot::tests {
class TestFixture;
}

namespace steamrot::tests::execution {

/////////////////////////////////////////////////
/// @brief Execute a single SceneManager update cycle
///
/// Calls SceneManager::UpdateSceneManager() which processes subscriptions
/// and updates all scenes.
///
/// @param scene_manager Reference to the SceneManager to update
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSceneManagerUpdate(SceneManager &scene_manager);

/////////////////////////////////////////////////
/// @brief Configuration for SceneManager execution
/////////////////////////////////////////////////
struct SceneManagerConfig {
  /// Initial scene type to load (default: TITLE)
  SceneType initial_scene = SceneType_TITLE;

  /// Event sequence to inject (optional)
  const EventSequence *event_sequence = nullptr;

  /// Input sequence to inject (optional)
  const InputSequence *input_sequence = nullptr;

  /// Number of update cycles to execute (default: 1)
  uint32_t num_updates = 1;
};

/////////////////////////////////////////////////
/// @brief Execute configured SceneManager updates
///
/// Handles scene loading, event/input injection, and multi-update execution.
///
/// @param scene_manager Reference to the SceneManager
/// @param game_resources Reference to GameResources for event handling
/// @param config Configuration specifying how to execute
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteConfiguredSceneManager(SceneManager &scene_manager,
                              GameResources &game_resources,
                              const SceneManagerConfig &config);

/////////////////////////////////////////////////
/// @brief Execute SceneManager updates with event processing
///
/// Wraps ExecuteSceneManagerUpdate with event tick start/end processing
/// to match game engine behavior.
///
/// @param scene_manager Reference to the SceneManager
/// @param game_resources Reference to GameResources
/// @param num_updates Number of update cycles to execute
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSceneManagerUpdatesWithEvents(SceneManager &scene_manager,
                                     GameResources &game_resources,
                                     uint32_t num_updates = 1);

/////////////////////////////////////////////////
/// @brief Execute SceneManager update using TestFixture
///
/// Convenience function that extracts SceneManager and GameResources
/// from the fixture. Note: TestFixture doesn't have a full SceneManager,
/// so this function uses the existing TestFixture patterns.
///
/// @param fixture Reference to the TestFixture
/// @param num_updates Number of update cycles to execute
/// @return Success or failure information
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSceneManagerWithFixture(TestFixture &fixture, uint32_t num_updates = 1);

} // namespace steamrot::tests::execution
