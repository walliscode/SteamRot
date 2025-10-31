# Test Harness Overview - Quick Start

## What is the Test Harness?

The test harness is a **data-driven testing framework** that lets you write complex tests in JSON files instead of code. Think of it as a "test automation engine" that can:

- Load test scenarios from JSON files
- Set up game resources and entities
- Simulate user input (mouse clicks, keyboard presses)
- Inject engine events
- Execute game logic step-by-step
- Compare expected vs actual results

## Why Use It?

**Before the test harness:**
```cpp
// Test requires lots of boilerplate code
TEST_CASE("Button click test") {
  // Set up window, resources, entities... (50+ lines)
  EntityMemoryPool pool;
  // Configure entities manually... (30+ lines)
  // Simulate input... (20+ lines)
  // Run logic... (15+ lines)
  // Compare results... (10+ lines)
}
```

**With the test harness:**
```json
// test_button_click.test_data.json
{
  "metadata": {"test_name": "button_click_test"},
  "start_entity_collection": { /* entities */ },
  "input_sequence": { /* mouse clicks */ },
  "simulation_data": { /* logic to run */ },
  "expected_entity_collection": { /* expected result */ }
}
```

```cpp
// Test code is now just 5 lines!
TEST_CASE("Button click test") {
  auto configs = steamrot::tests::load_test_data_configs();
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

## How Does It Work?

### The Big Picture

```
1. You write JSON test data
   ↓
2. Build system compiles JSON → binary (FlatBuffers)
   ↓
3. Test loads binary at runtime
   ↓
4. Test harness creates game environment (TestFixture)
   ↓
5. Test harness executes your test scenario tick-by-tick
   ↓
6. Test harness compares actual vs expected results
   ↓
