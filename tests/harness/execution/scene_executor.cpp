/////////////////////////////////////////////////
/// @file
/// @brief Implementation of Level 2 Scene executor
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "scene_executor.h"
#include "TestFixture.h"
#include "event_processing.h"
#include "logic_execution.h"
#include "logic_executor.h"

namespace steamrot::tests::execution {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSceneTick(const LogicCollection &logic_map) {
  // Use the extracted logic execution function for scene ticks
  // This executes in order: Action -> Movement -> Collision -> Render
  logic::execution::ExecuteSceneTick(logic_map);
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSceneTickWithFixture(TestFixture &fixture,
                            const LogicCollection &logic_map) {
  return ExecuteSceneTick(logic_map);
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteConfiguredSceneTicks(TestFixture &fixture,
                            const SceneTickConfig &config) {

  // Determine number of ticks
  uint32_t num_ticks = config.num_ticks > 0 ? config.num_ticks : 1;

  for (uint32_t tick = 0; tick < num_ticks; ++tick) {

    // If simulation data is provided, execute workflow instead of scene tick
    if (config.simulation && config.simulation->steps()) {
      auto result =
          ExecuteWorkflow(config.simulation, fixture.GetSceneContext());
      if (!result.has_value()) {
        return result;
      }
    } else if (config.logic_map) {
      // Execute standard scene tick if logic_map is provided
      auto result = ExecuteSceneTick(*config.logic_map);
      if (!result.has_value()) {
        return result;
      }
    }
    // If neither simulation nor logic_map provided, tick is a no-op
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteSceneTicksWithEvents(TestFixture &fixture,
                            const LogicCollection &logic_map,
                            uint32_t num_ticks) {

  for (uint32_t tick = 0; tick < num_ticks; ++tick) {
    // 1. Process event tick start (headless mode - no window)
    events::processing::ProcessEventTickStart(
        fixture.GetGameResources().event_handler, nullptr);

    // 2. Execute scene tick
    auto result = ExecuteSceneTick(logic_map);
    if (!result.has_value()) {
      return result;
    }

    // 3. Process event tick end
    events::processing::ProcessEventTickEnd(
        fixture.GetGameResources().event_handler);
  }

  return std::monostate{};
}

} // namespace steamrot::tests::execution
