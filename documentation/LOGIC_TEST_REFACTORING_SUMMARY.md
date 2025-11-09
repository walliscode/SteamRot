# Logic Test Refactoring - Executive Summary

## Quick Overview

**Objective:** Migrate logic unit tests from manual setup to data-driven tests using the test harness system.

**Timeline:** 6 weeks (Stages 0-5)

**Alignment:** Phase 2.3 of [TEST_HARNESS_ADOPTION_PLAN.md](TEST_HARNESS_ADOPTION_PLAN.md)

## The Change

### From Manual Tests (Current)
```cpp
TEST_CASE("UICollisionLogic test", "[unit]") {
  steamrot::PathProvider pp{steamrot::EnvironmentType::Test};
  steamrot::tests::TestFixture fixture;
  
  // Manual entity setup (20-30 lines)
  steamrot::ButtonElement button;
  button.position = {100.0f, 100.0f};
  button.size = {200.0f, 50.0f};
  // ... more setup
  
  // Manual Logic instantiation
  steamrot::UICollisionLogic logic(fixture.GetSceneContext());
  logic.RunLogic();
  
  // Manual assertions
  REQUIRE(button.is_mouse_over == true);
}
```

### To Data-Driven Tests (Target)
```cpp
TEST_CASE("UICollisionLogic tests", "[unit][data-driven]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

**Plus JSON test data:**
```json
{
  "metadata": {
    "test_name": "ui_collision_basic",
    "tags": ["unit", "logic"]
  },
  "simulation_data": {
    "steps": [{"simulation_type": "Collision", "logic_class_type": "UICollisionLogic"}]
  },
  "start_entity_collection": { /* entities */ },
  "expected_entity_collection": { /* expected state */ }
}
```

## 6-Week Plan

| Week | Stage | Focus | Output |
|------|-------|-------|--------|
| 1 | Stage 0 | Setup & POC | Template + 1-2 migrated tests |
| 2 | Stage 1 | Simple tests | 4-8 test data files |
| 3 | Stage 2 | Multi-step | 8-12 workflow test files |
| 4 | Stage 3 | Input/Events | 10-15 test files with input simulation |
| 5 | Stage 4 | Edge cases | 8-12 negative/edge case test files |
| 6 | Stage 5 | Cleanup | Remove old tests, finalize docs |

## Benefits

1. **Less code** - Test data in JSON, not C++ (~40% reduction)
2. **Easier maintenance** - Add test cases without code changes
3. **Better testing** - Multi-step workflows, input simulation, tick-based execution
4. **Parameterization** - Use Catch2 generators for multiple test cases
5. **Consistency** - Aligned with test harness adoption strategy

## File Organization

```
tests/unit/logic/
├── data/                              # NEW: All test data here
│   ├── ui_collision/
│   │   ├── basic_collision.test_data.json
│   │   └── edge_outside_bounds.test_data.json
│   ├── ui_action/
│   │   ├── button_click.test_data.json
│   │   └── button_with_event.test_data.json
│   └── workflows/
│       └── button_click_workflow.test_data.json
├── UIActionLogic.test.cpp             # UPDATED: Data-driven
├── UICollisionLogic.test.cpp          # UPDATED: Data-driven
└── UIRenderLogic.test.cpp             # UPDATED: Data-driven
```

## Success Criteria

**Per Stage:**
- All tests pass
- Coverage maintained
- Performance acceptable
- Documentation updated

**Overall:**
- ≥90% of logic tests migrated
- ≥40% code reduction
- Zero coverage regression
- Team satisfied with approach

## Risk Mitigation

| Risk | Mitigation |
|------|------------|
| Learning curve | Start simple, provide examples, pair programming |
| Complex tests | Keep manual tests for truly complex cases |
| Migration bugs | Keep old tests initially, validate behavior |
| Performance | Benchmark at each stage, optimize if needed |
| Maintenance | Clear naming, organized structure, documentation |

## Key Resources

- **Full Plan:** [LOGIC_TEST_REFACTORING_PLAN.md](LOGIC_TEST_REFACTORING_PLAN.md)
- **Test Harness API:** [tests/harness/README.md](../tests/harness/README.md)
- **Test Data System:** [TEST_DATA_CONFIGURATION.md](TEST_DATA_CONFIGURATION.md)
- **Overall Strategy:** [TEST_HARNESS_ADOPTION_PLAN.md](TEST_HARNESS_ADOPTION_PLAN.md)

## Quick Start (Stage 0)

1. **Create directory:** `tests/unit/logic/data/`
2. **Create template:** Use examples from full plan
3. **Migrate 1 test:** Pick simplest test (e.g., constructor test)
4. **Validate:** Ensure new test matches old behavior
5. **Review:** Get team feedback before proceeding

## Test Harness Key Functions

```cpp
// Main function - does everything automatically
auto result = steamrot::tests::run_fixture_test(config);

// Manual control if needed
auto configs = steamrot::tests::load_test_data_configs();
auto fixture = steamrot::tests::create_fixture_from_test_data(config);
auto sim_result = steamrot::tests::execute_simulation_with_fixture(
    config->simulation_data(), fixture.value());
```

## Example Test Data Patterns

### Simple Logic Execution
```json
{
  "simulation_data": {
    "steps": [
      {"simulation_type": "Collision", "execution_mode": "LogicClass", 
       "logic_class_type": "UICollisionLogic"}
    ]
  }
}
```

### Multi-Step Workflow
```json
{
  "simulation_data": {
    "steps": [
      {"simulation_type": "Collision", "logic_class_type": "UICollisionLogic"},
      {"simulation_type": "Action", "logic_class_type": "UIActionLogic"}
    ]
  }
}
```

### With Input Simulation
```json
{
  "input_sequence": {
    "inputs": [
      {"input_type": "MouseMove", "input_data": {"position": {"x": 150, "y": 125}}, "tick": 0}
    ]
  },
  "simulation_data": {
    "steps": [
      {"simulation_type": "Collision", "logic_class_type": "UICollisionLogic"}
    ]
  }
}
```

## Questions?

Refer to:
- [Full refactoring plan](LOGIC_TEST_REFACTORING_PLAN.md) - Detailed stages, examples, patterns
- [Test harness README](../tests/harness/README.md) - API documentation
- [Adoption plan Phase 2.3](TEST_HARNESS_ADOPTION_PLAN.md#23-logic-class-tests-with-simulations) - Strategy context

## Status Tracking

Track progress in the full plan document:
- [ ] Stage 0: Preparation
- [ ] Stage 1: Simple tests
- [ ] Stage 2: Multi-step sequences
- [ ] Stage 3: Input/event integration
- [ ] Stage 4: Edge cases
- [ ] Stage 5: Cleanup

Update the checkboxes in [LOGIC_TEST_REFACTORING_PLAN.md](LOGIC_TEST_REFACTORING_PLAN.md) as stages complete.
