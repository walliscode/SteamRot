# Test Harness System Adoption Plan

## Overview

This document provides a phased adoption plan for migrating existing tests to the new data-driven test harness system and identifying areas where new data-driven tests should be created. The plan progresses from simple cases to complex scenarios, prioritizing high-value use cases and identifying necessary harness extensions.

## Current Test Harness Capabilities

The test harness system (`tests/harness/`) currently supports:

1. **Test Data Configuration**: Loading test data from JSON files via FlatBuffers
2. **Entity Pool Comparisons**: Comparing start and expected entity memory pools
3. **TestFixture Integration**: Automatic creation of test fixtures with game/scene resources
4. **Simulations**: Executing sequences of Logic classes and free functions
5. **Metadata Support**: Test name, description, tags, expected outcomes
6. **Catch2 Integration**: Works with generators for parameterized testing

### Current Simulation Support

- **Logic Classes**: UIActionLogic, UICollisionLogic, UIRenderLogic, UIStateLogic, CraftingRenderLogic
- **Free Functions**: ProcessUIActionsAndEvents, ProcessNestedUIActionsAndEvents, ProcessButtonElementActions, ProcessDropDownListElementActions
- **System Types**: Action, Movement, Render, Collision

## Phase 1: Simple Use Cases (Low Complexity, High Value)

These are the foundational use cases that demonstrate basic harness functionality and can be adopted immediately.

### 1.1 Entity Pool Comparison Tests

**Target**: `tests/unit/entity/FlatbuffersConfigurator.test.cpp`

**Current State**: Tests manually create and compare entity pools

**Migration Target**: Use `start_entity_collection` and `expected_entity_collection`

**Benefits**:
- Clearer test intent
- Easier to maintain test data
- Automatic pool comparison with detailed error messages

**Example Test Data** (`tests/unit/entity/data/configurator_basic.test_data.json`):
```json
{
  "metadata": {
    "test_name": "configurator_basic",
    "description": "Basic entity configuration from FlatBuffers data",
    "tags": ["unit", "entity", "configurator"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": []
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "test_ui",
          "start_visible": true
        }
      }
    ]
  }
}
```

**Harness Extensions Needed**: None - fully supported

**Priority**: High (existing tests can be migrated)

### 1.2 Metadata-Only Test Cases

**Target**: New tests that only need to validate test infrastructure

**Use Case**: Testing that the harness itself works correctly, validating data loading

**Example Test Data** (`tests/harness/data/metadata_validation.test_data.json`):
```json
{
  "metadata": {
    "test_name": "metadata_validation",
    "description": "Validates test harness metadata loading",
    "tags": ["unit", "harness", "infrastructure"],
    "expected_to_pass": true,
    "author": "Test Infrastructure Team",
    "version": 1
  }
}
```

**Harness Extensions Needed**: None - fully supported

**Priority**: Medium (useful for infrastructure validation)

## Phase 2: Intermediate Use Cases (Medium Complexity, High Value)

These use cases involve more complex scenarios like simulations and multi-step workflows.

### 2.1 Logic Class Tests with Simulations

**Target**: `tests/unit/logic/*.test.cpp`

**Current State**: Tests manually instantiate Logic classes and test behavior

**Migration Target**: Use simulation data to test Logic execution

**Benefits**:
- Test complex multi-step scenarios
- Mimic real Scene system behavior
- Test Logic interactions

**Example**: Testing UIActionLogic button processing

**Test Data** (`tests/unit/logic/data/ui_action_button_click.test_data.json`):
```json
{
  "metadata": {
    "test_name": "ui_action_button_click",
    "description": "Test button click processing via UIActionLogic",
    "tags": ["unit", "logic", "UIActionLogic"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "button_ui",
          "start_visible": true,
          "root_ui_element": {
            "button_data": {
              "label": "Test Button",
              "is_mouse_over": true
            }
          }
        }
      }
    ]
  },
  "simulation_data": {
    "description": "Simulate button click: collision -> action",
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
        "description": "Process button click"
      }
    ]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "button_ui",
          "start_visible": true
        }
      }
    ]
  }
}
```

**Harness Extensions Needed**: None - simulations fully supported

**Priority**: High (logic tests are core to game engine)

### 2.2 UI Interaction Workflows

