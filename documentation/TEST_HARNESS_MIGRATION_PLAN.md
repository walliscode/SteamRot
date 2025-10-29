# Test Harness Migration Plan

## Overview

This document outlines the staged migration strategy for replacing current tests with the test harness framework where applicable, while ensuring visual confirmation tests are properly managed and excluded from default test runs.

## Objectives

1. Create a staged migration plan for replacing current tests with test harness where applicable
2. Ensure test harness unit tests are comprehensive and cover all edge cases
3. Tag visual confirmation tests to exclude from default runs
4. Document the migration strategy and provide examples for future development

## Current State Analysis

### Test Infrastructure

- **Total Test Files**: 37 test files across unit and integration tests
- **Test Harness Location**: `tests/harness/`
  - `test_data_harness.h/cpp` - Unified API for loading test data
  - `TestFixture.h/cpp` - Resource management for tests
  - `test_data_harness.test.cpp` - Unit tests for the harness
- **Test Data System**: FlatBuffers-based test data configuration in `src/flatbuffers_headers/test_data.fbs`
- **Existing Test Data**: Only `tests/unit/entity/data/` currently has test data files (3 files)

### Visual Confirmation Tests

**14 tests** require manual visual confirmation via `DisplayRenderTexture()`:

**Location**: `tests/unit/logic/`

1. **UIRenderLogic.test.cpp** (3 visual tests):
   - "UIRenderLogic draws default for test scene"
   - "UIRenderLogic draws title scene"
   - "UIRenderLogic draws crafting scene"

2. **draw_ui_elements.test.cpp** (11 visual tests):
   - Various panel and button drawing tests

These tests display rendered output in a window and require human verification of visual correctness.

### Test Harness Capabilities

The test harness provides:

1. **Data Loading**: `load_test_data_configs()` - Load test data from FlatBuffers
2. **Fixture Creation**: `create_fixture_from_test_data()` - Create TestFixture instances
3. **State Comparison**: `run_entity_memory_pool_comparison_test()` - Compare entity states
4. **Unified Workflow**: `run_fixture_test()` - Complete test execution wrapper
5. **Generator Support**: Seamless integration with Catch2 generators for parameterized testing

## Test Categorization

### ✅ Suitable for Harness Migration

#### High Priority - Entity/Component State Tests

These tests verify entity and component state transitions, making them ideal for data-driven testing:

- **`tests/unit/entity/FlatbuffersConfigurator.test.cpp`**
  - Already uses entity pool comparison patterns
  - Can be enhanced with more test data scenarios
  
- **`tests/unit/entity/ArchetypeManager.test.cpp`**
  - Tests archetype generation and management
  - Predictable state changes suitable for comparison
  
- **`tests/unit/entity/EntityManager.test.cpp`**
  - Entity manager state verification
  - Can benefit from fixture-based testing
  
- **`tests/unit/components/component_equality.test.cpp`**
  - Component comparison logic
  - Natural fit for data-driven comparison tests

#### Medium Priority - Logic Tests with Predictable State

Logic tests that have deterministic outcomes without visual requirements:

- **`tests/unit/logic/UICollisionLogic.test.cpp`**
  - UI collision detection (non-visual aspects)
  - State-based collision detection can be verified via entity states
  
- **`tests/unit/logic/UIActionLogic.test.cpp`**
  - UI action processing
  - Action results can be verified through entity state changes
  
- **`tests/unit/logic/UIStateLogic.test.cpp`**
  - UI state management
  - State transitions are predictable and testable
  
- **`tests/integration/scene_change/`**
  - Scene transition tests
  - Entity states before/after scene changes can be compared

#### Low Priority - May Not Benefit from Harness

These tests are simple enough that harness overhead may not be justified:

- Component default value tests (CMeta, CUserInterface, CGrimoireMachina, etc.)
- Simple construction tests
- Getter/setter validation

### ❌ NOT Suitable for Harness Migration

#### Visual Confirmation Tests

Tests requiring manual visual inspection:

- **`tests/unit/logic/UIRenderLogic.test.cpp`** - All 3 tests
- **`tests/unit/logic/draw_ui_elements.test.cpp`** - All 11 tests

**Reason**: These tests verify visual rendering correctness, which requires human judgment. They will be tagged for manual execution instead.

#### Factory Creation Tests

Tests focused on object creation patterns rather than state:

