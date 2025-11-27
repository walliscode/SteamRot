/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Level 3 SceneManager executor
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "scene_manager_executor.h"
#include "TestFixture.h"
#include "event_processing.h"
#include "game_loop.h"

namespace steamrot::tests::execution {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSceneManagerUpdate(SceneManager &scene_manager) {
  // Execute the SceneManager update cycle
  // This processes subscriptions and updates all scenes
  scene_manager.UpdateSceneManager();
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteConfiguredSceneManager(SceneManager &scene_manager,
                              GameResources &game_resources,
                              const SceneManagerConfig &config) {

  // Load the initial scene if specified
  if (config.initial_scene != SceneType_NONE) {
    auto load_result = scene_manager.LoadStandAloneScene(config.initial_scene);
    if (!load_result.has_value()) {
      return std::unexpected(load_result.error());
    }
  }

  // Execute the configured number of updates
  for (uint32_t update = 0; update < config.num_updates; ++update) {

    // TODO: Inject events for this tick if event_sequence is provided
    // This would use the event simulation infrastructure

    // TODO: Inject inputs for this tick if input_sequence is provided
    // This would use the input simulation infrastructure

    // Process event tick start
    events::processing::ProcessEventTickStart(game_resources.event_handler,
                                              nullptr);

    // Execute SceneManager update
    auto result = ExecuteSceneManagerUpdate(scene_manager);
    if (!result.has_value()) {
      return result;
    }

    // Process event tick end
    events::processing::ProcessEventTickEnd(game_resources.event_handler);
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSceneManagerUpdatesWithEvents(SceneManager &scene_manager,
                                     GameResources &game_resources,
                                     uint32_t num_updates) {

  for (uint32_t update = 0; update < num_updates; ++update) {
    // 1. Update game resources (mouse position, etc.)
    game_loop::UpdateGameResources(game_resources);

    // 2. Process event tick start
    events::processing::ProcessEventTickStart(game_resources.event_handler,
                                              nullptr);

    // 3. Execute SceneManager update
    auto result = ExecuteSceneManagerUpdate(scene_manager);
    if (!result.has_value()) {
      return result;
    }

    // 4. Process event tick end
    events::processing::ProcessEventTickEnd(game_resources.event_handler);
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSceneManagerWithFixture(TestFixture &fixture, uint32_t num_updates) {

  // TestFixture provides GameResources but doesn't have a full SceneManager
  // For Level 3 testing with TestFixture, we simulate the update pattern
  // by executing event processing cycles
  for (uint32_t update = 0; update < num_updates; ++update) {
    // 1. Update game resources
    game_loop::UpdateGameResources(fixture.GetGameResources());

    // 2. Process event tick start
    events::processing::ProcessEventTickStart(
        fixture.GetGameResources().event_handler, nullptr);

    // Note: TestFixture doesn't have a SceneManager, so we skip that step
    // Tests using Level 3 with actual SceneManager should use
    // ExecuteSceneManagerUpdatesWithEvents directly

    // 3. Process event tick end
    events::processing::ProcessEventTickEnd(
        fixture.GetGameResources().event_handler);
  }

  return std::monostate{};
}

} // namespace steamrot::tests::execution
