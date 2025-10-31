/////////////////////////////////////////////////
/// @file
/// @brief Implementation of tick-based test execution utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "tick_executor.h"
#include "event_simulation.h"
#include "input_simulation.h"
#include "simulation_runner.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
uint32_t determine_num_ticks(const TestDataConfig *config) {
  // Only use TestDataConfig level num_ticks field
  // Do NOT auto-detect from input_sequence, event_sequence, or simulation_data
  if (config->num_ticks() > 0) {
    return config->num_ticks();
  }
  // Default to 1 tick if not specified
  return 1;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_single_tick(uint32_t tick, const TestDataConfig *config,
                    TestFixture &fixture) {

  // Handles null config
  if (!config) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "TestDataConfig is null"));
  }

  // 1. Execute inputs scheduled for this tick
  if (config->input_sequence()) {
    auto input_result =
        execute_input_events_for_tick(config->input_sequence(), tick, fixture);
    if (!input_result.has_value()) {
      return std::unexpected(input_result.error());
    }
  }

  // 2. Execute events scheduled for this tick
  if (config->event_sequence()) {
    auto event_result =
        execute_events_for_tick(config->event_sequence(), tick, fixture);
    if (!event_result.has_value()) {
      return std::unexpected(event_result.error());
    }

    // 3. Process event waiting room
    fixture.GetGameResources().event_handler.ProcessWaitingRoomEventBus();
  }

  // 4. Execute simulation steps (all steps execute on every tick)
  if (config->simulation_data() && config->simulation_data()->steps()) {
    for (const SimulationStep *step : *config->simulation_data()->steps()) {
      if (step) {
        auto sim_result =
            execute_simulation_step(step, fixture.GetSceneContext());
        if (!sim_result.has_value()) {
          return std::unexpected(sim_result.error());
        }
      }
    }
  }

  // 5. Tick the global event bus
  fixture.GetGameResources().event_handler.TickGlobalEventBus();

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_tick_based_test(const TestDataConfig *config, TestFixture &fixture) {

  // Validate config
  if (!config) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "TestDataConfig is null"));
  }

  // Determine number of ticks to execute
  uint32_t num_ticks = determine_num_ticks(config);

  // Execute each tick in sequence
  for (uint32_t tick = 0; tick < num_ticks; ++tick) {
    auto tick_result = execute_single_tick(tick, config, fixture);
    if (!tick_result.has_value()) {
      return std::unexpected(tick_result.error());
    }
  }

  return std::monostate{};
}

} // namespace steamrot::tests
