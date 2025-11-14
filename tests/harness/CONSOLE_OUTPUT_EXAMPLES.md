# Test Harness Console Output Examples

This document shows examples of the improved console output formatting for the test harness.

## Features

- ✓ Success indicators with tick boxes
- ✗ Error indicators with cross marks
- • Info bullets for general information
- ➤ Tick progress arrows
- ━━━━ Section dividers
- [Tick N] Tick number annotations (when relevant)
- Proper spacing with newlines for readability

## Example Output: Tick-Based Test Execution

```
• Running test: sample_tick_based_execution

┌─────────────────────────────────────
│ Running Test: sample_tick_based_execution
└─────────────────────────────────────

• Description: Demonstrates tick-based test execution with coordinated inputs, events, and simulation

━━━━ Tick-Based Execution ━━━━

• Total ticks to execute: 5

➤ Executing Tick 1 of 5

• [Tick 1] Executing tick

✓ [Tick 1] Tick completed

➤ Executing Tick 2 of 5

• [Tick 2] Executing tick

✓ [Tick 2] Tick completed

➤ Executing Tick 3 of 5

• [Tick 3] Executing tick

• [Tick 3] Comparing snapshot

✓ [Tick 3] Tick completed

➤ Executing Tick 4 of 5

• [Tick 4] Executing tick

✓ [Tick 4] Tick completed

➤ Executing Tick 5 of 5

• [Tick 5] Executing tick

✓ [Tick 5] Tick completed

✓ All 5 ticks executed successfully

━━━━ Entity Pool Comparison ━━━━

✓ Entity pool comparison completed
```

## Example Output: Error Case

```
┌─────────────────────────────────────
│ Running Test: failing_test
└─────────────────────────────────────

━━━━ Tick-Based Execution ━━━━

• Total ticks to execute: 3

➤ Executing Tick 1 of 3

• [Tick 1] Executing tick

✓ [Tick 1] Tick completed

➤ Executing Tick 2 of 3

• [Tick 2] Executing tick

✗ [Tick 2] Simulation step failed

✗ [Tick 2] Tick execution failed
```

## Example Output: Snapshot Comparison

```
➤ Executing Tick 3 of 5

• [Tick 3] Executing tick

• [Tick 3] Comparing snapshot

✓ [Tick 3] Tick completed
```

## Example Output: TestFixture Errors

When errors occur during fixture initialization:

```
✗ Error loading game resources data: Failed to load configuration file

✗ Error configuring entities: Invalid entity index

✗ Error generating archetypes: Duplicate archetype ID
```

## Benefits

1. **Visual Clarity**: Tick boxes (✓/✗) provide instant visual feedback
2. **Tick Context**: Tick numbers show exactly when events/errors occur
3. **Structure**: Section dividers organize output into logical sections
4. **Readability**: Proper newlines and indentation improve scanning
5. **Debugging**: Easy to spot failures and their context (tick number, phase)

## Usage in Tests

Tests using `RunFixtureTest()` automatically get formatted output:

```cpp
TEST_CASE("Data-driven test", "[unit]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Automatically generates formatted output
  auto result = steamrot::tests::RunFixtureTest(config);
  REQUIRE(result.has_value());
}
```

## Console Output Functions

Available in `console_output.h`:

- `PrintSuccess(message, tick)` - ✓ with optional tick
- `PrintError(message, tick)` - ✗ with optional tick
- `PrintInfo(message, tick)` - • with optional tick
- `PrintSectionHeader(title)` - ━━━━ section divider
- `PrintTestStart(test_name)` - Box-drawn test banner
- `PrintTickProgress(current, total)` - ➤ tick indicator
- `PrintComparisonResult(success, context, tick)` - Comparison result

All functions include proper newlines for spacing and support optional tick numbers.
