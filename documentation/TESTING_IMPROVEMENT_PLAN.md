# SteamRot Testing System Improvement Plan

## Executive Summary

This document outlines a comprehensive plan for improving and implementing a better test system for the SteamRot game engine. The plan focuses on:
- **Reusability**: Creating test functions that can be reused across different test scenarios
- **Extensibility**: Building a test infrastructure that easily accommodates new test types
- **Data-Driven Testing**: Enabling easy configuration and feeding of different test data
- **Clear Test Classification**: Establishing naming conventions for unit, integration, and system tests
- **CI/CD Integration**: Automating testing workflows for continuous quality assurance

The implementation is broken down into 5 stages for incremental rollout.

---

## Current State Analysis

### Current Testing Infrastructure

**Strengths:**
- ✅ Uses Catch2 framework (modern, feature-rich)
- ✅ 27 test files covering multiple subsystems
- ✅ TestContext class provides mock dependencies for testing
- ✅ Test helper files exist (`*_test_helpers.h/cpp`)
- ✅ TDD approach documented in workflows
- ✅ Consistent file naming conventions (`.test.cpp` suffix)
- ✅ Tests organized by subsystem (mirrors `src/` structure)

**Current Test Distribution:**
- Logic: 8 test files
- Events: 4 test files  
- Scenes: 4 test files
- Entity: 3 test files
- Components: 2 test files
- User Interface: 2 test files
- Assets: 1 test file
- Display: 1 test file
- Systems: 1 test file
- Integration: 1 test file

**Areas for Improvement:**
- ❌ No clear distinction between unit, integration, and system tests
- ❌ Limited reusability - tests often duplicate setup code
- ❌ No data-driven testing framework
- ❌ CI/CD workflow exists but is disabled (`cmake-multi-platform-disabled.txt`)
- ❌ Test data is hard-coded in test files
- ❌ No test coverage reporting
- ❌ No performance/benchmark tests
- ❌ Limited integration test coverage (only 1 integration test)

---

## Testing Taxonomy and Naming Conventions

### Test Type Definitions

#### 1. Unit Tests
**Definition**: Test a single class or function in isolation with mocked dependencies.

**Naming Convention**: `{ClassName}.unit.test.cpp` or `{function_file}.unit.test.cpp`

**Examples:**
- `EntityManager.unit.test.cpp` - Tests EntityManager class in isolation
- `logic_collision.unit.test.cpp` - Tests collision logic functions
- `ArchetypeUtils.unit.test.cpp` - Tests archetype utility templates

**Current State**: Most existing `.test.cpp` files are actually unit tests but not explicitly named as such.

#### 2. Integration Tests
**Definition**: Test interactions between multiple components/classes, with some real dependencies.

**Naming Convention**: `{Feature}.integration.test.cpp`

**Examples:**
- `entity_loading.integration.test.cpp` - Tests loading entities from FlatBuffers through configurator
- `event_subscription.integration.test.cpp` - Tests event handler + subscriber factory interaction
- `ui_collision_render.integration.test.cpp` - Tests UI collision + rendering logic together

**Current State**: Limited (1 file in `tests/integration/scene_change/`)

#### 3. System Tests (End-to-End)
**Definition**: Test complete workflows through the entire system, minimal mocking.

**Naming Convention**: `{Workflow}.system.test.cpp`

**Examples:**
- `crafting_workflow.system.test.cpp` - Tests complete crafting scene workflow
- `scene_transition.system.test.cpp` - Tests transitioning between scenes
- `player_action.system.test.cpp` - Tests complete player input → action → state change

**Current State**: Not present

#### 4. Performance Tests
**Definition**: Benchmark and performance testing for critical paths.

**Naming Convention**: `{Component}.perf.test.cpp`

**Examples:**
- `EntityManager.perf.test.cpp` - Tests entity pool performance at scale
- `archetype_lookup.perf.test.cpp` - Tests archetype query performance

**Current State**: Not present

#### 5. Data-Driven Tests
**Definition**: Tests that run the same logic with multiple data configurations.

**Naming Convention**: `{Feature}.data.test.cpp` with external data files

**Examples:**
- `component_configuration.data.test.cpp` - Tests with various JSON configs
- `ui_layout.data.test.cpp` - Tests UI with different layout data

**Current State**: Not present (data is hard-coded in tests)

---

## Stage 1: Foundation - Test Classification and Organization

**Goal**: Establish clear test taxonomy without breaking existing tests.

### Tasks

#### 1.1 Create Test Type Documentation
- **Action**: Document test type definitions in this plan (✓ Done above)
- **Files**: This document
- **Effort**: Low (complete)

