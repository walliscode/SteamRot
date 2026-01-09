# Analysis: Loading SceneCollectionData from TestData

**Date**: 2026-01-09  
**Status**: Analysis Complete  
**Issue**: Analysis and documentation for loading SceneCollectionData from TestData

## Executive Summary

This document analyzes the feasibility and architectural considerations for loading `SceneCollectionData` from `TestData` in the SteamRot engine. The analysis reveals that:

1. ✅ **SceneManager is ready** - The `AddScenesFromSceneCollectionData()` method already exists and works correctly
2. ⚠️ **EngineSnapshot needs clarification** - Currently designed for capturing output, not providing input
3. 📋 **Minor updates needed** - TestData FlatBuffers schema and provider need SceneCollectionData support
4. 💡 **Architectural recommendation** - Use separate containers for input (TestData) and output (EngineSnapshot)

---

## Question 1: Loading SceneCollectionData from TestData

### Current State

The `TestData` struct already has a `starting_scene_collection_data` field:

```cpp
// src/types/test_structs/TestData.h
struct TestData {
  TestMetaData meta_data{};
  SimulationData simulation_data{};
  uint32_t number_of_ticks{1};
  SceneCollectionData starting_scene_collection_data{};  // ✅ Already exists
  std::map<size_t, EngineSnapshot> expected_engine_snapshots{};
};
```

