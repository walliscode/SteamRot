# Tests Directory Analysis: Data-Driven vs Code-Embedded Tests

[← Back to Documentation](../README.md) | [Testing Overview](TESTING_OVERVIEW.md) | [Test Data Configuration](TEST_DATA_CONFIGURATION.md)

This document provides a comprehensive analysis of the SteamRot tests directory, identifying which tests can be converted to data-driven approaches and which should remain code-embedded.

---

## Table of Contents

- [Executive Summary](#executive-summary)
- [Analysis Methodology](#analysis-methodology)
- [Unit Tests Analysis](#unit-tests-analysis)
  - [Components](#components)
  - [Logic](#logic)
  - [Entity](#entity)
  - [Events](#events)
  - [User Interface](#user-interface)
  - [Scenes](#scenes)
  - [Assets](#assets)
  - [Systems](#systems)
  - [Display](#display)
  - [Data Handlers](#data-handlers)
  - [Resources](#resources)
- [Integration Tests Analysis](#integration-tests-analysis)
- [Test Harness Analysis](#test-harness-analysis)
- [Recommendations Summary](#recommendations-summary)
- [Migration Priority](#migration-priority)

---

## Executive Summary

### Current State

The SteamRot test suite consists of approximately:
- **Unit Tests**: ~50+ test files across 11 subsystems
- **Integration Tests**: 2 test categories (scene_change, context_configuration)
- **Test Harness**: Comprehensive data-driven infrastructure already in place

### Key Findings

| Category | Can Switch to Data-Driven | Keep Code-Embedded | Already Data-Driven |
|----------|---------------------------|--------------------|--------------------|
| Components | 5 files | 0 files | 0 files |
| Logic | 1 file | 5 files | 1 file |
| Entity | 0 files | 4 files | 1 file |
| Events | 2 files | 4 files | 0 files |
| User Interface | 1 file | 0 files | 0 files |
| Scenes | 2 files | 2 files | 0 files |
| Assets | 0 files | 1 file | 0 files |
| Systems | 0 files | 1 file | 0 files |
| Display | 0 files | 1 file | 0 files |
| Integration | 1 file | 1 file | 0 files |

---

## Analysis Methodology

Tests were evaluated based on these criteria:

### Criteria for Data-Driven Testing

1. **State Validation**: Tests that verify entity/component states before and after operations
2. **Configuration Testing**: Tests that validate proper configuration from data
3. **Input/Output Mapping**: Tests with clear input states and expected output states
4. **Parameterizable Values**: Tests that vary only in input/expected values
5. **Simulation Scenarios**: Tests that can be expressed as simulation steps
6. **Repeatable Patterns**: Tests that follow similar patterns across different data

### Criteria for Code-Embedded Testing

1. **Complex Logic Flow**: Tests requiring programmatic control flow
2. **Dynamic Dependencies**: Tests with runtime-determined dependencies
3. **Mock/Stub Requirements**: Tests needing mock objects or behavior verification
4. **Error Path Testing**: Tests verifying exception handling and error modes
5. **API Contract Testing**: Tests validating method signatures and return types
6. **Performance Testing**: Tests measuring execution time or resource usage
7. **Visual Confirmation**: Tests requiring human verification ([.visual] tests)

---

## Unit Tests Analysis

### Components

**Location**: `tests/unit/components/`

#### Files Analysis

| File | Current Approach | Recommendation | Rationale |
|------|------------------|----------------|-----------|
| `CGrimoireMachina.test.cpp` | Code-embedded | **Switch to Data-Driven** | Tests default values - perfect for JSON comparison |
| `CMachinaForm.test.cpp` | Code-embedded | **Switch to Data-Driven** | Tests default values - can express as entity state |
| `CMeta.test.cpp` | Code-embedded | **Switch to Data-Driven** | Tests default values - JSON compatible |
| `CUIState.test.cpp` | Code-embedded | **Switch to Data-Driven** | Tests default values - state validation |
| `CUserInterface.test.cpp` | Code-embedded | **Switch to Data-Driven** | Tests default values - entity state comparison |

#### Current Pattern (Code-Embedded)

```cpp
TEST_CASE("CGrimoireMachina has correct default values", "[unit][Components]") {
  steamrot::CGrimoireMachina grimoire;
  REQUIRE(grimoire.m_active == false);
  REQUIRE(grimoire.m_all_fragments.empty());
  REQUIRE(grimoire.m_all_joints.empty());
}
```

#### Recommended Pattern (Data-Driven)

```json
{
  "metadata": {
    "test_name": "grimoire_machina_default_values",
    "description": "Verify CGrimoireMachina has correct default values",
    "tags": ["unit", "component", "CGrimoireMachina"]
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 1,
    "entities": [{
      "index": 0,
      "c_grimoire_machina": {
        "fragments": [],
        "joints": []
      }
    }]
  },
  "expected_entity_collection": { /* same as start */ }
}
```

**Benefits**:
- Easy to add new component default value tests
- Consistent with existing test data infrastructure
- No code changes needed for new test cases

---

### Logic

**Location**: `tests/unit/logic/`

#### Files Analysis

| File | Current Approach | Recommendation | Rationale |
|------|------------------|----------------|-----------|
| `logic_data_driven.test.cpp` | Data-driven | **Keep Data-Driven** | Already uses test harness correctly |
| `collision.test.cpp` | Code-embedded (GENERATE) | **Switch to Data-Driven** | Parameterized tests perfect for JSON |
| `Logic.test.cpp` | Code-embedded | **Keep Code-Embedded** | Tests API contracts, mock dependencies |
| `LogicFactory.test.cpp` | Code-embedded | **Keep Code-Embedded** | Tests factory behavior, dynamic_cast checks |
| `draw_ui_elements.test.cpp` | Code-embedded | **Keep Code-Embedded** | Visual tests, pixel validation |
| `ui_helpers.test.cpp` | Code-embedded | **Keep Code-Embedded** | Tests utility function return values, requires programmatic verification |

#### Already Data-Driven Example

```cpp
// logic_data_driven.test.cpp - KEEP AS IS
TEST_CASE("Logic classes data-driven tests", "[unit][logic][data-driven]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  auto result = steamrot::tests::RunFixtureTest(config);
}
```

#### Should Convert to Data-Driven

```cpp
// collision.test.cpp - CURRENT (Code-embedded with GENERATE)
TEST_CASE("IsMouseOverBounds returns correct results", "[unit][collision]") {
  auto cases = GENERATE(
      TestCase{{50, 50}, sf::FloatRect({0, 0}, {100, 100}), true},
      TestCase{{0, 0}, sf::FloatRect({0, 0}, {100, 100}), true},
      // ... more cases
  );
}
```

**Recommended**: Convert to JSON test data files where each test case becomes a separate JSON file or use tick snapshots for collision state validation.

#### Must Keep Code-Embedded

```cpp
// Logic.test.cpp - KEEP CODE-EMBEDDED
TEST_CASE("Logic::AddSubscriber adds a subscriber", "[unit][Logic]") {
  steamrot::tests::TestFixture test_context;
  steamrot::UIRenderLogic logic(test_context.GetSceneContext());
  auto subscriber = std::make_shared<steamrot::Subscriber>(...);
  logic.AddSubscriber(subscriber);
  // API contract testing - requires mock objects
}
```

**Reason**: Tests mock object interactions, API contracts, and method behavior verification.

---

### Entity

**Location**: `tests/unit/entity/`

#### Files Analysis

| File | Current Approach | Recommendation | Rationale |
|------|------------------|----------------|-----------|
| `FlatbuffersConfigurator.test.cpp` | Data-driven | **Keep Data-Driven** | Already uses test harness |
| `ArchetypeManager.test.cpp` | Code-embedded | **Keep Code-Embedded** | Tests archetype generation API; state validation helper already abstracts complexity |
| `EntityManager.test.cpp` | Code-embedded | **Keep Code-Embedded** | API testing, error handling |
| `archetype_helpers.test.cpp` | Code-embedded | **Keep Code-Embedded** | Utility function testing |
| `entity_test_helpers.test.cpp` | Code-embedded | **Keep Code-Embedded** | Test helper verification |

#### Data-Driven Entity Tests (Current)

The `data/` subdirectory already contains test data files:
- `configurator_basic.test_data.json`
- `pool_comparison_equal.test_data.json`
- `pool_comparison_different_size.test_data.json`
- `pool_comparison_different_values.test_data.json`

#### ArchetypeManager Tests (Keep Code-Embedded)

```cpp
// ArchetypeManager.test.cpp - KEEP CODE-EMBEDDED
TEST_CASE("ArchetypeManager generates archetype IDs correctly", "[unit]") {
  // Setup code requires runtime context
  steamrot::tests::TestFixture test_context;
  steamrot::EntityManager entity_manager{...};
  
  // API behavior verification requires programmatic control
  auto generate_result = archetype_manager.GenerateAllArchetypes();
  
  // State validation uses existing test helper
  steamrot::tests::TestArchetypesOfConfiguredEMPfromDefaultData(
      archetypes, steamrot::SceneType_TEST);
}
```

**Rationale**: The ArchetypeManager tests verify API behavior (archetype generation), not just state validation. The existing test helper (`TestArchetypesOfConfiguredEMPfromDefaultData`) already abstracts the validation logic, making data-driven conversion unnecessary.

---

### Events

**Location**: `tests/unit/events/`

#### Files Analysis

| File | Current Approach | Recommendation | Rationale |
|------|------------------|----------------|-----------|
| `EventHandler.test.cpp` | Code-embedded | **Keep Code-Embedded** | Tests subscriber registration, pointer lifetime, API contracts |
| `Subscriber.test.cpp` | Code-embedded | **Keep Code-Embedded** | API contract testing |
| `SubscriberFactory.test.cpp` | Code-embedded | **Keep Code-Embedded** | Factory pattern testing |
| `UserInputBitset.test.cpp` | Code-embedded | **Switch to Data-Driven** | State validation tests |
| `event_bus_conversion.test.cpp` | Code-embedded | **Switch to Data-Driven** | Conversion tests are parameterizable |
| `event_factory.test.cpp` | Code-embedded | **Keep Code-Embedded** | Factory behavior testing |

#### EventHandler Tests (Keep Code-Embedded)

Most EventHandler tests verify API behavior rather than state:
- `RegisterSubscriber` - Tests pointer management and weak_ptr behavior
- `DecrementEventLifetimes` - Tests event bus mutation
- `UpdateSubscribers` - Tests subscriber activation logic
- Shared pointer lifetime verification

These require programmatic control and cannot be expressed as state comparisons.

#### Should Convert to Data-Driven

The event sequence testing already supported by the test harness makes these tests ideal candidates:

```cpp
// EventHandler.test.cpp - CONVERT THESE PARTS
TEST_CASE("AddEvent adds an event to an EventBus", "[unit][EventHandler]") {
  steamrot::EventHandler event_handler;
  steamrot::EventPacket event{5};
  event_handler.AddEvent(event);
  event_handler.ProcessWaitingRoomEventBus();
  REQUIRE(event_handler.GetGlobalEventBus().size() == 1);
}
```

**Recommended JSON Pattern**:
```json
{
  "metadata": { "test_name": "add_event_to_bus" },
  "event_sequence": {
    "events": [{
      "tick": 1,
      "event_packet": { "event_lifetime": 5, "event_type": "EVENT_TEST" }
    }]
  },
  "tick_snapshots": [{
    "tick": 1,
    "engine_state": {
      "event_bus": { "size": 1 }
    }
  }]
}
```

---

### User Interface

**Location**: `tests/unit/user_interface/`

#### Files Analysis

| File | Current Approach | Recommendation | Rationale |
|------|------------------|----------------|-----------|
| `UIElementFactory.test.cpp` | Code-embedded | **Switch to Data-Driven** | Configuration tests are parameterizable |
| Test helpers (*.cpp, *.h) | N/A | **Keep as helpers** | Support code for tests |

#### Should Convert to Data-Driven

```cpp
// UIElementFactory.test.cpp - CONVERT
TEST_CASE("UIElementFactory::CreateUIElement - Panel", "[UIElementFactory]") {
  // Panel configuration can be expressed in JSON
  // Expected properties can be validated via entity comparison
}
```

**Recommended**: Create JSON files for different UIElement configurations and validate resulting entity states.

---

### Scenes

**Location**: `tests/unit/scenes/`

#### Files Analysis

| File | Current Approach | Recommendation | Rationale |
|------|------------------|----------------|-----------|
| `SceneFactory.test.cpp` | Code-embedded | **Keep Code-Embedded** | Tests factory creation, uses dynamic_cast for type verification |
| `SceneManager.test.cpp` | Code-embedded | **Keep Code-Embedded** | Manager behavior testing |
| `TitleScene.test.cpp` | Code-embedded | **Switch to Data-Driven** | Scene configuration validation |
| `CraftingScene.test.cpp` | Code-embedded | **Switch to Data-Driven** | Scene configuration validation |
| Test helpers (*.cpp, *.h) | N/A | **Keep as helpers** | Support code |

#### SceneFactory Tests (Keep Code-Embedded)

```cpp
// SceneFactory.test.cpp - KEEP CODE-EMBEDDED
TEST_CASE("SceneFactory can create a TitleScene", "[unit][SceneFactory]") {
  // Factory behavior verification
  auto scene_creation_result = scene_factory.CreateDefaultScene(...);
  
  // Type verification requires dynamic_cast
  REQUIRE(dynamic_cast<steamrot::TitleScene *>(title_scene.get()));
  
  // Configuration check delegated to helper
  steamrot::tests::CheckDefaultSceneConfiguration(*title_scene);
}
```

**Rationale**: Factory tests verify runtime type creation using dynamic_cast, which cannot be expressed in JSON.

#### Scene Tests Suitable for Data-Driven

Scene configuration and state validation:
```json
{
  "metadata": { "test_name": "title_scene_configuration" },
  "starting_engine_state": {
    "scene_manager_data": {
      "scene_data": [{ "scene_type": "TITLE", "scene_id": "title" }]
    }
  },
  "expected_entity_collection": { /* expected scene entities */ }
}
```

---

### Assets

**Location**: `tests/unit/assets/`

| File | Current Approach | Recommendation | Rationale |
|------|------------------|----------------|-----------|
| `AssetManager.test.cpp` | Code-embedded | **Keep Code-Embedded** | Tests file system operations, resource loading |

**Reason**: Asset loading tests verify file system access and resource management which require runtime verification.

---

### Systems

**Location**: `tests/unit/systems/`

| File | Current Approach | Recommendation | Rationale |
|------|------------------|----------------|-----------|
| `GameEngine.test.cpp` | Code-embedded | **Keep Code-Embedded** | Engine lifecycle, subscriber management |

**Reason**: GameEngine tests verify:
- Initialization behavior
- Frame counting
- Subscriber registration/processing
- Game loop execution

These require programmatic control and verification.

---

### Display

**Location**: `tests/unit/display/`

| File | Current Approach | Recommendation | Rationale |
|------|------------------|----------------|-----------|
| `DisplayManager.test.cpp` | Code-embedded | **Keep Code-Embedded** | Window management, render cycle |

**Reason**: Display tests verify SFML window and rendering behavior which requires runtime execution.

---

### Data Handlers

**Location**: `tests/unit/data_handlers/`

| File | Current Approach | Recommendation | Rationale |
|------|------------------|----------------|-----------|
| `FlatbuffersDataLoader.test.cpp.in` | Template | **Keep Code-Embedded** | File loading validation |
| `PathProvider.test.cpp.in` | Template | **Keep Code-Embedded** | Path resolution testing |

**Reason**: These test file system operations and path resolution which are inherently runtime-dependent.

---

## Integration Tests Analysis

**Location**: `tests/integration/`

### Scene Change Tests

| File | Current Approach | Recommendation | Rationale |
|------|------------------|----------------|-----------|
| `scene_change.test.cpp` | Mostly empty | **Switch to Data-Driven** | Scene transitions perfect for tick-based testing |

**Recommended**: Use multi-tick simulations with event sequences to test scene transitions.

### Context Configuration Tests

| File | Current Approach | Recommendation | Rationale |
|------|------------------|----------------|-----------|
| `context_configuration.integration.test.cpp` | Code-embedded | **Keep Code-Embedded** | Configuration loading and validation |

**Reason**: Tests verify FlatBuffers data loading and resource configuration - requires runtime verification.

---

## Test Harness Analysis

**Location**: `tests/harness/`

The test harness is well-designed and already supports:

| Feature | Status | Coverage |
|---------|--------|----------|
| Entity state comparison | ✅ Implemented | Full |
| Simulation steps | ✅ Implemented | Logic classes, functions |
| Event sequences | ✅ Implemented | Full |
| Input sequences | ✅ Implemented | Mouse, keyboard |
| Tick-based execution | ✅ Implemented | Multi-tick support |
| Event bus snapshots | ✅ Implemented | Full |

### Existing Test Data Files

The harness already has comprehensive test data:
- `harness_basic_001-003.test_data.json`
- `harness_event_001.test_data.json`
- `harness_input_001.test_data.json`
- `harness_simulation_001-002.test_data.json`
- `harness_tick_001.test_data.json`
- `harness_workflow_001-002.test_data.json`
- Multiple sample and negative test files

---

## Recommendations Summary

### High Priority Conversions

1. **Component Tests** (All 5 files)
   - Simple state validation
   - Direct JSON mapping to component properties
   - Easy migration path

2. **Collision Tests** (`collision.test.cpp`)
   - Already uses GENERATE pattern
   - Parameterized data fits JSON format perfectly

3. **UI Element Factory Tests** (`UIElementFactory.test.cpp`)
   - Configuration-based testing
   - Entity state validation

### Medium Priority Conversions

4. **Event Conversion Tests** (`UserInputBitset.test.cpp`, `event_bus_conversion.test.cpp`)
   - State validation tests
   - Conversion tests are parameterizable

5. **Scene Tests** (`TitleScene.test.cpp`, `CraftingScene.test.cpp`)
   - Scene configuration validation
   - Entity state verification

6. **Scene Change Integration Tests**
   - Multi-tick scenarios
   - Event-driven transitions

### Keep Code-Embedded

These test categories should remain code-embedded:

1. **API Contract Tests**
   - Logic.test.cpp (subscriber management)
   - EntityManager.test.cpp (API behavior)
   - SubscriberFactory.test.cpp (factory pattern)
   - EventHandler.test.cpp (pointer lifecycle, registration)

2. **Factory Behavior Tests**
   - LogicFactory.test.cpp (dynamic_cast verification)
   - SceneFactory.test.cpp (factory instantiation)
   - event_factory.test.cpp

3. **Visual Tests** ([.visual] tag)
   - draw_ui_elements.test.cpp (pixel validation)

4. **System Tests**
   - GameEngine.test.cpp (lifecycle management)
   - DisplayManager.test.cpp (render cycle)

5. **Resource Loading Tests**
   - AssetManager.test.cpp (file system)
   - FlatbuffersDataLoader tests (path resolution)

6. **Helper/Utility Tests**
   - Test infrastructure validation
   - Helper function verification
   - ui_helpers.test.cpp (utility function return values)

---

## Migration Priority

### Phase 1: Quick Wins (1-2 days)

| Test File | Effort | Impact |
|-----------|--------|--------|
| Component tests (all 5) | Low | High |
| collision.test.cpp | Low | Medium |

### Phase 2: Medium Effort (3-5 days)

| Test File | Effort | Impact |
|-----------|--------|--------|
| UIElementFactory.test.cpp | Medium | High |
| UserInputBitset.test.cpp | Medium | Medium |
| event_bus_conversion.test.cpp | Medium | Medium |
| TitleScene.test.cpp | Medium | Medium |
| CraftingScene.test.cpp | Medium | Medium |

### Phase 3: Longer Term (1-2 weeks)

| Test File | Effort | Impact |
|-----------|--------|--------|
| Scene change integration | High | High |

---

## Implementation Guidelines

### Converting a Test to Data-Driven

1. **Identify the test pattern**:
   - Is it state validation? → Use `start_entity_collection` / `expected_entity_collection`
   - Is it parameterized? → Create multiple JSON files
   - Does it involve simulation? → Use `simulation_data`

2. **Create JSON file**:
   ```bash
   tests/<subsystem>/data/<test_name>.test_data.json
   ```

3. **Update CMakeLists.txt** (if needed):
   - Ensure `data/` directory is included in compilation

4. **Write wrapper test**:
   ```cpp
   TEST_CASE("Data-driven <subsystem> tests", "[unit][<subsystem>][data-driven]") {
     auto configs = steamrot::tests::load_test_data_configs();
     REQUIRE(configs.has_value());
     const auto *config = GENERATE_COPY(from_range(configs.value()));
     auto result = steamrot::tests::RunFixtureTest(config);
     REQUIRE(result.has_value());
   }
   ```

### Creating New Test Data Files

Follow the workflow guide: [FILLING_TEST_DATA.md](../workflows/FILLING_TEST_DATA.md)

### Maintaining Code-Embedded Tests

For tests that remain code-embedded:
- Keep clear documentation explaining why they're code-embedded
- Use existing test fixtures (TestFixture, TestContext)
- Follow existing patterns for mock/stub creation

---

## Appendix: Test File Inventory

### Unit Tests by Subsystem

```
tests/unit/
├── assets/
│   └── AssetManager.test.cpp [KEEP CODE]
├── components/
│   ├── CGrimoireMachina.test.cpp [CONVERT]
│   ├── CMachinaForm.test.cpp [CONVERT]
│   ├── CMeta.test.cpp [CONVERT]
│   ├── CUIState.test.cpp [CONVERT]
│   └── CUserInterface.test.cpp [CONVERT]
├── data_handlers/
│   ├── FlatbuffersDataLoader.test.cpp.in [KEEP CODE]
│   └── PathProvider.test.cpp.in [KEEP CODE]
├── display/
│   └── DisplayManager.test.cpp [KEEP CODE]
├── entity/
│   ├── ArchetypeManager.test.cpp [KEEP CODE]
│   ├── EntityManager.test.cpp [KEEP CODE]
│   ├── FlatbuffersConfigurator.test.cpp [ALREADY DATA-DRIVEN]
│   ├── archetype_helpers.test.cpp [KEEP CODE]
│   └── entity_test_helpers.test.cpp [KEEP CODE]
├── events/
│   ├── EventHandler.test.cpp [KEEP CODE]
│   ├── Subscriber.test.cpp [KEEP CODE]
│   ├── SubscriberFactory.test.cpp [KEEP CODE]
│   ├── UserInputBitset.test.cpp [CONVERT]
│   ├── event_bus_conversion.test.cpp [CONVERT]
│   └── event_factory.test.cpp [KEEP CODE]
├── logic/
│   ├── Logic.test.cpp [KEEP CODE]
│   ├── LogicFactory.test.cpp [KEEP CODE]
│   ├── collision.test.cpp [CONVERT]
│   ├── draw_ui_elements.test.cpp [KEEP CODE - VISUAL]
│   ├── logic_data_driven.test.cpp [ALREADY DATA-DRIVEN]
│   └── ui_helpers.test.cpp [KEEP CODE]
├── scenes/
│   ├── CraftingScene.test.cpp [CONVERT]
│   ├── SceneFactory.test.cpp [KEEP CODE]
│   ├── SceneManager.test.cpp [KEEP CODE]
│   └── TitleScene.test.cpp [CONVERT]
├── systems/
│   └── GameEngine.test.cpp [KEEP CODE]
└── user_interface/
    └── UIElementFactory.test.cpp [CONVERT]
```

### Integration Tests

```
tests/integration/
├── context_configuration/
│   └── context_configuration.integration.test.cpp [KEEP CODE]
└── scene_change/
    └── scene_change.test.cpp [CONVERT]
```

---

## Conclusion

The SteamRot test infrastructure is well-positioned for increased data-driven testing. The existing test harness provides comprehensive support for:

- Entity state comparison
- Simulation execution
- Event/input sequences
- Multi-tick scenarios

**Priority should be given to**:
1. Component default value tests (quick wins)
2. Parameterized tests already using GENERATE
3. Configuration validation tests

**Keep code-embedded for**:
1. API contract tests
2. Factory behavior verification
3. Visual/pixel tests
4. System lifecycle tests

This migration approach allows incremental adoption while maintaining test reliability and coverage.

---

[← Back to Documentation](../README.md) | [Testing Overview](TESTING_OVERVIEW.md) | [Test Data Configuration](TEST_DATA_CONFIGURATION.md)
