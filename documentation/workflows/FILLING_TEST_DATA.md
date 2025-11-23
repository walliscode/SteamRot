# Filling Out test_data.json Files - Workflow Guide

<!--toc:start-->

- [Filling Out test_data.json Files - Workflow Guide](#filling-out-testdatajson-files-workflow-guide)
  - [What is test_data.json?](#what-is-testdatajson)
  - [Basic Workflow](#basic-workflow)
    - [Step 1: Decide on Test Scenario](#step-1-decide-on-test-scenario)
    - [Step 2: Create JSON File](#step-2-create-json-file)
    - [Step 3: Fill Out Required Fields](#step-3-fill-out-required-fields)
    - [Step 4: Add Test-Specific Data](#step-4-add-test-specific-data)
    - [Step 5: Build and Test](#step-5-build-and-test)
  - [Testing Workflows by Scenario](#testing-workflows-by-scenario)
    - [Workflow 1: Metadata-Only Test](#workflow-1-metadata-only-test)
    - [Workflow 2: Simple Entity Comparison](#workflow-2-simple-entity-comparison)
    - [Workflow 3: Entity State Transformation](#workflow-3-entity-state-transformation)
    - [Workflow 4: Simulation with Logic Classes](#workflow-4-simulation-with-logic-classes)
    - [Workflow 5: Simulation with Free Functions](#workflow-5-simulation-with-free-functions)
    - [Workflow 6: Input Sequence Testing](#workflow-6-input-sequence-testing)
    - [Workflow 7: Event Sequence Testing](#workflow-7-event-sequence-testing)
    - [Workflow 8: Combined Testing (Input + Events + Simulation)](#workflow-8-combined-testing-input-events-simulation)
    - [Workflow 9: Negative Testing (Expected to Fail)](#workflow-9-negative-testing-expected-to-fail)
  - [Field Reference](#field-reference)
    - [metadata (Required)](#metadata-required)
    - [num_ticks (Optional)](#numticks-optional)
    - [start_entity_collection (Optional)](#startentitycollection-optional)
    - [expected_entity_collection (Optional)](#expectedentitycollection-optional)
    - [simulation_data (Optional)](#simulationdata-optional)
    - [input_sequence (Optional)](#inputsequence-optional)
    - [event_sequence (Optional)](#eventsequence-optional)
  - [Component Data Structures](#component-data-structures)
    - [CUserInterface Component](#cuserinterface-component)
    - [CGrimoireMachina Component](#cgrimoiremachina-component)
  - [Tips and Best Practices](#tips-and-best-practices)
    - [Organizing Test Data](#organizing-test-data)
    - [Writing Entity Collections](#writing-entity-collections)
    - [Working with Simulation Steps](#working-with-simulation-steps)
    - [Tick-Based Testing](#tick-based-testing)
    - [Using expected_to_pass](#using-expectedtopass)
  - [Troubleshooting](#troubleshooting)
    - [Common Issues](#common-issues)
      - [1. File Not Found Error](#1-file-not-found-error)
      - [2. JSON Parse Error](#2-json-parse-error)
      - [3. Pool Size Mismatch](#3-pool-size-mismatch)
      - [4. Component Not Found](#4-component-not-found)
      - [5. Simulation Step Failed](#5-simulation-step-failed)
      - [6. Unexpected Test Result](#6-unexpected-test-result)
    - [Debugging Tips](#debugging-tips)
  - [Example Test Data Files](#example-test-data-files)
    - [Example 1: Simple Component Test](#example-1-simple-component-test)
    - [Example 2: Multi-Tick Input Test](#example-2-multi-tick-input-test)
  - [Additional Resources](#additional-resources)
  - [Quick Reference Card](#quick-reference-card)
  <!--toc:end-->

[← Back to Documentation](../README.md) |
[Testing Overview](../testing/TESTING_OVERVIEW.md) |
[Test Data Configuration Reference](../testing/TEST_DATA_CONFIGURATION.md) |
[Test Data Naming Conventions](../testing/TEST_DATA_NAMING_CONVENTIONS.md)

This guide provides practical workflows for creating test_data.json files for
different testing scenarios in SteamRot.

---

## What is test_data.json?

Test data files (`*.test_data.json`) are JSON files that define test scenarios
for data-driven testing. They allow you to:

- Define test cases without writing C++ code
- Specify entity states, inputs, events, and simulation steps
- Run complex multi-step test scenarios
- Easily add new test cases by creating new JSON files

**Location**: Place test data files in `tests/<test_executable_dir>/data/`
directories.

**Naming**: Follow the hybrid naming conventions defined in
[Test Data Naming Conventions](../testing/TEST_DATA_NAMING_CONVENTIONS.md):

- **Unit tests**: Descriptive short names (e.g.,
  `ui_collision_basic.test_data.json`)
- **Integration tests**: ID-based (e.g., `ui_workflow_001.test_data.json`)
- **System tests**: Scenario-based (e.g., `crafting_success_001.test_data.json`)

**Compilation**: JSON files are automatically compiled to `.test_data.bin`
during build.

---

## Basic Workflow

### Step 1: Decide on Test Scenario

Ask yourself:

- What am I testing? (component, logic, workflow, integration)
- Do I need to compare entity states? (start vs expected)
- Do I need to simulate user inputs?
- Do I need to inject events?
- Do I need to run Logic classes or functions?
- How many game ticks should the test run?

### Step 2: Create JSON File

Create a new file in the appropriate `data/` directory:

```bash
# Component test
tests/unit/components/data/my_test.test_data.json

# Logic test
tests/unit/logic/data/my_test.test_data.json

# Integration test
tests/integration/my_feature/data/my_test.test_data.json
```

### Step 3: Fill Out Required Fields

At minimum, every test data file needs metadata:

```json
{
  "metadata": {
    "test_name": "descriptive_test_name",
    "description": "What this test validates",
    "tags": ["unit", "component"],
    "expected_to_pass": true,
    "version": 1
  }
}
```

### Step 4: Add Test-Specific Data

Add additional fields based on your test scenario (see
[Testing Workflows](#testing-workflows-by-scenario)).

### Step 5: Build and Test

```bash
# Build project (compiles JSON to binary)
cmake --build --preset Debug

# Run tests
ctest --preset Debug -R my_test
```

---

## Testing Workflows by Scenario

### Workflow 1: Metadata-Only Test

**When to use:** Testing infrastructure, validation logic, or configuration
without entities.

**Example:**

```json
{
  "metadata": {
    "test_name": "validation_test",
    "description": "Test that validation detects invalid configurations",
    "tags": ["unit", "infrastructure"],
    "expected_to_pass": true,
    "version": 1
  }
}
```

**Fields used:**

- `metadata` (required)

**Use case:** Testing that your test harness correctly loads and validates
configurations.

---

### Workflow 2: Simple Entity Comparison

**When to use:** Testing that entities start and end in specific states (no
simulation).

**Example:**

```json
{
  "metadata": {
    "test_name": "entity_default_state",
    "description": "Verify entity has correct default state",
    "tags": ["unit", "entity"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "test_ui",
          "is_visible": false,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 100, "y": 200 },
              "size": { "x": 50, "y": 30 },
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
  "expected_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "test_ui",
          "is_visible": false,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 100, "y": 200 },
              "size": { "x": 50, "y": 30 },
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
```

**Fields used:**

- `metadata` (required)
- `start_entity_collection` - Initial entity state
- `expected_entity_collection` - Expected entity state after test
- `num_ticks` (optional, defaults to 1)

**Use case:** Verifying entity configuration, component default values, or that
entities remain unchanged when they should.

---

### Workflow 3: Entity State Transformation

**When to use:** Testing that entities change from one state to another (with
simulation).

**Example:**

```json
{
  "metadata": {
    "test_name": "ui_visibility_toggle",
    "description": "Test that UI visibility toggles when clicked",
    "tags": ["integration", "ui", "simulation"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "toggle_button",
          "is_visible": false,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 100, "y": 100 },
              "size": { "x": 100, "y": 50 },
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
  "simulation_data": {
    "description": "Simulate UI interaction to toggle visibility",
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "LogicClass",
        "logic_class_type": "UICollisionLogic",
        "description": "Detect mouse collision with button"
      },
      {
        "simulation_type": "Action",
        "execution_mode": "LogicClass",
        "logic_class_type": "UIActionLogic",
        "description": "Process button action to toggle visibility"
      }
    ]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "toggle_button",
          "is_visible": true,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 100, "y": 100 },
              "size": { "x": 100, "y": 50 },
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
```

**Fields used:**

- `metadata` (required)
- `start_entity_collection` - Initial state
- `simulation_data` - Steps to execute
- `expected_entity_collection` - Expected state after simulation
- `num_ticks` (optional, defaults to 1)

**Use case:** Testing that Logic classes or functions correctly transform entity
state.

---

### Workflow 4: Simulation with Logic Classes

**When to use:** Testing complete workflows that require multiple Logic classes
to execute.

**Example:**

```json
{
  "metadata": {
    "test_name": "complete_ui_workflow",
    "description": "Test complete UI interaction: collision -> render -> action",
    "tags": ["integration", "simulation", "ui"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "main_menu",
          "is_visible": true,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 100, "y": 100 },
              "size": { "x": 300, "y": 400 },
              "children_active": true,
              "children": [],
              "layout": "Vertical",
              "spacing_strategy": "Even"
            }
          }
        }
      }
    ]
  },
  "simulation_data": {
    "description": "Complete UI interaction workflow",
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "LogicClass",
        "logic_class_type": "UICollisionLogic",
        "description": "Step 1: Detect mouse collision"
      },
      {
        "simulation_type": "Action",
        "execution_mode": "LogicClass",
        "logic_class_type": "UIStateLogic",
        "description": "Step 2: Update UI element states"
      },
      {
        "simulation_type": "Render",
        "execution_mode": "LogicClass",
        "logic_class_type": "UIRenderLogic",
        "description": "Step 3: Render UI elements"
      },
      {
        "simulation_type": "Action",
        "execution_mode": "LogicClass",
        "logic_class_type": "UIActionLogic",
        "description": "Step 4: Process UI actions"
      }
    ]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "main_menu",
          "is_visible": true,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 100, "y": 100 },
              "size": { "x": 300, "y": 400 },
              "children_active": true,
              "children": [],
              "layout": "Vertical",
              "spacing_strategy": "Even"
            }
          }
        }
      }
    ]
  }
}
```

**Fields used:**

- `metadata` (required)
- `start_entity_collection`
- `simulation_data` with `execution_mode: "LogicClass"`
- `expected_entity_collection`
- `num_ticks` (optional, defaults to 1)

**Available Logic Classes:**

- `UIActionLogic` - Process UI actions and events
- `UICollisionLogic` - Check UI collision with mouse
- `UIRenderLogic` - Render UI elements
- `UIStateLogic` - Update UI state
- `CraftingRenderLogic` - Render crafting UI

**Use case:** Integration testing of complete system workflows.

---

### Workflow 5: Simulation with Free Functions

**When to use:** Testing specific free functions in isolation or mixed with
Logic classes.

**Example:**

```json
{
  "metadata": {
    "test_name": "nested_ui_action_processing",
    "description": "Test nested UI action processing function",
    "tags": ["unit", "simulation", "ui", "function"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "nested_panel",
          "is_visible": true,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 0, "y": 0 },
              "size": { "x": 200, "y": 200 },
              "children_active": true,
              "children": [],
              "layout": "Vertical",
              "spacing_strategy": "Even"
            }
          }
        }
      }
    ]
  },
  "simulation_data": {
    "description": "Process UI actions recursively for nested elements",
    "steps": [
      {
        "simulation_type": "Action",
        "execution_mode": "Function",
        "function_type": "ProcessNestedUIActionsAndEvents",
        "description": "Process nested UI actions"
      }
    ]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "nested_panel",
          "is_visible": true,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 0, "y": 0 },
              "size": { "x": 200, "y": 200 },
              "children_active": true,
              "children": [],
              "layout": "Vertical",
              "spacing_strategy": "Even"
            }
          }
        }
      }
    ]
  }
}
```

**Fields used:**

- `metadata` (required)
- `start_entity_collection`
- `simulation_data` with `execution_mode: "Function"`
- `expected_entity_collection`
- `num_ticks` (optional, defaults to 1)

**Available Free Functions:**

- `ProcessUIActionsAndEvents` - Process UI actions for elements
- `ProcessNestedUIActionsAndEvents` - Process UI actions recursively
- `ProcessButtonElementActions` - Process button-specific actions
- `ProcessDropDownListElementActions` - Process dropdown actions

**Use case:** Unit testing of specific free functions or mixing function calls
with Logic class execution.

---

### Workflow 6: Input Sequence Testing

**When to use:** Testing behavior with simulated user input over multiple ticks.

**Example:**

```json
{
  "metadata": {
    "test_name": "mouse_movement_and_click",
    "description": "Test mouse movement and button click sequence",
    "tags": ["integration", "input", "ui"],
    "expected_to_pass": true,
    "version": 1
  },
  "num_ticks": 3,
  "input_sequence": {
    "description": "Move mouse and click sequence",
    "inputs": [
      {
        "input_type": "MouseMove",
        "input_data_type": "MouseInputData",
        "input_data": {
          "position": { "x": 100.0, "y": 100.0 },
          "button": 0
        },
        "tick": 1,
        "description": "Initial mouse position"
      },
      {
        "input_type": "MouseMove",
        "input_data_type": "MouseInputData",
        "input_data": {
          "position": { "x": 150.0, "y": 125.0 },
          "button": 0
        },
        "tick": 1,
        "description": "Move mouse to button center"
      },
      {
        "input_type": "MouseClick",
        "input_data_type": "MouseInputData",
        "input_data": {
          "position": { "x": 150.0, "y": 125.0 },
          "button": 0
        },
        "tick": 2,
        "description": "Click left mouse button"
      }
    ]
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 2,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "button_ui",
          "is_visible": true,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 100, "y": 100 },
              "size": { "x": 100, "y": 50 },
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
```

**Fields used:**

- `metadata` (required)
- `num_ticks` - Total ticks to run
- `input_sequence` - Tick-by-tick input injection
- `start_entity_collection`
- `expected_entity_collection` (optional)

**Input Types:**

- `MouseMove` - Mouse movement
- `MouseClick` - Mouse button click
- `KeyPress` - Keyboard key press
- `KeyRelease` - Keyboard key release

**Use case:** Testing user input handling, mouse interactions, keyboard
controls.

---

### Workflow 7: Event Sequence Testing

**When to use:** Testing behavior with events injected at specific ticks.

**Example:**

```json
{
  "metadata": {
    "test_name": "multi_tick_event_processing",
    "description": "Test event processing over multiple ticks",
    "tags": ["integration", "event"],
    "expected_to_pass": true,
    "version": 1
  },
  "num_ticks": 3,
  "event_sequence": {
    "description": "Sequence of test events added on different ticks",
    "events": [
      {
        "tick": 1,
        "event_packet": {
          "event_lifetime": 5,
          "event_type": "EVENT_TEST",
          "event_data_data_type": "NONE"
        },
        "description": "Initial test event at tick 1"
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
        "description": "User input event at tick 1"
      },
      {
        "tick": 2,
        "event_packet": {
          "event_lifetime": 1,
          "event_type": "EVENT_TEST",
          "event_data_data_type": "NONE"
        },
        "description": "Another test event at tick 2"
      }
    ]
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 1,
    "entities": []
  }
}
```

**Fields used:**

- `metadata` (required)
- `num_ticks` - Total ticks to run
- `event_sequence` - Tick-by-tick event injection
- `start_entity_collection`
- `expected_entity_collection` (optional)

**Event Types:**

- `EVENT_TEST` - Test events
- `EVENT_USER_INPUT` - User input events
- `EVENT_SCENE_CHANGE` - Scene change events
- Other event types defined in your system

**Use case:** Testing event handling, event-driven behaviors, system responses
to events.

---

### Workflow 8: Combined Testing (Input + Events + Simulation)

**When to use:** Testing complex scenarios with coordinated inputs, events, and
simulation.

**Example:**

```json
{
  "metadata": {
    "test_name": "complete_interaction_test",
    "description": "Test complete user interaction with inputs, events, and simulation",
    "tags": ["integration", "simulation", "input", "event"],
    "expected_to_pass": true,
    "version": 1
  },
  "num_ticks": 5,
  "input_sequence": {
    "description": "Mouse movement over 5 ticks",
    "inputs": [
      {
        "input_type": "MouseMove",
        "input_data_type": "MouseInputData",
        "input_data": {
          "position": { "x": 100.0, "y": 100.0 },
          "button": 0
        },
        "tick": 1,
        "description": "Start position"
      },
      {
        "input_type": "MouseClick",
        "input_data_type": "MouseInputData",
        "input_data": {
          "position": { "x": 150.0, "y": 125.0 },
          "button": 0
        },
        "tick": 2,
        "description": "Click button"
      }
    ]
  },
  "event_sequence": {
    "description": "Events triggered at specific ticks",
    "events": [
      {
        "tick": 1,
        "event_packet": {
          "event_lifetime": 5,
          "event_type": "EVENT_TEST",
          "event_data_data_type": "NONE"
        },
        "description": "Start event"
      }
    ]
  },
  "simulation_data": {
    "description": "Simulation steps executed on every tick",
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "LogicClass",
        "logic_class_type": "UICollisionLogic",
        "description": "Collision check (runs every tick)"
      },
      {
        "simulation_type": "Action",
        "execution_mode": "Function",
        "function_type": "ProcessButtonElementActions",
        "description": "Process actions (runs every tick)"
      }
    ]
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 2,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "button_ui",
          "is_visible": true,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 100, "y": 100 },
              "size": { "x": 100, "y": 50 },
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
  "expected_entity_collection": {
    "entity_memory_pool_size": 2,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "button_ui",
          "is_visible": true,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 100, "y": 100 },
              "size": { "x": 100, "y": 50 },
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
```

**Fields used:**

- `metadata` (required)
- `num_ticks` - Total ticks to run
- `input_sequence` - Inputs injected tick-by-tick
- `event_sequence` - Events injected tick-by-tick
- `simulation_data` - Steps executed every tick
- `start_entity_collection`
- `expected_entity_collection`

**Execution order per tick:**

1. Inject scheduled inputs for this tick
2. Inject scheduled events for this tick
3. Execute all simulation steps
4. Advance to next tick

**Use case:** Comprehensive integration testing, end-to-end workflows, realistic
gameplay scenarios.

---

### Workflow 9: Negative Testing (Expected to Fail)

**When to use:** Testing that validation detects incorrect states or mismatches.

**Example:**

```json
{
  "metadata": {
    "test_name": "validation_detects_mismatch",
    "description": "Verify validation detects when entity states don't match",
    "tags": ["unit", "validation", "negative"],
    "expected_to_pass": false,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "original_ui",
          "is_visible": true,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 100, "y": 200 },
              "size": { "x": 50, "y": 30 },
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
  "expected_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "different_ui",
          "is_visible": false,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 200, "y": 300 },
              "size": { "x": 100, "y": 60 },
              "children_active": false,
              "children": [],
              "layout": "Vertical",
              "spacing_strategy": "None"
            }
          }
        }
      }
    ]
  }
}
```

**Key difference:**

- `expected_to_pass: false` - Test expects pools to be **different**
- If pools match when they shouldn't, test fails

**Use case:** Testing error detection, validation logic, negative scenarios.

---

## Field Reference

### metadata (Required)

```json
"metadata": {
  "test_name": "string",        // Required: Human-readable test name
  "description": "string",       // Optional: What this test validates
  "tags": ["string"],           // Optional: Tags for categorization
  "expected_to_pass": bool,     // Optional: true (default) or false
  "author": "string",           // Optional: Test author
  "version": number             // Optional: Data format version (default: 1)
}
```

**expected_to_pass:**

- `true` (default): Test expects actual and expected entity pools to **match**
- `false`: Test expects actual and expected entity pools to **NOT match**

### num_ticks (Optional)

```json
"num_ticks": 5
```

- Specifies how many game ticks the test runs
- Default: 1 tick if not specified
- Inputs, events, and simulation steps are processed each tick

### start_entity_collection (Optional)

```json
"start_entity_collection": {
  "entity_memory_pool_size": 10,
  "entities": [
    {
      "index": 0,
      "c_user_interface": { /* component data */ },
      "c_grimoire_machina": { /* component data */ }
    }
  ]
}
```

- Initial entity state before test runs
- If omitted, uses default-constructed EntityMemoryPool
- Must specify `entity_memory_pool_size`
- Each entity at a specific `index` can have multiple components

### expected_entity_collection (Optional)

```json
"expected_entity_collection": {
  "entity_memory_pool_size": 10,
  "entities": [
    {
      "index": 0,
      "c_user_interface": { /* component data */ }
    }
  ]
}
```

- Expected entity state after test completes
- Used for comparison with actual state
- Pool size must match `start_entity_collection`
- Only specify entities and components you want to validate

### simulation_data (Optional)

```json
"simulation_data": {
  "description": "What this simulation does",
  "steps": [
    {
      "simulation_type": "Action|Movement|Render|Collision",
      "execution_mode": "LogicClass|Function",
      "logic_class_type": "ClassName",       // If execution_mode is LogicClass
      "function_type": "FunctionName",       // If execution_mode is Function
      "description": "What this step does"
    }
  ]
}
```

**simulation_type values:**

- `Action` - UI actions, input processing
- `Movement` - Entity movement, physics
- `Render` - Drawing and rendering
- `Collision` - Collision detection

**execution_mode values:**

- `LogicClass` - Execute entire Logic class
- `Function` - Execute individual free function

**Steps execute in order, every tick.**

### input_sequence (Optional)

```json
"input_sequence": {
  "description": "Input sequence description",
  "inputs": [
    {
      "input_type": "MouseMove|MouseClick|KeyPress|KeyRelease",
      "input_data_type": "MouseInputData|KeyboardInputData",
      "input_data": {
        "position": { "x": 100.0, "y": 100.0 },  // For mouse inputs
        "button": 0                               // For mouse inputs
      },
      "tick": 1,
      "description": "What this input does"
    }
  ]
}
```

- Inputs are injected at the specified tick
- Multiple inputs can be scheduled for the same tick
- Inputs execute before simulation steps on each tick

### event_sequence (Optional)

```json
"event_sequence": {
  "description": "Event sequence description",
  "events": [
    {
      "tick": 1,
      "event_packet": {
        "event_lifetime": 5,
        "event_type": "EVENT_TEST|EVENT_USER_INPUT|EVENT_SCENE_CHANGE",
        "event_data_data_type": "NONE|UserInputBitsetData|...",
        "event_data_data": { /* event-specific data */ }
      },
      "description": "What this event does"
    }
  ]
}
```

- Events are injected at the specified tick
- Multiple events can be scheduled for the same tick
- Events execute before simulation steps on each tick
- `event_lifetime` controls how many ticks the event persists

---

## Component Data Structures

### CUserInterface Component

```json
"c_user_interface": {
  "ui_name": "string",
  "is_visible": bool,
  "root_ui_element": {
    "base_data": {
      "position": { "x": float, "y": float },
      "size": { "x": float, "y": float },
      "children_active": bool,
      "children": [],
      "layout": "Horizontal|Vertical|Grid|None",
      "spacing_strategy": "None|Even|Fixed"
    }
  }
}
```

### CGrimoireMachina Component

```json
"c_grimoire_machina": {
  "machina_form": "Grimoire|Spell|Item",
  "machina_name": "string",
  "description": "string"
}
```

**Note:** See `src/flatbuffers_headers/entities.fbs` for complete component
schemas.

---

## Tips and Best Practices

### Organizing Test Data

1. **Use descriptive names:**
   - ✅ `ui_button_click_toggles_visibility.test_data.json`
   - ❌ `test1.test_data.json`

2. **Place in correct directory:**
   - Component tests: `tests/unit/components/data/`
   - Logic tests: `tests/unit/logic/data/`
   - Integration tests: `tests/integration/<feature>/data/`

3. **One scenario per file:**
   - Each file should test one specific behavior
   - Keep files focused and maintainable

### Writing Entity Collections

1. **Specify pool size:**
   - Always set `entity_memory_pool_size`
   - Start and expected pool sizes must match

2. **Only specify what matters:**
   - Only include entities you're testing
   - Only include components that are relevant
   - Only include fields that matter for the test

3. **Use consistent indices:**
   - Entity index should match your test logic
   - Keep indices sequential when possible

### Working with Simulation Steps

1. **Order matters:**
   - Steps execute in the order specified
   - Think about dependencies between steps

2. **Choose appropriate execution mode:**
   - Use `LogicClass` for complete system behavior
   - Use `Function` for specific functionality

3. **Document each step:**
   - Use `description` field to explain what each step does
   - Helps with debugging and maintenance

### Tick-Based Testing

1. **Plan your tick count:**
   - Set `num_ticks` based on how long your scenario needs
   - Remember: inputs/events/simulation run every tick

2. **Schedule inputs and events carefully:**
   - Use `tick` field to control timing
   - Consider what needs to happen before/after each event

3. **Simulation runs every tick:**
   - Simulation steps execute on every tick
   - Plan accordingly for multi-tick scenarios

### Using expected_to_pass

1. **Default behavior (true):**

   ```json
   "expected_to_pass": true
   ```

   - Test expects pools to match
   - Most common use case
   - Validates correct behavior

2. **Negative testing (false):**

   ```json
   "expected_to_pass": false
   ```

   - Test expects pools to be different
   - Useful for validation testing
   - Tests error detection

---

## Troubleshooting

### Common Issues

#### 1. File Not Found Error

**Error:** `Test data file not found`

**Solutions:**

- Check filename ends with `.test_data.json`
- Verify file is in correct `tests/<test_dir>/data/` directory
- Rebuild project to compile JSON to binary
- Check CMakeLists.txt includes the data directory

#### 2. JSON Parse Error

**Error:** `Failed to parse test data`

**Solutions:**

- Validate JSON syntax (use a JSON validator)
- Check all required fields are present (especially `metadata`)
- Verify field types match schema
- Check for trailing commas (not allowed in JSON)

#### 3. Pool Size Mismatch

**Error:** `Pool sizes differ`

**Solutions:**

- Ensure `start_entity_collection` and `expected_entity_collection` have same
  `entity_memory_pool_size`
- Check that you specified the size in both collections

#### 4. Component Not Found

**Error:** `Component not found on entity`

**Solutions:**

- Verify component name starts with `c_` prefix
- Check component is registered in ComponentRegister
- Ensure entity index exists in pool
- Verify entity has that component activated

#### 5. Simulation Step Failed

**Error:** `Simulation step failed to execute`

**Solutions:**

- Check `logic_class_type` or `function_type` is spelled correctly
- Verify Logic class or function is registered in simulation system
- Check that required components exist on entities
- Review simulation step order and dependencies

#### 6. Unexpected Test Result

**Symptom:** Test passes/fails when it shouldn't

**Solutions:**

- Check `expected_to_pass` value matches your intention
- Verify entity states in start vs expected collections
- Review simulation steps are in correct order
- Check num_ticks is appropriate for your scenario
- Add `description` fields to help debug

### Debugging Tips

1. **Start simple:**
   - Begin with metadata-only test
   - Add entity collections
   - Then add simulation
   - Finally add inputs/events

2. **Test incrementally:**
   - Add one simulation step at a time
   - Verify each step works before adding the next

3. **Use descriptions:**
   - Fill out all `description` fields
   - Helps identify which part of test is failing

4. **Check examples:**
   - Look at sample files in `tests/harness/data/`
   - Copy structure from working examples

5. **Validate JSON:**
   - Use online JSON validator
   - Check for syntax errors before building

---

## Example Test Data Files

### Example 1: Simple Component Test

File: `tests/unit/components/data/ui_default_values.test_data.json`

```json
{
  "metadata": {
    "test_name": "ui_component_default_values",
    "description": "Verify UI component initializes with correct defaults",
    "tags": ["unit", "component", "ui"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 1,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "default_ui",
          "is_visible": false,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 0, "y": 0 },
              "size": { "x": 100, "y": 100 },
              "children_active": false,
              "children": [],
              "layout": "None",
              "spacing_strategy": "None"
            }
          }
        }
      }
    ]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 1,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "default_ui",
          "is_visible": false,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 0, "y": 0 },
              "size": { "x": 100, "y": 100 },
              "children_active": false,
              "children": [],
              "layout": "None",
              "spacing_strategy": "None"
            }
          }
        }
      }
    ]
  }
}
```

### Example 2: Multi-Tick Input Test

File: `tests/integration/ui/data/button_click_sequence.test_data.json`

```json
{
  "metadata": {
    "test_name": "button_click_sequence",
    "description": "Test button responds to mouse movement and click",
    "tags": ["integration", "ui", "input"],
    "expected_to_pass": true,
    "version": 1
  },
  "num_ticks": 3,
  "input_sequence": {
    "description": "Move mouse over button and click",
    "inputs": [
      {
        "input_type": "MouseMove",
        "input_data_type": "MouseInputData",
        "input_data": {
          "position": { "x": 50.0, "y": 50.0 },
          "button": 0
        },
        "tick": 1,
        "description": "Mouse outside button"
      },
      {
        "input_type": "MouseMove",
        "input_data_type": "MouseInputData",
        "input_data": {
          "position": { "x": 150.0, "y": 125.0 },
          "button": 0
        },
        "tick": 1,
        "description": "Mouse over button center"
      },
      {
        "input_type": "MouseClick",
        "input_data_type": "MouseInputData",
        "input_data": {
          "position": { "x": 150.0, "y": 125.0 },
          "button": 0
        },
        "tick": 2,
        "description": "Click button"
      }
    ]
  },
  "simulation_data": {
    "description": "Check collision and process button actions",
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "LogicClass",
        "logic_class_type": "UICollisionLogic",
        "description": "Check mouse collision with button"
      },
      {
        "simulation_type": "Action",
        "execution_mode": "Function",
        "function_type": "ProcessButtonElementActions",
        "description": "Process button click action"
      }
    ]
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 1,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "test_button",
          "is_visible": true,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 100, "y": 100 },
              "size": { "x": 100, "y": 50 },
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
```

---

## Additional Resources

- **[Test Data Configuration Reference](../testing/TEST_DATA_CONFIGURATION.md)** -
  Complete technical reference
- **[Testing Overview](../testing/TESTING_OVERVIEW.md)** - Testing strategy and
  infrastructure
- **Sample Test Data Files** - See `tests/harness/data/` for working examples
- **FlatBuffers Schema** - See `src/flatbuffers_headers/test_data.fbs` for
  complete schema

---

## Quick Reference Card

| Workflow          | Required Fields                                                     | Optional Fields                              |
| ----------------- | ------------------------------------------------------------------- | -------------------------------------------- |
| Metadata Only     | `metadata`                                                          | -                                            |
| Entity Comparison | `metadata`, `start_entity_collection`, `expected_entity_collection` | `num_ticks`                                  |
| Simulation        | `metadata`, `start_entity_collection`, `simulation_data`            | `expected_entity_collection`, `num_ticks`    |
| Input Sequence    | `metadata`, `input_sequence`, `num_ticks`                           | `start_entity_collection`, `simulation_data` |
| Event Sequence    | `metadata`, `event_sequence`, `num_ticks`                           | `start_entity_collection`, `simulation_data` |
| Combined          | `metadata`, `num_ticks`                                             | All others                                   |

**Default Values:**

- `expected_to_pass`: `true`
- `num_ticks`: `1`
- `version`: `1`

**File Naming:**

- Pattern: `<descriptive_name>.test_data.json`
- Location: `tests/<test_dir>/data/`
- Compiled: `<descriptive_name>.test_data.bin`

---

[← Back to Documentation](../README.md) |
[Testing Overview](../testing/TESTING_OVERVIEW.md) |
[Test Data Configuration Reference](../testing/TEST_DATA_CONFIGURATION.md)