#### 1.2 Create Directory Structure for Test Types
- **Action**: Reorganize tests into subdirectories by type
- **Structure**:
  ```
  tests/
  ├── unit/                    # Unit tests (isolated)
  │   ├── components/
  │   ├── entity/
  │   ├── logic/
  │   └── ...
  ├── integration/             # Integration tests (2+ components)
  │   ├── entity_loading/
  │   ├── event_system/
  │   └── scene_change/        # Existing
  ├── system/                  # End-to-end tests
  │   ├── scene_workflows/
  │   └── gameplay/
  ├── perf/                    # Performance tests
  │   └── benchmarks/
  ├── data/                    # Test data files (existing)
  └── context/                 # Test utilities (existing)
  ```
- **Migration Strategy**: 
  - Don't rename existing files immediately
  - Add new tests in the new structure
  - Migrate opportunistically
- **Effort**: Medium (1-2 days)
- **Risk**: Low (additive change)

#### 1.3 Update CMakeLists.txt for New Structure
- **Action**: Add subdirectories to test CMake configuration
- **Files**: `tests/CMakeLists.txt`, `tests/unit/CMakeLists.txt`, etc.
- **Effort**: Low (few hours)

#### 1.4 Create Test Type Markers
- **Action**: Use Catch2 tags to mark test types in existing tests
- **Example**:
  ```cpp
  TEST_CASE("EntityManager constructor", "[unit][EntityManager]") { ... }
  TEST_CASE("Entity loading from FlatBuffers", "[integration][entity]") { ... }
  ```
- **Benefit**: Can run specific test types: `ctest -L unit` or `ctest -L integration`
- **Files**: All existing `.test.cpp` files
- **Effort**: Medium (tag all existing tests)

### Deliverables
- ✅ Updated test directory structure (COMPLETED)
- ✅ CMake configuration for new directories (COMPLETED)
- ✅ All tests tagged by type (COMPLETED)
- ✅ Documentation on test classification (COMPLETED)

### Success Metrics
- ✅ Can run `ctest --preset Debug -L unit` to run only unit tests
- ✅ Can run `ctest --preset Debug -L integration` to run only integration tests
- ✅ All existing tests now have appropriate tags
- ✅ No test is unclassified

### Implementation Status: COMPLETE ✅

**Date Completed**: 2025-10-13

**Changes Made**:
1. Created new directory structure: `tests/unit/`, `tests/system/`, `tests/perf/benchmarks/`
2. Added CMakeLists.txt files for all new test directories
3. Updated `tests/CMakeLists.txt` to include new directories
4. Tagged all 27 existing test files with `[unit]` or `[integration]` tags
5. Updated README.md with comprehensive testing documentation and CMake preset usage
6. Updated .github/copilot-instructions.md with test classification guidelines and preset usage
7. All build and test commands now use CMake presets (`--preset Debug`)

**Migration Notes**:
- Existing tests remain in their current subsystem-based locations
- New directories (`unit/`, `system/`, `perf/`) are reserved for future tests
- Migration to new structure will happen opportunistically
- All documentation updated to use CMake presets instead of direct cmake commands

---

## Stage 2: Reusable Test Infrastructure

**Goal**: Create reusable test components to reduce duplication and improve maintainability.

### Tasks

#### 2.1 Expand TestContext for Different Scenarios
- **Action**: Create scenario-specific contexts
- **New Classes**:
  ```cpp
  // tests/context/TestScenarios.h
  namespace steamrot::tests {
    // Provides pre-configured entity setups
    class EntityTestScenario {
    public:
      EntityMemoryPool CreateEmptyPool();
      EntityMemoryPool CreatePoolWithNEntities(size_t n);
      EntityMemoryPool CreatePoolWithComponents(ArchetypeID archetype);
    };
    
    // Provides pre-configured UI test data
    class UITestScenario {
    public:
      UIElement* CreateButtonAt(Position pos);
      UIElement* CreateNestedUIElements(int depth);
    };
  }
  ```
- **Benefit**: Tests can reuse common setups without duplication
- **Files**: `tests/context/TestScenarios.h/cpp`
- **Effort**: Medium (2-3 days)
- **Status**: ✅ COMPLETE

#### 2.2 Create Reusable Assertion Functions
- **Action**: Build domain-specific assertion helpers
- **Example**:
  ```cpp
  // tests/context/test_assertions.h
  namespace steamrot::tests {
    // Verify component configuration
    void AssertComponentConfigured(const Component& component, 
                                   const ComponentData* expected_data);
    
    // Verify archetype membership
    void AssertEntityInArchetype(EntityID id, ArchetypeID archetype,
                                const ArchetypeManager& manager);
    
    // Verify UI element state
    void AssertUIElementState(const UIElement& element,
                             const UIElementData* expected_data);
  }
  ```
- **Benefit**: If class B uses class A, any tests for A can be reused via these helpers
- **Files**: `tests/context/test_assertions.h/cpp`
- **Effort**: Medium (2-3 days)
- **Status**: ✅ COMPLETE

