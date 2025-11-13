# Tick-by-Tick Entity State Comparison - Design Proposal

## Executive Summary

This document proposes adding the ability to compare entity states on a tick-by-tick basis within the test harness. This feature would allow test data to specify intermediate "snapshots" of expected entity states at specific ticks, enabling more granular verification of multi-step simulations.

## Background

### Current System

The SteamRot test harness currently supports:

1. **Tick-based execution** - Tests can run for multiple ticks with coordinated inputs, events, and simulation steps
2. **Start/End comparison** - Tests can compare entity state at the beginning (`start_entity_collection`) and end (`expected_entity_collection`)
3. **Multi-tick simulations** - Complex sequences of logic classes and functions can execute over many ticks

### The Problem

Currently, there is no way to verify intermediate states during a multi-tick simulation. If a test runs for 10 ticks with complex state transformations, we can only verify:
- The starting state (tick -1, before execution)
- The final state (after all ticks complete)

We **cannot** verify:
- State at tick 2 after a specific transformation
- State at tick 5 before a critical operation
- State at tick 8 to ensure a toggle worked correctly

This makes debugging failures difficult because:
- A mismatch in the final state could be caused by any tick
- We can't isolate which step in a sequence caused the problem
- Complex multi-step scenarios are hard to validate incrementally

### Use Cases

**Use Case 1: Multi-Step UI State Changes**
```
Tick 0: Button starts invisible
Tick 1: User clicks → Button becomes visible
Tick 2: User clicks again → Button becomes invisible
Tick 3: Verify final state
```
Currently, we can only verify tick 3. We'd like to verify ticks 0, 1, and 2 as well.

**Use Case 2: Event-Driven State Machine**
```
Tick 0: Entity in IDLE state
Tick 2: Event arrives → Entity transitions to PROCESSING state
Tick 5: Processing completes → Entity transitions to COMPLETE state
Tick 7: Cleanup happens → Entity back to IDLE state
```
We want to verify the state at ticks 0, 2, 5, and 7 to ensure each transition is correct.

**Use Case 3: Accumulating Values**
```
Tick 0: Counter = 0
Tick 1: Counter = 1 (incremented)
Tick 2: Counter = 3 (incremented by 2)
Tick 3: Counter = 6 (incremented by 3)
```
Verify that the accumulation pattern is correct at each tick.

## Proposed Solution

### High-Level Design

Add **tick snapshots** - optional checkpoints in test data that specify expected entity states at specific ticks. During test execution, after simulation steps complete for a tick, the harness checks if a snapshot exists for that tick and compares the current state.

### Key Principles

1. **Non-Intrusive**: Snapshots don't modify execution, only verify
2. **Optional**: Existing tests work unchanged; snapshots are opt-in
3. **Reuse Existing Infrastructure**: Leverage `EntityMemoryPoolEqualsMatcher` for comparison
4. **Clear Timing**: Snapshots checked AFTER simulation, BEFORE event bus tick
5. **Test Metadata Integration**: Include snapshot info in failure messages

### Schema Design

#### New FlatBuffers Table: `TickSnapshot`

```fbs
namespace steamrot;

////////////////////////////////////////////////////////////
/// @brief Snapshot of expected entity state at a specific tick
///
/// Represents a checkpoint during test execution where the
/// actual entity state should match the expected state defined
/// in the entity_collection field.
////////////////////////////////////////////////////////////
table TickSnapshot {
  /// @brief Tick number when this snapshot should be compared (0-based)
  /// The comparison happens AFTER simulation steps execute for this tick,
  /// but BEFORE the event bus is ticked.
  tick: uint32;
  
  /// @brief Expected entity state at this tick
  entity_collection: EntityCollection (required);
  
  /// @brief Optional human-readable description of this checkpoint
  /// Used in failure messages to identify which snapshot failed.
  description: string;
}
```

#### Updated `TestDataConfig`

```fbs
table TestDataConfig {
  /// ... existing fields ...
  
  /// @brief Intermediate entity state snapshots for tick-by-tick validation
  /// Optional field. If present, the test harness will compare actual entity
  /// state with expected state at each specified tick.
  /// Snapshots are compared AFTER simulation steps but BEFORE event bus tick.
  tick_snapshots: [TickSnapshot];
}
```

### Execution Flow