**Target**: `tests/unit/user_interface/*.test.cpp`

**Current State**: Manual UI element setup and validation

**Migration Target**: Complete UI workflows with render + collision + action

**Benefits**:
- Test real UI interaction patterns
- Validate UI element behavior end-to-end
- Easy to test edge cases

**Example Test Data** (`tests/unit/user_interface/data/dropdown_interaction.test_data.json`):
```json
{
  "metadata": {
    "test_name": "dropdown_interaction",
    "description": "Test dropdown menu interaction workflow",
    "tags": ["unit", "ui", "dropdown"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 2,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "dropdown_menu",
          "start_visible": true,
          "root_ui_element": {
            "dropdown_data": {
              "options": ["Option A", "Option B", "Option C"],
              "selected_index": 0,
              "is_open": false
            }
          }
        }
      }
    ]
  },
  "simulation_data": {
    "description": "Open dropdown, hover over option, select",
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "LogicClass",
        "logic_class_type": "UICollisionLogic",
        "description": "Check collision with dropdown"
      },
      {
        "simulation_type": "Action",
        "execution_mode": "Function",
        "function_type": "ProcessDropDownListElementActions",
        "description": "Toggle dropdown open"
      },
      {
        "simulation_type": "Render",
        "execution_mode": "LogicClass",
        "logic_class_type": "UIRenderLogic",
        "description": "Render expanded dropdown"
      }
    ]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 2,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "dropdown_menu",
          "start_visible": true,
          "root_ui_element": {
            "dropdown_data": {
              "is_open": true
            }
          }
        }
      }
    ]
  }
}
```

**Harness Extensions Needed**: None - fully supported

**Priority**: High (UI is core game functionality)

### 2.3 Multi-Component Entity Tests

**Target**: Tests involving entities with multiple components (e.g., CUserInterface + CGrimoireMachina)

**Current State**: Tests focus on single components

**Migration Target**: Test entity configurations with multiple components

**Benefits**:
- Test realistic entity configurations
- Validate component interactions
- Test archetype generation

**Example Test Data** (`tests/unit/components/data/multi_component_entity.test_data.json`):
```json
{
  "metadata": {
    "test_name": "multi_component_entity",
    "description": "Entity with UI and Grimoire components",
    "tags": ["unit", "entity", "multi-component"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "grimoire_ui",
          "start_visible": true
        },
        "c_grimoire_machina": {
          "grimoire_name": "Test Grimoire",
          "page_count": 10
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
          "ui_name": "grimoire_ui",
          "start_visible": true
        },
        "c_grimoire_machina": {
          "grimoire_name": "Test Grimoire",
          "page_count": 10
        }
      }
    ]
  }
}
```

**Harness Extensions Needed**: None - fully supported

**Priority**: Medium (important for integration-like tests)

## Phase 3: Advanced Use Cases (High Complexity, Extensions Required)

These use cases require extending the harness with new capabilities. They should be prioritized based on testing needs.

### 3.1 Event Sequence Testing (HIGH PRIORITY EXTENSION)

**Target**: `tests/unit/events/*.test.cpp`

**Current State**: Tests manually create and verify events

**Needed Extension**: Event sequence data in test_data.fbs

**Benefits**:
- Test event-driven workflows
- Validate event handler behavior
- Test event subscriptions and responses

**Proposed Schema Addition** (`src/flatbuffers_headers/event_test_data.fbs`):
```fbs
namespace steamrot;

table EventTestData {
  event_type: EventType;
  event_data: EventDataData;
  expected_subscribers: [string];  // List of expected subscriber names
}

table EventSequence {
  events: [EventTestData];
  description: string;
}
```

**Update test_data.fbs**:
```fbs
table TestDataConfig {
  metadata: TestMetadata (required);
  start_entity_collection: EntityCollection;
  expected_entity_collection: EntityCollection;
  game_resources: GameResourcesData;
  scene_resources: SceneResourcesData;
  simulation_data: SimulationData;
  event_sequence: EventSequence;  // NEW
}
```