- **`tests/unit/logic/LogicFactory.test.cpp`**
- **`tests/unit/scenes/SceneFactory.test.cpp`**
- **`tests/unit/user_interface/UIElementFactory.test.cpp`**

**Reason**: These test object creation and type verification, not state transitions.

#### Configuration/Style Tests

Tests verifying configuration loading and parsing:

- **`tests/unit/resources/resources_configuration.test.cpp`**
- **`tests/unit/scenes/StylesConfigurator.test.cpp`**

**Reason**: Different testing paradigm focused on file I/O and parsing rather than entity state.

#### Event Handler Tests

Tests using callback-based patterns:

- **`tests/unit/events/EventHandler.test.cpp`**
- **`tests/unit/events/Subscriber.test.cpp`**
- **`tests/unit/events/SubscriberFactory.test.cpp`**

**Reason**: Event-driven testing uses different patterns (callbacks, observers) not well-suited for entity state comparison.

## Migration Strategy

### Phase 1: Test Harness Improvements

**Goal**: Ensure the test harness itself is robust and well-tested before migration.

#### Current Harness Test Coverage

From `tests/harness/test_data_harness.test.cpp`:

- ✅ Basic loading from adjacent directory
- ✅ Loading with Catch2 generators
- ✅ Loading from specific subdirectory
- ✅ Error handling for non-existent directories
- ✅ Basic workflow demonstration
- ✅ Validation with `run_test_data_config`
- ✅ Fixture creation and comparison

#### Additional Tests Needed

**Error Handling Tests**:
- [ ] Null pointer handling in all wrapper functions
- [ ] Missing metadata validation
- [ ] Malformed test data handling
- [ ] Empty entity collections
- [ ] Mismatched pool sizes between start and expected
- [ ] Invalid FlatBuffers data

**Edge Case Tests**:
- [ ] Empty adjacent data directory
- [ ] Test data with only metadata (no entity data)
- [ ] Very large entity pools (performance validation)
- [ ] Nested component configurations
- [ ] Multiple components per entity

**Integration Tests**:
- [ ] Full workflow: load → create fixture → simulate → compare
- [ ] Multiple test data files processed in sequence
- [ ] Cross-subdirectory loading scenarios

**Deliverables**:
- Enhanced `test_data_harness.test.cpp` with comprehensive coverage
- Documentation of test harness limitations and best practices
- Examples demonstrating all harness features

### Phase 2: Visual Test Tagging

**Goal**: Tag all visual confirmation tests and configure CTest to exclude them from default runs.

#### Implementation Steps

1. **Add `[visual]` Tag to Test Cases**

   Update test files to include the `[visual]` tag:
   
   ```cpp
   TEST_CASE("UIRenderLogic draws default for test scene", 
             "[unit][UIRenderLogic][visual]") {
     // Test implementation
   }
   ```

   **Files to Update**:
   - `tests/unit/logic/UIRenderLogic.test.cpp` (3 tests)
   - `tests/unit/logic/draw_ui_elements.test.cpp` (11 tests)

2. **Configure CTest to Exclude Visual Tests**

   **Option A: Tag-based exclusion (Recommended)**
   
   Catch2 automatically discovers tags. Run tests excluding visual:
   ```bash
   ctest --preset Debug -E "visual"
   ```
   
   Run only visual tests:
   ```bash
   ctest --preset Debug -R "visual"
   ```

   **Option B: CTest Labels**
   
   Modify `catch_discover_tests()` in CMakeLists.txt:
   ```cmake
   catch_discover_tests(test_logic
     PROPERTIES LABELS "unit;logic"
   )
   ```

3. **Update Test Presets**

   Modify `CMakePresets.json` to exclude visual tests by default:
   ```json
   {
     "name": "Debug",
     "configurePreset": "Debug",
     "output": {
       "outputOnFailure": true
     },
     "filter": {
       "exclude": {
         "name": ".*\\[visual\\].*"
       }
     }
   }
   ```

#### Documentation Updates

**README.md**:
- Add section: "Running Visual Tests"
- Document tag system and how to run visual tests
- Explain when visual confirmation is needed

**Copilot Instructions**:
- Add guidelines for tagging visual tests
- Document when to create visual vs automated tests

**Test File Comments**:
- Add clear documentation explaining visual test purpose
- Document what visual aspects should be verified

**Deliverables**:
- All 14 visual tests tagged with `[visual]`
- CTest configuration excludes visual tests from default runs
- Documentation updated with visual test execution guide

