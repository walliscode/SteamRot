# Logic Unit Testing Refactoring Plan

## Executive Summary

This document outlines the staged approach to refactoring logic unit tests from manual TestFixture setup to data-driven tests using the test harness system. This refactoring aligns with **Phase 2.3** of the [TEST_HARNESS_ADOPTION_PLAN.md](TEST_HARNESS_ADOPTION_PLAN.md).

## Current State Analysis

### Existing Logic Tests

The current logic tests in `tests/unit/logic/` use manual setup:

**Test Files:**
- `UIActionLogic.test.cpp` - Tests UI action processing and event generation
- `UICollisionLogic.test.cpp` - Tests mouse collision detection
- `UIRenderLogic.test.cpp` - Tests UI rendering
- `UIStateLogic.test.cpp` - Tests UI state updates
- `LogicFactory.test.cpp` - Tests Logic instantiation
- `collision.test.cpp` - Tests collision detection helpers
- `draw_ui_elements.test.cpp` - Tests UI drawing functions
- `ui_helpers.test.cpp` - Tests UI helper functions

**Current Pattern:**
```cpp
TEST_CASE("UIActionLogic test", "[unit][UIActionLogic]") {
  // 1. Create PathProvider and TestFixture manually
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture test_context;
  
  // 2. Manually create and configure entities/components
  steamrot::ButtonElement button_element;
  button_element.position = {100.0f, 100.0f};
  button_element.size = {200.0f, 50.0f};
  // ... more manual setup
  
  // 3. Instantiate Logic class manually
  steamrot::UIActionLogic ui_action_logic(test_context.GetSceneContext());
  
  // 4. Execute and assert
  steamrot::ProcessButtonElementActions(button_element, ...);
  REQUIRE(...);
}
```

### Test Harness Capabilities

The test harness (documented in `tests/harness/README.md`) provides:

1. **Automatic fixture creation** from test data
2. **Entity configuration** from JSON (`start_entity_collection`)
3. **Simulation execution** - runs Logic classes or functions in sequence
4. **Tick-based coordination** - inputs, events, and simulation per tick
5. **Input sequences** - simulate user input (mouse/keyboard)
6. **Event sequences** - inject engine events
7. **State comparison** - compare with `expected_entity_collection`