**However**, this field is:
- ❌ Not loaded from FlatBuffers (test_data.fbs doesn't include it)
- ❌ Not configured by FlatbuffersTestDataProvider
- ✅ Already the correct type (`SceneCollectionData`)

### What Needs to Be Done

To enable loading `SceneCollectionData` from TestData:

#### 1. Update FlatBuffers Schema

**File**: `src/types/flatbuffers/testing/test_data.fbs`

```fbs
include "../entities/entities.fbs";
include "simulation_data.fbs";
include "input_test_data.fbs";
include "../events/event_packet_data.fbs";
include "../events/event_bus_data.fbs";
include "../engine/engine_state.fbs";
include "../scenes/scene_collection_data.fbs";  // ADD THIS

namespace steamrot;

table TestDataFbs {
  meta_data: TestMetadataFbs (required);
  simulation_data: SimulationDataFbs;
  num_ticks: uint32;
  
  // ADD THIS FIELD
  starting_scene_collection_data: SceneCollectionDataFbs;
}
```

#### 2. Update FlatbuffersTestDataProvider

**File**: `tests/harness/FlatbuffersTestDataProvider.cpp`

Add to `CreateTestData()` method:

```cpp
std::expected<steamrot::TestData, steamrot::FailInfo>
FlatbuffersTestDataProvider::CreateTestData(
    const steamrot::TestDataFbs *fbs_test_data) const {
  
  // ... existing code ...
  
  // Configure starting_scene_collection_data (OPTIONAL FIELD)
  if (fbs_test_data->starting_scene_collection_data()) {
    auto scene_collection_result = ConfigureSceneCollectionData(
        test_data.starting_scene_collection_data,
        fbs_test_data->starting_scene_collection_data());
    if (!scene_collection_result)
      return std::unexpected(scene_collection_result.error());
  }
  
  return test_data;
}
```

Add new configuration method:

```cpp
std::expected<std::monostate, steamrot::FailInfo>
FlatbuffersTestDataProvider::ConfigureSceneCollectionData(
    steamrot::SceneCollectionData &scene_collection_data,
    const steamrot::SceneCollectionDataFbs *fbs_scene_collection) const {
  
  if (!fbs_scene_collection)
    return std::unexpected(steamrot::FailInfo{
        steamrot::FailMode::FlatbuffersDataNotFound,
        "Input Flatbuffers SceneCollectionDataFbs is null."});
  
  // Use FlatbuffersSceneLoadDataProvider to convert each scene
  FlatbuffersSceneLoadDataProvider scene_provider(/* EventHandler reference */);
  
  if (!fbs_scene_collection->scene_data())
    return std::monostate{}; // Empty collection is valid
  
  for (const auto *scene_fbs : *fbs_scene_collection->scene_data()) {
    auto scene_load_data_result = 
        scene_provider.ProvideSceneLoadDataFromData(scene_fbs);
    if (!scene_load_data_result)
      return std::unexpected(scene_load_data_result.error());
    
    scene_collection_data.push_back(
        std::move(scene_load_data_result.value()));
  }
  
  return std::monostate{};
}
```

#### 3. Use in TestEngine

**File**: `tests/harness/TestEngine.cpp`

Modify `StartUp()` to load scenes from TestData:

```cpp
std::expected<std::monostate, FailInfo> TestEngine::StartUp() {
  // call base class StartUp
  auto base_startup_result = Engine::StartUp();
  if (!base_startup_result.has_value())
    return std::unexpected(base_startup_result.error());

  // Load scenes from TestData if provided
  if (!m_test_data.starting_scene_collection_data.empty()) {
    // Need to make a mutable copy since AddScenesFromSceneCollectionData takes non-const
    SceneCollectionData scene_data = m_test_data.starting_scene_collection_data;
    auto load_result = m_scene_manager.AddScenesFromSceneCollectionData(scene_data);
    if (!load_result.has_value())
      return std::unexpected(load_result.error());
  }

  // assign TestEngine specific variables from TestData
  m_target_ticks = m_test_data.number_of_ticks;

  return std::monostate{};
}
```

### Recommendation for Question 1

✅ **Yes, SceneCollectionData can be loaded from TestData with minimal changes**

The infrastructure is already in place:
- TestData struct has the field
- SceneManager has `AddScenesFromSceneCollectionData()`
- Only FlatBuffers schema and provider need updates

---

## Question 2: Is EngineSnapshot a Good Container?

### Current Design Analysis

`EngineSnapshot` is designed as an **output container** for capturing engine state:

```cpp
// src/types/test_structs/EngineSnapshot.h
struct EngineSnapshot {
  // Captures state AFTER execution
  std::optional<std::unordered_map<uuids::uuid, EntityMemoryPool>> scene_snapshots;
  std::optional<EventBus> global_event_bus;
  std::optional<size_t> tick_number;
  
  bool HasData() const;
};
```

**Key characteristics:**
- ✅ Uses `std::optional` for selective comparison
- ✅ Extensible (can add fields without breaking tests)
- ✅ Well-documented as a snapshot/capture mechanism
- ✅ Used in `TestEngine::CaptureSnapShot()` to record state **after** tick execution

### For Simulation Input vs. Output

| Aspect | EngineSnapshot | Ideal Input Container |
|--------|----------------|----------------------|
| **Purpose** | Capture output state | Provide input configuration |
| **UUID Keys** | Runtime-generated UUIDs | Scene type enums or test-defined IDs |
| **Data Format** | Direct EntityMemoryPool | SceneLoadData (config + importer) |
| **Mutability** | Immutable snapshot | Mutable for engine consumption |
| **Ownership** | Deep copies for comparison | Move semantics for efficiency |
| **Timing** | Post-execution capture | Pre-execution configuration |

### Can EngineSnapshot Extract Data for Simulation?

**Current usage pattern:**

```cpp
// TestEngine::CaptureSnapShot() - OUTPUT CAPTURE
for (const auto &[scene_uuid, scene_ptr] : scenes) {
  const EntityMemoryPool &scene_pool = 
      scene_ptr->GetEntityManager().GetEntityMemoryPool();
  scene_map[scene_uuid] = scene_pool;  // Deep copy
}
snapshot.scene_snapshots = std::move(scene_map);
```

**Hypothetical input usage (not recommended):**

```cpp
// Would need to reverse-engineer SceneLoadData from EngineSnapshot
for (const auto &[uuid, pool] : snapshot.scene_snapshots.value()) {
  // ❌ Problem 1: How to determine SceneType from UUID?
  // ❌ Problem 2: How to create SceneData config?
  // ❌ Problem 3: How to wrap pool in IEntityImporter?
  // ❌ Problem 4: UUID mismatch (snapshot UUIDs != desired test UUIDs)
}
```

### Recommendation for Question 2

❌ **No, EngineSnapshot is NOT a good container for simulation input**

**Reasons:**
1. **Semantic mismatch**: Designed for capturing output, not providing input
2. **Data format mismatch**: Contains EntityMemoryPool, not SceneLoadData
3. **Key mismatch**: Uses runtime UUIDs, not scene types or test identifiers
4. **Architectural clarity**: Mixing input and output in same type reduces clarity

**Alternative approach:**
- ✅ Use `TestData.starting_scene_collection_data` for input
- ✅ Use `EngineSnapshot` for output validation
- ✅ Keep clear separation between configuration (input) and validation (output)

---

## Question 3: Different Container for Input?

### Architectural Options

#### Option A: Enhance EngineSnapshot (❌ Not Recommended)

Add fields for input configuration:

```cpp
struct EngineSnapshot {
  // Output (current)
  std::optional<std::unordered_map<uuids::uuid, EntityMemoryPool>> scene_snapshots;
  std::optional<EventBus> global_event_bus;
  std::optional<size_t> tick_number;
  
  // Input (new) - ❌ CONFUSING!
  std::optional<SceneCollectionData> input_scenes;
};
```

**Problems:**
- Mixes input and output semantics
- `scene_snapshots` vs `input_scenes` - which is which?
- Violates single responsibility principle
- Reduces code clarity

#### Option B: Use Existing TestData Field (✅ Recommended)

Keep input and output separate:

```cpp
struct TestData {
  TestMetaData meta_data{};
  SimulationData simulation_data{};
  uint32_t number_of_ticks{1};
  
  // INPUT: Initial scene configuration
  SceneCollectionData starting_scene_collection_data{};
  
  // OUTPUT: Expected states for validation
  std::map<size_t, EngineSnapshot> expected_engine_snapshots{};
};
```

**Benefits:**
- ✅ Clear separation of concerns
- ✅ `starting_scene_collection_data` = INPUT
- ✅ `expected_engine_snapshots` = OUTPUT
- ✅ Matches existing SaveData pattern
- ✅ No confusion about purpose

#### Option C: Create TestInputConfiguration Struct (⚠️ Over-engineering)

```cpp
struct TestInputConfiguration {
  SceneCollectionData initial_scenes;
  std::optional<EventBus> initial_event_bus;
  std::map<size_t, InputSequence> input_events_per_tick;
};

struct TestData {
  TestMetaData meta_data{};
  TestInputConfiguration input_config;  // Input
  std::map<size_t, EngineSnapshot> expected_snapshots;  // Output
};
```

**Assessment:**
- ✅ Very clear separation
- ❌ Adds complexity without clear benefit
- ❌ TestData field already exists
- ⚠️ Consider if we need to configure initial EventBus or other engine state

### Recommendation for Question 3

✅ **Use existing TestData.starting_scene_collection_data field**

This provides:
1. **Clear semantics**: `starting_` prefix indicates input
2. **Correct type**: Already `SceneCollectionData`
3. **Parallel structure**: Matches `SaveData.scene_collection_data`
4. **Minimal change**: Just needs FlatBuffers support
5. **Future extensibility**: Can add other input fields to TestData if needed

---

## Question 4: Is SceneManager Ready for SceneCollectionData?

### Current Implementation Review

The `SceneManager::AddScenesFromSceneCollectionData()` method is **fully implemented**:

```cpp
std::expected<std::monostate, FailInfo>
SceneManager::AddScenesFromSceneCollectionData(
    SceneCollectionData &scene_collection_data) {

  // Clear existing scenes and check that it is cleared
  m_scenes.clear();
  if (!m_scenes.empty()) {
    return std::unexpected(FailInfo{FailMode::NotImplemented,
                           "Existing scenes were not cleared correctly"});
  }

  // Create SceneFactory object
  SceneFactory scene_factory(m_game_context);

  // Loop through scene collection data and add each scene
  for (auto &scene_data : scene_collection_data) {
    auto scene_creation_result =
        scene_factory.CreateSceneFromSceneLoadData(scene_data);
    if (!scene_creation_result.has_value())
      return std::unexpected(scene_creation_result.error());

    auto adding_result =
        m_scenes.emplace(scene_creation_result.value()->GetSceneInfo().id,
                        std::move(scene_creation_result.value()));
    if (!adding_result.second) {
      return std::unexpected(FailInfo{FailMode::NotAddedToMap,
                             "Scene with this ID already exists"});
    }
  }
  return std::monostate{};
}
```

### Test Coverage Review

**Test**: `tests/unit/scenes/SceneManager.test.cpp`

Existing test demonstrates successful loading:

```cpp
TEST_CASE("SceneManager::AddScenesFromSceneCollectionData loads three scenes "
          "with valid data", "[SceneManager]") {

  // Load test data from JSON (compiled to binary)
  auto [data_buffer, scene_collection_data_fbs] = LoadSceneCollectionTestData();
  
  // Convert FlatBuffers to SceneCollectionData
  steamrot::SceneCollectionData scene_collection_data;
  for (const auto *scene_fbs : *scenes) {
    auto scene_load_data_result =
        scene_data_provider.ProvideSceneLoadDataFromData(scene_fbs);
    scene_collection_data.push_back(std::move(scene_load_data_result.value()));
  }

  // Load into SceneManager
  auto result = scene_manager.AddScenesFromSceneCollectionData(scene_collection_data);
  
  // ✅ Test passes - validates 3 scenes loaded correctly
  REQUIRE(scene_map.size() == 3);
}
```

### Workflow Readiness

The complete loading workflow exists and works:

```
FlatBuffers JSON
    ↓
SceneCollectionDataFbs (binary)
    ↓
FlatbuffersSceneLoadDataProvider
    ↓
SceneCollectionData (runtime)
    ↓
SceneManager::AddScenesFromSceneCollectionData()
    ↓
Loaded Scenes in m_scenes map
```

### Recommendation for Question 4

✅ **Yes, SceneManager and loading workflow are fully ready**

**Evidence:**
1. ✅ Method implemented and tested
2. ✅ Clears existing scenes first
3. ✅ Iterates through collection correctly
4. ✅ Uses SceneFactory for scene creation
5. ✅ Handles errors with std::expected
6. ✅ Test coverage demonstrates success with 3 scenes
7. ✅ FlatBuffers schema and providers already exist

**No changes needed** to SceneManager or loading workflow.

---

## Overall Recommendations

### Summary of Findings

| Question | Answer | Changes Needed |
|----------|--------|----------------|
| Can we load SceneCollectionData from TestData? | ✅ Yes | Minor: Update FlatBuffers schema and provider |
| Is EngineSnapshot a good container? | ❌ No | None: Use TestData field instead |
| Should we use a different container? | ✅ Yes | None: TestData field already exists |
| Is SceneManager ready? | ✅ Yes | None: Already implemented and tested |

### Implementation Priority

1. **High Priority**: Update test_data.fbs to include SceneCollectionDataFbs
2. **High Priority**: Add ConfigureSceneCollectionData to FlatbuffersTestDataProvider
3. **Medium Priority**: Update TestEngine::StartUp() to use starting_scene_collection_data
4. **Low Priority**: Document the input vs output pattern in TestData

### Architectural Pattern

**Established Pattern:**

```cpp
struct TestData {
  // Test metadata
  TestMetaData meta_data{};
  
  // INPUT: What to simulate
  SimulationData simulation_data{};              // Logic steps to execute
  uint32_t number_of_ticks{1};                   // How many ticks
  SceneCollectionData starting_scene_collection_data{};  // Initial scenes
  
  // OUTPUT: What to expect
  std::map<size_t, EngineSnapshot> expected_engine_snapshots{};
};
```

This mirrors the SaveData pattern:

```cpp
struct SaveData {
  SaveMetaData meta_data;
  SceneManagerData scene_manager_data;
  SceneCollectionData scene_collection_data;  // Parallel to TestData
};
```

### Benefits of This Approach

1. **Consistency**: TestData and SaveData use same types
2. **Reusability**: FlatbuffersSceneLoadDataProvider works for both
3. **Clarity**: Input (starting_) vs Output (expected_) is obvious
4. **Testability**: Can validate state transitions
5. **Maintainability**: Single source of truth for scene data format

---

## Example Test Data Usage

### Test Scenario: Scene State Transition

```json
// test_scene_transition.test_data.json
{
  "meta_data": {
    "test_name": "scene_transition_validates_state_change",
    "test_description": "Load title scene, run simulation, validate state"
  },
  "num_ticks": 5,
  "starting_scene_collection_data": {
    "scene_data": [
      {
        "scene_info": {
          "scene_type": "TITLE"
        },
        "scene_resources_config": {
          "texture_width": 800,
          "texture_height": 600
        },
        "entity_collection": {
          "entity_memory_pool_size": 30,
          "entities": [
            // Initial entity states
          ]
        }
      }
    ]
  },
  "simulation_data": {
    "steps": [
      {"logic_class_type": "UIActionLogic"},
      {"logic_class_type": "UICollisionLogic"}
    ]
  },
  "expected_engine_snapshots": {
    "5": {
      "tick_number": 5,
      "scene_snapshots": {
        // Expected scene state after 5 ticks
      }
    }
  }
}
```

### Test Code

```cpp
TEST_CASE("Scene transition test", "[integration]") {
  // Load test data (includes starting_scene_collection_data)
  FlatbuffersTestDataProvider provider(__FILE__);
  auto test_data_result = provider.ProviderAllTestData();
  REQUIRE(test_data_result.has_value());
  
  const TestData &test_data = test_data_result.value()[0];
  
  // Create TestEngine with the test data
  TestEngine engine(test_data);
  
  // StartUp will load starting_scene_collection_data
  auto startup_result = engine.StartUp();
  REQUIRE(startup_result.has_value());
  
  // Run simulation
  engine.Run();
  
  // Validate output against expected_engine_snapshots
  const auto &data_bank = engine.GetDataBank();
  REQUIRE(data_bank.contains(5));
  
  const EngineSnapshot &actual = data_bank.at(5);
  const EngineSnapshot &expected = test_data.expected_engine_snapshots.at(5);
  
  // Compare (using future matcher)
  REQUIRE(actual == expected);
}
```

---

## Conclusion

The SteamRot engine architecture is well-positioned to support loading `SceneCollectionData` from `TestData`:

1. ✅ **SceneManager is ready** - No changes needed
2. ✅ **TestData has the right structure** - Just needs FlatBuffers support
3. ✅ **Clear separation** - Input (TestData) vs Output (EngineSnapshot)
4. ⚠️ **Minor implementation needed** - FlatBuffers schema and provider updates

The recommended approach maintains architectural clarity, reuses existing infrastructure, and follows established patterns in the codebase.

---

## Next Steps

If proceeding with implementation:

1. Update `src/types/flatbuffers/testing/test_data.fbs`
2. Add `ConfigureSceneCollectionData` to `FlatbuffersTestDataProvider`
3. Modify `TestEngine::StartUp()` to load scenes from TestData
4. Add integration test demonstrating the feature
5. Document the pattern in testing guides

Estimated effort: **1-2 hours** for a skilled developer familiar with the codebase.