### Phase 3: Create Test Data for High Priority Tests

**Goal**: Build comprehensive test data files for entity and component state tests.

#### Directory Structure to Create

```
tests/unit/
├── entity/
│   └── data/
│       ├── configurator_basic.test_data.json
│       ├── configurator_complex.test_data.json
│       ├── archetype_single.test_data.json
│       ├── archetype_multiple.test_data.json
│       ├── entity_manager_activation.test_data.json
│       └── entity_manager_lifecycle.test_data.json
├── components/
│   └── data/  (NEW)
│       ├── component_equality_simple.test_data.json
│       ├── component_equality_complex.test_data.json
│       └── component_state_transitions.test_data.json
└── logic/
    └── data/  (NEW)
        ├── ui_collision_basic.test_data.json
        ├── ui_collision_overlap.test_data.json
        ├── ui_action_click.test_data.json
        ├── ui_action_hover.test_data.json
        ├── ui_state_visible.test_data.json
        └── ui_state_transitions.test_data.json
```

#### Test Data Scenarios

**Entity/FlatbuffersConfigurator**:
- Basic single entity configuration
- Complex multi-entity with multiple components
- Edge cases: empty entities, inactive components
- Error scenarios: missing required data

**ArchetypeManager**:
- Single archetype generation
- Multiple archetypes with different component combinations
- Archetype updates after component activation/deactivation

**EntityManager**:
- Entity activation/deactivation lifecycle
- Entity creation and destruction
- Archetype regeneration scenarios

**Component Equality**:
- Simple component comparisons
- Complex component with nested data
- Component state transitions

**Logic Tests**:
- UI collision detection scenarios
- UI action processing (clicks, hovers)
- UI state management (visibility, active/inactive)

**Deliverables**:
- Minimum 15-20 new test data files
- Each file documents a specific test scenario
- All test data follows FlatBuffers schema correctly

### Phase 4: Migrate High Priority Tests

**Goal**: Migrate entity and component state tests to use test harness.

#### Migration Pattern

**Before (Traditional Test)**:
```cpp
TEST_CASE("Configure entities from default data", "[unit][entity]") {
  EntityMemoryPool pool;
  // Manual pool setup
  std::apply([size](auto &...vec) { (vec.resize(size), ...); }, pool);
  
  FlatbuffersConfigurator configurator{...};
  auto result = configurator.ConfigureEntitiesFromDefaultData(pool, ...);
  
  // Manual verification
  REQUIRE(result.has_value());
  // Manual state checks
}
```