#### 2.3 Create Component Test Mixins
- **Action**: Reusable test suites for components
- **Example**:
  ```cpp
  // tests/components/component_test_mixin.h
  namespace steamrot::tests {
    // Template that tests common Component behavior
    template<typename TComponent>
    class ComponentTestMixin {
    public:
      static void TestDefaultConstruction();
      static void TestComponentRegisterIndex();
      static void TestSerialization();
    };
  }
  
  // Usage in CNewComponent.unit.test.cpp:
  TEST_CASE("CNewComponent follows Component contract", "[unit][CNewComponent]") {
    ComponentTestMixin<CNewComponent>::TestDefaultConstruction();
    ComponentTestMixin<CNewComponent>::TestComponentRegisterIndex();
  }
  ```
- **Benefit**: All components automatically get standard tests
- **Files**: `tests/components/component_test_mixin.h`
- **Effort**: Medium (2-3 days)
- **Status**: ✅ COMPLETE

#### 2.4 Create Logic Test Base
- **Action**: Reusable base for Logic class tests
- **Example**:
  ```cpp
  // tests/logic/logic_test_base.h
  namespace steamrot::tests {
    template<typename TLogic>
    class LogicTestBase {
    protected:
      TestContext context;
      std::unique_ptr<TLogic> logic;
      
      void SetupLogic(SceneType scene);
      void TestConstruction();
      void TestProcessWithEmptyArchetype();
      void TestProcessWithSingleEntity();
    };
  }
  ```
- **Benefit**: Reduces boilerplate in Logic tests
- **Files**: `tests/logic/logic_test_base.h`
- **Effort**: Medium (2 days)
- **Status**: ✅ COMPLETE

### Deliverables
- ✅ TestScenarios class with common setups
- ✅ Reusable assertion functions
- ✅ Component test mixin template
- ✅ Logic test base class
- ✅ Updated README.md with comprehensive TDD documentation
- ✅ Updated copilot-instructions.md with Stage 2 patterns
- ⏸️ Updated existing tests to use new infrastructure (deferred to opportunistic migration)

### Success Metrics
- ✅ Test infrastructure files created and compiling
- ✅ Documentation updated with TDD guidelines and examples
- ⏸️ At least 5 existing test files refactored to use new infrastructure (deferred)
- ⏸️ Test code duplication reduced by >30% (will measure after migration)
- ⏸️ New tests can be written 50% faster (will measure with new tests)
- ✅ Test helpers have their own structure and documentation

### Implementation Status: COMPLETE ✅

**Date Completed**: 2025-10-20

**Changes Made**:
1. Created `tests/context/TestScenarios.h/cpp` with pre-configured entity setups
2. Created `tests/context/test_assertions.h/cpp` with domain-specific assertions
3. Created `tests/components/component_test_mixin.h` template for component contract tests
4. Created `tests/logic/logic_test_base.h` base class for Logic tests
5. Updated `tests/context/CMakeLists.txt` to include new infrastructure files
6. Added comprehensive TDD section to README.md with:
   - TDD workflow explanation
   - Documentation of all reusable test components
   - Detailed usage examples for each component
   - TDD examples for common scenarios (Component and Logic)
   - Best practices and testing checklist
7. Updated `.github/copilot-instructions.md` with Stage 2 testing patterns
8. All infrastructure compiles successfully with g++
9. Existing tests continue to pass

**Migration Notes**:
- New test infrastructure is available for immediate use
- Existing tests remain unchanged for stability
- Migration to use new infrastructure will happen opportunistically as tests are modified
- All new tests should use the reusable infrastructure
- Documentation provides clear examples for adoption

**Files Created**:
- `tests/context/TestScenarios.h` (143 lines)
- `tests/context/TestScenarios.cpp` (118 lines)
- `tests/context/test_assertions.h` (154 lines)
- `tests/context/test_assertions.cpp` (110 lines)
- `tests/components/component_test_mixin.h` (165 lines)
- `tests/logic/logic_test_base.h` (182 lines)

**Documentation Updates**:
- README.md: Added 350+ lines of TDD documentation
- .github/copilot-instructions.md: Added 140+ lines of testing patterns

---

## Stage 3: Data-Driven Testing Framework

**Goal**: Enable tests to run with multiple data configurations without code duplication.

### Tasks

#### 3.1 Create Test Data Configuration System
- **Action**: Define JSON schema for test data configurations
- **Example**:
  ```json
  // tests/data/component_configs/cgrimoire_machina_test_data.json
  {
    "test_cases": [
      {
        "name": "default_configuration",
        "input": {
          "c_grimoire_machina": {
            "data_field": "test_value",
            "numeric_value": 42
          }
        },
        "expected": {
          "m_data_field": "test_value",
          "m_numeric_value": 42
        }
      },
      {
        "name": "edge_case_empty_string",
        "input": {
          "c_grimoire_machina": {
            "data_field": "",
            "numeric_value": 0
          }
        },
        "expected": {
          "m_data_field": "",
          "m_numeric_value": 0
        }
      }
    ]
  }
  ```
