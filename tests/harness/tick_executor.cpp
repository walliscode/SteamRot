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
#include <algorithm>

namespace steamrot::tests {

/////////////////////////////////////////////////
uint32_t determine_num_ticks(const TestDataConfig *config) {
  // If num_ticks is explicitly specified, use it
  if (config->num_ticks() > 0) {
    return config->num_ticks();
  }

  // Otherwise, auto-detect from input_sequence, event_sequence, and
  // simulation_data
  uint32_t max_tick = 0;

  // Check input sequence
  if (config->input_sequence() && config->input_sequence()->inputs()) {
    for (const InputEvent *input_event : *config->input_sequence()->inputs()) {
      if (input_event) {
        max_tick = std::max(max_tick, input_event->tick());
      }
    }
  }

  // Check event sequence
  if (config->event_sequence() && config->event_sequence()->events()) {
    for (const EventTestData *event_data :
         *config->event_sequence()->events()) {
      if (event_data) {
        max_tick = std::max(max_tick, event_data->tick());
      }
    }
  }

  // Return max_tick + 1 (since ticks are 0-based), with minimum of 1
  return std::max(1u, max_tick + 1);
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
execute_single_tick(uint32_t tick, const TestDataConfig *config,
                    TestFixture &fixture) {

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