**After (Harness-Based Test)**:
```cpp
TEST_CASE("Configure entities with harness", "[unit][entity][data-driven]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

#### Migration Steps per Test File

1. **Keep Original Tests**
   - Do not remove original tests initially
   - Add harness-based tests alongside existing tests
   - Validate both produce same results

2. **Create Test Data**
   - Create `.test_data.json` files for each test scenario
   - Place in appropriate `data/` subdirectory
   - Build to generate `.test_data.bin` files

3. **Add Harness-Based Tests**
   - Add new test cases using harness patterns
   - Use Catch2 generators for parameterized testing
   - Tag with `[data-driven]` for identification

4. **Validate Equivalence**
   - Run both original and harness tests
   - Verify same coverage and behavior
   - Document any differences

5. **Deprecate Original (Optional)**
   - After validation, original tests can be removed
   - Or keep for reference/documentation

#### Files to Migrate

1. **`tests/unit/entity/FlatbuffersConfigurator.test.cpp`**
   - Priority: Highest
   - Complexity: Medium
   - Test data: 3-5 scenarios
   
2. **`tests/unit/entity/ArchetypeManager.test.cpp`**
   - Priority: High
   - Complexity: Medium
   - Test data: 4-6 scenarios
   
3. **`tests/unit/entity/EntityManager.test.cpp`**
   - Priority: High
   - Complexity: Low-Medium
   - Test data: 3-4 scenarios
   
4. **`tests/unit/components/component_equality.test.cpp`**
   - Priority: High
   - Complexity: Low
   - Test data: 3-5 scenarios

**Deliverables**:
- 4 test files migrated to use harness
- Original tests preserved (marked as `[legacy]` if needed)
- All tests passing with equivalent coverage

### Phase 5: Migrate Medium Priority Tests

**Goal**: Migrate logic tests with predictable state to use test harness.

#### Logic Test Migration Strategy

Logic tests are more complex because they:
- Require resource initialization (render textures, windows, etc.)
- Involve simulation (running game logic)
- Need specific scene contexts

**Migration Approach**:
1. Use `TestFixture` for resource management
2. Create test data with start/expected entity states
3. Simulate logic execution between start and expected
4. Compare results using harness

**Example Pattern**:
```cpp
TEST_CASE("UI collision with harness", "[unit][logic][data-driven]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // Create fixture from start state
  auto fixture_result = steamrot::tests::create_fixture_from_test_data(config);
  REQUIRE(fixture_result.has_value());
  
  auto &fixture = fixture_result.value();
  
  // Run logic simulation
  UICollisionLogic logic(fixture.GetSceneContext());
  logic.RunLogic();
  
  // Compare with expected state
  if (config->expected_entity_collection()) {
    EntityMemoryPool expected_pool;
    FlatbuffersConfigurator configurator(fixture.GetGameResources().event_handler);
    configurator.ConfigureEntitiesFromCollection(expected_pool, 
                                                  config->expected_entity_collection());
    
    steamrot::tests::run_entity_memory_pool_comparison_test(
        fixture.GetEntityManager().GetEntityMemoryPool(), 
        expected_pool);
  }
}
```

#### Files to Migrate

1. **`tests/unit/logic/UICollisionLogic.test.cpp`**
   - Test collision detection between UI elements
   - Verify component state changes after collision checks
   
2. **`tests/unit/logic/UIActionLogic.test.cpp`**
   - Test action processing (clicks, hovers)
   - Verify event generation and state changes
   
3. **`tests/unit/logic/UIStateLogic.test.cpp`**
   - Test UI state management
   - Verify visibility and active state transitions
   
4. **`tests/integration/scene_change/`**
   - Test scene transitions
   - Verify entity states before/after scene changes

**Deliverables**:
- 4 logic test files migrated
- Test data for logic simulation scenarios
- Documentation of logic testing patterns

### Phase 6: Documentation and Guidelines

**Goal**: Comprehensive documentation for test harness usage and migration patterns.

#### Documentation Updates

**1. `tests/harness/README.md` Enhancements**

Add sections:
- **Migration Guide**: Step-by-step migration examples
- **When to Use Harness**: Decision tree for harness vs traditional tests
- **Common Patterns**: Code examples for common scenarios
- **Troubleshooting**: Solutions to common issues
- **Best Practices**: Guidelines for effective harness usage

**2. Main `README.md` Updates**

Add sections:
- **Visual Test Execution**: How to run visual confirmation tests
- **Test Classification**: Update with `[visual]` and `[data-driven]` tags
- **Harness Usage Examples**: Quick examples for common patterns

**3. `.github/copilot-instructions.md` Updates**

Add sections:
- **Test Harness Guidelines**: When and how to use harness
- **Visual Test Tagging**: Requirements for tagging visual tests
- **Test Data Creation**: Guidelines for creating test data files
- **Migration Patterns**: Examples for migrating existing tests

**4. New Document: `documentation/TEST_HARNESS_USAGE_GUIDE.md`**

Comprehensive guide covering:
- Quick start guide
- Detailed API reference
- Migration examples
- Advanced patterns (simulation, parameterization)
- Performance considerations
- FAQ and troubleshooting

#### Code Examples to Document

**Pattern 1: Simple State Comparison**
```cpp
TEST_CASE("Simple entity state test", "[unit][data-driven]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  auto result = steamrot::tests::run_fixture_test(config);
  REQUIRE(result.has_value());
}
```

**Pattern 2: Logic Simulation**
```cpp
TEST_CASE("Logic simulation test", "[unit][data-driven]") {
  auto configs = steamrot::tests::load_test_data_configs();
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  auto fixture_result = create_fixture_from_test_data(config);
  REQUIRE(fixture_result.has_value());
  
  // Simulate logic
  MyLogic logic(fixture_result.value().GetSceneContext());
  logic.RunLogic();
  
  // Verify results (comparison happens automatically if expected_entity_collection present)
}
```

**Pattern 3: Custom Comparison**
```cpp
TEST_CASE("Custom comparison test", "[unit][data-driven]") {
  auto configs = steamrot::tests::load_test_data_configs();
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  auto fixture_result = create_fixture_from_test_data(config);
  auto &actual_pool = fixture_result.value().GetEntityManager().GetEntityMemoryPool();
  
  // Custom verification logic
  REQUIRE(/* custom checks */);
}
```

**Deliverables**:
- Updated README.md with visual test documentation
- Enhanced harness README with migration guide
- Updated copilot instructions with harness guidelines
- New comprehensive usage guide document
- Code examples for all common patterns

## Implementation Timeline

### Phase 1: Harness Improvements (Week 1)
- Days 1-2: Add error handling tests
- Days 3-4: Add edge case tests
- Day 5: Integration tests and documentation

### Phase 2: Visual Test Tagging (Week 1)
- Day 1: Tag visual tests
- Day 2: Configure CTest exclusion
- Day 3: Update documentation

### Phase 3: Test Data Creation (Week 2)
- Days 1-2: Entity test data
- Days 3-4: Component test data
- Day 5: Logic test data

### Phase 4: High Priority Migration (Week 3)
- Days 1-2: Migrate FlatbuffersConfigurator and ArchetypeManager
- Days 3-4: Migrate EntityManager and component_equality
- Day 5: Validation and cleanup

### Phase 5: Medium Priority Migration (Week 4)
- Days 1-2: Migrate UI logic tests
- Days 3-4: Migrate integration tests
- Day 5: Validation and cleanup

### Phase 6: Documentation (Week 5)
- Days 1-2: Update all documentation
- Days 3-4: Create usage guide and examples
- Day 5: Review and finalization

## Success Criteria

### Test Harness Quality
- [ ] 100% code coverage for harness functions
- [ ] All error conditions tested
- [ ] All edge cases covered
- [ ] Integration scenarios validated

### Visual Test Management
- [ ] All 14 visual tests tagged with `[visual]`
- [ ] CTest excludes visual tests from default runs
- [ ] Documentation explains visual test purpose and execution
- [ ] Visual tests runnable via `ctest -R visual`

### Migration Completeness
- [ ] High priority tests (4 files) migrated with harness equivalents
- [ ] Medium priority tests (4 files) migrated or documented as deferred
- [ ] Original tests preserved until validation complete
- [ ] All migrated tests passing

### Documentation Quality
- [ ] README.md updated with visual test guidance
- [ ] Harness README enhanced with migration guide
- [ ] Copilot instructions include harness guidelines
- [ ] New usage guide document created with examples

### Code Quality
- [ ] All tests follow consistent patterns
- [ ] Test data follows schema correctly
- [ ] No regression in test coverage
- [ ] No increase in test execution time (excluding visual tests)

## Benefits of This Approach

### Incremental Migration
- Tests migrated gradually without breaking existing coverage
- Original tests remain until harness versions validated
- Can be done in phases without blocking other work

### Improved Test Maintainability
- Test data in JSON files easier to modify than C++ code
- Adding new test cases requires only JSON files
- Test logic separated from test data

### Better Test Organization
- Visual tests clearly separated and documented
- Data-driven tests use consistent patterns
- Easy to identify test type via tags

### Enhanced Test Infrastructure
- Robust, well-tested harness ready for future use
- Patterns documented for future development
- Foundation for advanced testing (property-based, simulation)

## Future Enhancements (Post-Migration)

### Simulation Framework
- Add support for multi-step simulations in test data
- Define simulation steps in JSON
- Execute complex workflows from data

### Property-Based Testing
- Generate test data programmatically
- Test properties rather than specific scenarios
- Fuzzing support for robustness testing

### Test Data Generation Tools
- CLI tools to generate test data from templates
- Validation tools for test data files
- Conversion tools for migrating old tests

### Visual Test Automation
- Screenshot capture and comparison
- Automated visual regression testing
- Pixel-perfect rendering verification

### Performance Benchmarking
- Integrate with `tests/perf/benchmarks/`
- Track test execution time trends
- Performance regression detection

## Conclusion

This staged migration plan provides a clear path forward for improving the SteamRot test infrastructure. By enhancing the test harness, properly managing visual tests, and gradually migrating appropriate tests to data-driven approaches, we will achieve:

1. **Better test quality** through comprehensive harness testing
2. **Clearer test organization** through visual test tagging
3. **Easier test maintenance** through data-driven testing
4. **Foundation for future improvements** in testing capabilities

The incremental nature of this plan ensures existing functionality remains intact while progressively improving the test infrastructure.