- **Files**: `tests/data/test_configs/`, JSON schema file
- **Effort**: Medium (2-3 days)
- **Status**: ✅ COMPLETE

**Implementation Notes (Section 3.1):**
- Created `tests/data/test_configs/` directory structure with subdirectories:
  - `component_configs/` - Component configuration test data
  - `integration_configs/` - Integration test data
  - `ui_configs/` - UI element test data
- Implemented JSON schema in `test_data_schema.json` with validation support
- Created example test data files for five different test scenarios:
  - `cgrimoire_machina_test_data.json` - 5 test cases covering happy path, edge cases, and stress tests
  - `cuser_interface_test_data.json` - 5 test cases covering panels, buttons, nested elements
  - `cmeta_test_data.json` - 3 test cases for simple component configuration
  - `entity_loading_test_data.json` - 4 integration test cases for entity loading workflows
  - `ui_element_factory_test_data.json` - 7 test cases for UI element creation and edge cases
- Documented system in `tests/data/test_configs/README.md` with:
  - Schema structure and field descriptions
  - Usage examples and best practices
  - Common test tags for categorization
  - Directory organization guidelines
  - Validation instructions
  - Future integration with TestDataLoader

**Files Created:**
- `tests/data/test_configs/test_data_schema.json` (60 lines)
- `tests/data/test_configs/README.md` (235 lines)
- `tests/data/test_configs/component_configs/cgrimoire_machina_test_data.json` (89 lines)
- `tests/data/test_configs/component_configs/cuser_interface_test_data.json` (141 lines)
- `tests/data/test_configs/component_configs/cmeta_test_data.json` (37 lines)
- `tests/data/test_configs/integration_configs/entity_loading_test_data.json` (123 lines)
- `tests/data/test_configs/ui_configs/ui_element_factory_test_data.json` (221 lines)

**Total Test Cases**: 24 test cases across 5 test data files covering components, integration, and UI scenarios

**Date Completed**: 2025-10-20

#### 3.2 Create Test Data Loader
- **Action**: Build utility to load test configurations
- **Example**:
  ```cpp
  // tests/context/TestDataLoader.h
  namespace steamrot::tests {
    class TestDataLoader {
    public:
      // Load test cases from JSON
      std::vector<TestCase> LoadTestCases(const std::string& config_file);
      
      // Load specific test data type
      template<typename TData>
      std::vector<TData> LoadTestData(const std::string& config_file);
    };
  }
  ```
- **Files**: `tests/context/TestDataLoader.h/cpp`
- **Effort**: Medium (2-3 days)

#### 3.3 Integrate with Catch2 Generators
- **Action**: Use Catch2's GENERATE for data-driven tests
- **Example**:
  ```cpp
  TEST_CASE("Component configuration with various inputs", 
            "[unit][data-driven][CGrimoireMachina]") {
    TestDataLoader loader;
    auto test_cases = loader.LoadTestCases("cgrimoire_machina_test_data.json");
    
    auto test_case = GENERATE_COPY(from_range(test_cases));
    
    SECTION(test_case.name) {
      // Test with test_case.input
      // Assert test_case.expected
    }
  }
  ```
- **Files**: Existing test files, updated with GENERATE
- **Effort**: High (refactor multiple tests, 3-5 days)

#### 3.4 Create Test Data Generators
- **Action**: Build utilities to generate random but valid test data
- **Example**:
  ```cpp
  // tests/context/TestDataGenerators.h
  namespace steamrot::tests {
    class TestDataGenerator {
    public:
      // Generate random component data
      template<typename TComponentData>
      TComponentData GenerateRandomComponentData();
      
      // Generate valid UI element data
      UIElementData GenerateRandomUIElement();
      
      // Generate entity with N components
      EntityData GenerateEntityWithComponents(std::vector<ComponentType> types);
    };
  }
  ```
- **Benefit**: Property-based testing capabilities
- **Files**: `tests/context/TestDataGenerators.h/cpp`
- **Effort**: Medium (3 days)

### Deliverables
- ✅ JSON schema for test configurations
- ✅ TestDataLoader implementation
- ✅ At least 5 tests converted to data-driven format
- ✅ Test data generator utilities
- ✅ Documentation on creating data-driven tests

### Success Metrics
- Can add new test cases by editing JSON, not code
- Test coverage increases without adding more test files
- At least 10 test cases per data-driven test
- Property-based tests catch edge cases

---

## Stage 4: Integration and System Testing

**Goal**: Expand test coverage to include integration and end-to-end scenarios.

### Tasks

#### 4.1 Create Integration Test Framework
- **Action**: Build utilities for integration testing
- **Example**:
  ```cpp
  // tests/integration/IntegrationTestBase.h
  namespace steamrot::tests {
    class IntegrationTestBase {
    protected:
      // Real AssetManager (not mocked)
      AssetManager asset_manager;
      // Real EventHandler
      EventHandler event_handler;
      // Real EntityManager
      EntityManager entity_manager;
      
      // Setup with real dependencies
      void SetupRealDependencies();
      
      // Load real data files
      void LoadTestScene(const std::string& scene_file);
    };
  }
  ```