7. Pass or Fail (with detailed error messages)
```

### What Goes in the JSON?

A test data file can contain:

**1. Metadata** (required)
- Test name, description, tags
- Whether it should pass or fail

**2. Entity State** (optional)
- `start_entity_collection` - How entities should be set up initially
- `expected_entity_collection` - How entities should look after the test

**3. Inputs** (optional)
- `input_sequence` - Mouse moves, clicks, key presses at specific ticks
- Example: "At tick 0, move mouse to (100, 200). At tick 1, click left button."

**4. Events** (optional)
- `event_sequence` - Engine events to inject at specific ticks
- Example: "At tick 0, add a TEST_EVENT with lifetime 5."

**5. Simulation** (optional)
- `simulation_data` - Which Logic classes or functions to run
- Example: "Run UICollisionLogic, then UIActionLogic, then ProcessNestedUIActionsAndEvents"

**6. Timing** (optional)
- `num_ticks` - How many ticks to run (or auto-detect from inputs/events)

## Example: Button Click Test

Let's walk through a complete example.

### Step 1: Create Test Data JSON

```json
// tests/ui/data/button_click.test_data.json
{
  "metadata": {
    "test_name": "button_click_test",
    "description": "Click a button and verify it was clicked",
    "tags": ["unit", "ui", "button"]
  },
  
  "num_ticks": 3,
  
  "start_entity_collection": {
    "entity_memory_pool_size": 2,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "test_button",
          "start_visible": true,
          "root_ui_element": {
            "base_data": {
              "position": {"x": 100, "y": 100},
              "size": {"x": 100, "y": 50}
            }
          }
        }
      }
    ]
  },
  
  "input_sequence": {
    "inputs": [
      {
        "input_type": "MouseMove",
        "input_data": {"position": {"x": 150, "y": 125}},
        "tick": 0
      },
      {
        "input_type": "MouseClick",
        "input_data": {"position": {"x": 150, "y": 125}, "button": 0},
        "tick": 1
      }
    ]
  },
  
  "simulation_data": {
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "LogicClass",
        "logic_class_type": "UICollisionLogic"
      },
      {
        "simulation_type": "Action",
        "execution_mode": "Function",
        "function_type": "ProcessButtonElementActions"
      }
    ]
  },
  
  "expected_entity_collection": {
    "entity_memory_pool_size": 2,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "test_button",
          "start_visible": true,
          "root_ui_element": {
            "base_data": {
              "position": {"x": 100, "y": 100},
              "size": {"x": 100, "y": 50},
              "state": "Clicked"
            }
          }
        }
      }
    ]
  }
}
```

### Step 2: Build Project

```bash
cmake --build --preset Debug
```

The build system automatically:
- Finds `button_click.test_data.json`
- Compiles it to `button_click.test_data.bin` using flatc
- Makes it available for tests

### Step 3: Write Test Code

```cpp
// tests/ui/button_tests.test.cpp
#include "test_data_harness.h"
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("Button interaction tests", "[unit][ui][button]") {
  // Load all test data from adjacent data/ directory
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  // Generator runs test for each config
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Run the test - everything is automatic!
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

### Step 4: Run Tests

```bash
ctest --preset Debug -R button
```

### What Happens During Execution?

**Tick 0:**
1. Input: Mouse moves to (150, 125)
2. Simulation: UICollisionLogic checks if mouse is over button → Yes!
3. Simulation: ProcessButtonElementActions updates button state → Hovered

**Tick 1:**
1. Input: Mouse clicks at (150, 125)
2. Simulation: UICollisionLogic checks collision → Still over button
3. Simulation: ProcessButtonElementActions handles click → Clicked!

**Tick 2:**
1. Simulation: UICollisionLogic checks collision
2. Simulation: ProcessButtonElementActions (no new input)

**After all ticks:**
- Compare actual entity state with expected state
- Button should be in "Clicked" state
- If matches: Test passes ✅
- If doesn't match: Detailed error shows exactly what differs ❌

## Key Concepts

### TestFixture

A `TestFixture` is your test environment. It contains:
- **GameResources**: Window, asset manager, event handler
- **SceneResources**: Render texture, mouse position
- **EntityManager**: Entity memory pool with all your entities

The test harness creates and configures this for you automatically based on your test data.

### Tick-Based Execution

Tests run in discrete "ticks" (like game frames):

```
Tick 0: Process inputs → Process events → Run simulation
Tick 1: Process inputs → Process events → Run simulation
Tick 2: Process inputs → Process events → Run simulation
...
```

This gives you precise control over timing and sequencing.

### Simulation Steps

Simulation steps define what logic to execute. Two modes:

**Function Mode**: Call a specific free function
```json
{
  "execution_mode": "Function",
  "function_type": "ProcessNestedUIActionsAndEvents"
}
```

**LogicClass Mode**: Instantiate and run a Logic class
```json
{
  "execution_mode": "LogicClass",
  "logic_class_type": "UIActionLogic"
}
```

All simulation steps run on **every tick**.

### State Comparison

The test harness automatically compares:
- Entity pool sizes
- Component counts per entity
- Component activation states
- Component field values

If anything differs, you get a detailed error message:
```
EntityMemoryPool mismatch [Test: button_click_test]: 
Entity 0, Component CUserInterface, Field 'state' differs:
  Expected: "Clicked"
  Actual: "Hovered"
```

## Common Workflows

### Testing Entity Configuration

```json
{
  "metadata": {"test_name": "entity_config_test"},
  "start_entity_collection": { /* your entities */ },
  "expected_entity_collection": { /* same entities */ }
}
```

No simulation, just validates entity configuration works correctly.

### Testing Logic Execution

```json
{
  "metadata": {"test_name": "logic_test"},
  "start_entity_collection": { /* initial state */ },
  "simulation_data": { /* logic to run */ },
  "expected_entity_collection": { /* expected result */ }
}
```

### Testing User Interaction

```json
{
  "metadata": {"test_name": "interaction_test"},
  "start_entity_collection": { /* UI entities */ },
  "input_sequence": { /* user actions */ },
  "simulation_data": { /* UI logic */ },
  "expected_entity_collection": { /* updated state */ }
}
```

### Testing Event Processing

```json
{
  "metadata": {"test_name": "event_test"},
  "event_sequence": { /* events to inject */ },
  "simulation_data": { /* event processing logic */ },
  "expected_entity_collection": { /* result */ }
}
```

## API Quick Reference

### Main Entry Point

```cpp
auto result = steamrot::tests::run_fixture_test(config);
```

This function:
1. Creates TestFixture
2. Configures entities from `start_entity_collection`
3. Executes tick-based test (inputs, events, simulation)
4. Compares with `expected_entity_collection`
5. Returns success or error

### Loading Test Data

```cpp
// From adjacent data/ directory
auto configs = steamrot::tests::load_test_data_configs();

// From specific subdirectory
auto configs = steamrot::tests::load_test_data_configs("entity");
```

### Manual Control

```cpp
// Create fixture manually
auto fixture = steamrot::tests::create_fixture_from_test_data(config);

// Execute simulation manually
steamrot::tests::execute_simulation_with_fixture(simulation_data, fixture);

// Compare pools manually
steamrot::tests::run_entity_memory_pool_comparison_test(actual, expected);
```

## Where to Learn More

- **Complete architecture**: [TEST_HARNESS_DATA_FLOW.md](TEST_HARNESS_DATA_FLOW.md)
  - Detailed component interactions
  - Complete function call hierarchies
  - Extension points

- **Data-driven testing guide**: [TEST_DATA_CONFIGURATION.md](TEST_DATA_CONFIGURATION.md)
  - Creating test data files
  - Available input types, event types, Logic classes
  - Schema reference

- **API reference**: [tests/harness/README.md](../tests/harness/README.md)
  - All available functions
  - Usage examples
  - Best practices

## Tips and Tricks

### Use Descriptive Test Names

```json
// Good
"test_name": "button_click_changes_state_to_clicked"

// Not as good
"test_name": "test1"
```

### Start Simple

Begin with just entity configuration, then add:
1. Simulation steps
2. Input sequences
3. Event sequences
4. Multiple ticks

### Use Catch2 Generators

```cpp
const auto *config = GENERATE_COPY(from_range(configs.value()));
```

This runs your test code once per test data file - perfect for parameterized testing!

### Check Test Metadata in Error Messages

The test harness includes test name and description in error messages, making it easy to identify which test failed when running many data-driven tests.

### Leverage Auto-Tick Detection

Omit `num_ticks` to let the harness auto-detect from your inputs and events:

```json
{
  "input_sequence": {
    "inputs": [
      {"tick": 0, ...},
      {"tick": 3, ...},
      {"tick": 7, ...}
    ]
  }
}
// Will automatically run for 8 ticks (0-7, plus one more)
```

## Summary

The test harness is a powerful system that:

✅ **Reduces boilerplate** - Write JSON instead of repetitive test code
✅ **Increases clarity** - Test scenarios are declarative and readable
✅ **Enables complex tests** - Coordinate input, events, and logic precisely
✅ **Provides detailed feedback** - Know exactly what went wrong
✅ **Supports rapid iteration** - Add new tests without code changes
✅ **Integrates seamlessly** - Works with Catch2 and existing infrastructure

Start with simple entity configuration tests, then gradually explore input simulation, event injection, and complex multi-tick scenarios. The system grows with your testing needs!
