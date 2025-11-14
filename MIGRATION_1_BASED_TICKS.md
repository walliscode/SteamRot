# Migration Guide: 0-Based to 1-Based Tick Indexing

**Date**: 2025-11-14  
**Status**: Completed

## Overview

The test harness has been updated to use **1-based tick indexing** instead of 0-based indexing. This change makes the test harness more intuitive and aligns it with the game loop behavior.

## What Changed

### Before (0-Based)
```cpp
for (uint32_t tick = 0; tick < num_ticks; ++tick) {
    // Tick 0 is the first tick
    // Tick 1 is the second tick
    // etc.
}
```

### After (1-Based)
```cpp
for (uint32_t tick = 1; tick <= num_ticks; ++tick) {
    // Setup phase occurs before tick 1 via TestFixture::Initialize()
    // Tick 1 is the first game loop tick
    // Tick 2 is the second game loop tick
    // etc.
}
```

## Key Concepts

### Setup Phase
The **setup phase** is now clearly separate from tick execution:
- Occurs in `TestFixture::Initialize()`
- Configures entities, resources, and initial state
- Is NOT called "tick 0" to avoid confusion
- Happens before tick 1

### Tick Numbering
All tick values in test data are now **1-based**:
- First game loop tick: **1** (was 0)
- Second game loop tick: **2** (was 1)
- Third game loop tick: **3** (was 2)
- And so on...

## Migration Steps for Existing Tests

### 1. Update Test Data JSON Files

**Before:**
```json
{
  "num_ticks": 3,
  "input_sequence": [
    {
      "tick": 0,
      "inputs": [...]
    },
    {
      "tick": 1,
      "inputs": [...]
    }
  ],
  "tick_snapshots": [
    {
      "tick": 0,
      "entity_collection": {...}
    },
    {
      "tick": 2,
      "entity_collection": {...}
    }
  ]
}
```

**After:**
```json
{
  "num_ticks": 3,
  "input_sequence": [
    {
      "tick": 1,
      "inputs": [...]
    },
    {
      "tick": 2,
      "inputs": [...]
    }
  ],
  "tick_snapshots": [
    {
      "tick": 1,
      "entity_collection": {...}
    },
    {
      "tick": 3,
      "entity_collection": {...}
    }
  ]
}
```

**Rule:** Add 1 to all tick values in JSON files.

### 2. Update Test Code

**Before:**
```cpp
// Test creating events at tick 0
auto event = CreateInputEvent(builder, InputType_MouseMove, ..., 0);

// Execute tick 0
execute_single_tick(0, config, fixture);
```

**After:**
```cpp
// Test creating events at tick 1
auto event = CreateInputEvent(builder, InputType_MouseMove, ..., 1);

// Execute tick 1
execute_single_tick(1, config, fixture);
```

**Rule:** Change all hardcoded tick values to start at 1 instead of 0.

### 3. Update Comments

**Before:**
```cpp
// Event at tick 0
// Execute ticks 0, 1, 2
// Tick 0: Initial state
```

**After:**
```cpp
// Event at tick 1
// Execute ticks 1, 2, 3
// Tick 1: Initial state after first tick
```

**Rule:** Update all comments mentioning tick numbers.

## Files Changed

### Core Implementation
- `tests/harness/tick_executor.h` - Updated documentation
- `tests/harness/tick_executor.cpp` - Changed loop from `tick = 0; tick < num_ticks` to `tick = 1; tick <= num_ticks`

### Schema
- `src/flatbuffers_headers/test_data.fbs` - Updated TickSnapshot comments

### Test Files
- `tests/harness/tick_executor.test.cpp`
- `tests/harness/event_simulation.test.cpp`
- `tests/harness/input_simulation.test.cpp`

### Test Data (JSON)
- `tests/harness/data/tick_snapshot_example.test_data.json`
- `tests/harness/data/sample_input_sequence.test_data.json`
- `tests/harness/data/sample_input_event_simulation.test_data.json`
- `tests/harness/data/sample_event_sequence.test_data.json`
- `tests/harness/data/event_bus_snapshot_test.test_data.json`
- `tests/harness/data/sample_tick_based_execution.test_data.json`
- `tests/unit/logic/data/logic_collision_check_mouse_over.test_data.json`
- `tests/unit/logic/data/UICollisionLogic_001.test_data.json`

### Documentation
- `documentation/testing/TESTING_HARNESS_LOOP.md`
- `documentation/workflows/FILLING_TEST_DATA.md`

## Benefits

1. **Clarity**: The setup phase is clearly separate from the first game tick
2. **Intuitive**: Tick 1 is the first game loop tick (matches how people think)
3. **Consistency**: Aligns with the game loop where `loop_number` starts at 1
4. **Avoids Confusion**: No more "tick 0" causing confusion about initialization vs. execution

## Breaking Changes

⚠️ **This is a breaking change** ⚠️

All existing test data files that use tick-based features (input_sequence, event_sequence, tick_snapshots) need to be updated. The migration is straightforward: add 1 to all tick values.

## Verification

To verify your migration:

1. Rebuild the project to regenerate FlatBuffers headers
2. Run all tests: `ctest --preset Debug`
3. Check that tick-related tests pass
4. Verify tick snapshot comparisons work correctly

## Questions?

If you have questions about this migration:
- See the updated documentation in `documentation/testing/TESTING_HARNESS_LOOP.md`
- Review the examples in `documentation/workflows/FILLING_TEST_DATA.md`
- Check the test files for concrete examples

---

**Effective Date**: All new tests should use 1-based tick indexing immediately.  
**Backward Compatibility**: None - all existing tick-based tests must be migrated.