- **Files**: `tests/integration/IntegrationTestBase.h/cpp`
- **Effort**: Medium (2-3 days)

#### 4.2 Add Integration Tests for Critical Paths
- **Action**: Write integration tests for key workflows
- **Tests to Add**:
  1. **Entity Loading Integration**
     - Test: FlatBuffers → Configurator → EntityManager → Component setup
     - File: `tests/integration/entity_loading.integration.test.cpp`
  
  2. **Event System Integration**
     - Test: User input → EventHandler → Subscribers → State change
     - File: `tests/integration/event_flow.integration.test.cpp`
  
  3. **Logic Pipeline Integration**
     - Test: LogicFactory → Multiple Logic classes → State updates
     - File: `tests/integration/logic_pipeline.integration.test.cpp`
  
  4. **UI Interaction Integration**
     - Test: Mouse input → UICollisionLogic → UIActionLogic → Event
     - File: `tests/integration/ui_interaction.integration.test.cpp`
  
  5. **Scene Loading Integration**
     - Test: SceneFactory → Entity loading → Logic setup → Archetype population
     - File: `tests/integration/scene_loading.integration.test.cpp`

- **Effort**: High (5-7 days)

#### 4.3 Add System Tests for Game Workflows
- **Action**: Write end-to-end tests for complete workflows
- **Tests to Add**:
  1. **Complete Scene Transition**
     - Test: Title → Crafting scene with full state
     - File: `tests/system/scene_transition.system.test.cpp`
  
  2. **Complete Crafting Workflow**
     - Test: Open crafting → Select items → Craft → Update inventory
     - File: `tests/system/crafting_workflow.system.test.cpp`
  
  3. **Complete Player Action**
     - Test: Keyboard input → Movement → Collision → Position update
     - File: `tests/system/player_movement.system.test.cpp`

- **Effort**: High (5-7 days)

#### 4.4 Create System Test Runner
- **Action**: Build harness for system tests with real rendering
- **Example**:
  ```cpp
  // tests/system/SystemTestRunner.h
  namespace steamrot::tests {
    class SystemTestRunner {
    public:
      // Run a complete game loop iteration
      void RunGameLoopIteration();
      
      // Simulate user input sequence
      void SimulateInputSequence(std::vector<UserInput> inputs);
      
      // Capture and verify game state
      GameState CaptureGameState();
    };
  }
  ```
- **Files**: `tests/system/SystemTestRunner.h/cpp`
- **Effort**: Medium (3-4 days)

### Deliverables
- ✅ IntegrationTestBase infrastructure
- ✅ 5+ integration tests for critical paths
- ✅ 3+ system tests for complete workflows
- ✅ SystemTestRunner for E2E testing
- ✅ Documentation on writing integration/system tests

### Success Metrics
- Integration tests catch real dependency issues
- System tests validate complete workflows
- Integration/system tests run in <10 seconds total
- Critical paths have >80% coverage

---

## Stage 5: CI/CD and Automation

**Goal**: Automate testing with continuous integration and reporting.

### Tasks

#### 5.1 Enable GitHub Actions CI/CD
- **Action**: Activate the existing workflow
- **Files**: 
  - Rename `cmake-multi-platform-disabled.txt` → `.github/workflows/cmake-multi-platform.yml`
  - Update workflow to run on push and PR
- **Configuration**:
  ```yaml
  on:
    push:
      branches: [ "master", "develop" ]
    pull_request:
      branches: [ "master", "develop" ]
  ```
- **Effort**: Low (few hours)

#### 5.2 Add Test Categorization to CI
- **Action**: Run different test types in separate jobs
- **Workflow Jobs**:
  ```yaml
  jobs:
    unit-tests:
      runs-on: ubuntu-latest
      steps:
        - name: Run Unit Tests
          run: ctest -L unit
    
    integration-tests:
      runs-on: ubuntu-latest
      steps:
        - name: Run Integration Tests
          run: ctest -L integration
    
    system-tests:
      runs-on: ubuntu-latest
      steps:
        - name: Run System Tests
          run: ctest -L system
  ```
- **Benefit**: Fast feedback (unit tests fail fast), parallel execution
- **Effort**: Low (few hours)

#### 5.3 Add Test Coverage Reporting
- **Action**: Integrate code coverage tools
- **Tools**: gcov/lcov for C++ coverage
- **Configuration**:
  ```cmake
  # CMakeLists.txt
  option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
  
  if(ENABLE_COVERAGE)
    add_compile_options(--coverage)
    add_link_options(--coverage)
  endif()
  ```
