# Test Harness

## Overview

The test harness provides a unified, simplified interface for:
1. Loading test data configurations for data-driven testing with Catch2 generators
2. Creating and configuring TestFixture instances from test data
3. **Executing input sequences** - simulating user input (mouse/keyboard) on a tick-by-tick basis
4. **Executing event sequences** - injecting engine events on a tick-by-tick basis
5. **Executing simulations** - running sequences of Logic classes or free functions
6. Running comparison tests between expected and actual entity states

This consolidates functionality for resource-based testing and data-driven test execution with support for complex simulation scenarios and tick-by-tick input/event injection.

## Purpose

- Provide a single, simple API for loading test data
- Integrate TestFixture for resource setup and management
- **Execute input sequences** - simulate user input tick-by-tick
- **Execute event sequences** - inject engine events tick-by-tick
- **Execute simulations** - run sequences of Logic classes or free functions defined in test data
- Enable data-driven testing with automatic fixture creation
- Work seamlessly with Catch2 generators and matchers
- Keep the interface minimal and easy to use

## Directory Structure

```
harness/
├── test_data_harness.h            # Unified API for loading test data
├── test_data_harness.cpp          # Implementation
├── test_data_harness.test.cpp     # Unit tests
├── simulation_runner.h            # Simulation execution engine
├── simulation_runner.cpp          # Implementation
├── simulation_runner.test.cpp     # Simulation tests
├── input_simulation.h             # Input sequence simulation
├── input_simulation.cpp           # Implementation
├── input_simulation.test.cpp      # Input simulation tests
├── event_simulation.h             # Event sequence simulation
├── event_simulation.cpp           # Implementation
├── event_simulation.test.cpp      # Event simulation tests
├── TestFixture.h                  # Resource management for tests
├── TestFixture.cpp                # Implementation
├── CMakeLists.txt                 # Build configuration
├── README.md                      # This file
└── data/                          # Sample test data files
    ├── sample_test_1.test_data.json
    ├── sample_test_2.test_data.json
    ├── sample_test_3.test_data.json
    ├── sample_simulation_test.test_data.json
    ├── sample_function_simulation.test_data.json
    ├── sample_input_sequence.test_data.json
    ├── sample_event_sequence.test_data.json
    └── sample_input_event_simulation.test_data.json
```

## Usage

### TestFixture Integration (Recommended)

The main wrapper function for data-driven testing with TestFixture:

```cpp
#include "test_data_harness.h"
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("Data-driven test with TestFixture", "[unit][my_component]") {
  // Load test configurations
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  // Use Catch2 generator to iterate through configs
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Create fixture, configure entities, and run comparisons
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

This pattern:
- Loads test data from adjacent `data/` directory
- Creates a TestFixture for each configuration
- Configures entities from `start_entity_collection`
- Compares with `expected_entity_collection` automatically
- Ready for future simulation functionality

### Manual Fixture Creation

For more control, create the fixture manually:

```cpp
#include "test_data_harness.h"

TEST_CASE("Manual fixture creation", "[unit]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = configs.value()[0];
  
  // Create and configure fixture from test data
  auto fixture_result = steamrot::tests::create_fixture_from_test_data(config);
  REQUIRE(fixture_result.has_value());
  
  auto &fixture = fixture_result.value();
  
  // Access resources and entity manager
  auto &entity_mgr = fixture.GetEntityManager();
  auto &game_resources = fixture.GetGameResources();
  // ... test logic ...
}
```

### Basic Usage - Adjacent Data Directory

The simplest and recommended approach - load test data from an adjacent `data/` directory:

```cpp
#include "test_data_harness.h"
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("Parameterized test with test data", "[unit][my_component]") {
  // One simple call to load all test data
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  // Use with Catch2 generator
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Process the test data
  REQUIRE(config->metadata() != nullptr);
  // ... test logic ...
}
```

### Loading from Specific Subdirectory

When you need to load test data from a specific test directory:

```cpp
#include "test_data_harness.h"

TEST_CASE("Load from subdirectory", "[unit]") {
  // Load from tests/entity/data/
  auto configs = steamrot::tests::load_test_data_configs("entity");
  REQUIRE(configs.has_value());
  
  for (const auto *config : configs.value()) {
    // Process each config
  }
}
```

### Using the Top-Level Wrapper

The `run_test_data_config()` wrapper validates that test data is properly structured:

```cpp
#include "test_data_harness.h"