**Example Test Data** (`tests/unit/events/data/button_click_event.test_data.json`):
```json
{
  "metadata": {
    "test_name": "button_click_event",
    "description": "Test button click generates correct event",
    "tags": ["unit", "events", "ui"],
    "expected_to_pass": true,
    "version": 1
  },
  "event_sequence": {
    "description": "Button click should trigger EVENT_USER_INPUT",
    "events": [
      {
        "event_type": "EVENT_USER_INPUT",
        "event_data": {
          "input_type": "mouse_click",
          "position": { "x": 150, "y": 120 }
        },
        "expected_subscribers": ["UIActionLogic", "button_handler"]
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
          "start_visible": true,
          "root_ui_element": {
            "button_data": {
              "position": { "x": 100, "y": 100 },
              "size": { "x": 100, "y": 50 }
            }
          }
        }
      }
    ]
  }
}
```

**Implementation Additions Needed**:
1. Add `event_test_data.fbs` schema
2. Update `test_data.fbs` with `event_sequence` field
3. Add event validation functions to harness
4. Add simulation support for triggering events
5. Add verification for event bus state

**Priority**: **HIGH** - Events are fundamental to game engine architecture

### 3.2 Input Simulation (HIGH PRIORITY EXTENSION)

**Target**: Tests that need to simulate user input (mouse, keyboard)

**Needed Extension**: Input event data in test_data.fbs

**Benefits**:
- Test input handling logic
- Validate input-driven behaviors
- Test UI interactions realistically

**Proposed Schema Addition** (`src/flatbuffers_headers/input_test_data.fbs`):
```fbs
namespace steamrot;

enum InputType : byte {
  MouseMove,
  MouseClick,
  MouseRelease,
  KeyPress,
  KeyRelease
}

table MouseInputData {
  position: Vec2f;
  button: uint8;  // SFML mouse button enum
}

table KeyboardInputData {
  key_code: uint32;  // SFML key code
  alt: bool = false;
  control: bool = false;
  shift: bool = false;
}

union InputEventData {
  MouseInputData,
  KeyboardInputData
}

table InputEvent {
  input_type: InputType;
  input_data: InputEventData;
  timestamp_ms: uint32;  // Relative timestamp for sequencing
  description: string;
}

table InputSequence {
  inputs: [InputEvent];
  description: string;
}
```

**Update test_data.fbs**:
```fbs
table TestDataConfig {
  metadata: TestMetadata (required);
  start_entity_collection: EntityCollection;
  expected_entity_collection: EntityCollection;
  game_resources: GameResourcesData;
  scene_resources: SceneResourcesData;
  simulation_data: SimulationData;
  event_sequence: EventSequence;
  input_sequence: InputSequence;  // NEW
}
```

**Example Test Data** (`tests/unit/logic/data/button_click_with_input.test_data.json`):
```json
{
  "metadata": {
    "test_name": "button_click_with_input",
    "description": "Simulate mouse movement and click on button",
    "tags": ["unit", "logic", "input"],
    "expected_to_pass": true,
    "version": 1
  },
  "input_sequence": {
    "description": "Move mouse over button and click",
    "inputs": [
      {
        "input_type": "MouseMove",
        "input_data": {
          "position": { "x": 150, "y": 125 }
        },
        "timestamp_ms": 0,
        "description": "Move mouse to button center"
      },
      {
        "input_type": "MouseClick",
        "input_data": {
          "position": { "x": 150, "y": 125 },
          "button": 0
        },
        "timestamp_ms": 100,
        "description": "Click left mouse button"
      },
      {
        "input_type": "MouseRelease",
        "input_data": {
          "position": { "x": 150, "y": 125 },
          "button": 0
        },
        "timestamp_ms": 150,
        "description": "Release left mouse button"
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
          "start_visible": true,
          "root_ui_element": {
            "button_data": {
              "position": { "x": 100, "y": 100 },
              "size": { "x": 100, "y": 50 },
              "label": "Click Me"
            }
          }
        }
      }
    ]
  },
  "simulation_data": {
    "description": "Process collision and action after input",
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
  }
}
```

**Implementation Additions Needed**:
1. Add `input_test_data.fbs` schema
2. Update `test_data.fbs` with `input_sequence` field
3. Add input injection mechanism to TestFixture
4. Add input replay functionality to simulation_runner
5. Integrate with SFML event system

**Priority**: **HIGH** - Essential for testing user interactions

### 3.3 Custom Resource Configuration (MEDIUM PRIORITY EXTENSION)

