/////////////////////////////////////////////////
/// @file
/// @brief Implementation of tick-based test execution utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "tick_executor.h"
#include "FlatbuffersConfigurator.h"
#include "event_bus_conversion.h"
#include "event_simulation.h"
#include "input_simulation.h"
#include "simulation_runner.h"
#include "test_data_harness.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
compare_tick_snapshot(uint32_t tick, const TestDataConfig *config,
                     TestFixture &fixture) {

  // Return early if no snapshots defined
  if (!config->tick_snapshots()) {
    return std::monostate{}; // No snapshots to compare
  }

  // Look for snapshot matching this tick
  for (const TickSnapshot *snapshot : *config->tick_snapshots()) {
    if (snapshot && snapshot->tick() == tick) {
      // Found snapshot for this tick

      // Validate snapshot has entity_collection
      if (!snapshot->entity_collection()) {
        return std::unexpected(FailInfo(
            FailMode::NullPointer,
            std::format("Snapshot at tick {} missing entity_collection", tick)));
      }

      // Create expected pool from snapshot
      EntityMemoryPool expected_pool;

      // Configure expected pool from snapshot's entity_collection
      FlatbuffersConfigurator configurator(
          fixture.GetGameResources().event_handler);
      auto configure_result = configurator.ConfigureEntitiesFromCollection(
          expected_pool, snapshot->entity_collection());

      if (!configure_result.has_value()) {
        return std::unexpected(configure_result.error());
      }

      // Get actual pool from fixture
      const EntityMemoryPool &actual_pool =
          fixture.GetEntityManager().GetEntityMemoryPool();

      // Build snapshot metadata string
      std::string snapshot_info = std::format("Tick {}", tick);
      if (snapshot->description()) {
        snapshot_info += " (" + std::string(snapshot->description()->c_str()) + ")";
      }

      // Get expected_to_pass from test metadata (default true)
      bool expected_to_pass = true;
      if (config->metadata()) {
        expected_to_pass = config->metadata()->expected_to_pass();
      }

      // Compare pools using existing infrastructure
      run_entity_memory_pool_comparison_test(actual_pool, expected_pool,
                                             snapshot_info, expected_to_pass);

      // Compare EventBus if present in snapshot
      if (snapshot->event_bus()) {
        // Convert EventBusData to EventBus
        auto expected_event_bus_result =
            event::conversion::ConvertEventBusDataToEventBus(snapshot->event_bus());

        if (!expected_event_bus_result.has_value()) {
          return std::unexpected(expected_event_bus_result.error());
        }

        EventBus expected_event_bus = expected_event_bus_result.value();

        // Get actual event bus from fixture
        const EventBus &actual_event_bus =
            fixture.GetGameResources().event_handler.GetGlobalEventBus();

        // Compare event buses using existing infrastructure
        run_event_bus_comparison_test(actual_event_bus, expected_event_bus,
                                      snapshot_info, expected_to_pass);
      }

      // Only one snapshot per tick expected, so break after finding it
      break;
    }
  }

  return std::monostate{};
}

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

  // 5. Check for tick snapshot (compare after simulation, before event bus tick)
  auto snapshot_result = compare_tick_snapshot(tick, config, fixture);
  if (!snapshot_result.has_value()) {
    return std::unexpected(snapshot_result.error());
  }

  // 6. Tick the global event bus
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
