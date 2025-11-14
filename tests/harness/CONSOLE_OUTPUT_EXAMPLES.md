# Test Harness Console Output Examples

This document shows examples of the improved console output formatting for the test harness.

## Features

- ✓ Success indicators with tick boxes (colored green)
- ✗ Error indicators with cross marks (colored red)
- • Info bullets for general information (colored blue)
- ➤ Tick progress arrows (colored magenta)
- ━━━━ Section dividers (colored yellow)
- [Tick N] Tick number annotations in cyan (when relevant)
- Proper spacing with newlines for readability
- **ANSI color support** with automatic detection

## Color Support

Colors are enabled by default and provide:
- **Green** for success messages (✓)
- **Red** for error messages (✗)
- **Blue** for informational bullets (•)
- **Cyan** for tick numbers ([Tick N])
- **Magenta** for progress indicators (➤)
- **Yellow** for section headers (━━━━)
- **Bold cyan** for test banners

### Disabling Colors

Set the `NO_COLOR` or `STEAMROT_NO_COLOR` environment variable to disable colors:

```bash
export NO_COLOR=1
# or
export STEAMROT_NO_COLOR=1
```

This follows the [NO_COLOR standard](https://no-color.org/) for respecting user preferences and CI environments.

## Example Output: Tick-Based Test Execution (with colors)

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

1. **Visual Clarity**: Colored tick boxes (✓/✗) provide instant visual feedback
2. **Tick Context**: Cyan tick numbers show exactly when events/errors occur
3. **Structure**: Yellow section dividers organize output into logical sections
4. **Readability**: Proper newlines, colors, and indentation improve scanning
5. **Debugging**: Easy to spot failures and their context (tick number, phase)
6. **Accessibility**: Colors can be disabled via environment variable

## Usage in Tests

Tests using `RunFixtureTest()` automatically get formatted, colored output:

```cpp
TEST_CASE("Data-driven test", "[unit]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Automatically generates formatted, colored output
  auto result = steamrot::tests::RunFixtureTest(config);
  REQUIRE(result.has_value());
}
```

## Console Output Functions

Available in `console_output.h`:

**Color Support:**
- `IsColorEnabled()` - Check if colors are enabled (respects NO_COLOR)
- Colors automatically disabled when NO_COLOR or STEAMROT_NO_COLOR is set

- `PrintSuccess(message, tick)` - ✓ with optional tick
- `PrintError(message, tick)` - ✗ with optional tick
- `PrintInfo(message, tick)` - • with optional tick
- `PrintSectionHeader(title)` - ━━━━ section divider
- `PrintTestStart(test_name)` - Box-drawn test banner
- `PrintTickProgress(current, total)` - ➤ tick indicator
- `PrintComparisonResult(success, context, tick)` - Comparison result

All functions include proper newlines for spacing and support optional tick numbers.