- **Workflow Step**:
  ```yaml
  - name: Generate Coverage Report
    run: |
      cmake -B build -DENABLE_COVERAGE=ON
      cmake --build build
      ctest --test-dir build
      lcov --capture --directory build --output-file coverage.info
      lcov --remove coverage.info '/usr/*' --output-file coverage.info
      lcov --list coverage.info
  
  - name: Upload Coverage to Codecov
    uses: codecov/codecov-action@v3
    with:
      files: coverage.info
  ```
- **Effort**: Medium (1-2 days)

#### 5.4 Add Performance Test Benchmarking
- **Action**: Integrate Google Benchmark or Catch2 benchmarks
- **Example**:
  ```cpp
  // tests/perf/EntityManager.perf.test.cpp
  TEST_CASE("EntityManager performance", "[perf][EntityManager]") {
    BENCHMARK("Create 1000 entities") {
      EntityManager manager;
      for (int i = 0; i < 1000; ++i) {
        manager.CreateEntity();
      }
    };
    
    BENCHMARK("Query archetype with 10000 entities") {
      // Setup
      EntityManager manager;
      // ... populate with entities
      
      return manager.QueryArchetype(archetype_id);
    };
  }
  ```
- **CI Integration**: Track benchmark results over time
- **Effort**: Medium (2-3 days)

#### 5.5 Add Test Result Reporting
- **Action**: Generate HTML test reports
- **Tool**: Catch2 XML output + report generator
- **Workflow Step**:
  ```yaml
  - name: Run Tests with XML Output
    run: ctest --output-on-failure --output-junit test-results.xml
  
  - name: Publish Test Results
    uses: EnricoMi/publish-unit-test-result-action@v2
    if: always()
    with:
      files: test-results.xml
  ```
- **Effort**: Low (few hours)

#### 5.6 Add Pre-commit Hooks
- **Action**: Run tests locally before commit
- **Configuration**: `.pre-commit-config.yaml`
  ```yaml
  repos:
    - repo: local
      hooks:
        - id: unit-tests
          name: Run Unit Tests
          entry: bash -c 'cd build && ctest -L unit'
          language: system
          pass_filenames: false
  ```
- **Benefit**: Catch issues before pushing
- **Effort**: Low (few hours)

#### 5.7 Add Automated Test Generation
- **Action**: Generate test stubs for new classes
- **Tool**: Custom CMake function or script
- **Example**:
  ```cmake
  # cmake/TestGeneration.cmake
  function(generate_test_stub CLASS_NAME)
    configure_file(
      ${CMAKE_SOURCE_DIR}/cmake/templates/unit_test.cpp.in
      ${CMAKE_SOURCE_DIR}/tests/unit/${CLASS_NAME}.unit.test.cpp
    )
  endfunction()
  ```
- **Benefit**: Ensures all classes have test files
- **Effort**: Low-Medium (1-2 days)

### Deliverables
- ✅ Enabled CI/CD pipeline on GitHub Actions
- ✅ Separate CI jobs for unit/integration/system tests
- ✅ Code coverage reporting with Codecov
- ✅ Performance benchmarking in CI
- ✅ Test result HTML reports
- ✅ Pre-commit hooks for local testing
- ✅ Test stub generation scripts

### Success Metrics
- All PRs automatically tested
- Test results visible in GitHub PR checks
- Code coverage visible and tracked
- Performance regressions detected automatically
- 100% of new classes have test stubs
- Tests run in <5 minutes for unit, <15 for all

---

## Additional Testing Best Practices

### Test Doubles Strategy

#### Mocks
- Use for external dependencies (file I/O, network)
- Create mock interfaces for: `AssetManager`, `EventHandler`
- Tool: Can use manual mocks or introduce library like FakeIt

#### Stubs
- Use for returning fixed values
- Example: Stub `PathProvider` to return test paths

#### Fakes
- Use for lightweight implementations
- Example: `FakeEntityMemoryPool` with simplified logic

### Property-Based Testing

Use test data generators to verify properties:

```cpp
TEST_CASE("Component serialization round-trip property", "[unit][property]") {
  auto random_component = GENERATE(take(100, 
    randomComponent<CGrimoireMachina>()));
  
  // Property: serialize then deserialize should equal original
  auto serialized = Serialize(random_component);
  auto deserialized = Deserialize<CGrimoireMachina>(serialized);
  
  REQUIRE(deserialized == random_component);
}
```

### Mutation Testing

Consider adding mutation testing to verify test quality:
- Tool: `mull` for C++
- Verifies tests actually catch bugs by introducing mutations
- Run periodically (weekly), not on every commit

### Test Documentation

Each test file should have:
- File-level comment explaining what is tested
- Section comments explaining test groupings
- Individual test names that describe the scenario