**Target**: Tests that need specific asset or resource configurations

**Current State**: test_data.fbs has `game_resources` and `scene_resources` fields, but they're not fully utilized

**Needed Enhancement**: Implement resource configuration in TestFixture creation

**Benefits**:
- Test with specific assets
- Validate resource loading
- Test resource-dependent behaviors

**Example**: The schema already exists but needs implementation

**Test Data** (`tests/unit/assets/data/font_loading.test_data.json`):
```json
{
  "metadata": {
    "test_name": "font_loading",
    "description": "Test custom font resource loading",
    "tags": ["unit", "assets", "resources"],
    "expected_to_pass": true,
    "version": 1
  },
  "game_resources": {
    "fonts": [
      {
        "name": "test_font",
        "path": "data/fonts/test.ttf",
        "size": 24
      }
    ]
  },
  "scene_resources": {
    "textures": [
      {
        "name": "background",
        "path": "data/textures/test_bg.png"
      }
    ]
  }
}
```

**Implementation Additions Needed**:
1. Enhance `create_fixture_from_test_data()` to configure resources
2. Add resource validation to TestFixture
3. Add resource accessor methods for tests
4. Document resource configuration patterns

**Priority**: MEDIUM - Useful but not critical for most tests

### 3.4 Scene Transition Testing (MEDIUM PRIORITY EXTENSION)

**Target**: `tests/integration/scene_change/*.test.cpp`

**Needed Extension**: Scene transition data

**Benefits**:
- Test scene change logic
- Validate state persistence
- Test scene initialization

**Proposed Schema Addition** (`src/flatbuffers_headers/scene_test_data.fbs`):
```fbs
namespace steamrot;

table SceneTransition {
  from_scene: SceneType;
  to_scene: SceneType;
  transition_data: SceneChangePacketData;
  description: string;
}

table SceneTransitionSequence {
  transitions: [SceneTransition];
  description: string;
}
```

**Update test_data.fbs**:
```fbs
table TestDataConfig {
  metadata: TestMetadata (required);
  start_entity_collection: EntityCollection;
  expected_entity_collection: EntityCollection;
  game_resources: GameResourcesData;
  scene_resources: SceneResourcesData;
  simulation_data: SimulationData;
  event_sequence: EventSequence;
  input_sequence: InputSequence;
  scene_transitions: SceneTransitionSequence;  // NEW
}
```

**Example Test Data** (`tests/integration/scene_change/data/title_to_crafting.test_data.json`):
```json
{
  "metadata": {
    "test_name": "title_to_crafting",
    "description": "Test transition from title scene to crafting scene",
    "tags": ["integration", "scene", "transition"],
    "expected_to_pass": true,
    "version": 1
  },
  "scene_transitions": {
    "description": "Navigate from title screen to crafting interface",
    "transitions": [
      {
        "from_scene": "SceneType_TITLE",
        "to_scene": "SceneType_CRAFTING",
        "transition_data": {
          "scene_type": "SceneType_CRAFTING"
        },
        "description": "Select crafting from menu"
      }
    ]
  }
}
```

**Implementation Additions Needed**:
1. Add `scene_test_data.fbs` schema
2. Update `test_data.fbs` with `scene_transitions` field
3. Add scene transition simulation to harness
4. Add scene state validation helpers
5. Integrate with Scene system

**Priority**: MEDIUM - Important for integration tests

### 3.5 Asset Validation Testing (LOW PRIORITY EXTENSION)

**Target**: `tests/unit/assets/*.test.cpp`

**Needed Extension**: Asset validation criteria

**Benefits**:
- Validate asset integrity
- Test asset loading failures
- Test asset format compatibility

**Proposed Schema Addition** (`src/flatbuffers_headers/asset_test_data.fbs`):
```fbs
namespace steamrot;

enum AssetType : byte {
  Texture,
  Font,
  Sound,
  Music,
  Data
}

table AssetValidation {
  asset_type: AssetType;
  asset_path: string;
  should_load_successfully: bool = true;
  expected_dimensions: Vec2i;  // For textures
  expected_file_size_bytes: uint32;
  description: string;
}

table AssetValidationSequence {
  validations: [AssetValidation];
  description: string;
}
```

**Priority**: LOW - Nice to have, but manual tests are sufficient

