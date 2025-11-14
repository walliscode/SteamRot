/////////////////////////////////////////////////
/// @file
/// @brief Implementation of tick-based test execution utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "tick_executor.h"
#include "FlatbuffersConfigurator.h"
#include "console_output.h"
#include "entity_memory_pool_matchers.h"
#include "event_bus_conversion.h"
#include "event_matchers.h"
#include "event_simulation.h"
#include "input_simulation.h"
#include "simulation_runner.h"
#include "test_data_harness.h"
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
CompareTickSnapshot(uint32_t tick, const TestDataConfig *config,
                    TestFixture &fixture) {

  // Return early if no snapshots defined
  if (!config->tick_snapshots()) {
    return std::monostate{}; // No snapshots to compare
  }

  // Look for snapshot matching this tick
  for (const TickSnapshot *snapshot : *config->tick_snapshots()) {
    if (snapshot && snapshot->tick() == tick) {
      // Found snapshot for this tick
      // Removed console output - let matchers handle formatting

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

      // Build test context with tick information
      TestContext context;
      if (config->metadata() && config->metadata()->test_name()) {
        context.test_name = config->metadata()->test_name()->str();
      }
      if (snapshot->description()) {
        context.description = snapshot->description()->c_str();
      }
      context.current_tick = tick;
      if (config->num_ticks() > 0) {
        context.total_ticks = config->num_ticks();
      }

      // Get expected_to_pass from test metadata (default true)
      bool expected_to_pass = true;
      if (config->metadata()) {
        expected_to_pass = config->metadata()->expected_to_pass();
      }

      // Use Catch2 matchers - they will format and display only on failure
      if (expected_to_pass) {
        REQUIRE_THAT(actual_pool, EqualsEntityMemoryPool(expected_pool, context));
      } else {
        REQUIRE_THAT(actual_pool, !EqualsEntityMemoryPool(expected_pool, context));
      }

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

        // Use Catch2 matchers - they will format and display only on failure
        if (expected_to_pass) {
          REQUIRE_THAT(actual_event_bus, EqualsEventBus(expected_event_bus, context));
        } else {
          REQUIRE_THAT(actual_event_bus, !EqualsEventBus(expected_event_bus, context));
        }
      }

      // Only one snapshot per tick expected, so break after finding it
      break;
    }
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
uint32_t DetermineNumTicks(const TestDataConfig *config) {
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
ExecuteSingleTick(uint32_t tick, const TestDataConfig *config,
                  TestFixture &fixture) {

  // Handles null config
  if (!config) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "TestDataConfig is null"));
  }

  // Removed console output - let matchers handle formatting

  // 1. Execute inputs scheduled for this tick
  if (config->input_sequence()) {
    auto input_result =
        ExecuteInputEventsForTick(config->input_sequence(), tick, fixture);
    if (!input_result.has_value()) {
      console::PrintError("Input execution failed", tick);
      return std::unexpected(input_result.error());
    }
  }

  // 2. Execute events scheduled for this tick
  if (config->event_sequence()) {
    auto event_result =
        ExecuteEventsForTick(config->event_sequence(), tick, fixture);
    if (!event_result.has_value()) {
      console::PrintError("Event execution failed", tick);
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
            ExecuteSimulationStep(step, fixture.GetSceneContext());
        if (!sim_result.has_value()) {
          console::PrintError("Simulation step failed", tick);
          return std::unexpected(sim_result.error());
        }
      }
    }
  }

  // 5. Check for tick snapshot (compare after simulation, before event bus tick)
  auto snapshot_result = CompareTickSnapshot(tick, config, fixture);
  if (!snapshot_result.has_value()) {
    console::PrintError("Snapshot comparison failed", tick);
    return std::unexpected(snapshot_result.error());
  }

  // 6. Tick the global event bus
  fixture.GetGameResources().event_handler.TickGlobalEventBus();

  // Removed console output - let Catch2 handle success messages

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ExecuteTickBasedTest(const TestDataConfig *config, TestFixture &fixture) {

  // Validate config
  if (!config) {
    return std::unexpected(
        FailInfo(FailMode::NullPointer, "TestDataConfig is null"));
  }

  // Determine number of ticks to execute
  uint32_t num_ticks = DetermineNumTicks(config);

  // Removed console output - let Catch2 control verbosity

  // Execute each tick in sequence (1-based to match game loop)
  for (uint32_t tick = 1; tick <= num_ticks; ++tick) {
    // Removed console output - let matchers handle formatting
    
    auto tick_result = ExecuteSingleTick(tick, config, fixture);
    if (!tick_result.has_value()) {
      console::PrintError("Tick execution failed", tick);
      return std::unexpected(tick_result.error());
    }
  }

  // Removed console output - let Catch2 handle success messages

  return std::monostate{};
}

} // namespace steamrot::tests