Example:
```cpp
/////////////////////////////////////////////////
/// @file EntityManager.unit.test.cpp
/// @brief Unit tests for EntityManager class
/// 
/// Tests cover:
/// - Entity creation and deletion
/// - Component addition and removal
/// - Archetype management
/// - Edge cases and error handling
/////////////////////////////////////////////////

TEST_CASE("EntityManager creates entities with unique IDs", "[unit][EntityManager]") {
  // Test implementation
}
```

---

## Testing Metrics and KPIs

### Coverage Goals
- **Unit Test Coverage**: >80% of classes
- **Integration Test Coverage**: >60% of critical paths
- **System Test Coverage**: 100% of user workflows
- **Overall Line Coverage**: >70%

### Performance Goals
- **Unit Tests**: <5 minutes total
- **Integration Tests**: <10 minutes total
- **System Tests**: <15 minutes total
- **Full Suite**: <30 minutes total

### Quality Goals
- **Test Failure Rate**: <5% (flaky tests)
- **Test Maintenance Burden**: <20% of development time
- **Bug Escape Rate**: <10% (bugs not caught by tests)
- **Regression Detection**: >95% (changes breaking tests)

---

## Implementation Timeline

### Phase 1: Foundation (Weeks 1-2)
- **Stage 1**: Test classification and organization
- **Deliverables**: Directory structure, tagging, documentation
- **Effort**: 5-7 days

### Phase 2: Reusability (Weeks 3-4)
- **Stage 2**: Reusable test infrastructure
- **Deliverables**: TestScenarios, assertions, mixins, base classes
- **Effort**: 8-10 days

### Phase 3: Data-Driven (Weeks 5-6)
- **Stage 3**: Data-driven testing framework
- **Deliverables**: Data loader, generators, converted tests
- **Effort**: 8-10 days

### Phase 4: Integration/System (Weeks 7-9)
- **Stage 4**: Integration and system testing
- **Deliverables**: Integration tests, system tests, runners
- **Effort**: 12-15 days

### Phase 5: Automation (Weeks 10-11)
- **Stage 5**: CI/CD and automation
- **Deliverables**: CI pipeline, coverage, benchmarks, hooks
- **Effort**: 7-10 days

**Total Timeline**: ~11 weeks for complete implementation

**Incremental Rollout**: Each stage is independently valuable and can be deployed as completed.

---

## Risk Assessment

### High Risk Items
1. **Breaking Existing Tests**: Mitigate by thorough testing after each change
2. **CI/CD Resource Usage**: Monitor GitHub Actions minutes, optimize test parallelization
3. **Test Maintenance Burden**: Keep tests simple, use reusable infrastructure

### Medium Risk Items
1. **Learning Curve**: Provide good documentation and examples
2. **Performance Overhead**: Monitor test execution time, optimize slow tests
3. **Flaky Tests**: Implement retry logic, fix flaky tests immediately

### Low Risk Items
1. **Directory Reorganization**: Can be done gradually
2. **Adding Tags**: Non-breaking change
3. **Documentation**: Low risk, high value

---

## Success Criteria

This plan will be considered successful when:

✅ **Reusability**: New tests reuse existing test infrastructure 80% of the time
✅ **Extensibility**: Adding a new test type takes <1 hour
✅ **Classification**: All tests clearly marked as unit/integration/system
✅ **Data-Driven**: >20% of tests use data-driven approach
✅ **CI/CD**: All commits tested automatically within 15 minutes
✅ **Coverage**: >70% overall code coverage maintained
✅ **Quality**: <5% test failure rate from flakiness
✅ **Developer Experience**: Developers prefer TDD with new infrastructure

---

## Maintenance Plan

### Ongoing Activities
- **Weekly**: Review test failures and fix flaky tests
- **Monthly**: Review test coverage and identify gaps
- **Quarterly**: Review test performance and optimize slow tests
- **Yearly**: Evaluate testing tools and frameworks for upgrades

### Ownership
- **Test Infrastructure**: Core team maintains TestContext, helpers, base classes
- **Unit Tests**: Developer who wrote the code maintains unit tests
- **Integration Tests**: Feature owner maintains integration tests
- **CI/CD**: DevOps/Build engineer maintains pipelines

---

## Appendix A: Tool Recommendations

### Testing Frameworks
- **Catch2** (current): ✅ Keep - modern, header-only, feature-rich
- **Google Test**: ❌ Not needed - Catch2 is sufficient
- **Doctest**: ❌ Not needed - similar to Catch2

### Mocking Frameworks
- **FakeIt**: ⭐ Consider adding - modern, header-only C++ mocking
- **Google Mock**: ⚠️ Alternative - but requires Google Test
- **Trompeloeil**: ⚠️ Alternative - header-only but complex syntax

### Coverage Tools
- **lcov/gcov**: ✅ Recommended - standard for C++, works with GCC
- **llvm-cov**: ⚠️ Alternative - for Clang builds
- **Codecov**: ✅ Recommended - free for open source, good reports