**Target Pattern:**
```cpp
TEST_CASE("Data-driven logic test", "[unit][logic][data-driven]") {
  // 1. Load test configurations from JSON files
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  // 2. Use Catch2 generator for parameterized testing
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // 3. Run test - fixture creation, entity config, simulation, comparison
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

## Refactoring Goals

1. **Reduce test code complexity** - Move setup to JSON data files
2. **Improve test maintainability** - Easier to add/modify test cases
3. **Leverage simulation capabilities** - Test multi-step Logic sequences
4. **Enable data-driven testing** - Use Catch2 generators for parameterization
5. **Maintain test coverage** - No regression in test coverage
6. **Follow adoption plan** - Implement Phase 2.3 from TEST_HARNESS_ADOPTION_PLAN.md

## Stage Plan

### Stage 0: Preparation (Week 1)

**Goal:** Set up infrastructure and validate approach

**Tasks:**
- [x] Create this planning document
- [ ] Create `tests/unit/logic/data/` directory structure
- [ ] Create template JSON test data file
- [ ] Validate test harness works with logic tests (proof of concept)
- [ ] Document migration pattern in this file
- [ ] Review with team

**Deliverables:**
- `tests/unit/logic/data/` directory
- Template test data file
- 1-2 proof-of-concept tests migrated
- Updated documentation

**Success Criteria:**
- Template works with test harness
- At least one existing test successfully migrated
- Team approves approach

### Stage 1: Simple Logic Tests (Week 2)

**Goal:** Migrate straightforward tests with single Logic class execution

**Target Tests:**
- `UICollisionLogic.test.cpp` - Constructor test
- `UIActionLogic.test.cpp` - Constructor test
- `UIRenderLogic.test.cpp` - Constructor test
- `UIStateLogic.test.cpp` - Constructor test

**Approach:**
1. Create minimal test data with basic entity setup
2. Use simulation to execute single Logic class
3. Keep original tests initially for comparison
4. Validate behavior matches

**Example Test Data:**
```json
{
  "metadata": {
    "test_name": "ui_collision_basic",
    "description": "Test UICollisionLogic detects mouse over element",
    "tags": ["unit", "logic", "UICollisionLogic"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "test_panel",
          "start_visible": true,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 100, "y": 100 },
              "size": { "x": 200, "y": 50 }
            }
          }
        }
      }
    ]
  },
  "simulation_data": {
    "description": "Execute collision detection",
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "LogicClass",
        "logic_class_type": "UICollisionLogic",
        "description": "Detect mouse collision"
      }
    ]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 3,
    "entities": [
      {
        "index": 0,
        "c_user_interface": {
          "ui_name": "test_panel",
          "start_visible": true,
          "root_ui_element": {
            "base_data": {
              "position": { "x": 100, "y": 100 },
              "size": { "x": 200, "y": 50 },
              "is_mouse_over": true
            }
          }
        }
      }
    ]
  }
}
```

**Deliverables:**
- 4-8 JSON test data files
- Updated test files using `load_test_data_configs()`
- Documentation of patterns used

**Success Criteria:**
- All migrated tests pass
- Original tests still pass (kept for validation)
- Code coverage maintained

### Stage 2: Multi-Step Logic Sequences (Week 3)

**Goal:** Migrate tests that require multiple Logic classes or functions in sequence

**Target Tests:**
- Tests combining collision → action
- Tests combining collision → render → action
- Tests with nested UI processing

**Approach:**
1. Create test data with multi-step simulations
2. Use simulation steps to orchestrate Logic execution order
3. Validate state changes after each step (via expected entities)
4. Test Logic interactions

**Example Test Data:**
```json
{
  "metadata": {
    "test_name": "ui_button_click_workflow",
    "description": "Complete button click: collision → action → event",
    "tags": ["unit", "logic", "workflow"],
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
    "description": "Simulate button click workflow",
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "LogicClass",
        "logic_class_type": "UICollisionLogic",
        "description": "Check mouse collision"
      },
      {
        "simulation_type": "Action",
        "execution_mode": "Function",
        "function_type": "ProcessButtonElementActions",
        "description": "Process button action"
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
          "start_visible": true,
          "root_ui_element": {
            "button_data": {
              "is_mouse_over": true,
              "label": "Click Me"
            }
          }
        }
      }
    ]
  }
}
```

**Deliverables:**
- 8-12 JSON test data files with multi-step simulations
- Updated test files
- Documentation of interaction patterns

**Success Criteria:**
- Multi-step simulations execute correctly
- State changes validated at end
- Logic interactions work as expected

### Stage 3: Input and Event Integration (Week 4)

**Goal:** Add input sequences and event sequences to logic tests

**Target Tests:**
- Tests requiring mouse input simulation
- Tests requiring keyboard input
- Tests requiring event injection

**Approach:**
1. Add input sequences to test data (mouse movement, clicks)
2. Add event sequences where needed
3. Use tick-based execution for precise timing
4. Coordinate inputs → events → simulation per tick

**Example Test Data:**
```json
{
  "metadata": {
    "test_name": "button_click_with_input",
    "description": "Button interaction with input sequence",
    "tags": ["unit", "logic", "input"],
    "expected_to_pass": true,
    "version": 1
  },
  "num_ticks": 3,
  "input_sequence": {
    "description": "Move mouse and click button",
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
  },
  "event_sequence": {
    "description": "Expected events",
    "events": [
      {
        "tick": 1,
        "event_packet": {
          "event_lifetime": 3,
          "event_type": "EVENT_USER_INPUT"
        },
        "description": "User input event"
      }
    ]
  },
  "simulation_data": {
    "description": "Process collision and action per tick",
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
  "start_entity_collection": { /* ... */ },
  "expected_entity_collection": { /* ... */ }
}
```

**Deliverables:**
- 10-15 JSON test data files with input/event sequences
- Updated test files
- Documentation of tick-based testing patterns

**Success Criteria:**
- Input sequences work correctly
- Event sequences inject events properly
- Tick-based coordination works as expected

### Stage 4: Edge Cases and Negative Tests (Week 5)

**Goal:** Migrate edge case and negative tests

**Target Tests:**
- Tests with invalid/missing entities
- Tests with edge case UI configurations
- Tests expecting specific failures
- Boundary condition tests

**Approach:**
1. Use `expected_to_pass: false` for negative tests
2. Create test data with edge case configurations
3. Test error handling and validation
4. Test boundary conditions

**Example Test Data:**
```json
{
  "metadata": {
    "test_name": "collision_outside_bounds",
    "description": "Mouse outside UI element bounds",
    "tags": ["unit", "logic", "edge-case"],
    "expected_to_pass": true,
    "version": 1
  },
  "input_sequence": {
    "inputs": [
      {
        "input_type": "MouseMove",
        "input_data_type": "MouseInputData",
        "input_data": {
          "position": { "x": -100.0, "y": -100.0 },
          "button": 0
        },
        "tick": 0,
        "description": "Mouse far outside element"
      }
    ]
  },
  "simulation_data": {
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "LogicClass",
        "logic_class_type": "UICollisionLogic"
      }
    ]
  },
  "start_entity_collection": { /* UI element at (100, 100) */ },
  "expected_entity_collection": { /* is_mouse_over should be false */ }
}
```

**Deliverables:**
- 8-12 edge case test data files
- Updated test files
- Documentation of edge case patterns

**Success Criteria:**
- Edge cases handled correctly
- Negative tests work as expected
- Boundary conditions validated

### Stage 5: Cleanup and Optimization (Week 6)

**Goal:** Remove old tests, optimize, and finalize migration

**Tasks:**
- Remove original manual tests (after validation)
- Consolidate similar test cases
- Optimize test data files
- Add comprehensive documentation
- Update CMakeLists.txt if needed
- Performance testing of data-driven tests

**Approach:**
1. Validate all migrated tests pass
2. Remove old test code
3. Consolidate duplicate patterns
4. Document best practices
5. Review and iterate

**Deliverables:**
- Clean test files using only data-driven approach
- Comprehensive test data coverage
- Updated documentation
- Performance benchmarks

**Success Criteria:**
- Zero old manual tests remaining (except helpers)
- Test execution time acceptable
- Full test coverage maintained
- Team satisfied with approach

## Migration Patterns

### Pattern 1: Simple Logic Execution

**Before (Manual):**
```cpp
TEST_CASE("Logic test", "[unit]") {
  steamrot::PathProvider pp{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  steamrot::MyLogic logic(fixture.GetSceneContext());
  logic.RunLogic();
  // Manual assertions
}
```

**After (Data-Driven):**
```cpp
TEST_CASE("Logic test", "[unit][data-driven]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

**JSON:**
```json
{
  "metadata": { /* ... */ },
  "simulation_data": {
    "steps": [
      {
        "simulation_type": "Action",
        "execution_mode": "LogicClass",
        "logic_class_type": "MyLogic"
      }
    ]
  },
  "start_entity_collection": { /* ... */ },
  "expected_entity_collection": { /* ... */ }
}
```

### Pattern 2: Multi-Step Workflow

**Before (Manual):**
```cpp
TEST_CASE("Workflow test", "[unit]") {
  // Setup
  steamrot::tests::TestFixture fixture;
  
  // Step 1
  steamrot::CollisionLogic collision(fixture.GetSceneContext());
  collision.RunLogic();
  
  // Step 2
  steamrot::ActionLogic action(fixture.GetSceneContext());
  action.RunLogic();
  
  // Assert
}
```

**After (Data-Driven):**
```json
{
  "simulation_data": {
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "LogicClass",
        "logic_class_type": "CollisionLogic"
      },
      {
        "simulation_type": "Action",
        "execution_mode": "LogicClass",
        "logic_class_type": "ActionLogic"
      }
    ]
  }
}
```

### Pattern 3: Input + Simulation

**Before (Manual):**
```cpp
TEST_CASE("Input test", "[unit]") {
  steamrot::tests::TestFixture fixture;
  
  // Manually set mouse position
  fixture.GetGameContext().mouse_position = sf::Vector2i(150, 125);
  
  // Run logic
  steamrot::UICollisionLogic collision(fixture.GetSceneContext());
  collision.RunLogic();
  
  // Assert
}
```

**After (Data-Driven):**
```json
{
  "input_sequence": {
    "inputs": [
      {
        "input_type": "MouseMove",
        "input_data": { "position": { "x": 150.0, "y": 125.0 } },
        "tick": 0
      }
    ]
  },
  "simulation_data": {
    "steps": [
      {
        "simulation_type": "Collision",
        "execution_mode": "LogicClass",
        "logic_class_type": "UICollisionLogic"
      }
    ]
  }
}
```

## File Organization

### Directory Structure

The test harness automatically discovers all `.test_data.bin` files in the adjacent `data/` directory. We use a **flat directory structure** with descriptive file names rather than subdirectories, since the test harness doesn't traverse subdirectories and the file names themselves provide clear categorization.

```
tests/unit/logic/
├── data/                                          # NEW: All test data files
│   ├── templates/                                 # Templates (not loaded by harness)
│   │   ├── README.md
│   │   ├── simple_logic_test.template.json
│   │   ├── multi_step_workflow.template.json
│   │   └── with_input_simulation.template.json
│   ├── ui_collision_basic.test_data.json          # UICollisionLogic tests
│   ├── ui_collision_edge_outside_bounds.test_data.json
│   ├── ui_collision_nested_elements.test_data.json
│   ├── ui_action_button_click.test_data.json      # UIActionLogic tests
│   ├── ui_action_button_with_event.test_data.json
│   ├── ui_action_nested_actions.test_data.json
│   ├── ui_render_basic.test_data.json             # UIRenderLogic tests
│   ├── ui_render_complex_layout.test_data.json
│   ├── ui_state_update.test_data.json             # UIStateLogic tests
│   ├── workflow_button_click.test_data.json       # Multi-Logic workflows
│   ├── workflow_dropdown_interaction.test_data.json
│   └── workflow_complete_ui_cycle.test_data.json
├── UIActionLogic.test.cpp                         # Updated: data-driven
├── UICollisionLogic.test.cpp                      # Updated: data-driven
├── UIRenderLogic.test.cpp                         # Updated: data-driven
├── UIStateLogic.test.cpp                          # Updated: data-driven
├── LogicFactory.test.cpp                          # Keep as-is (unit test)
├── collision.test.cpp                             # Keep or migrate helpers
├── draw_ui_elements.test.cpp                      # Keep or migrate helpers
├── ui_helpers.test.cpp                            # Keep or migrate helpers
├── logic_test_base.h                              # Keep helper infrastructure
├── logic_test_helpers.cpp                         # Keep helper infrastructure
└── logic_test_helpers.h                           # Keep helper infrastructure
```

**Note:** The test harness uses `std::filesystem::directory_iterator` which is non-recursive, so all test data files must be directly in the `data/` directory (not in subdirectories). The `templates/` subdirectory is an exception since template files are not loaded by the test harness (they don't have the `.test_data.bin` extension after compilation).

### Naming Conventions

**Test Data Files:**
- `{logic_class}_{test_scenario}.test_data.json` for Logic-specific tests
- `workflow_{scenario}.test_data.json` for multi-Logic workflows
- Use underscores to separate words, be descriptive
- Examples:
  - `ui_collision_basic.test_data.json`
  - `ui_action_button_click.test_data.json`
  - `workflow_complete_interaction.test_data.json`
  - `ui_collision_edge_outside_bounds.test_data.json`

**Test Names in Metadata:**
- Clear, descriptive names
- Include Logic class or function being tested
- Examples:
  - `"ui_collision_basic"`
  - `"ui_action_button_click_with_event"`
  - `"workflow_collision_render_action"`

## Validation Criteria

### Per Stage

Each stage must meet these criteria before moving to the next:

1. **All migrated tests pass** - No test failures
2. **Code coverage maintained** - Use coverage tools to verify
3. **Performance acceptable** - Test execution time reasonable
4. **Documentation complete** - Patterns documented for team
5. **Team review passed** - Code review approved

### Overall Success Criteria

At completion of Stage 5:

1. **Test Coverage:**
   - ≥90% of logic unit tests migrated to data-driven approach
   - No reduction in code coverage
   - Edge cases and negative tests included

2. **Code Quality:**
   - Test code reduced by ≥40% (lines of code)
   - Test data files > test code files
   - Clear, maintainable test patterns

3. **Maintainability:**
   - Easy to add new test cases (just JSON)
   - Test data easier to review than code
   - Patterns well-documented

4. **Performance:**
   - Test execution time comparable or better
   - No significant slowdowns

5. **Team Adoption:**
   - Team prefers data-driven approach for new tests
   - Team comfortable with test harness
   - Documentation sufficient for new contributors

## Risks and Mitigations

### Risk 1: Learning Curve

**Risk:** Team unfamiliar with test harness and JSON test data format

**Mitigation:**
- Provide comprehensive documentation (this document)
- Start with simple examples (Stage 1)
- Pair programming during migration
- Regular check-ins and Q&A sessions

### Risk 2: Complex Test Cases

**Risk:** Some tests may be too complex for data-driven approach

**Mitigation:**
- Identify complex tests early
- Keep manual tests for truly complex cases
- Extend test harness if needed
- Document which tests stay manual and why

### Risk 3: Test Data Maintenance

**Risk:** Large number of JSON files becomes hard to maintain

**Mitigation:**
- Clear naming conventions
- Organized directory structure
- Documentation of patterns
- Regular cleanup and consolidation

### Risk 4: Migration Bugs

**Risk:** Migrated tests may not match original behavior

**Mitigation:**
- Keep original tests initially for comparison
- Run both old and new tests in parallel during migration
- Validate behavior matches before removing old tests
- Thorough code review for each stage

### Risk 5: Performance Regression

**Risk:** Data-driven tests may be slower than manual tests

**Mitigation:**
- Performance benchmarks at each stage
- Optimize test harness if needed
- Consider parallel test execution
- Monitor test execution times

## Implementation Notes

### Test Harness Functions to Use

**Primary Function:**
```cpp
auto result = steamrot::tests::run_fixture_test(config);
```
This handles:
- Fixture creation
- Entity configuration
- Input sequence execution
- Event sequence execution  
- Simulation execution
- State comparison

**For Manual Control:**
```cpp
auto fixture = steamrot::tests::create_fixture_from_test_data(config);
auto sim_result = steamrot::tests::execute_simulation_with_fixture(
    config->simulation_data(), fixture.value());
```

### Catch2 Generator Pattern

```cpp
TEST_CASE("Data-driven logic tests", "[unit][logic][data-driven]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  INFO("Test: " << config->metadata()->test_name()->str());
  
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

### JSON Schema Reference

See:
- `src/flatbuffers_headers/test_data.fbs` - Main schema
- `src/flatbuffers_headers/simulation.fbs` - Simulation schema
- `src/flatbuffers_headers/input_test_data.fbs` - Input schema
- `src/flatbuffers_headers/event_test_data.fbs` - Event schema

## Timeline

**Total Duration:** 6 weeks

| Stage | Week | Focus | Deliverables |
|-------|------|-------|-------------|
| 0 | 1 | Preparation | Infrastructure, templates, POC |
| 1 | 2 | Simple tests | 4-8 test data files, basic migration |
| 2 | 3 | Multi-step | 8-12 test data files, workflow tests |
| 3 | 4 | Input/Event | 10-15 test data files, input integration |
| 4 | 5 | Edge cases | 8-12 test data files, negative tests |
| 5 | 6 | Cleanup | Final cleanup, optimization, docs |

## References

- [TEST_HARNESS_ADOPTION_PLAN.md](TEST_HARNESS_ADOPTION_PLAN.md) - Overall adoption plan
- [TEST_DATA_CONFIGURATION.md](TEST_DATA_CONFIGURATION.md) - Test data system docs
- [tests/harness/README.md](../tests/harness/README.md) - Test harness API
- [Phase 2.3 in Adoption Plan](TEST_HARNESS_ADOPTION_PLAN.md#23-logic-class-tests-with-simulations) - Relevant section

## Conclusion

This refactoring plan provides a clear, staged approach to migrating logic unit tests to the test harness system. By following this plan, we will:

1. Reduce test code complexity and maintenance burden
2. Enable data-driven testing with better parameterization
3. Leverage the full power of the test harness (simulations, input/event sequences)
4. Maintain test coverage and quality throughout migration
5. Align with the overall test harness adoption strategy

The plan is designed to be incremental, with clear validation criteria at each stage, minimizing risk while maximizing the benefits of the test harness system.