#### Current Flow (Per Tick)

```
1. Execute inputs for this tick
2. Execute events for this tick
3. Process event waiting room
4. Execute simulation steps
5. Tick the global event bus
```

#### Proposed Flow (Per Tick)

```
1. Execute inputs for this tick
2. Execute events for this tick
3. Process event waiting room
4. Execute simulation steps
5. **NEW: Check for tick snapshot**
   a. Look for snapshot with matching tick number
   b. If found, compare current EntityMemoryPool with snapshot's entity_collection
   c. Use run_entity_memory_pool_comparison_test() for comparison
   d. Include snapshot description in failure message
6. Tick the global event bus
```

### Implementation Details

#### File Changes

**1. Schema (`src/flatbuffers_headers/test_data.fbs`)**
- Add `TickSnapshot` table definition
- Add `tick_snapshots: [TickSnapshot]` to `TestDataConfig`
- Rebuild will auto-generate `test_data_generated.h`

**2. Tick Executor (`tests/harness/tick_executor.cpp`)**

Add new function:
```cpp
std::expected<std::monostate, FailInfo>
compare_tick_snapshot(uint32_t tick, const TestDataConfig *config,
                     const EntityMemoryPool &actual_pool) {
  // Find snapshot for this tick
  if (!config->tick_snapshots()) {
    return std::monostate{}; // No snapshots to compare
  }
  
  for (const TickSnapshot *snapshot : *config->tick_snapshots()) {
    if (snapshot && snapshot->tick() == tick) {
      // Found snapshot for this tick
      
      // Create expected pool from snapshot
      EntityMemoryPool expected_pool;
      // Configure from snapshot->entity_collection()
      // ... (use existing configurator infrastructure)
      
      // Compare pools
      std::string snapshot_info = "Tick " + std::to_string(tick);
      if (snapshot->description()) {
        snapshot_info += " (" + std::string(snapshot->description()->c_str()) + ")";
      }
      
      run_entity_memory_pool_comparison_test(
          actual_pool, expected_pool, snapshot_info,
          config->metadata()->expected_to_pass());
      
      break; // Only one snapshot per tick expected
    }
  }
  
  return std::monostate{};
}
```

Modify `execute_single_tick()`:
```cpp
std::expected<std::monostate, FailInfo>
execute_single_tick(uint32_t tick, const TestDataConfig *config,
                   TestFixture &fixture) {
  // ... existing code for steps 1-4 ...
  
  // 5. NEW: Check for tick snapshot
  auto snapshot_result = compare_tick_snapshot(tick, config, 
                                               fixture.GetEntityManager().GetEntityMemoryPool());
  if (!snapshot_result.has_value()) {
    return std::unexpected(snapshot_result.error());
  }
  
  // 6. Tick the global event bus
  fixture.GetGameResources().event_handler.TickGlobalEventBus();
  
  return std::monostate{};
}
```

**3. Test Harness Header (`tests/harness/tick_executor.h`)**

Add declaration:
```cpp
/////////////////////////////////////////////////
/// @brief Compare actual entity state with tick snapshot if present
///
/// Checks if a snapshot exists for the given tick number. If found,
/// creates an EntityMemoryPool from the snapshot's entity_collection
/// and compares it with the actual pool.
///
/// @param tick Tick number to check for snapshot
/// @param config Test data configuration containing snapshots
/// @param actual_pool Current entity memory pool state
/// @return std::monostate on success or match, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
compare_tick_snapshot(uint32_t tick, const TestDataConfig *config,
                     const EntityMemoryPool &actual_pool);
```

### Example Test Data

#### Example 1: Simple Toggle Verification