TEST_CASE("Validate test data configuration", "[unit]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Validate the configuration is well-formed
  auto result = steamrot::tests::run_test_data_config(config);
  REQUIRE(result.has_value());
  
  INFO("Test name: " << config->metadata()->test_name()->str());
}
```

### Running Entity Memory Pool Comparison Tests

The `run_entity_memory_pool_comparison_test()` function compares two EntityMemoryPool instances directly:

```cpp
#include "test_data_harness.h"

TEST_CASE("Entity memory pool comparison test", "[unit]") {
  // Create and configure pools
  EntityMemoryPool actual_pool;
  EntityMemoryPool expected_pool;
  
  // ... configure pools, simulate logic, etc ...
  
  // Compare pools using matcher
  steamrot::tests::run_entity_memory_pool_comparison_test(actual_pool, expected_pool);
}
```

## API

### `load_test_data_configs()`

Loads all test data from the adjacent `data/` directory (determined via `__FILE__` macro).

**Parameters:** None

**Returns:** `std::expected<std::vector<const TestDataConfig *>, FailInfo>`

**Example:**
```cpp
auto configs = steamrot::tests::load_test_data_configs();
```

### `load_test_data_configs(subdirectory)`

Loads all test data from `tests/<subdirectory>/data/` directory.

**Parameters:**
- `subdirectory`: Test subdirectory name (e.g., "entity", "components")

**Returns:** `std::expected<std::vector<const TestDataConfig *>, FailInfo>`

**Example:**
```cpp
auto configs = steamrot::tests::load_test_data_configs("entity");
```

### `run_test_data_config(config)`

Top-level wrapper that runs tests based on TestDataConfig contents. This examines the configuration and validates that the appropriate data is present for testing.

**Parameters:**
- `config`: Pointer to TestDataConfig object

**Returns:** `std::expected<std::monostate, FailInfo>`

**Example:**
```cpp
auto configs = steamrot::tests::load_test_data_configs();
REQUIRE(configs.has_value());

const auto *config = configs.value()[0];
auto result = steamrot::tests::run_test_data_config(config);
REQUIRE(result.has_value());
```

**Supported Data Types:**
- Entity Memory Pool comparisons (`start_entity_collection` + `expected_entity_collection`)
- Future: Event sequences, UI configurations, Logic tests, etc.

**Note:** This wrapper performs validation only. For actual test execution, use specialized test functions like `run_entity_memory_pool_comparison_test()`.

### `run_entity_memory_pool_comparison_test(actual, expected)`

Compares two EntityMemoryPool instances using the EqualsEntityMemoryPool matcher.

**Parameters:**
- `actual`: The actual EntityMemoryPool to test
- `expected`: The expected EntityMemoryPool to compare against

**Returns:** None (uses Catch2 assertions internally)

**Example:**
```cpp
EntityMemoryPool actual_pool;
EntityMemoryPool expected_pool;

// Configure pools, simulate logic, etc.
// ...

steamrot::tests::run_entity_memory_pool_comparison_test(actual_pool, expected_pool);
```

**Behavior:**
- Compares pools using `EqualsEntityMemoryPool` matcher
- Provides detailed error messages on mismatch
- Allows pools to be instantiated and manipulated before comparison (e.g., simulating logic)

### `create_fixture_from_test_data(config, scene_type)`

Creates and configures a TestFixture from test data configuration.

**Parameters:**
- `config`: Pointer to TestDataConfig containing entity setup
- `scene_type`: Scene type for the fixture (default: SceneType_TEST)

**Returns:** `std::expected<TestFixture, FailInfo>`

**Example:**
```cpp
auto configs = steamrot::tests::load_test_data_configs();
const auto *config = configs.value()[0];

auto fixture_result = steamrot::tests::create_fixture_from_test_data(config);
REQUIRE(fixture_result.has_value());