### Benchmarking
- **Catch2 Benchmarking**: ✅ Recommended - already have Catch2
- **Google Benchmark**: ⚠️ Alternative - more features but extra dependency
- **nanobench**: ⚠️ Alternative - header-only, lightweight

### CI/CD
- **GitHub Actions**: ✅ Recommended - integrated, free for public repos
- **Travis CI**: ❌ Not recommended - less features than Actions
- **CircleCI**: ❌ Not recommended - overkill for this project

### Static Analysis (Bonus)
- **clang-tidy**: ⭐ Recommended - catches common bugs
- **cppcheck**: ⭐ Recommended - additional static analysis
- **SonarQube**: ⚠️ Consider - comprehensive but resource-heavy

---

## Appendix B: Example Test Templates

### Unit Test Template
```cpp
/////////////////////////////////////////////////
/// @file ClassName.unit.test.cpp
/// @brief Unit tests for ClassName
///
/// Tests cover:
/// - Constructor and initialization
/// - Public methods with various inputs
/// - Edge cases and error conditions
/////////////////////////////////////////////////

#include "ClassName.h"
#include "test_assertions.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ClassName default construction", "[unit][ClassName]") {
  steamrot::ClassName instance;
  REQUIRE_NOTHROW(instance);
}

TEST_CASE("ClassName method behavior", "[unit][ClassName]") {
  steamrot::ClassName instance;
  
  SECTION("with valid input") {
    auto result = instance.Method(valid_input);
    REQUIRE(result.has_value());
  }
  
  SECTION("with invalid input") {
    auto result = instance.Method(invalid_input);
    REQUIRE_FALSE(result.has_value());
  }
}
```

### Integration Test Template
```cpp
/////////////////////////////////////////////////
/// @file feature_name.integration.test.cpp
/// @brief Integration tests for FeatureName
///
/// Tests interaction between:
/// - ComponentA and ComponentB
/// - Real dependencies with minimal mocking
/////////////////////////////////////////////////

#include "IntegrationTestBase.h"
#include <catch2/catch_test_macros.hpp>

class FeatureIntegrationTest : public steamrot::tests::IntegrationTestBase {
protected:
  void SetUp() {
    SetupRealDependencies();
    LoadTestScene("test_scene.json");
  }
};

TEST_CASE_METHOD(FeatureIntegrationTest, 
                 "Feature workflow", 
                 "[integration][feature]") {
  SetUp();
  
  // Exercise the integration
  auto result = ExecuteWorkflow();
  
  // Verify integrated behavior
  REQUIRE(result.has_value());
  // Additional assertions
}
```

### Data-Driven Test Template
```cpp
/////////////////////////////////////////////////
/// @file component.data.test.cpp
/// @brief Data-driven tests for Component configuration
/////////////////////////////////////////////////

#include "TestDataLoader.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("Component configuration with test data", 
          "[unit][data-driven][Component]") {
  steamrot::tests::TestDataLoader loader;
  auto test_cases = loader.LoadTestCases("component_test_data.json");
  
  auto test_case = GENERATE_COPY(from_range(test_cases));
  
  SECTION(test_case.name) {
    // Configure component with test_case.input
    // Verify against test_case.expected
  }
}
```

---

## Appendix C: Migration Checklist

When migrating existing tests to new infrastructure:

### Per-Test File Migration
- [ ] Add appropriate Catch2 tags (`[unit]`, `[integration]`, etc.)
- [ ] Move to appropriate directory (unit/integration/system)
- [ ] Update CMakeLists.txt for new location
- [ ] Replace duplicated setup with TestScenarios
- [ ] Replace custom assertions with reusable assertion functions
- [ ] Add file-level documentation comment
- [ ] Verify tests still pass
- [ ] Update test count in documentation

### Per-Subsystem Migration
- [ ] Identify common test patterns
- [ ] Create subsystem-specific test helpers if needed
- [ ] Convert at least one test to data-driven format
- [ ] Add integration test for subsystem
- [ ] Document subsystem testing approach
- [ ] Update subsystem README

### Project-Wide Migration
- [ ] All tests tagged by type
- [ ] CI/CD pipeline running all test types
- [ ] Coverage reporting enabled
- [ ] Test documentation updated
- [ ] Developer guide updated with examples
- [ ] Migration considered complete

---

## Conclusion

This comprehensive testing improvement plan provides a structured approach to enhancing the SteamRot game engine's test infrastructure. By implementing the plan in stages, the team can realize incremental benefits while minimizing risk and disruption.

The focus on reusability, extensibility, and data-driven testing will:
- Reduce test maintenance burden
- Increase test coverage
- Improve developer productivity
- Catch bugs earlier in development
- Provide confidence in refactoring and changes

The plan is designed to be practical and achievable, with clear deliverables, success metrics, and timelines for each stage.

---

**Document Version**: 1.0  
**Last Updated**: 2025-10-11  
**Author**: GitHub Copilot Agent  
**Status**: Ready for Review and Implementation
