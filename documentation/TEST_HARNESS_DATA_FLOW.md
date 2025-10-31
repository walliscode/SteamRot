# Test Harness Data/Function Flow Overview

## Table of Contents
- [Introduction](#introduction)
- [High-Level Architecture](#high-level-architecture)
- [Data Flow Diagram](#data-flow-diagram)
- [Component Overview](#component-overview)
- [Complete Test Lifecycle](#complete-test-lifecycle)
- [Data Structures](#data-structures)
- [Function Flow Details](#function-flow-details)
- [Usage Patterns](#usage-patterns)
- [Extension Points](#extension-points)

## Introduction

The SteamRot test harness is a comprehensive data-driven testing framework that enables complex, multi-step test scenarios to be defined in JSON files and executed automatically. It provides precise control over timing, input simulation, event injection, and game logic execution on a tick-by-tick basis.

**Key Capabilities:**
- **Data-Driven Testing**: Define tests in JSON, no code changes needed
- **Tick-Based Execution**: Coordinate inputs, events, and logic per tick
- **Input Simulation**: Simulate mouse and keyboard interactions
- **Event Injection**: Inject engine events into the event system
- **Logic Execution**: Run sequences of game Logic classes or free functions
- **State Comparison**: Compare actual vs expected entity states
- **Integration with Catch2**: Works seamlessly with Catch2 generators and matchers

## High-Level Architecture

The test harness consists of several interconnected subsystems:

```
┌─────────────────────────────────────────────────────────────────┐
│                         Test Data Layer                          │
│  JSON Test Data → FlatBuffers Compilation → Binary Test Data    │
└──────────────────────────┬──────────────────────────────────────┘
                           │
                           ↓
┌─────────────────────────────────────────────────────────────────┐
│                      Test Harness API                            │
│  load_test_data_configs() → run_fixture_test()                  │
└──────────────────────────┬──────────────────────────────────────┘
                           │
                           ↓
┌─────────────────────────────────────────────────────────────────┐
│                       TestFixture                                │
│  GameResources + SceneResources + EntityManager                 │
└──────────────────────────┬──────────────────────────────────────┘
                           │
                           ↓
┌─────────────────────────────────────────────────────────────────┐
│                  Tick-Based Execution                            │
│  execute_tick_based_test() → execute_single_tick()              │
└──────────────────────────┬──────────────────────────────────────┘
                           │
        ┌──────────────────┼──────────────────┐
        ↓                  ↓                   ↓
┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│    Input     │  │    Event     │  │  Simulation  │
│  Simulation  │  │  Simulation  │  │    Runner    │
└──────────────┘  └──────────────┘  └──────────────┘
        │                  │                   │
        └──────────────────┼──────────────────┘
                           ↓
┌─────────────────────────────────────────────────────────────────┐
│                  State Comparison                                │
│  EntityMemoryPool Matcher → Pass/Fail                           │
└─────────────────────────────────────────────────────────────────┘
```

## Data Flow Diagram

### From JSON to Test Execution

```
Step 1: Test Data Creation
─────────────────────────
[Developer]
    │
    ↓ writes
[test_case.test_data.json]
    │
    ↓ CMake build process
[flatc compiler]
    │
    ↓ produces
[test_case.test_data.bin]


Step 2: Test Data Loading
─────────────────────────
[Test Case Execution]
    │
    ↓ calls
[load_test_data_configs()]
    │
    ├→ discover .test_data.bin files
    ├→ load binary data
    ├→ parse with FlatBuffers
    │
    ↓ returns
[vector<TestDataConfig*>]


Step 3: Fixture Creation
────────────────────────
[TestDataConfig*]
    │
    ↓ passed to
[run_fixture_test(config)]
    │
    ↓ creates
[TestFixture]
    │
    ├→ Configure GameResources
    ├→ Configure SceneResources  
    ├→ Initialize EntityManager
    ├→ Load entities from start_entity_collection
    │
    ↓ provides
[GameContext + SceneContext]


Step 4: Tick-Based Execution
────────────────────────────
[execute_tick_based_test(config, fixture)]
    │
    ↓ for each tick (0 to num_ticks-1)
[execute_single_tick(tick, config, fixture)]
    │
    ├─→ [execute_input_events_for_tick()]
    │   │
    │   ├→ MouseMove → update mouse position
    │   ├→ MouseClick → inject click event
    │   └→ KeyPress → inject key event
    │
    ├─→ [execute_events_for_tick()]
    │   │
    │   ├→ Convert EventTestData → EventPacket
    │   └→ Add to event handler waiting room
    │
    ├─→ [ProcessWaitingRoomEventBus()]
    │   │
    │   └→ Move events to global event bus
    │
    ├─→ [execute_simulation()]
    │   │
    │   ├→ For each SimulationStep:
    │   │   ├→ Function mode: call free function
    │   │   └→ LogicClass mode: instantiate & run Logic
    │   │
    │   └→ Modify entity state
    │
    └─→ [TickGlobalEventBus()]
        │
        └→ Decrement event lifetimes, remove expired


Step 5: State Comparison
────────────────────────
[expected_entity_collection]
    │
    ↓ configure into
[expected EntityMemoryPool]
    │
    ↓ compare with
[actual EntityMemoryPool from fixture]
    │
    ↓ using
[EqualsEntityMemoryPool matcher]
    │
    ↓ produces
[Pass/Fail + detailed error messages]
```

## Component Overview

### 1. test_data_harness (tests/harness/test_data_harness.h/cpp)

**Purpose**: Unified API for loading test data and running data-driven tests.

**Key Functions**:
- `load_test_data_configs()` - Load test data from adjacent data/ directory
- `load_test_data_configs(subdirectory)` - Load from specific subdirectory
- `run_fixture_test(config)` - **Main entry point** for data-driven testing
- `run_test_data_config(config)` - Validate test configuration structure
- `create_fixture_from_test_data(config)` - Create configured TestFixture
- `run_entity_memory_pool_comparison_test()` - Compare entity pools

**Data Flow**:
```
JSON files → Binary discovery → FlatBuffers parsing → TestDataConfig* vector
```

### 2. TestFixture (tests/harness/TestFixture.h/cpp)

**Purpose**: Resource management and context creation for tests.

**Owns**:
- `GameResources` - Game-level resources (asset manager, event handler, window)
- `SceneResources` - Scene-level resources (render texture, logic factories)
- `EntityManager` - Entity memory pool and archetype management

**Provides**:
- `GameContext` - Context for game-level operations
- `SceneContext` - Context for scene-level operations

**Lifecycle**:
```
Constructor → Initialize() → Configure GameResources → Configure SceneResources
→ Load/Configure Entities → Provide Contexts
```

### 3. tick_executor (tests/harness/tick_executor.h/cpp)

**Purpose**: Coordinate tick-based test execution.

**Key Functions**:
- `execute_tick_based_test(config, fixture)` - Run complete test
- `execute_single_tick(tick, config, fixture)` - Execute one tick
- `determine_num_ticks(config)` - Calculate number of ticks

**Per-Tick Execution Order**:
```
1. Execute inputs for this tick
2. Execute events for this tick
3. Process event waiting room
4. Execute simulation steps (all steps run every tick)
5. Tick global event bus
```

### 4. input_simulation (tests/harness/input_simulation.h/cpp)

**Purpose**: Simulate user input events (mouse/keyboard).

**Key Functions**:
- `execute_input_event(input_event, fixture)` - Single input
- `execute_input_events_for_tick(sequence, tick, fixture)` - Inputs for one tick
- `execute_input_sequence(sequence, fixture)` - All inputs

**Supported Input Types**:
- `MouseMove` - Update mouse position
- `MouseClick` - Mouse button press
- `MouseRelease` - Mouse button release
- `KeyPress` - Keyboard key press
- `KeyRelease` - Keyboard key release

**Data Flow**:
```
InputSequence → Filter by tick → Convert to SFML events → Apply to fixture
```

### 5. event_simulation (tests/harness/event_simulation.h/cpp)

**Purpose**: Inject engine events into the event system.

**Key Functions**:
- `execute_event_test_data(event_data, fixture)` - Single event
- `execute_events_for_tick(sequence, tick, fixture)` - Events for one tick
- `execute_event_sequence(sequence, fixture)` - All events

**Data Flow**:
```
EventSequence → Filter by tick → Convert to EventPacket → Add to waiting room
→ ProcessWaitingRoomEventBus() → Move to global event bus
```

### 6. simulation_runner (tests/harness/simulation_runner.h/cpp)

**Purpose**: Execute sequences of Logic classes or free functions.

**Key Functions**:
- `execute_simulation_step(step, scene_context)` - Single step
- `execute_simulation(simulation_data, scene_context)` - Full simulation
- `execute_simulation_with_fixture(simulation_data, fixture)` - With fixture wrapper

**Execution Modes**:
- **Function Mode**: Directly call free function (e.g., `ProcessNestedUIActionsAndEvents`)
- **LogicClass Mode**: Instantiate Logic class and call `RunLogic()` (e.g., `UIActionLogic`)

**Dispatcher Pattern**:
```cpp
switch (execution_mode) {
  case Function:
    switch (function_type) {
      case ProcessUIActionsAndEvents: ProcessUIActionsAndEvents(context); break;
      case ProcessNestedUIActionsAndEvents: ProcessNestedUIActionsAndEvents(context); break;
      // ...
    }
  case LogicClass:
    switch (logic_class_type) {
      case UIActionLogic: UIActionLogic(logic_context).RunLogic(); break;
      case UICollisionLogic: UICollisionLogic(logic_context).RunLogic(); break;
      // ...
    }
}
```

## Complete Test Lifecycle

### Phase 1: Test Data Creation (Developer Time)

```
1. Developer creates JSON test data file
   - Location: tests/<test_dir>/data/my_test.test_data.json
   - Content: metadata, start_entity_collection, expected_entity_collection,
              input_sequence, event_sequence, simulation_data

2. CMake build process runs
   - CMake discovers .test_data.json files
   - flatc compiles JSON → binary (.test_data.bin)
   - Binary files available for test runtime
```

### Phase 2: Test Initialization (Test Runtime)

```
3. Test case starts execution
   TEST_CASE("My test", "[unit]") {
     auto configs = load_test_data_configs();
     const auto *config = GENERATE_COPY(from_range(configs.value()));
     ...
   }

4. load_test_data_configs() executes
   - Discover all .test_data.bin files in adjacent data/ directory
   - Load binary data into memory
   - Parse with FlatBuffers GetTestDataConfig()
   - Validate metadata presence
   - Return vector<TestDataConfig*>

5. Catch2 generator iterates through configs
   - Each TestDataConfig* represents one test scenario
   - Generator calls test body for each config
```

### Phase 3: Fixture Creation

```
6. run_fixture_test(config) is called

7. create_fixture_from_test_data() executes
   - Create TestFixture instance with scene type
   - TestFixture constructor:
     * Initialize PathProvider for test environment
     * Create empty GameResources, SceneResources, EntityManager
   
8. TestFixture.Initialize() runs
   - ConfigureGameResourcesForTest():
     * Load game configuration
     * Create window, asset manager, event handler
     * Load assets into memory
   
   - ConfigureSceneLevelResources():
     * Load scene configuration
     * Configure render texture
     * Create EntityManager with pool size from config
   
   - FlatbuffersConfigurator.ConfigureEntitiesFromCollection():
     * Parse start_entity_collection
     * Configure each entity's components
     * Activate components as specified
   
   - ArchetypeManager.GenerateArchetypes():
     * Analyze entity component signatures
     * Build archetype map for efficient iteration
```

### Phase 4: Tick-Based Execution

```
9. execute_tick_based_test(config, fixture) executes

10. Determine number of ticks
    - If config->num_ticks() specified: use that value
    - Otherwise: auto-detect from max tick in inputs/events/simulation + 1
    - Minimum: 1 tick

11. For each tick (0 to num_ticks-1):

    execute_single_tick(tick, config, fixture):
    
    A. Execute Inputs for this tick
       - Filter input_sequence by tick number
       - For each matching input:
         * Convert to SFML event
         * Apply to fixture context (update mouse position, inject key events)
    
    B. Execute Events for this tick
       - Filter event_sequence by tick number
       - For each matching event:
         * Convert EventTestData → EventPacket
         * Add to event handler waiting room
    
    C. Process Event Waiting Room
       - event_handler.ProcessWaitingRoomEventBus()
       - Moves waiting room events → global event bus
    
    D. Execute Simulation (ALL steps run every tick)
       - For each SimulationStep in simulation_data:
         
         If execution_mode == Function:
           - Switch on function_type
           - Call corresponding free function with SceneContext
           - Examples: ProcessUIActionsAndEvents, ProcessNestedUIActionsAndEvents
         
         If execution_mode == LogicClass:
           - Switch on logic_class_type
           - Instantiate Logic class with LogicContext
           - Call Logic.RunLogic()
           - Examples: UIActionLogic, UICollisionLogic, UIRenderLogic
         
         - Logic/functions modify entity state:
           * Update component values
           * Change UI visibility
           * Process collisions
           * Generate events
    
    E. Tick Event Bus
       - event_handler.TickGlobalEventBus()
       - Decrement lifetime of all events
       - Remove events with lifetime = 0

12. After all ticks complete, state has been fully simulated
```

### Phase 5: State Comparison

```
13. If config->expected_entity_collection() is present:

    A. Create expected EntityMemoryPool
       - Initialize empty pool
       - FlatbuffersConfigurator.ConfigureEntitiesFromCollection()
       - Configure entities from expected_entity_collection
    
    B. Get actual EntityMemoryPool
       - fixture.GetEntityManager().GetEntityMemoryPool()
    
    C. Build test metadata string
       - Extract test_name and description from config->metadata()
       - Format: "Test: <name>, Description: <desc>"
    
    D. Compare pools
       - run_entity_memory_pool_comparison_test(actual, expected, metadata)
       - Uses EqualsEntityMemoryPool matcher
       - Matcher compares:
         * Pool sizes
         * Component counts per entity
         * Component activation states
         * Component values (field by field)
    
    E. Report results
       - On match: test passes
       - On mismatch: detailed error message with:
         * Test metadata
         * Which entity differs
         * Which component differs
         * Which field differs
         * Expected vs actual values

14. Test completes - Pass or Fail
```

## Data Structures

### TestDataConfig (Root)

```cpp
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
}
```

**Fields**:
- `metadata` - Test name, description, tags, expected outcome
- `start_entity_collection` - Initial entity state (optional, defaults to empty pool)
- `expected_entity_collection` - Expected final state (for comparison tests)
- `game_resources` - Game-level configuration overrides
- `scene_resources` - Scene-level configuration overrides
- `simulation_data` - Sequence of Logic classes/functions to execute
- `input_sequence` - User input events to simulate
- `event_sequence` - Engine events to inject
- `num_ticks` - Number of ticks to execute (optional, auto-detected if omitted)

### SimulationData

```cpp
table SimulationData {
  steps: [SimulationStep];
  description: string;
  num_ticks: uint32 = 1;
}

table SimulationStep {
  simulation_type: SimulationType;    // Action, Movement, Render, Collision
  execution_mode: ExecutionMode;      // Function or LogicClass
  function_type: FunctionType;        // Which function (if Function mode)
  logic_class_type: LogicClassType;   // Which Logic class (if LogicClass mode)
  description: string;
}
```

**Important**: All simulation steps execute on **every tick**. The simulation configuration is set once and does not change during test execution.

### InputSequence

```cpp
table InputSequence {
  inputs: [InputEvent];
  description: string;
}

table InputEvent {
  input_type: InputType;          // MouseMove, MouseClick, KeyPress, etc.
  input_data: InputEventData;     // MouseInputData or KeyboardInputData
  tick: uint32;                   // Which tick to execute on
  description: string;
}
```

### EventSequence

```cpp
table EventSequence {
  events: [EventTestData];
  description: string;
}

table EventTestData {
  tick: uint32;                   // Which tick to inject on
  event_packet: EventPacket;      // Event data (type, lifetime, data)
  description: string;
}
```

### EntityCollection

```cpp
table EntityCollection {
  entity_memory_pool_size: uint32;
  entities: [EntityData];
}

table EntityData {
  index: uint32;
  c_user_interface: UserInterfaceData;
  c_grimoire_machina: GrimoireMachinaData;
  // ... other components
}
```

## Function Flow Details

### Main Entry Point: run_fixture_test()

```
run_fixture_test(const TestDataConfig *config)
│
├─→ Validate config is not null
├─→ create_fixture_from_test_data(config)
│   │
│   ├─→ Create TestFixture with scene type
│   ├─→ TestFixture.Initialize(start_entity_collection)
│   │   │
│   │   ├─→ ConfigureGameResourcesForTest()
│   │   │   ├─→ Load game configuration
│   │   │   ├─→ Create window, asset manager, event handler
│   │   │   └─→ Load assets
│   │   │
│   │   └─→ ConfigureSceneLevelResources(scene_type, entity_collection)
│   │       ├─→ Load scene configuration
│   │       ├─→ Create render texture
│   │       ├─→ Configure EntityManager
│   │       │   ├─→ Set pool size
│   │       │   └─→ FlatbuffersConfigurator.ConfigureEntitiesFromCollection()
│   │       │       ├─→ For each entity in entity_collection:
│   │       │       │   ├─→ Configure component from FlatBuffers data
│   │       │       │   ├─→ Set component values
│   │       │       │   └─→ Activate/deactivate as specified
│   │       │       └─→ Return configured pool
│   │       │
│   │       └─→ Generate archetypes
│   │
│   └─→ Return configured TestFixture
│
├─→ execute_tick_based_test(config, fixture)
│   │
│   ├─→ num_ticks = determine_num_ticks(config)
│   │   │
│   │   ├─→ If config->num_ticks() specified: return it
│   │   └─→ Else: find max tick in inputs/events/simulation + 1
│   │
│   └─→ For tick = 0 to num_ticks-1:
│       │
│       └─→ execute_single_tick(tick, config, fixture)
│           │
│           ├─→ If config->input_sequence():
│           │   └─→ execute_input_events_for_tick(input_sequence, tick, fixture)
│           │       └─→ For each input where input->tick() == tick:
│           │           └─→ execute_input_event(input, fixture)
│           │               ├─→ Switch on input_type:
│           │               │   ├─→ MouseMove: update fixture mouse position
│           │               │   ├─→ MouseClick: inject click event
│           │               │   ├─→ KeyPress: inject key event
│           │               │   └─→ etc.
│           │               └─→ Return success/failure
│           │
│           ├─→ If config->event_sequence():
│           │   └─→ execute_events_for_tick(event_sequence, tick, fixture)
│           │       └─→ For each event where event->tick() == tick:
│           │           └─→ execute_event_test_data(event, fixture)
│           │               ├─→ Convert EventTestData → EventPacket
│           │               └─→ Add to event_handler.waiting_room_event_bus
│           │
│           ├─→ event_handler.ProcessWaitingRoomEventBus()
│           │   └─→ Move events from waiting_room → global_event_bus
│           │
│           ├─→ If config->simulation_data():
│           │   └─→ execute_simulation(simulation_data, scene_context)
│           │       └─→ For each step in simulation_data->steps():
│           │           └─→ execute_simulation_step(step, scene_context)
│           │               ├─→ If execution_mode == Function:
│           │               │   └─→ Switch on function_type:
│           │               │       ├─→ ProcessUIActionsAndEvents: call function
│           │               │       ├─→ ProcessNestedUIActionsAndEvents: call function
│           │               │       └─→ etc.
│           │               │
│           │               └─→ If execution_mode == LogicClass:
│           │                   └─→ Switch on logic_class_type:
│           │                       ├─→ UIActionLogic: instantiate & RunLogic()
│           │                       ├─→ UICollisionLogic: instantiate & RunLogic()
│           │                       └─→ etc.
│           │
│           └─→ event_handler.TickGlobalEventBus()
│               └─→ Decrement event lifetimes, remove expired events
│
└─→ If config->expected_entity_collection():
    │
    ├─→ Create expected_pool
    ├─→ FlatbuffersConfigurator.ConfigureEntitiesFromCollection(expected_pool, expected_entity_collection)
    ├─→ Get actual_pool from fixture.GetEntityManager()
    ├─→ Build test_metadata string from config->metadata()
    └─→ run_entity_memory_pool_comparison_test(actual_pool, expected_pool, test_metadata)
        └─→ REQUIRE_THAT(actual, EqualsEntityMemoryPool(expected, metadata))
            └─→ Matcher compares pools field-by-field
                ├─→ Pass: test succeeds
                └─→ Fail: detailed error message with test metadata
```

### Input Simulation Flow

```
execute_input_sequence(input_sequence, fixture)
│
└─→ Find unique tick values in input_sequence
    │
    └─→ For each unique tick:
        │
        └─→ execute_input_events_for_tick(input_sequence, tick, fixture)
            │
            └─→ Filter inputs where input->tick() == tick
                │
                └─→ For each matching input:
                    │
                    └─→ execute_input_event(input, fixture)
                        │
                        └─→ Switch on input->input_type():
                            │
                            ├─→ MouseMove:
                            │   ├─→ Get MouseInputData
                            │   ├─→ Extract position (x, y)
                            │   ├─→ fixture.GetSceneResources().mouse_position = position
                            │   └─→ Update GameContext mouse_position
                            │
                            ├─→ MouseClick:
                            │   ├─→ Get MouseInputData
                            │   ├─→ Extract button
                            │   ├─→ Create SFML MouseButtonPressed event
                            │   ├─→ Inject into event system
                            │   └─→ May trigger UI actions
                            │
                            ├─→ MouseRelease:
                            │   └─→ Similar to MouseClick but MouseButtonReleased
                            │
                            ├─→ KeyPress:
                            │   ├─→ Get KeyboardInputData
                            │   ├─→ Extract key_code, modifiers
                            │   ├─→ Create SFML KeyPressed event
                            │   └─→ Inject into event system
                            │
                            └─→ KeyRelease:
                                └─→ Similar to KeyPress but KeyReleased
```

### Event Simulation Flow

```
execute_event_sequence(event_sequence, fixture)
│
└─→ Find unique tick values in event_sequence
    │
    └─→ For each unique tick:
        │
        └─→ execute_events_for_tick(event_sequence, tick, fixture)
            │
            └─→ Filter events where event->tick() == tick
                │
                └─→ For each matching event:
                    │
                    └─→ execute_event_test_data(event_data, fixture)
                        │
                        ├─→ Get EventPacket from event_data
                        ├─→ Extract event_type, event_lifetime, event_data_data
                        ├─→ Convert FlatBuffers EventPacket → engine EventPacket
                        │
                        └─→ fixture.GetGameResources().event_handler.AddEvent(event_packet)
                            │
                            └─→ Event added to waiting_room_event_bus
                                (will be moved to global_event_bus by ProcessWaitingRoomEventBus())
```

### Simulation Execution Flow

```
execute_simulation(simulation_data, scene_context)
│
└─→ For each step in simulation_data->steps():
    │
    └─→ execute_simulation_step(step, scene_context)
        │
        ├─→ Validate step is not null
        │
        ├─→ Switch on step->execution_mode():
        │   │
        │   ├─→ Case ExecutionMode::Function:
        │   │   │
        │   │   └─→ Switch on step->function_type():
        │   │       │
        │   │       ├─→ ProcessUIActionsAndEvents:
        │   │       │   └─→ ProcessUIActionsAndEvents(scene_context)
        │   │       │       └─→ Iterate UI entities, process actions
        │   │       │
        │   │       ├─→ ProcessNestedUIActionsAndEvents:
        │   │       │   └─→ ProcessNestedUIActionsAndEvents(scene_context)
        │   │       │       └─→ Recursively process UI tree actions
        │   │       │
        │   │       ├─→ ProcessButtonElementActions:
        │   │       │   └─→ ProcessButtonElementActions(scene_context)
        │   │       │       └─→ Handle button click/hover/release
        │   │       │
        │   │       └─→ ProcessDropDownListElementActions:
        │   │           └─→ ProcessDropDownListElementActions(scene_context)
        │   │               └─→ Handle dropdown open/close/select
        │   │
        │   └─→ Case ExecutionMode::LogicClass:
        │       │
        │       └─→ Switch on step->logic_class_type():
        │           │
        │           ├─→ UIActionLogic:
        │           │   ├─→ Create LogicContext from SceneContext
        │           │   ├─→ UIActionLogic logic(logic_context)
        │           │   └─→ logic.RunLogic()
        │           │       └─→ Process UI actions for all UI entities
        │           │
        │           ├─→ UICollisionLogic:
        │           │   ├─→ Create LogicContext
        │           │   ├─→ UICollisionLogic logic(logic_context)
        │           │   └─→ logic.RunLogic()
        │           │       └─→ Check mouse collision with UI elements
        │           │
        │           ├─→ UIRenderLogic:
        │           │   ├─→ Create LogicContext
        │           │   ├─→ UIRenderLogic logic(logic_context)
        │           │   └─→ logic.RunLogic()
        │           │       └─→ Render UI elements to scene texture
        │           │
        │           ├─→ UIStateLogic:
        │           │   └─→ Update UI state based on events
        │           │
        │           └─→ CraftingRenderLogic:
        │               └─→ Render crafting-specific UI
        │
        └─→ Return success/failure for this step
```

## Usage Patterns

### Basic Data-Driven Test

```cpp
#include "test_data_harness.h"
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("Simple data-driven test", "[unit][component]") {
  // Load all test configs from adjacent data/ directory
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  // Generator iterates through each config
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Run the test - creates fixture, executes ticks, compares states
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

**What happens**:
1. `load_test_data_configs()` discovers all .test_data.bin files
2. Catch2 generator runs test body for each config
3. `run_fixture_test()` creates fixture, configures entities, executes simulation
4. If expected state provided, comparison happens automatically
5. Test passes/fails with detailed error messages

### Test with Manual Fixture Control

```cpp
TEST_CASE("Manual fixture control", "[unit]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = configs.value()[0];
  
  // Create fixture manually
  auto fixture_result = steamrot::tests::create_fixture_from_test_data(config);
  REQUIRE(fixture_result.has_value());
  auto &fixture = fixture_result.value();
  
  // Access resources for custom testing
  auto &entity_mgr = fixture.GetEntityManager();
  auto &game_resources = fixture.GetGameResources();
  
  // Manually execute simulation
  if (config->simulation_data()) {
    auto sim_result = steamrot::tests::execute_simulation_with_fixture(
        config->simulation_data(), fixture);
    REQUIRE(sim_result.has_value());
  }
  
  // Custom assertions on entity state
  const auto &pool = entity_mgr.GetEntityMemoryPool();
  // ... test logic ...
}
```

### Loading from Specific Subdirectory

```cpp
TEST_CASE("Load from entity tests", "[unit][entity]") {
  // Load test data from tests/entity/data/
  auto configs = steamrot::tests::load_test_data_configs("entity");
  REQUIRE(configs.has_value());
  
  for (const auto *config : configs.value()) {
    // Process each entity test config
    auto result = steamrot::tests::run_fixture_test(config);
    REQUIRE(result.has_value());
  }
}
```

### Direct Entity Pool Comparison

```cpp
TEST_CASE("Direct pool comparison", "[unit]") {
  EntityMemoryPool actual_pool;
  EntityMemoryPool expected_pool;
  
  // Configure pools, run logic, etc.
  // ...
  
  // Compare using matcher
  steamrot::tests::run_entity_memory_pool_comparison_test(actual_pool, expected_pool);
}
```

## Extension Points

### Adding New Input Types

1. **Add to enum** in `src/flatbuffers_headers/input_test_data.fbs`:
```fbs
enum InputType : byte {
  // ... existing types ...
  GamepadButton = 10,
}
```

2. **Add input data table**:
```fbs
table GamepadInputData {
  button: uint8;
  controller_id: uint8;
}
```

3. **Update union**:
```fbs
union InputEventData {
  MouseInputData,
  KeyboardInputData,
  GamepadInputData
}
```

4. **Add case in dispatcher** (`tests/harness/input_simulation.cpp`):
```cpp
case InputType::GamepadButton: {
  const auto *gamepad_data = input_event->input_data_as_GamepadInputData();
  // Handle gamepad input
  return std::monostate{};
}
```

5. **Rebuild** to regenerate headers

### Adding New Logic Classes to Simulation

1. **Add to enum** in `src/flatbuffers_headers/simulation.fbs`:
```fbs
enum LogicClassType : byte {
  // ... existing types ...
  MyNewLogic = 20,
}
```

2. **Add case in dispatcher** (`tests/harness/simulation_runner.cpp`):
```cpp
case LogicClassType::MyNewLogic: {
  LogicContext logic_context = CreateLogicContext(scene_context);
  MyNewLogic logic(logic_context);
  logic.RunLogic();
  return std::monostate{};
}
```

3. **Rebuild** to regenerate headers

4. **Use in test data**:
```json
{
  "simulation_type": "Action",
  "execution_mode": "LogicClass",
  "logic_class_type": "MyNewLogic"
}
```

### Adding New Free Functions to Simulation

1. **Add to enum** in `src/flatbuffers_headers/simulation.fbs`:
```fbs
enum FunctionType : byte {
  // ... existing types ...
  MyNewFunction = 50,
}
```

2. **Add case in dispatcher** (`tests/harness/simulation_runner.cpp`):
```cpp
case FunctionType::MyNewFunction: {
  MyNewFunction(scene_context);
  return std::monostate{};
}
```

3. **Rebuild** and use in test data

### Adding New Event Types

1. **Define in FlatBuffers schema** (`src/flatbuffers_headers/events.fbs`)
2. **Add case in event simulation** if custom handling needed
3. **Use in test data**:
```json
{
  "event_sequence": {
    "events": [
      {
        "tick": 0,
        "event_packet": {
          "event_type": "MY_NEW_EVENT",
          "event_lifetime": 5,
          "event_data_data_type": "MyNewEventData",
          "event_data_data": { /* ... */ }
        }
      }
    ]
  }
}
```

### Adding New Test Data Types

The `TestDataConfig` schema is designed to be extensible. To add a new data type:

1. **Create FlatBuffers schema** for the new data type
2. **Add field to TestDataConfig** in `test_data.fbs`:
```fbs
table TestDataConfig {
  // ... existing fields ...
  my_new_data: MyNewDataType;
}
```

3. **Rebuild** to regenerate headers

4. **Add handler in test harness** (optional):
```cpp
if (config->my_new_data()) {
  // Process new data type
}
```

5. **Use in test data**:
```json
{
  "metadata": { /* ... */ },
  "my_new_data": { /* ... */ }
}
```

## Summary

The test harness provides a complete data-driven testing solution:

**Key Strengths**:
- **Declarative**: Tests defined in JSON, minimal code changes
- **Comprehensive**: Supports entity state, input, events, logic execution
- **Precise**: Tick-based execution for exact timing control
- **Flexible**: Extensible schema and dispatcher patterns
- **Integrated**: Works seamlessly with Catch2 and existing infrastructure

**Main Components**:
- `test_data_harness` - Loading and execution API
- `TestFixture` - Resource and context management
- `tick_executor` - Tick-based coordination
- `input_simulation` - User input injection
- `event_simulation` - Engine event injection
- `simulation_runner` - Logic execution

**Data Flow**: JSON → FlatBuffers → TestDataConfig → TestFixture → Tick Execution → State Comparison → Pass/Fail

**For Developers**:
- Write JSON test data files
- Use `run_fixture_test()` for automatic execution
- Extend with new input types, Logic classes, and data types as needed
- Leverage Catch2 generators for parameterized testing
