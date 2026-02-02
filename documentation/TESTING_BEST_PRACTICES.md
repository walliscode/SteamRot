# Testing Best Practices for SteamRot

## Purpose

This document provides clear guidelines on **when and how to use different testing approaches** in the SteamRot codebase. It addresses the key concern: avoiding inappropriate use of test_data.json for pure unit tests.

## Quick Reference: Which Testing Approach?

| Test Type | Tool/Approach | When to Use | Example |
|-----------|---------------|-------------|---------|
| **Pure Unit Test** | Direct instantiation | Single component/class in isolation | Component construction, single function behavior |
| **Component Configuration** | `entity_test_data.json` | Testing entity/component setup | FlatBuffers → Component mapping |
| **Integration Test** | `test_data.json` (TestDataConfig) | Multiple systems interacting | UI collision + action logic |
| **System Test** | `test_data.json` (TestDataConfig) | End-to-end workflows | Complete user interaction flows |
| **Regression Test** | `test_data.json` (TestDataConfig) | Capturing specific bugs/behaviors | Bug reproduction scenarios |

## The Golden Rule

> **Use the simplest testing approach that validates your specific requirement.**

- Testing component construction? → Direct instantiation
- Testing entity configuration? → entity_test_data.json
- Testing system interactions? → test_data.json with TestEngine
- Testing end-to-end workflow? → test_data.json with TestEngine

## Testing Approaches Explained

### 1. Pure Unit Tests - Direct Instantiation

**What:** Create objects directly in test code, exercise methods, validate results.

**When to Use:**
- Testing a single class/component in isolation
- No dependencies on game systems
- Fast, focused validation

**Characteristics:**
- ✅ Fast execution (no engine simulation)
- ✅ Stable (not affected by game logic changes)
- ✅ Clear failures (one thing tested)
- ✅ Easy to write and maintain

**Example:**

```cpp
TEST_CASE("CUserInterface default construction", "[unit][CUserInterface]") {
  steamrot::CUserInterface component;
  REQUIRE(component.m_active == false);
  REQUIRE(component.m_ui_name.empty());
  REQUIRE(component.m_is_visible == false);
}

TEST_CASE("ArchetypeManager adds entity to correct archetype", "[unit][ArchetypeManager]") {
  steamrot::ArchetypeManager manager;
  steamrot::ArchetypeID archetype_id = 123;
  uint32_t entity_id = 5;
  
  manager.AddEntity(entity_id, archetype_id);
  
  auto &archetype = manager.GetArchetypes()[archetype_id];
  REQUIRE(archetype.Contains(entity_id));
}
```

**When NOT to Use:**
- Testing interactions between systems
- Testing game logic execution
- Testing state changes over time

---

### 2. Component Configuration Tests - entity_test_data.json

**What:** Simple entity data (EntityCollectionFbs) loaded from JSON, configured into components. No simulation or game logic execution.

**When to Use:**
- Testing FlatBuffers → Component configuration
- Testing entity setup and validation
- Testing component field mapping

**Characteristics:**
- ✅ Reusable test data across tests
- ✅ No game simulation overhead
- ✅ Stable (not affected by Logic class changes)
- ⚠️ Still depends on FlatBuffers schema (but that's what's being tested)

**File Location:**
```
tests/unit/<subsystem>/data/entity_test_data.json
```

**File Structure:**
```json
{
  "entity_memory_pool_size": 20,
  "entities": [
    {
      "index": 0,
      "c_user_interface": {
        "ui_name": "test_ui",
        "is_visible": true,
        "root_ui_element": { ... }
      }
    }
  ]
}
```

**Example:**

```cpp
// Helper to load entity test data
auto LoadEntityTestData() {
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  std::filesystem::path data_path = 
    path_provider.GetObjectDirectoryPath(__FILE__) / "data" / "entity_test_data.bin";
  
  // Load and return EntityCollectionFbs
  // ...
}

TEST_CASE("FlatbuffersEntityConfigurator configures CUserInterface", "[unit][configurator]") {
  auto [data, entity_collection] = LoadEntityTestData();
  
  steamrot::EventHandler event_handler;
  steamrot::FlatbuffersEntityConfigurator configurator(event_handler, *entity_collection);
  
  steamrot::EntityMemoryPool pool;
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, 20);
  
  auto result = configurator.ConfigureEntities(pool);
  REQUIRE(result.has_value());
  
  // Validate component was configured correctly
  auto &ui = steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(1, pool);
  REQUIRE(ui.m_ui_name == "test_ui");
  REQUIRE(ui.m_is_visible == true);
}
```

**When NOT to Use:**
- Testing game logic execution
- Testing system interactions
- Testing state changes over multiple ticks

---

### 3. Integration Tests - test_data.json (Simple Scenarios)

**What:** TestDataConfig with TestEngine, simulating limited ticks/systems to test interactions.

**When to Use:**
- Testing 2+ systems working together
- Testing event propagation
- Testing component interactions through Logic classes
- **Still relatively focused** (not full game scenarios)