```json
{
  "metadata": {
    "test_name": "button_toggle_tick_verification",
    "description": "Verify button visibility toggles correctly each tick",
    "tags": ["integration", "ui", "tick-snapshot"],
    "expected_to_pass": true,
    "version": 1
  },
  "num_ticks": 3,
  "start_entity_collection": {
    "entity_memory_pool_size": 2,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "toggle_button",
          "start_visible": false,
          "root_ui_element": {
            "base_data": {
              "position": {"x": 100, "y": 100},
              "size": {"x": 100, "y": 50},
              "children_active": false,
              "children": [],
              "layout": "Horizontal",
              "spacing_strategy": "None"
            }
          }
        }
      }
    ]
  },
  "tick_snapshots": [
    {
      "tick": 0,
      "description": "After first click - should be visible",
      "entity_collection": {
        "entity_memory_pool_size": 2,
        "entities": [
          {
            "index": 0,
            "c_user_interface": {
              "ui_name": "toggle_button",
              "start_visible": true,
              "root_ui_element": {
                "base_data": {
                  "position": {"x": 100, "y": 100},
                  "size": {"x": 100, "y": 50},
                  "children_active": false,
                  "children": [],
                  "layout": "Horizontal",
                  "spacing_strategy": "None"
                }
              }
            }
          }
        ]
      }
    },
    {
      "tick": 1,
      "description": "After second click - should be invisible",
      "entity_collection": {
        "entity_memory_pool_size": 2,
        "entities": [
          {
            "index": 0,
            "c_user_interface": {
              "ui_name": "toggle_button",
              "start_visible": false,
              "root_ui_element": {
                "base_data": {
                  "position": {"x": 100, "y": 100},
                  "size": {"x": 100, "y": 50},
                  "children_active": false,
                  "children": [],
                  "layout": "Horizontal",
                  "spacing_strategy": "None"
                }
              }
            }
          }
        ]
      }
    },
    {
      "tick": 2,
      "description": "After third click - should be visible again",
      "entity_collection": {
        "entity_memory_pool_size": 2,
        "entities": [
          {
            "index": 0,
            "c_user_interface": {
              "ui_name": "toggle_button",
              "start_visible": true,
              "root_ui_element": {
                "base_data": {
                  "position": {"x": 100, "y": 100},
                  "size": {"x": 100, "y": 50},
                  "children_active": false,
                  "children": [],
                  "layout": "Horizontal",
                  "spacing_strategy": "None"
                }
              }
            }
          }
        ]
      }
    }
  ],
  "input_sequence": {
    "description": "Click button each tick",
    "inputs": [
      {
        "input_type": "MouseClick",
        "input_data_type": "MouseInputData",
        "input_data": {"position": {"x": 150, "y": 125}, "button": 0},
        "tick": 0,
        "description": "First click"
      },
      {
        "input_type": "MouseClick",
        "input_data_type": "MouseInputData",
        "input_data": {"position": {"x": 150, "y": 125}, "button": 0},
        "tick": 1,
        "description": "Second click"
      },
      {
        "input_type": "MouseClick",
        "input_data_type": "MouseInputData",
        "input_data": {"position": {"x": 150, "y": 125}, "button": 0},
        "tick": 2,
        "description": "Third click"
      }
    ]
  },
  "simulation_data": {
    "description": "Process button clicks",
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "LogicClass",
        "logic_class_type": "UICollisionLogic",
        "description": "Detect mouse collision"
      },
      {
        "simulation_type": "Action",
        "execution_mode": "Function",
        "function_type": "ProcessButtonElementActions",
        "description": "Process button action"
      }
    ]
  }
}
```

#### Example 2: Sparse Snapshots

```json
{
  "metadata": {
    "test_name": "sparse_snapshot_verification",
    "description": "Only verify key ticks, not every tick",
    "expected_to_pass": true
  },
  "num_ticks": 10,
  "tick_snapshots": [
    {
      "tick": 0,
      "description": "Initial state",
      "entity_collection": {...}
    },
    {
      "tick": 4,
      "description": "Mid-point state",
      "entity_collection": {...}
    },
    {
      "tick": 9,
      "description": "Final state",
      "entity_collection": {...}
    }
  ],
  "simulation_data": {...}
}
```

### Edge Cases & Error Handling

#### Edge Case 1: No Snapshots
**Scenario**: Test data has no `tick_snapshots` field
**Behavior**: Test runs normally, no snapshot comparisons
**Implementation**: Check `config->tick_snapshots()` for null

#### Edge Case 2: Empty Snapshot Array
**Scenario**: `tick_snapshots: []`
**Behavior**: Test runs normally, no snapshot comparisons
**Implementation**: Check array size

#### Edge Case 3: Snapshot for Non-Existent Tick
**Scenario**: Snapshot has `tick: 10` but `num_ticks: 5`
**Behavior**: Snapshot is never checked (no error)
**Rationale**: Allows reusing test data with different tick counts

