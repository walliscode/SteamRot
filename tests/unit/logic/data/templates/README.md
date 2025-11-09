# Logic Test Data Templates

## Overview

This directory contains template test data files for migrating logic unit tests to the test harness system. Use these as starting points for creating new data-driven logic tests.

## Available Templates

### 1. Simple Logic Test (`simple_logic_test.template.json`)

**Use for:** Basic tests that execute a single Logic class

**Pattern:**
- Single entity with UI component
- One simulation step (single Logic class)
- Direct state comparison

**Good for:**
- Constructor tests
- Basic collision detection
- Simple rendering
- Single action processing

**Example scenarios:**
- Test UICollisionLogic detects mouse over element
- Test UIRenderLogic draws a single UI element
- Test UIActionLogic processes a button click

### 2. Multi-Step Workflow (`multi_step_workflow.template.json`)

**Use for:** Tests requiring multiple Logic classes in sequence

**Pattern:**
- Button or interactive UI element
- Multiple simulation steps (Logic classes and/or functions)
- Tests Logic interactions

**Good for:**
- Complete UI workflows
- Testing Logic order dependencies
- Integration-like scenarios
- Multi-step processes

**Example scenarios:**
- Button click workflow: collision → render → action
- Dropdown interaction: collision → state → render → action
- Complex UI update cycle

### 3. With Input Simulation (`with_input_simulation.template.json`)

**Use for:** Tests requiring user input simulation (mouse/keyboard)

**Pattern:**
- Tick-based execution (`num_ticks`)
- Input sequence with mouse/keyboard events
- Simulation runs on each tick
- Precise timing control

**Good for:**
- Mouse interaction tests
- Keyboard input tests
- Click-drag scenarios
- Multi-frame interactions

**Example scenarios:**
- Mouse moves over button then clicks
- Keyboard input followed by processing
- Drag-and-drop simulation
- Complex user interaction sequences

## How to Use Templates

### Step 1: Choose Template

Pick the template that matches your test scenario:
- **Simple** → Single Logic class execution
- **Multi-step** → Multiple Logic classes/functions in sequence
- **Input** → Need mouse/keyboard simulation

### Step 2: Copy and Rename

```bash
cp simple_logic_test.template.json ../ui_collision/my_test.test_data.json
```

**Naming convention:** `{logic_class}_{scenario}.test_data.json`

Examples:
- `ui_collision_basic.test_data.json`
- `ui_action_button_click.test_data.json`
- `workflow_complete_cycle.test_data.json`

### Step 3: Customize Metadata

Update the metadata section:

```json
{
  "metadata": {
    "test_name": "ui_collision_basic",  // Match filename
    "description": "Test basic collision detection",
    "tags": ["unit", "logic", "UICollisionLogic"],  // Relevant tags
    "expected_to_pass": true,  // true for positive tests
    "author": "Your Name",
    "version": 1
  }
}
```

### Step 4: Configure Entities

Modify `start_entity_collection` with your test entities:

```json
{
  "start_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "my_test_ui",
          // Configure your UI element here
        }
      }
    ]
  }
}
```

### Step 5: Configure Simulation

Update simulation steps to match your test:

```json
{
  "simulation_data": {
    "description": "What this simulation does",
    "steps": [
      {
        "simulation_type": "Collision",  // Action, Movement, Render, Collision
        "execution_mode": "LogicClass",  // LogicClass or Function
        "logic_class_type": "UICollisionLogic",  // Which Logic class
        "description": "What this step does"
      }
    ]
  }
}
```

**Available Logic Classes:**
- `UICollisionLogic`
- `UIActionLogic`
- `UIRenderLogic`
- `UIStateLogic`
- `CraftingRenderLogic`

**Available Functions:**
- `ProcessUIActionsAndEvents`
- `ProcessNestedUIActionsAndEvents`
- `ProcessButtonElementActions`
- `ProcessDropDownListElementActions`

### Step 6: Set Expected State

Define the expected state after simulation:

```json
{
  "expected_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          // Expected state after simulation
        }
      }
    ]
  }
}
```

**Tip:** Start by copying `start_entity_collection` and modifying the fields that should change.

### Step 7: Add Input/Events (Optional)

For input simulation template, configure input sequence:

```json
{
  "num_ticks": 3,  // How many ticks to run
  "input_sequence": {
    "inputs": [
      {
        "input_type": "MouseMove",  // MouseMove, MouseClick, MouseRelease, KeyPress, KeyRelease
        "input_data_type": "MouseInputData",
        "input_data": {
          "position": { "x": 150.0, "y": 125.0 },
          "button": 0
        },
        "tick": 0,  // Which tick to execute on
        "description": "What this input does"
      }
    ]
  }
}
```

### Step 8: Build and Test

```bash
# Build project (compiles JSON to binary)
cmake --build --preset Debug

# Run tests
ctest --preset Debug -R logic
```

## Template Customization Guide

### Entity Types

**UI Panel:**
```json
{
  "c_user_interface": {
    "ui_name": "panel",
    "start_visible": true,
    "root_ui_element": {
      "base_data": {
        "position": { "x": 100, "y": 100 },
        "size": { "x": 200, "y": 150 }
      }
    }
  }
}
```