**Characteristics:**
- ✅ Tests realistic interactions
- ✅ Can validate state changes
- ⚠️ Affected by game logic changes (this is intentional)
- ⚠️ Slower than unit tests (acceptable tradeoff)
- ⚠️ More complex failures (multiple systems involved)

**File Location:**
```
tests/integration/<feature>/data/<test_name>.test_data.json
```

**File Structure:**
```json
{
  "meta_data": {
    "test_name": "ui_collision_action_test",
    "test_description": "Test UI collision detection triggers action logic",
    "tags": ["integration", "ui"],
    "will_pass": true
  },
  "num_ticks": 2,
  "starting_engine_snapshot": {
    "scene_collection_data": {
      "scene_data": [{
        "entity_collection": { /* entities with UI components */ }
      }]
    }
  },
  "simulation_data": {
    "steps": [
      { "logic_class_type": "UICollisionLogic" },
      { "logic_class_type": "UIActionLogic" }
    ]
  },
  "expected_engine_snapshots": {
    "1": { /* expected state after tick 1 */ }
  }
}
```

**Example:**

```cpp
TEST_CASE("UI collision triggers action logic", "[integration][ui]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  // TestEngine simulates game ticks with Logic classes
  auto result = steamrot::tests::RunTestEngineTest(config);
  REQUIRE(result.has_value());
}
```

**When NOT to Use:**
- Pure unit tests (single component)
- Full end-to-end workflows (use system tests)
- Testing components in isolation (use direct instantiation)

---

### 4. System Tests - test_data.json (Complete Workflows)

**What:** TestDataConfig with TestEngine, simulating complete user interactions end-to-end.

**When to Use:**
- Testing complete user workflows
- Testing multi-step game mechanics
- Testing scene transitions
- **Full game simulation** with realistic timing

**Characteristics:**
- ✅ Tests real user experience
- ✅ Catches integration issues
- ✅ Validates complete features
- ⚠️ Affected by any game logic changes (expected)
- ⚠️ Slowest tests (acceptable for system tests)
- ⚠️ Complex failures (entire workflow involved)

**File Location:**
```
tests/system/<workflow>/data/<scenario>.test_data.json
```

**File Structure:**
```json
{
  "meta_data": {
    "test_name": "crafting_workflow_success",
    "test_description": "User successfully crafts an item from title screen to crafting complete",
    "tags": ["system", "crafting"],
    "will_pass": true
  },
  "num_ticks": 10,
  "input_sequence": {
    "inputs": [
      { "input_type": "MouseMove", "tick": 0, "input_data": { ... } },
      { "input_type": "MouseClick", "tick": 1, "input_data": { ... } }
    ]
  },
  "event_sequence": {
    "events": [ /* engine events at specific ticks */ ]
  },
  "simulation_data": {
    "steps": [ /* All relevant Logic classes */ ]
  },
  "starting_engine_snapshot": { /* Initial game state */ },
  "expected_engine_snapshots": {
    "5": { /* Mid-workflow validation */ },
    "10": { /* Final state validation */ }
  }
}
```

**Example:**

```cpp
TEST_CASE("Crafting workflow - complete success path", "[system][crafting]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  auto result = steamrot::tests::RunTestEngineTest(config);
  REQUIRE(result.has_value());
}
```

**When NOT to Use:**
- Unit tests (too heavyweight)
- Quick validation (too slow)
- Component-level tests (use unit tests)

---

### 5. Regression Tests - test_data.json (Bug Reproductions)

**What:** TestDataConfig specifically crafted to reproduce and validate fixes for bugs.

**When to Use:**
- Capturing a specific bug scenario
- Preventing regression of fixed bugs
- Documenting complex failure cases

**Characteristics:**
- ✅ Captures exact conditions of bug
- ✅ Prevents regression
- ✅ Serves as documentation
- ⚠️ May become obsolete if systems change significantly (acceptable)

**File Location:**
```
tests/regression/<issue_number>/data/<bug_name>.test_data.json
```

**Example:**

```json
{
  "meta_data": {
    "test_name": "issue_123_button_click_not_detected",
    "test_description": "Reproduces issue #123 where button clicks were ignored when mouse moved quickly",
    "tags": ["regression", "ui", "issue_123"],
    "will_pass": true
  },
  "num_ticks": 3,
  "input_sequence": {
    "inputs": [
      { "input_type": "MouseMove", "tick": 0, "input_data": { "position": { "x": 100, "y": 100 } } },
      { "input_type": "MouseMove", "tick": 0, "input_data": { "position": { "x": 150, "y": 125 } } },
      { "input_type": "MouseClick", "tick": 1, "input_data": { ... } }
    ]
  },
  "simulation_data": { /* Specific Logic execution order that exposed bug */ },
  "expected_engine_snapshots": { /* Validation that bug is fixed */ }
}
```

## Decision Tree

```
Start: What are you testing?
│
├─ Single component/class behavior?
│  └─ YES → Direct instantiation (Pure Unit Test)
│
├─ FlatBuffers → Component configuration?
│  └─ YES → entity_test_data.json (Configuration Test)
│
├─ 2-3 systems interacting?
│  └─ YES → test_data.json (Integration Test)
│
├─ Complete user workflow?
│  └─ YES → test_data.json (System Test)
│
└─ Reproducing a specific bug?
   └─ YES → test_data.json (Regression Test)
```