### 3.6 Performance Benchmarking (LOW PRIORITY EXTENSION)

**Target**: New performance tests

**Needed Extension**: Performance criteria and measurement

**Benefits**:
- Regression testing for performance
- Identify bottlenecks
- Validate optimizations

**Proposed Schema Addition** (`src/flatbuffers_headers/perf_test_data.fbs`):
```fbs
namespace steamrot;

table PerformanceThresholds {
  max_execution_time_ms: uint32;
  max_memory_usage_bytes: uint64;
  min_fps: float;
  description: string;
}

table PerformanceBenchmark {
  thresholds: PerformanceThresholds;
  iterations: uint32 = 100;
  warmup_iterations: uint32 = 10;
  description: string;
}
```

**Priority**: LOW - Can be added later as optimization needs arise

## Phase 4: Migration Strategy

### Step 1: Start with New Tests (Weeks 1-2)

**Goal**: Build confidence with the harness without disrupting existing tests

**Actions**:
1. Create data-driven tests for new features/components
2. Document patterns and best practices
3. Create example test data files for each category
4. Train team on harness usage

**Success Metrics**:
- 5+ new data-driven tests created
- Team comfortable with harness basics
- Documentation complete

### Step 2: Migrate Simple Tests (Weeks 3-4)

**Goal**: Convert existing simple tests (Phase 1 use cases)

**Actions**:
1. Convert simple entity pool comparison tests
2. Add test data files to existing test directories
3. Keep old tests alongside new tests initially
4. Create metadata-only validation tests

**Success Metrics**:
- 10+ entity tests migrated
- Metadata validation tests operational
- No test coverage regression

### Step 3: Implement High-Priority Extensions (Weeks 5-8)

**Goal**: Enable advanced testing capabilities

**Actions**:
1. **Week 5-6**: Implement event sequence testing (Priority: HIGH)
   - Add event_test_data.fbs schema
   - Update test_data.fbs
   - Implement event validation in harness
   - Create example tests

2. **Week 7-8**: Implement input simulation (Priority: HIGH)
   - Add input_test_data.fbs schema
   - Update test_data.fbs
   - Implement input injection mechanism
   - Create example tests

**Success Metrics**:
- Event sequence testing operational
- Input simulation operational
- 10+ tests using new capabilities

### Step 4: Migrate Complex Tests (Weeks 9-12)

**Goal**: Convert logic and integration tests (Phase 2 use cases)

**Actions**:
1. Migrate logic class tests to use simulations
2. Convert UI interaction tests
3. Create multi-component entity tests
4. Add integration test data files

**Success Metrics**:
- 30+ logic tests migrated
- 15+ UI tests migrated
- Integration tests using harness

### Step 5: Implement Medium-Priority Extensions (Weeks 13-16)

**Goal**: Add remaining useful features

**Actions**:
1. **Week 13-14**: Enhance resource configuration
   - Implement resource loading in TestFixture
   - Document resource patterns
   - Create example tests

2. **Week 15-16**: Implement scene transition testing
   - Add scene_test_data.fbs schema
   - Implement scene transition simulation
   - Create integration tests

**Success Metrics**:
- Resource configuration working
- Scene transition tests operational
- Integration test suite complete

### Step 6: Cleanup and Optimization (Weeks 17-18)

**Goal**: Remove old tests, optimize harness

**Actions**:
1. Remove old non-data-driven tests
2. Optimize test data compilation
3. Add harness performance improvements
4. Final documentation updates

**Success Metrics**:
- All tests migrated or documented as intentionally manual
- Test execution time acceptable
- Documentation complete

## Implementation Priority Summary

### Immediate (Start Now)
1. ✅ Create this plan document
2. Phase 1.1: Migrate entity pool comparison tests
3. Phase 1.2: Create metadata-only validation tests
4. Phase 2.1: Create logic class simulation tests

### High Priority (Weeks 1-8)
1. **Phase 3.1: Event sequence testing** (CRITICAL EXTENSION)
2. **Phase 3.2: Input simulation** (CRITICAL EXTENSION)
3. Phase 2.2: UI interaction workflow tests
4. Complete Phase 1 and Phase 2 migrations

### Medium Priority (Weeks 9-16)
1. Phase 3.3: Resource configuration enhancement
2. Phase 3.4: Scene transition testing
3. Phase 2.3: Multi-component entity tests
4. Integration test migrations

