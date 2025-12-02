/////////////////////////////////////////////////
/// @file
/// @brief Implementation of test harness utilities
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "test_harness.h"
#include "TestEngine.h"
#include "test_context.h"
#include "test_data_comparison.h"
#include <format>

namespace steamrot::tests {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
RunTestEngineTest(TestDataConfig *config) {

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

} // namespace steamrot::tests