## Common Mistakes and How to Avoid Them

### ❌ Mistake 1: Using TestEngine for Unit Tests

**Problem:**
```cpp
// BAD - Using TestEngine to test component construction
TEST_CASE("CUserInterface defaults", "[unit][CUserInterface]") {
  auto configs = steamrot::tests::load_test_data_configs();
  const auto *config = configs.value()[0];
  
  steamrot::tests::TestEngine engine(config);
  // ... lots of setup ...
  // Just to check if component has correct defaults???
}
```

**Solution:**
```cpp
// GOOD - Direct instantiation
TEST_CASE("CUserInterface defaults", "[unit][CUserInterface]") {
  steamrot::CUserInterface component;
  REQUIRE(component.m_active == false);
  REQUIRE(component.m_ui_name.empty());
}
```

**Why:** Component construction doesn't need game simulation. Direct instantiation is simpler, faster, and more stable.

---

### ❌ Mistake 2: Creating test_data.json for Simple Configuration

**Problem:**
```json
// BAD - Full TestDataConfig just to test entity configuration
{
  "meta_data": { ... },
  "num_ticks": 1,
  "simulation_data": { "steps": [] },
  "starting_engine_snapshot": {
    "scene_collection_data": {
      "scene_data": [{
        "entity_collection": { /* Just testing if this loads correctly */ }
      }]
    }
  }
}
```

**Solution:**
```json
// GOOD - Use entity_test_data.json directly
{
  "entity_memory_pool_size": 10,
  "entities": [
    { "index": 0, "c_user_interface": { ... } }
  ]
}
```

**Why:** No simulation needed, just configuration validation. entity_test_data.json is simpler and purpose-built for this.

---

### ❌ Mistake 3: Not Using Data-Driven Tests When Appropriate

**Problem:**
```cpp
// BAD - Manually repeating test logic for multiple scenarios
TEST_CASE("Button click scenario 1", "[integration]") {
  // 50 lines of setup and validation
}

TEST_CASE("Button click scenario 2", "[integration]") {
  // 50 lines of setup and validation (almost identical)
}

TEST_CASE("Button click scenario 3", "[integration]") {
  // 50 lines of setup and validation (almost identical)
}
```

**Solution:**
```cpp
// GOOD - Data-driven with Catch2 generators
TEST_CASE("Button click scenarios", "[integration]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  auto result = steamrot::tests::RunTestEngineTest(config);
  REQUIRE(result.has_value());
}

// Just add new JSON files to data/ directory - no code changes needed
```

**Why:** Data-driven tests reduce code duplication and make it easy to add new scenarios.

---

## Guidelines for Test Data Files

### Naming Conventions

**Unit Tests:**
- `entity_test_data.json` - Shared entity data for unit tests

**Integration Tests:**
- `<feature>_<scenario>.test_data.json`
- Example: `ui_collision_button_click.test_data.json`

**System Tests:**
- `<workflow>_<outcome>.test_data.json`
- Example: `crafting_workflow_success.test_data.json`

**Regression Tests:**
- `issue_<number>_<bug_name>.test_data.json`
- Example: `issue_123_button_click_ignored.test_data.json`

### File Organization

```
tests/
├── unit/
│   ├── <subsystem>/
│   │   ├── <Class>.test.cpp
│   │   └── data/
│   │       └── entity_test_data.json  # Shared entity data, no simulation
├── integration/
│   ├── <feature>/
│   │   ├── <feature>.integration.test.cpp
│   │   └── data/
│   │       ├── scenario1.test_data.json  # Full TestDataConfig
│   │       └── scenario2.test_data.json
└── system/
    ├── <workflow>/
    │   ├── <workflow>.system.test.cpp
    │   └── data/
    │       ├── success_path.test_data.json
    │       └── failure_path.test_data.json
```

## Summary

### Key Principles

1. ✅ **Use the simplest approach** that validates your requirement
2. ✅ **Unit tests should be isolated** - no game simulation
3. ✅ **Integration tests validate interactions** - limited simulation
4. ✅ **System tests validate workflows** - full simulation
5. ✅ **test_data.json is for game logic testing** - not pure unit tests

### Quick Checklist

Before writing a test, ask yourself:

- [ ] Am I testing a single component/class? → **Direct instantiation**
- [ ] Am I testing entity/component configuration? → **entity_test_data.json**
- [ ] Am I testing system interactions? → **test_data.json (Integration)**
- [ ] Am I testing a complete workflow? → **test_data.json (System)**
- [ ] Am I reproducing a bug? → **test_data.json (Regression)**

### Remember

> **The goal of unit tests is stability and isolation. The goal of integration/system tests is realistic validation. Choose your tool based on your goal.**

## See Also

- `TEST_HARNESS_ANALYSIS.md` - Architectural analysis of test harness
- `ADDING_TEST_HELPERS_GUIDE.md` - Guide for extending test infrastructure
- `tests/harness/README.md` - Detailed test harness API reference