#### Edge Case 4: Multiple Snapshots for Same Tick
**Scenario**: Two snapshots both have `tick: 2`
**Behavior**: Only first snapshot is compared
**Rationale**: Simplifies implementation; user error should be avoided

#### Edge Case 5: Snapshot Without entity_collection
**Scenario**: Schema requires it, so this shouldn't happen
**Behavior**: FlatBuffers validation would catch this during build
**Implementation**: Trust schema validation

#### Edge Case 6: Snapshot Comparison Fails
**Scenario**: Actual state doesn't match snapshot
**Behavior**: Test fails with detailed error message including:
- Tick number
- Snapshot description
- Detailed pool differences
**Implementation**: Catch2 assertion from `run_entity_memory_pool_comparison_test()`

### Backward Compatibility

**100% Backward Compatible**:
- `tick_snapshots` is optional field
- Existing tests continue to work unchanged
- No changes to existing test behavior
- Only new feature added

**Migration**:
- No migration needed
- Tests can adopt snapshots incrementally
- No breaking changes to existing APIs

### Performance Considerations

**Overhead per Tick**:
- Array lookup: O(n) where n = number of snapshots
- Entity pool creation: Only if snapshot found for this tick
- Comparison: Same cost as existing end-of-test comparison

**Optimization Opportunities**:
- Pre-index snapshots by tick in a map (if many snapshots)
- Skip comparison if no snapshots defined (current design)
- Lazy load snapshot entity collections

**Expected Impact**:
- Minimal for tests without snapshots (single null check)
- Moderate for tests with snapshots (same as existing comparison)
- No impact on production code

### Testing Strategy

#### Unit Tests

**Test File**: `tests/harness/tick_executor.test.cpp`

Tests to add:
1. `compare_tick_snapshot()` with matching snapshot
2. `compare_tick_snapshot()` with no snapshots
3. `compare_tick_snapshot()` with non-matching tick
4. `execute_single_tick()` with snapshot present
5. `execute_tick_based_test()` with multiple snapshots

#### Integration Tests

**Test File**: `tests/harness/test_data_harness.test.cpp`

Tests using sample test data:
1. Load test data with `tick_snapshots`
2. Execute test with snapshots at multiple ticks
3. Verify snapshot failures produce correct error messages
4. Verify snapshots work with input/event sequences

#### Sample Test Data

**Files to create**:
1. `tests/harness/data/single_tick_snapshot.test_data.json`
2. `tests/harness/data/multiple_tick_snapshots.test_data.json`
3. `tests/harness/data/sparse_tick_snapshots.test_data.json`
4. `tests/harness/data/tick_snapshot_mismatch.test_data.json` (expected_to_pass: false)

### Documentation Updates

#### 1. Test Harness README (`tests/harness/README.md`)

Add new section:
```markdown
## Tick-by-Tick State Verification

### Overview

Tick snapshots allow verification of intermediate entity states during 
multi-tick simulations. Instead of only comparing start and end states,
you can specify expected states at any tick.

### Usage

Add `tick_snapshots` to your test data...
```

#### 2. Workflow Guide (`documentation/workflows/FILLING_TEST_DATA.md`)

Add new workflow:
```markdown
### Workflow 10: Tick-by-Tick State Verification

**When to use:** Testing multi-step transformations where intermediate 
states matter...
```

#### 3. Test Data Configuration (`documentation/testing/TEST_DATA_CONFIGURATION.md`)

Add field reference:
```markdown
### tick_snapshots (Optional)

```json
"tick_snapshots": [
  {
    "tick": 2,
    "description": "After toggle",
    "entity_collection": {...}
  }
]
```
```

### Benefits Summary

1. **Granular Verification**
   - Verify correctness at each step, not just final state
   - Catch errors earlier in simulation sequence
   - Understand exactly when state diverges

2. **Better Debugging**
   - Pinpoint exact tick where failure occurs
   - Reduce time spent debugging multi-step simulations
   - Clear error messages with tick and description

3. **Incremental Testing**
   - Build up test complexity step by step
   - Start with single-tick verification
   - Add more snapshots as needed

4. **Non-Intrusive**
   - Doesn't modify simulation behavior
   - Pure verification mechanism
   - Backward compatible with all existing tests