### Low Priority (Future)
1. Phase 3.5: Asset validation testing
2. Phase 3.6: Performance benchmarking
3. Additional simulation features as needed

## Required Harness Extensions Detail

### Extension 1: Event Sequence Testing (HIGH PRIORITY)

**Files to Create/Modify**:
- `src/flatbuffers_headers/event_test_data.fbs` (NEW)
- `src/flatbuffers_headers/test_data.fbs` (UPDATE)
- `tests/harness/event_simulation.h` (NEW)
- `tests/harness/event_simulation.cpp` (NEW)
- `tests/harness/event_simulation.test.cpp` (NEW)

**API Design**:
```cpp
namespace steamrot::tests {

// Execute a sequence of events
std::expected<std::monostate, FailInfo>
execute_event_sequence(const EventSequence *event_sequence,
                       EventHandler &event_handler);

// Validate expected subscribers
std::expected<std::monostate, FailInfo>
validate_event_subscribers(const EventTestData *event_data,
                           EventHandler &event_handler);

// Execute event sequence with fixture
std::expected<std::monostate, FailInfo>
execute_event_sequence_with_fixture(const EventSequence *event_sequence,
                                    TestFixture &fixture);
}
```

**Integration**: Add to `run_fixture_test()` to automatically execute event sequences

### Extension 2: Input Simulation (HIGH PRIORITY)

**Files to Create/Modify**:
- `src/flatbuffers_headers/input_test_data.fbs` (NEW)
- `src/flatbuffers_headers/test_data.fbs` (UPDATE)
- `tests/harness/input_simulation.h` (NEW)
- `tests/harness/input_simulation.cpp` (NEW)
- `tests/harness/input_simulation.test.cpp` (NEW)
- `tests/harness/TestFixture.h` (UPDATE - add input injection)
- `tests/harness/TestFixture.cpp` (UPDATE)

**API Design**:
```cpp
namespace steamrot::tests {

// Execute a sequence of input events
std::expected<std::monostate, FailInfo>
execute_input_sequence(const InputSequence *input_sequence,
                       TestFixture &fixture);

// Inject single input event
std::expected<std::monostate, FailInfo>
inject_input_event(const InputEvent *input_event,
                   TestFixture &fixture);

// Simulate time passing for input sequencing
void advance_time(uint32_t ms, TestFixture &fixture);
}
```

**Integration**: Add to `run_fixture_test()` to execute input sequences before simulations

### Extension 3: Resource Configuration (MEDIUM PRIORITY)

**Files to Modify**:
- `tests/harness/test_data_harness.cpp` (UPDATE `create_fixture_from_test_data()`)
- `tests/harness/TestFixture.h` (ADD resource configuration methods)
- `tests/harness/TestFixture.cpp` (UPDATE)

**API Enhancement**:
```cpp
namespace steamrot::tests {

// Enhanced fixture creation with resource configuration
std::expected<TestFixture, FailInfo>
create_fixture_from_test_data(const TestDataConfig *config,
                               SceneType scene_type = SceneType_TEST) {
  // Existing implementation +
  // Configure game_resources if present
  // Configure scene_resources if present
}
}
```

**No Schema Changes Needed**: Fields already exist in test_data.fbs

### Extension 4: Scene Transition Testing (MEDIUM PRIORITY)

**Files to Create/Modify**:
- `src/flatbuffers_headers/scene_test_data.fbs` (NEW)
- `src/flatbuffers_headers/test_data.fbs` (UPDATE)
- `tests/harness/scene_simulation.h` (NEW)
- `tests/harness/scene_simulation.cpp` (NEW)
- `tests/harness/scene_simulation.test.cpp` (NEW)

**API Design**:
```cpp
namespace steamrot::tests {

// Execute scene transition sequence
std::expected<std::monostate, FailInfo>
execute_scene_transitions(const SceneTransitionSequence *transitions,
                          TestFixture &fixture);

// Validate scene state after transition
std::expected<std::monostate, FailInfo>
validate_scene_state(SceneType expected_scene,
                     const TestFixture &fixture);
}
```

## Testing the Extensions

Each extension should have its own test file demonstrating usage:

