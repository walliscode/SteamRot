# Testing Harness Loop

[← Back to Documentation](../README.md) | [Testing Overview](TESTING_OVERVIEW.md)

This document provides a comprehensive visual representation of the testing harness loop structure, including tick-based execution and state comparison.

**Related Documentation:**
- [Game Loop Architecture](../architecture/GAME_LOOP.md) - Production game loop
- [Test Data Configuration](TEST_DATA_CONFIGURATION.md) - Test data structure
- [Workflows: Filling Test Data](../workflows/FILLING_TEST_DATA.md) - Creating test data files

---

## Table of Contents
- [Overview](#overview)
- [Visual Diagram](#visual-diagram)
- [Test Fixture Setup](#test-fixture-setup)
- [Tick-Based Execution](#tick-based-execution)
- [Single Tick Execution](#single-tick-execution)
- [Simulation Runner](#simulation-runner)
- [Tick Management](#tick-management)
- [Comparison vs. Game Loop](#comparison-vs-game-loop)
- [Component Reference](#component-reference)

---

## Overview

The SteamRot testing harness provides a tick-based execution model for data-driven testing. Tests can specify inputs, events, simulation steps, and expected states at specific ticks, enabling precise control and validation of game logic.

**Key Files:**
- `tests/harness/tick_executor.h/cpp` - Tick-based test execution
- `tests/harness/simulation_runner.h/cpp` - Logic/function simulation
- `tests/harness/input_simulation.h/cpp` - Input event simulation
- `tests/harness/event_simulation.h/cpp` - Event simulation
- `tests/harness/TestFixture.h/cpp` - Test environment setup

---

## Visual Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                      TEST INITIALIZATION                        │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│  TestFixture created with:                                      │
│  • SceneType (TITLE, CRAFTING, etc.)                           │
│  • EntityCollection (start_entity_collection)                  │
│  • GameResources (window, event handler, assets)               │
│  • SceneContext (entities, archetypes, resources)              │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│  Load TestDataConfig from .test_data.bin file                  │
│  • metadata (test name, description, expected_to_pass)         │
│  • start_entity_collection                                     │
│  • expected_entity_collection                                  │
│  • input_sequence (inputs by tick)                             │
│  • event_sequence (events by tick)                             │
│  • simulation_data (logic steps)                               │
│  • tick_snapshots (expected states at specific ticks)          │
│  • num_ticks (number of ticks to execute)                      │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│  determine_num_ticks(config)                                    │
│  Returns: config->num_ticks() if > 0, else 1 (default)         │
│  🔢 TICK COUNT DETERMINED                                      │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
╔═════════════════════════════════════════════════════════════════╗
║              TICK-BASED EXECUTION LOOP                          ║
║  for (tick = 1; tick <= num_ticks; ++tick)                     ║
║  Note: 1-based indexing to mimic game loop                     ║
╚═════════════════════════════════════════════════════════════════╝
         │
         │   (Details in Single Tick Execution section)
         ▼
┌─────────────────────────────────────────────────────────────────┐
│  execute_single_tick(tick, config, fixture)                    │
│  🔢 CURRENT TICK: tick (1-indexed: 1, 2, 3, ...)              │
│  Setup phase occurs before tick 1 via TestFixture::Initialize │
└─────────────────────────────────────────────────────────────────┘
         │
         │  (Loop continues for num_ticks iterations)
         ▼
┌─────────────────────────────────────────────────────────────────┐
│                   FINAL COMPARISON                              │
│  Compare final state with expected_entity_collection           │
│  (if not already validated by tick snapshots)                  │
└─────────────────────────────────────────────────────────────────┘
```

---

## Test Fixture Setup

The TestFixture provides a complete test environment:

```cpp
// Create fixture with scene type and initial entities
TestFixture fixture(SceneType::SceneType_TITLE, start_entity_collection);

// Access test components
GameResources& game_resources = fixture.GetGameResources();
SceneContext& scene_context = fixture.GetSceneContext();
EntityManager& entity_manager = fixture.GetEntityManager();
```

**TestFixture Provides:**
- Mock game window (not displayed)
- EventHandler with event buses
- AssetManager with test assets
- EntityMemoryPool configured from test data
- ArchetypeManager tracking component combinations
- SceneResources (render texture, etc.)

---

## Tick-Based Execution

The main test execution flow:

```cpp
std::expected<std::monostate, FailInfo>
execute_tick_based_test(const TestDataConfig *config, TestFixture &fixture) {

  // 1. Determine number of ticks
  uint32_t num_ticks = determine_num_ticks(config);
  // Returns config->num_ticks() if > 0, else 1

  // 2. Execute each tick in sequence (1-based to mimic game loop)
  // Setup phase occurs before tick 1 via TestFixture::Initialize()
  for (uint32_t tick = 1; tick <= num_ticks; ++tick) {
    auto tick_result = execute_single_tick(tick, config, fixture);
    if (!tick_result.has_value()) {
      return std::unexpected(tick_result.error());
    }
  }

  return std::monostate{};
}
```

---

## Single Tick Execution

Each tick follows this precise execution order:

```
┌─────────────────────────────────────────────────────────────────┐
│  execute_single_tick(tick, config, fixture)                    │
│  🔢 TICK NUMBER: tick (1-indexed, e.g., 1, 2, 3, ...)         │
│  Setup phase occurs before tick 1 via TestFixture::Initialize │
└─────────────────────────────────────────────────────────────────┘
         │
         ├─→ Step 1: Execute Inputs for This Tick
         │   │
         │   ├─ execute_input_events_for_tick(input_sequence, tick, fixture)
         │   └─ Process inputs scheduled at this tick number
         │      (keyboard presses, mouse clicks, mouse moves)
         │
         ├─→ Step 2: Execute Events for This Tick
         │   │
         │   ├─ execute_events_for_tick(event_sequence, tick, fixture)
         │   ├─ Add events scheduled at this tick to waiting room
         │   └─ EventHandler::ProcessWaitingRoomEventBus()
         │      └─ Move events from waiting room → global event bus
         │
         ├─→ Step 3: Execute Simulation Steps
         │   │
         │   ├─ For each step in simulation_data->steps():
         │   │   execute_simulation_step(step, scene_context)
         │   │
         │   └─ Simulation steps execute on EVERY tick
         │      (Not tick-specific like inputs/events)
         │
         ├─→ Step 4: Compare Tick Snapshot (if present)
         │   │
         │   ├─ compare_tick_snapshot(tick, config, fixture)
         │   ├─ Look for snapshot with matching tick number
         │   ├─ If found:
         │   │   ├─ Create expected EntityMemoryPool from snapshot
         │   │   ├─ Compare actual vs expected pools
         │   │   └─ Compare actual vs expected event buses (if present)
         │   │
         │   └─ Snapshots enable state validation at specific points
         │
         └─→ Step 5: Tick Global Event Bus ⏱️
             │
             └─ EventHandler::TickGlobalEventBus()
                └─ Decrement event lifetimes and remove expired events
```

**Tick Execution Order Summary:**

1. **Inputs** → Simulated user input
2. **Events** → Add to waiting room → Process to global bus
3. **Simulation** → Execute Logic/functions
4. **Snapshot** → Validate state (if defined)
5. **Tick Event Bus** → Age events

---

## Simulation Runner

Simulation steps can execute either **free functions** or **Logic classes**:

```
┌─────────────────────────────────────────────────────────────────┐
│  execute_simulation_step(step, scene_context)                  │
└─────────────────────────────────────────────────────────────────┘
         │
         ├─→ ExecutionMode::Function
         │   │
         │   └─→ execute_function(function_type, scene_context)
         │       │
         │       ├─ FunctionType_ProcessUIActionsAndEvents
         │       ├─ FunctionType_ProcessNestedUIActionsAndEvents
         │       ├─ FunctionType_CheckMouseOverNestedUIElement
         │       ├─ FunctionType_UpdateCUserInterfaceVisibilityFromCUIState
         │       └─ ... (more function types)
         │
         └─→ ExecutionMode::LogicClass
             │
             └─→ execute_logic_class(logic_class_type, scene_context)
                 │
                 ├─ LogicClassType_UIActionLogic
                 ├─ LogicClassType_UICollisionLogic
                 ├─ LogicClassType_UIRenderLogic
                 ├─ LogicClassType_UIStateLogic
                 ├─ LogicClassType_CraftingRenderLogic
                 └─ ... (more logic class types)
```

**Example Simulation Step Execution:**

```cpp
// Step configured in test data:
// execution_mode: LogicClass
// logic_class_type: UIActionLogic

// Executed as:
UIActionLogic logic(scene_context);
logic.RunLogic();
```

---

## Tick Management

### Tick Number

Ticks are **1-indexed** in the testing harness to mimic the game loop:

```
Setup Phase: TestFixture::Initialize() - NOT a tick
Tick 1: First tick execution
Tick 2: Second tick execution
Tick 3: Third tick execution
...
Tick N: Last tick (where N = num_ticks)
```

The setup phase is separate from tick 1, avoiding confusion about "tick 0".

### Tick Specification in Test Data

```json
{
  "num_ticks": 5,  // Execute 5 ticks (1, 2, 3, 4, 5)
  
  "input_sequence": [
    {
      "tick": 1,     // Input executes at tick 1
      "inputs": [...]
    },
    {
      "tick": 3,     // Input executes at tick 3
      "inputs": [...]
    }
  ],
  
  "tick_snapshots": [
    {
      "tick": 1,     // Validate state at tick 1
      "description": "Initial state",
      "entity_collection": {...}
    },
    {
      "tick": 5,     // Validate state at tick 5
      "description": "Final state",
      "entity_collection": {...}
    }
  ]
}
```

### Event Bus Ticking

Similar to the game loop, the event bus is ticked at the **end** of each tick:

```cpp
// At end of execute_single_tick:
fixture.GetGameResources().event_handler.TickGlobalEventBus();
```

**Event Lifetime in Tests:**

```
Tick 1: Event added with lifetime 3
Tick 1 (end): Tick event bus → lifetime = 2
Tick 2 (end): Tick event bus → lifetime = 1
Tick 3 (end): Tick event bus → lifetime = 0 (removed)
```

### Simulation Steps Timing

**Important:** Simulation steps execute on **every tick**, not at specific ticks:

```json
{
  "simulation_data": {
    "steps": [
      {
        "execution_mode": "LogicClass",
        "logic_class_type": "UIActionLogic"
      }
    ]
  }
}
```

This configuration runs `UIActionLogic` on **every tick** (1, 2, 3, ... N).

For tick-specific actions, use `input_sequence` or `event_sequence`.

---

## Comparison vs. Game Loop

### Similarities

| Aspect | Game Loop | Testing Harness |
|--------|-----------|-----------------|
| **Event Flow** | Waiting Room → Global Bus → Tick | Same |
| **Event Bus Ticking** | At end of UpdateSystems | At end of execute_single_tick |
| **Logic Execution** | Logic classes via sAction/sCollision/sRender | Logic classes via simulation_runner |
| **Subscriber Updates** | From global event bus | From global event bus |

### Differences

| Aspect | Game Loop | Testing Harness |
|--------|-----------|-----------------|
| **Loop Counter** | `loop_number` (starts at 1, increments at end) | `tick` (starts at 1, loop variable, 1-indexed) |
| **Loop Count** | Infinite (until window closes) | Finite (`num_ticks` from config) |
| **Input Source** | Real user input via SFML | Simulated input from test data |
| **Rendering** | Actual window rendering | No visual rendering |
| **State Validation** | N/A | Tick snapshots and final comparison |
| **Execution Control** | Continuous real-time | Tick-by-tick with precise control |

### Event Bus Differences

**Game Loop:**
```cpp
// In UpdateSystems():
event_handler.PreloadEvents(window);        // Capture REAL user input
event_handler.ProcessWaitingRoomEventBus();
event_handler.UpateSubscribersFromGlobalEventBus();
// ... processing ...
event_handler.TickGlobalEventBus();
```

**Testing Harness:**
```cpp
// In execute_single_tick(tick):
execute_input_events_for_tick(..., tick);   // SIMULATED input (tick-specific)
execute_events_for_tick(..., tick);         // SIMULATED events (tick-specific)
event_handler.ProcessWaitingRoomEventBus();
// ... simulation steps ...
event_handler.TickGlobalEventBus();
```

---

## Component Reference

### tick_executor.h/cpp
- **Key Functions:**
  - `execute_tick_based_test()` - Main test execution entry point
  - `execute_single_tick()` - Execute one tick
  - `determine_num_ticks()` - Get tick count from config
  - `compare_tick_snapshot()` - Validate state at specific tick

### simulation_runner.h/cpp
- **Key Functions:**
  - `execute_simulation()` - Execute full simulation sequence
  - `execute_simulation_step()` - Execute single step (function or Logic)
  - `execute_function()` - Execute free function by type
  - `execute_logic_class()` - Instantiate and run Logic class

### input_simulation.h/cpp
- **Key Functions:**
  - `execute_input_events_for_tick()` - Process inputs for specific tick
  - Simulates keyboard, mouse button, and mouse movement events

### event_simulation.h/cpp
- **Key Functions:**
  - `execute_events_for_tick()` - Add events to waiting room for specific tick
  - Supports all EventType variants

### TestFixture.h/cpp
- **Provides:**
  - Complete test environment
  - Access to GameResources, SceneContext, EntityManager
  - Mock window and resources

### test_data_harness.h/cpp
- **Key Functions:**
  - `load_test_data_configs()` - Load all .test_data.bin files
  - `run_fixture_test()` - Execute complete test from config
  - `run_entity_memory_pool_comparison_test()` - Compare entity states

---

## Example Test Flow

Here's a complete example showing tick-based execution:

**Test Data (JSON):**
```json
{
  "metadata": {
    "test_name": "button_click_test",
    "num_ticks": 3
  },
  "input_sequence": [
    {
      "tick": 1,
      "inputs": [
        {
          "input_type": "MouseMove",
          "mouse_position": { "x": 100, "y": 200 }
        }
      ]
    },
    {
      "tick": 2,
      "inputs": [
        {
          "input_type": "MouseButtonPress",
          "mouse_button": "Left"
        }
      ]
    }
  ],
  "simulation_data": {
    "steps": [
      {
        "execution_mode": "LogicClass",
        "logic_class_type": "UICollisionLogic"
      },
      {
        "execution_mode": "LogicClass",
        "logic_class_type": "UIActionLogic"
      }
    ]
  },
  "tick_snapshots": [
    {
      "tick": 2,
      "description": "After click",
      "entity_collection": {...}
    }
  ]
}
```

**Execution Timeline:**

```
SETUP PHASE (before tick 1):
  - TestFixture::Initialize() configures entities and resources
  - NOT called "tick 0" to avoid confusion

TICK 1 (first game loop tick):
  1. Execute Input: MouseMove(100, 200)
  2. Process waiting room → global bus
  3. Execute simulation: UICollisionLogic → UIActionLogic
  4. No snapshot for tick 1
  5. Tick event bus

TICK 2:
  1. Execute Input: MouseButtonPress(Left)
  2. Process waiting room → global bus
  3. Execute simulation: UICollisionLogic → UIActionLogic
  4. Compare snapshot: Validate entity state after click
  5. Tick event bus

TICK 3:
  1. No inputs for tick 3
  2. Process waiting room → global bus (no new events)
  3. Execute simulation: UICollisionLogic → UIActionLogic
  4. No snapshot for tick 3
  5. Tick event bus

Final: Compare final state with expected_entity_collection
```

---

## Summary

The testing harness provides:

1. **Tick-based execution** for precise control and reproducibility
2. **Data-driven testing** via JSON test data files
3. **State validation** at specific ticks via snapshots
4. **Simulation flexibility** with free functions and Logic classes
5. **Controlled input/events** scheduled per tick
6. **Event bus parity** with production game loop

The tick-based model enables thorough testing of:
- Complex multi-frame interactions
- Event lifetime and propagation
- State transitions over time
- Integration of multiple systems

This architecture complements the production game loop while providing the control needed for deterministic testing.