5. **Consistent API**
   - Reuses existing EntityMemoryPool comparison
   - Same failure messages as end-of-test comparison
   - Familiar pattern for test authors

### Risks & Mitigation

| Risk | Impact | Mitigation |
|------|--------|------------|
| Performance overhead | Low | Only check when snapshot exists; same cost as existing comparison |
| Test data verbosity | Medium | Snapshots are optional; use only when needed |
| Configuration from entity_collection | Medium | Reuse existing configurator infrastructure |
| Schema evolution | Low | Backward compatible; optional field |

### Alternatives Considered

#### Alternative 1: Conditional Pause Points
**Description**: Add `pause_ticks: [2, 5, 8]` and manually compare in test code
**Rejected Because**: Requires code changes per test; not data-driven

#### Alternative 2: Record and Replay
**Description**: Record all states and compare after test completes
**Rejected Because**: More complex; harder to specify expected states in test data

#### Alternative 3: Diff-Based Snapshots
**Description**: Specify only what changed from previous tick
**Rejected Because**: More complex to specify and maintain; harder to read

#### Alternative 4: Snapshot Functions
**Description**: Execute custom validation functions at each tick
**Rejected Because**: Not data-driven; requires code per test

### Future Enhancements

**Out of scope for this proposal, but possible future additions:**

1. **Snapshot Tolerances**: Allow approximate matching for floating-point values
2. **Partial Snapshots**: Only specify subset of entities/components to check
3. **Snapshot Generation**: Tool to generate snapshots from running tests
4. **Interactive Debugging**: Pause execution and inspect state manually
5. **Snapshot Visualization**: GUI to view snapshot comparisons

### Conclusion

Adding tick-by-tick entity state comparison through `tick_snapshots` provides significant value for testing complex multi-step simulations. The implementation is straightforward, leveraging existing infrastructure, and maintains full backward compatibility. The feature enables more granular verification, better debugging, and incremental test development.

**Recommendation**: Proceed with implementation as proposed.

---

## Appendix

### A. Complete Schema Diff

```diff
+ table TickSnapshot {
+   tick: uint32;
+   entity_collection: EntityCollection (required);
+   description: string;
+ }

  table TestDataConfig {
    metadata: TestMetadata (required);
    start_entity_collection: EntityCollection;
    expected_entity_collection: EntityCollection;
    game_resources: GameResourcesData;
    scene_resources: SceneResourcesData;
    simulation_data: SimulationData;
    input_sequence: InputSequence;
    event_sequence: EventSequence;
    num_ticks: uint32;
+   tick_snapshots: [TickSnapshot];
  }
```

### B. Function Signatures

```cpp
// New function
std::expected<std::monostate, FailInfo>
compare_tick_snapshot(uint32_t tick, const TestDataConfig *config,
                     const EntityMemoryPool &actual_pool);

// Modified function (add one line)
std::expected<std::monostate, FailInfo>
execute_single_tick(uint32_t tick, const TestDataConfig *config,
                   TestFixture &fixture);
```

### C. Sample Error Message

```
EntityMemoryPool mismatch [Tick 2 (After toggle)]: 
Component value differs at entity 0: 
  CUserInterface.start_visible: actual=false, expected=true
```

### D. File Checklist

**Schema:**
- [ ] `src/flatbuffers_headers/test_data.fbs` - Add TickSnapshot table

**Implementation:**
- [ ] `tests/harness/tick_executor.h` - Add function declaration
- [ ] `tests/harness/tick_executor.cpp` - Implement compare_tick_snapshot()
- [ ] `tests/harness/tick_executor.cpp` - Modify execute_single_tick()

**Tests:**
- [ ] `tests/harness/tick_executor.test.cpp` - Unit tests
- [ ] `tests/harness/data/single_tick_snapshot.test_data.json` - Sample data
- [ ] `tests/harness/data/multiple_tick_snapshots.test_data.json` - Sample data

**Documentation:**
- [ ] `tests/harness/README.md` - Add section
- [ ] `documentation/workflows/FILLING_TEST_DATA.md` - Add workflow
- [ ] `documentation/testing/TEST_DATA_CONFIGURATION.md` - Add field reference

---

**Document Version**: 1.0  
**Date**: 2025-11-12  
**Status**: Proposal  
**Author**: GitHub Copilot Coding Agent