auto &fixture = fixture_result.value();
// Use fixture for testing
```

**Behavior:**
- Creates TestFixture with specified scene type
- Initializes game and scene resources
- Configures entities from `start_entity_collection` if present
- Generates archetypes automatically

### `run_fixture_test(config)`

Wrapper function for data-driven testing with TestFixture and Catch2 generators.

**Parameters:**
- `config`: Test data configuration

**Returns:** `std::expected<std::monostate, FailInfo>`

**Example:**
```cpp
TEST_CASE("Data-driven test with fixture", "[unit]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

**Behavior:**
1. Creates TestFixture from test data configuration
2. Configures entities from `start_entity_collection`
3. **Executes simulation if `simulation_data` is present**
4. If `expected_entity_collection` is present, compares entity states automatically

**This is the main wrapper function for data-driven testing with TestFixture.**

## Simulations

### Overview

The test harness now supports **data-driven simulations** that allow test data to specify which Logic classes or free functions to execute and in which order. This mimics the Scene systems organization (Action, Movement, Render, Collision) and enables complex, multi-step test scenarios.

### Simulation Concepts

**SimulationType**: Groups logic into categories matching Scene logic organization
- `Action` - UI actions, input processing
- `Movement` - Entity movement, physics
- `Render` - Drawing and rendering
- `Collision` - Collision detection

**ExecutionMode**: Determines what to execute
- `Function` - Execute individual free function
- `LogicClass` - Execute entire Logic class

**Simulation Step**: A single execution step with:
- `simulation_type` - Which system category
- `execution_mode` - Function or LogicClass
- `function_type` - Which specific function (if mode is Function)
- `logic_class_type` - Which Logic class (if mode is LogicClass)
- `description` - Optional description

### Using Simulations

#### Test Data Format

```json
{
  "metadata": {
    "test_name": "my_simulation_test",
    "description": "Test with simulation",
    "tags": ["unit", "simulation"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [...]
  },
  "simulation_data": {
    "description": "Execute collision, then render, then actions",
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "LogicClass",
        "logic_class_type": "UICollisionLogic",
        "description": "Detect UI collision"
      },
      {
        "simulation_type": "Render",
        "execution_mode": "LogicClass",
        "logic_class_type": "UIRenderLogic",
        "description": "Render UI elements"
      },
      {
        "simulation_type": "Action",
        "execution_mode": "Function",
        "function_type": "ProcessNestedUIActionsAndEvents",
        "description": "Process UI actions"
      }
    ]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [...]
  }
}
```

#### Executing Simulations

Simulations are automatically executed by `run_fixture_test()`:

```cpp
TEST_CASE("Run simulation from test data", "[unit][simulation]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // If config has simulation_data, it will be executed automatically
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

#### Manual Simulation Execution

For more control, use the simulation runner directly:

```cpp
#include "simulation_runner.h"

TEST_CASE("Manual simulation execution", "[unit]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = configs.value()[0];
  
  // Create fixture
  auto fixture_result = steamrot::tests::create_fixture_from_test_data(config);
  REQUIRE(fixture_result.has_value());
  auto &fixture = fixture_result.value();
  
  // Execute simulation manually
  if (config->simulation_data()) {
    auto sim_result = steamrot::tests::execute_simulation_with_fixture(
        config->simulation_data(), fixture);
    REQUIRE(sim_result.has_value());
  }
}
```

### Available Logic Classes

Current LogicClassType values:
- `UIActionLogic` - Process UI actions and events
- `UICollisionLogic` - Check UI collision with mouse
- `UIRenderLogic` - Render UI elements
- `UIStateLogic` - Update UI state
- `CraftingRenderLogic` - Render crafting UI

### Available Free Functions

Current FunctionType values:
- `ProcessUIActionsAndEvents` - Process UI actions for elements
- `ProcessNestedUIActionsAndEvents` - Process UI actions recursively
- `ProcessButtonElementActions` - Process button actions
- `ProcessDropDownListElementActions` - Process dropdown actions

### Simulation API

#### `execute_simulation_step(step, scene_context)`

Execute a single simulation step.

**Parameters:**
- `step`: SimulationStep to execute
- `scene_context`: SceneContext with resources and entities

**Returns:** `std::expected<std::monostate, FailInfo>`

#### `execute_simulation(simulation_data, scene_context)`

Execute a complete simulation sequence.

**Parameters:**
- `simulation_data`: SimulationData with steps
- `scene_context`: SceneContext with resources and entities

**Returns:** `std::expected<std::monostate, FailInfo>`

#### `execute_simulation_with_fixture(simulation_data, fixture)`

Execute simulation using a TestFixture (convenience wrapper).

**Parameters:**
- `simulation_data`: SimulationData with steps
- `fixture`: TestFixture containing the test environment

**Returns:** `std::expected<std::monostate, FailInfo>`

### Extending Simulations

To add new functions or Logic classes to simulations:

1. **Add to FlatBuffers enum** in `src/flatbuffers_headers/simulation.fbs`:
   ```fbs
   enum FunctionType : byte {
     // ... existing values ...
     MyNewFunction = 50,
   }
   ```

2. **Add case to dispatcher** in `tests/harness/simulation_runner.cpp`:
   ```cpp
   case FunctionType::MyNewFunction: {
     MyNewFunction(scene_context);
     return std::monostate{};
   }
   ```

3. **Rebuild** to regenerate headers

4. **Use in test data**:
   ```json
   {
     "simulation_type": "Action",
     "execution_mode": "Function",
     "function_type": "MyNewFunction"
   }
   ```

## Input and Event Sequences

### Overview

**New in Stage 1.1**: The test harness now supports tick-by-tick input and event injection. This allows test data to specify sequences of user inputs (mouse/keyboard) and engine events that should be injected at specific ticks during testing.

### Input Sequences

Input sequences allow simulation of user input events on a tick-by-tick basis.

#### Supported Input Types

- `MouseMove` - Update mouse position
- `MouseClick` - Mouse button press
- `MouseRelease` - Mouse button release  
- `KeyPress` - Keyboard key press
- `KeyRelease` - Keyboard key release

#### Test Data Format

```json
{
  "metadata": {
    "test_name": "input_simulation_test",
    "description": "Test with input sequence",
    "tags": ["unit", "input"],
    "expected_to_pass": true,
    "version": 1
  },
  "input_sequence": {
    "description": "User clicks a button",
    "inputs": [
      {
        "input_type": "MouseMove",
        "input_data_type": "MouseInputData",
        "input_data": {
          "position": { "x": 150.0, "y": 125.0 },
          "button": 0
        },
        "tick": 0,
        "description": "Move mouse to button"
      },
      {
        "input_type": "MouseClick",
        "input_data_type": "MouseInputData",
        "input_data": {
          "position": { "x": 150.0, "y": 125.0 },
          "button": 0
        },
        "tick": 1,
        "description": "Click button"
      }
    ]
  }
}
```

#### Execution

Input sequences are automatically executed by `run_fixture_test()` before simulations:

```cpp
TEST_CASE("Test with input sequence", "[unit][input]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Input sequence (if present) is executed automatically before simulation
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

#### Manual Input Execution

For fine-grained control:

```cpp
#include "input_simulation.h"

// Execute single input event
auto result = steamrot::tests::execute_input_event(input_event, fixture);

// Execute all inputs for a specific tick
auto result = steamrot::tests::execute_input_events_for_tick(input_sequence, tick, fixture);

// Execute entire input sequence
auto result = steamrot::tests::execute_input_sequence(input_sequence, fixture);
```

### Event Sequences

Event sequences allow injection of engine events into the event system on a tick-by-tick basis.

#### Test Data Format

```json
{
  "metadata": {
    "test_name": "event_simulation_test",
    "description": "Test with event sequence",
    "tags": ["unit", "event"],
    "expected_to_pass": true,
    "version": 1
  },
  "event_sequence": {
    "description": "Generate test events",
    "events": [
      {
        "tick": 0,
        "event_packet": {
          "event_lifetime": 5,
          "event_type": "EVENT_TEST",
          "event_data_data_type": "NONE"
        },
        "description": "Test event at tick 0"
      },
      {
        "tick": 1,
        "event_packet": {
          "event_lifetime": 3,
          "event_type": "EVENT_USER_INPUT",
          "event_data_data_type": "UserInputBitsetData",
          "event_data_data": {
            "keyboard_pressed": ["A", "W"],
            "mouse_pressed": ["LEFT_CLICK"]
          }
        },
        "description": "Input event at tick 1"
      }
    ]
  }
}
```

#### Execution

Event sequences are automatically executed by `run_fixture_test()` before simulations, with automatic processing of the waiting room event bus:

```cpp
TEST_CASE("Test with event sequence", "[unit][event]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Event sequence (if present) is executed automatically before simulation
  // Events are added to waiting room and processed into global event bus
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

#### Manual Event Execution

For fine-grained control:

```cpp
#include "event_simulation.h"

// Execute single event
auto result = steamrot::tests::execute_event_test_data(event_data, fixture);

// Execute all events for a specific tick  
auto result = steamrot::tests::execute_events_for_tick(event_sequence, tick, fixture);

// Execute entire event sequence
auto result = steamrot::tests::execute_event_sequence(event_sequence, fixture);

// Don't forget to process the waiting room
fixture.GetGameResources().event_handler.ProcessWaitingRoomEventBus();
```

### Combined Usage

Input sequences, event sequences, and simulations can be combined in a single test:

```json
{
  "metadata": {
    "test_name": "comprehensive_test",
    "description": "Combines input, events, and simulation",
    "tags": ["unit", "comprehensive"],
    "expected_to_pass": true,
    "version": 1
  },
  "input_sequence": {
    "description": "User interaction",
    "inputs": [...]
  },
  "event_sequence": {
    "description": "Engine events",
    "events": [...]
  },
  "simulation_data": {
    "description": "Process logic",
    "steps": [...]
  },
  "start_entity_collection": {...},
  "expected_entity_collection": {...}
}
```

**Execution order** in `run_fixture_test()`:
1. Create fixture and configure entities from `start_entity_collection`
2. Execute `input_sequence` (if present)
3. Execute `event_sequence` (if present) and process waiting room
4. Execute `simulation_data` (if present)
5. Compare with `expected_entity_collection` (if present)

### Tick-Based Execution

Both input and event sequences use a tick-based execution model:

- Each input/event has a `tick` field (default: 0)
- Events/inputs with the same tick are processed together in sequence order
- Ticks are processed in ascending order (0, 1, 2, ...)
- The caller is responsible for advancing game state between ticks if needed

This allows precise control over the timing and sequencing of inputs and events during testing.

## Integration with Matchers

The test harness is designed to work seamlessly with the existing matcher infrastructure in `tests/matchers/`. The `run_entity_memory_pool_comparison_test()` function uses the `EqualsEntityMemoryPool` matcher internally:

```cpp
#include "test_data_harness.h"

TEST_CASE("Test entity pools with matcher", "[unit]") {
  EntityMemoryPool actual_pool;
  EntityMemoryPool expected_pool;
  
  // Configure and manipulate pools...
  
  // Automatically uses EqualsEntityMemoryPool matcher for comparison
  steamrot::tests::run_entity_memory_pool_comparison_test(actual_pool, expected_pool);
}
```

## Key Features

- **Simple API**: One function call to load test data
- **Automatic Discovery**: Finds and loads all .test_data.bin files
- **Error Handling**: Uses `std::expected` for graceful error handling
- **Generator-Friendly**: Returns vectors suitable for Catch2's `from_range()`
- **Flexible**: Works with adjacent directories or specific subdirectories
- **Type-Safe**: Returns strongly-typed TestDataConfig pointers

## Migration Guide

### From test_data_generator functions

Old:
```cpp
auto test_names = steamrot::tests::get_test_names_for_generator();
auto test_name = GENERATE_COPY(from_range(test_names.value()));
steamrot::tests::TestDataLoader loader;
auto config = loader.LoadTestData(test_name, "subdirectory");
```

New:
```cpp
auto configs = steamrot::tests::load_test_data_configs();
const auto *config = GENERATE_COPY(from_range(configs.value()));
```

### From TestDataLoader

Old:
```cpp
steamrot::tests::TestDataLoader loader;
auto test_names = loader.DiscoverTestDataFiles("subdirectory");
auto configs = loader.LoadMultipleTestData(test_names.value(), "subdirectory");
```

New:
```cpp
auto configs = steamrot::tests::load_test_data_configs("subdirectory");
```

## Notes

- The harness consolidates functionality from `TestDataLoader` and `test_data_generator`
- Prioritizes simplicity - one call to get all test data
- Works with adjacent `data/` directory by default (via `__FILE__`)
- Seamlessly integrates with Catch2 generators and existing matchers
- Uses free functions following project conventions

## Test Data File Format

Test data JSON files must follow the `test_data.fbs` schema located in `src/flatbuffers_headers/test_data.fbs`.

**Required fields:**
- `metadata` (required) - Test metadata including `test_name`, `description`, `tags`, etc.

**Entity collection fields:**
- `start_entity_collection` - Starting state for comparison tests
- `expected_entity_collection` - Expected state for comparison tests

**Simulation fields:**
- `simulation_data` - Optional simulation steps to execute between start and expected states

**Example:**
```json
{
  "metadata": {
    "test_name": "my_test",
    "description": "Test description",
    "tags": ["unit"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": [...]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": [...]
  }
}
```

**Important:** The schema uses `start_entity_collection` and `expected_entity_collection`, NOT `entity_collection`. Always refer to `src/flatbuffers_headers/test_data.fbs` for the authoritative schema definition.