1. `tests/harness/event_simulation.test.cpp` - Event sequence tests
2. `tests/harness/input_simulation.test.cpp` - Input simulation tests
3. `tests/harness/resource_configuration.test.cpp` - Resource config tests
4. `tests/harness/scene_simulation.test.cpp` - Scene transition tests

## Success Criteria

### Quantitative Metrics
- 80%+ of unit tests converted to data-driven by end of Phase 6
- 50%+ of integration tests using harness
- Test data file count > test code file count
- Test code reduction of 40%+ (measured in lines of code)

### Qualitative Metrics
- New tests primarily data-driven
- Team prefers harness over manual tests
- Test maintenance time reduced
- Test data easier to review than code

## Risks and Mitigations

### Risk 1: Learning Curve
**Mitigation**: Provide training, examples, and gradual adoption

### Risk 2: Test Data Maintenance
**Mitigation**: Clear naming conventions, documentation, and validation tools

### Risk 3: Harness Bugs
**Mitigation**: Thorough testing of harness itself, gradual rollout

### Risk 4: Performance Overhead
**Mitigation**: Monitor test execution times, optimize as needed

### Risk 5: Complexity Creep
**Mitigation**: Keep harness simple, resist over-engineering

## Conclusion

This plan provides a clear path from simple to complex use cases, identifies necessary harness extensions, and prioritizes work based on value and complexity. The phased approach allows for learning and adjustment while making steady progress toward comprehensive data-driven testing.

**Next Steps**:
1. Review and approve this plan
2. Begin Phase 1 migrations
3. Start implementing high-priority extensions (Event sequence and Input simulation)
4. Monitor progress and adjust timeline as needed

## Appendix A: Example Directory Structure After Full Adoption

```
tests/
├── harness/
│   ├── test_data_harness.h/cpp              # Core harness
│   ├── simulation_runner.h/cpp              # Simulation execution
│   ├── event_simulation.h/cpp               # NEW: Event testing
│   ├── input_simulation.h/cpp               # NEW: Input testing
│   ├── scene_simulation.h/cpp               # NEW: Scene transitions
│   ├── TestFixture.h/cpp                    # Test fixture
│   └── data/                                # Harness test data
├── unit/
│   ├── components/
│   │   ├── data/                            # Component test data
│   │   │   └── multi_component_entity.test_data.json
│   │   └── *.test.cpp                       # Component tests
│   ├── logic/
│   │   ├── data/                            # Logic test data with simulations
│   │   │   ├── ui_action_button_click.test_data.json
│   │   │   ├── ui_collision_detection.test_data.json
│   │   │   └── ui_render_workflow.test_data.json
│   │   └── *.test.cpp                       # Logic tests (data-driven)
│   ├── entity/
│   │   ├── data/                            # Entity test data
│   │   │   ├── configurator_basic.test_data.json
│   │   │   └── archetype_generation.test_data.json
│   │   └── *.test.cpp                       # Entity tests (data-driven)
│   ├── events/
│   │   ├── data/                            # Event sequence test data
│   │   │   ├── button_click_event.test_data.json
│   │   │   └── event_chain.test_data.json
│   │   └── *.test.cpp                       # Event tests (data-driven)
│   └── user_interface/
│       ├── data/                            # UI workflow test data
│       │   ├── dropdown_interaction.test_data.json
│       │   └── button_states.test_data.json
│       └── *.test.cpp                       # UI tests (data-driven)
└── integration/
    └── scene_change/
        ├── data/                            # Scene transition test data
        │   ├── title_to_crafting.test_data.json
        │   └── crafting_to_title.test_data.json
        └── *.test.cpp                       # Integration tests (data-driven)
```

## Appendix B: Quick Reference Commands

### Creating New Test Data
```bash
# 1. Create JSON file in appropriate data/ directory
# tests/unit/components/data/my_test.test_data.json

# 2. Build project (auto-compiles to .bin)
cmake --build --preset Debug

# 3. Test data is automatically discovered and loaded
```

### Writing Data-Driven Test
```cpp
#include "test_data_harness.h"
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("My data-driven test", "[unit][my_component]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

### Running Tests
```bash
# Run all tests
ctest --preset Debug

# Run specific subsystem
ctest --preset Debug -R components

# Run with verbose output
ctest --preset Debug --output-on-failure
```