**Button:**
```json
{
  "c_user_interface": {
    "root_ui_element": {
      "button_data": {
        "position": { "x": 100, "y": 100 },
        "size": { "x": 100, "y": 50 },
        "label": "Button Text",
        "is_mouse_over": false
      }
    }
  }
}
```

**Multiple Entities:**
```json
{
  "entities": [
    {"index": 0, "c_user_interface": { /* ... */ }},
    {"index": 1, "c_user_interface": { /* ... */ }},
    {"index": 2, "c_user_interface": { /* ... */ }}
  ]
}
```

### Simulation Patterns

**Single Logic:**
```json
{
  "steps": [
    {"simulation_type": "Collision", "logic_class_type": "UICollisionLogic"}
  ]
}
```

**Logic Sequence:**
```json
{
  "steps": [
    {"simulation_type": "Collision", "logic_class_type": "UICollisionLogic"},
    {"simulation_type": "Action", "logic_class_type": "UIActionLogic"}
  ]
}
```

**Function Call:**
```json
{
  "steps": [
    {
      "simulation_type": "Action",
      "execution_mode": "Function",
      "function_type": "ProcessButtonElementActions"
    }
  ]
}
```

**Mixed:**
```json
{
  "steps": [
    {"simulation_type": "Collision", "logic_class_type": "UICollisionLogic"},
    {"simulation_type": "Action", "execution_mode": "Function", 
     "function_type": "ProcessNestedUIActionsAndEvents"},
    {"simulation_type": "Render", "logic_class_type": "UIRenderLogic"}
  ]
}
```

### Input Types

**Mouse Movement:**
```json
{
  "input_type": "MouseMove",
  "input_data_type": "MouseInputData",
  "input_data": {
    "position": { "x": 150.0, "y": 125.0 },
    "button": 0
  },
  "tick": 0
}
```

**Mouse Click:**
```json
{
  "input_type": "MouseClick",
  "input_data_type": "MouseInputData",
  "input_data": {
    "position": { "x": 150.0, "y": 125.0 },
    "button": 0  // 0=left, 1=right, 2=middle
  },
  "tick": 1
}
```

**Keyboard:**
```json
{
  "input_type": "KeyPress",
  "input_data_type": "KeyboardInputData",
  "input_data": {
    "key_code": 0,  // SFML key code
    "alt": false,
    "control": false,
    "shift": false
  },
  "tick": 0
}
```

## Common Patterns

### Pattern: Basic Collision Detection

```json
{
  "metadata": {"test_name": "collision_basic", "tags": ["unit", "collision"]},
  "start_entity_collection": {/* UI element at position */},
  "simulation_data": {
    "steps": [{"simulation_type": "Collision", "logic_class_type": "UICollisionLogic"}]
  },
  "expected_entity_collection": {/* is_mouse_over = true */}
}
```

### Pattern: Button Click Workflow

```json
{
  "metadata": {"test_name": "button_click", "tags": ["unit", "workflow"]},
  "start_entity_collection": {/* Button element */},
  "simulation_data": {
    "steps": [
      {"simulation_type": "Collision", "logic_class_type": "UICollisionLogic"},
      {"execution_mode": "Function", "function_type": "ProcessButtonElementActions"}
    ]
  },
  "expected_entity_collection": {/* Button clicked state */}
}
```

### Pattern: Input-Driven Test

```json
{
  "metadata": {"test_name": "mouse_interaction", "tags": ["unit", "input"]},
  "num_ticks": 2,
  "input_sequence": {
    "inputs": [
      {"input_type": "MouseMove", "tick": 0, /* position */},
      {"input_type": "MouseClick", "tick": 1, /* position */}
    ]
  },
  "simulation_data": {
    "steps": [{"simulation_type": "Collision", "logic_class_type": "UICollisionLogic"}]
  }
}
```

## Tips

1. **Start simple** - Begin with `simple_logic_test.template.json` for your first migration
2. **Copy working tests** - Once you have a working test, use it as a template
3. **Use descriptive names** - Make test names and descriptions clear
4. **Validate incrementally** - Build and test after each change
5. **Check entity state** - Use `expected_entity_collection` to validate results
6. **Reuse entities** - Create common entity configurations and reuse them
7. **Document changes** - Add clear descriptions for each simulation step

## Testing Your Test Data

```cpp
TEST_CASE("My logic tests", "[unit][logic][data-driven]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  INFO("Running: " << config->metadata()->test_name()->str());
  
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

## References

- [Full refactoring plan](../../documentation/LOGIC_TEST_REFACTORING_PLAN.md)
- [Test harness README](../../harness/README.md)
- [Test data configuration docs](../../documentation/TEST_DATA_CONFIGURATION.md)

## Questions?

See the full documentation:
- **Detailed plan:** `documentation/LOGIC_TEST_REFACTORING_PLAN.md`
- **Quick summary:** `documentation/LOGIC_TEST_REFACTORING_SUMMARY.md`
- **Test harness API:** `tests/harness/README.md`
