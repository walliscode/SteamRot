# TestEngine State Loading Analysis

**Date:** 2026-01-26  
**Status:** Analysis Complete  
**Type:** Documentation Only (No Code Changes)

## Executive Summary

This analysis examines the current state of loading TestEngine initial state via the `starting_engine_snapshot` field in `TestData`. 

**Key Findings:**
1. ✅ **Infrastructure Exists** - Data structures and FlatBuffers schemas are in place
2. ❌ **Not Implemented** - The `starting_engine_snapshot` data is loaded from JSON/FlatBuffers but **never applied** to TestEngine
3. ❌ **Missing Conversion Logic** - No code converts `EngineSnapshotFbs` to `EngineSnapshot` struct
4. ❌ **Missing Application Logic** - No mechanism in TestEngine to apply a starting snapshot

## Table of Contents

- [Current Architecture](#current-architecture)
- [Data Flow Analysis](#data-flow-analysis)
- [Gap Analysis](#gap-analysis)
- [Implementation Requirements](#implementation-requirements)
- [Recommendations](#recommendations)
- [Related Files](#related-files)

---

## Current Architecture

### TestData Structure

The `TestData` struct (defined in `src/types/test_structs/TestData.h`) contains:

```cpp
struct TestData {
  TestMetaData meta_data{};
  SimulationData simulation_data{};
  uint32_t number_of_ticks{1};
  EngineSnapshot starting_engine_snapshot{};        // ✅ Field exists
  std::map<size_t, EngineSnapshot> expected_engine_snapshots{};
};
```

### EngineSnapshot Structure

The `EngineSnapshot` struct (defined in `src/types/core/EngineSnapshot.h`) captures complete engine state:

```cpp
struct EngineSnapshot {
  std::optional<size_t> tick_number;
  std::optional<EventBus> global_event_bus;
  std::optional<SceneManagerData> scene_manager_data;
  SceneCollectionData scene_collection_data;
};
```

This struct is **designed for extensibility** - all fields are optional to allow selective state capture and comparison.

### FlatBuffers Schema

The `TestDataFbs` schema (defined in `src/types/flatbuffers/testing/test_data.fbs`) includes:

```fbs
table TestDataFbs {
  meta_data: TestMetadataFbs (required);
  simulation_data: SimulationDataFbs;
  num_ticks: uint32 = 1;
  starting_engine_snapshot: EngineSnapshotFbs;  // ✅ Schema field exists
}
```

The `EngineSnapshotFbs` schema is defined in `src/types/flatbuffers/core/engine_snapshot.fbs`:

```fbs
table EngineSnapshotFbs {
  tick_number: uint64;
  global_event_bus: EventBusData;
  scene_manager_data: SceneManagerDataFbs;
  scene_collection_data: SceneCollectionDataFbs;
}
```

### Test Data Example

Example test data file (`tests/unit/harness/data/harness.test_data.json`) demonstrates the structure:

```json
{
  "starting_engine_snapshot": {
    "tick_number": 0,
    "global_event_bus": {
      "description": "Starting event bus state",
      "events": [...]
    },
    "scene_collection_data": {
      "scene_data": [
        {
          "scene_info": { "scene_type": "TITLE" },
          "scene_resources_config": { ... },
          "entity_collection": { ... }
        }
      ]
    }
  }
}
```

---

## Data Flow Analysis

### Current Implementation

#### 1. JSON to FlatBuffers (✅ Working)

**Process:**
- JSON test data files (`.test_data.json`) are compiled to binary (`.test_data.bin`)
- CMake build system handles compilation via `CompileTestData.cmake`
- `starting_engine_snapshot` field is **successfully compiled** into FlatBuffers binary

**Evidence:**
- Test file: `tests/unit/harness/data/harness.test_data.json` contains `starting_engine_snapshot` 
- Binary file: `tests/unit/harness/data/harness.test_data.bin` is generated
- Test validates loading: `tests/unit/harness/FlatbuffersTestDataLoader.test.cpp` lines 139-180

#### 2. FlatBuffers Loading (✅ Working)

**Location:** `tests/harness/FlatbuffersTestDataLoader.cpp`

```cpp
std::expected<std::vector<steamrot::TestDataFbs *>, steamrot::FailInfo>
FlatbuffersTestDataLoader::LoadTestDataFbs() const {
  // Loads .test_data.bin files from data/ directory
  // Returns vector of TestDataFbs pointers
  // ✅ starting_engine_snapshot is accessible via fbs_test_data->starting_engine_snapshot()
}
```

**Status:** ✅ **Working** - FlatBuffers data is successfully loaded and accessible

#### 3. FlatBuffers to C++ Struct Conversion (❌ NOT IMPLEMENTED)

**Location:** `tests/harness/FlatbuffersTestDataProvider.cpp`

**Current Code:**
```cpp
std::expected<steamrot::TestData, steamrot::FailInfo>
FlatbuffersTestDataProvider::CreateTestData(
    const steamrot::TestDataFbs *fbs_test_data) const {
  
  steamrot::TestData test_data;
  
  // ✅ Converts meta_data
  ConfigureTestMetaData(test_data.meta_data, fbs_test_data->meta_data());
  
  // ✅ Converts simulation_data
  ConfigureSimulationData(test_data.simulation_data, fbs_test_data->simulation_data());
  
  // ✅ Converts num_ticks
  test_data.number_of_ticks = fbs_test_data->num_ticks();
  
  // ❌ MISSING: No conversion for starting_engine_snapshot
  // test_data.starting_engine_snapshot remains default-constructed (empty)
  
  return test_data;
}
```

**Problem:** The `starting_engine_snapshot` field in `TestData` is **never populated** from FlatBuffers data.

**Missing Method:**
```cpp
// DOES NOT EXIST:
std::expected<std::monostate, FailInfo> 
ConfigureEngineSnapshot(
    EngineSnapshot &engine_snapshot,
    const EngineSnapshotFbs *fbs_engine_snapshot) const;
```

#### 4. TestEngine Initialization (❌ NOT IMPLEMENTED)

**Location:** `tests/harness/TestEngine.cpp`

**Current Code:**
```cpp
std::expected<std::monostate, FailInfo> TestEngine::StartUp() {
  // ✅ Calls base Engine::StartUp()
  auto base_startup_result = Engine::StartUp();
  
  // ✅ Sets target_ticks from TestData
  m_target_ticks = m_test_data.number_of_ticks;
  
  // ❌ MISSING: No application of starting_engine_snapshot
  // Engine starts with default state from Engine::StartUp()
  
  return std::monostate{};
}
```

**Problem:** Even if `starting_engine_snapshot` were populated, TestEngine doesn't apply it.

### Data Flow Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│ JSON Test Data (.test_data.json)                               │
│   - starting_engine_snapshot defined                            │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 │ ✅ CMake Compilation
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│ FlatBuffers Binary (.test_data.bin)                            │
│   - starting_engine_snapshot compiled                           │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 │ ✅ FlatbuffersTestDataLoader::LoadTestDataFbs()
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│ TestDataFbs* (in memory)                                       │
│   - starting_engine_snapshot accessible                         │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 │ ❌ FlatbuffersTestDataProvider::CreateTestData()
                 │    MISSING: ConfigureEngineSnapshot() call
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│ TestData (C++ struct)                                          │
│   - starting_engine_snapshot EMPTY (default-constructed)       │
└────────────────┬────────────────────────────────────────────────┘
                 │
                 │ ❌ TestEngine::StartUp()
                 │    MISSING: Apply starting_engine_snapshot
                 ▼
┌─────────────────────────────────────────────────────────────────┐
│ TestEngine Running                                             │
│   - Uses default Engine state (from Engine::StartUp())         │
│   - starting_engine_snapshot data NEVER USED                   │
└─────────────────────────────────────────────────────────────────┘
```

---

## Gap Analysis

### Missing Components

#### 1. FlatBuffers to C++ Conversion (Priority: HIGH)

**Missing File/Functions:**

No configurator exists for converting `EngineSnapshotFbs` to `EngineSnapshot`. Need to create:

**File:** `src/data_providers/configure/configure_engine_snapshot.h`
**File:** `src/data_providers/configure/configure_engine_snapshot.cpp`

**Required Functions:**
```cpp
namespace steamrot::data::configure {

// Main EngineSnapshot configuration
std::expected<std::monostate, FailInfo>
ConfigureEngineSnapshot(
    EngineSnapshot &engine_snapshot,
    const EngineSnapshotFbs *fbs_engine_snapshot);

// Sub-component configurations (may delegate to existing configurators)
std::expected<std::monostate, FailInfo>
ConfigureEventBus(
    EventBus &event_bus,
    const EventBusData *fbs_event_bus);

std::expected<std::monostate, FailInfo>
ConfigureSceneManagerData(
    SceneManagerData &scene_manager_data,
    const SceneManagerDataFbs *fbs_scene_manager_data);

std::expected<std::monostate, FailInfo>
ConfigureSceneCollectionData(
    SceneCollectionData &scene_collection_data,
    const SceneCollectionDataFbs *fbs_scene_collection_data);

} // namespace steamrot::data::configure
```

**Note:** Some configurators may already exist (e.g., `configure_scene_manager_data.h`). Investigation required to identify reusable components.

#### 2. TestData Population (Priority: HIGH)

**File to Modify:** `tests/harness/FlatbuffersTestDataProvider.cpp`

**Required Changes:**

Add to `FlatbuffersTestDataProvider::CreateTestData()`:

```cpp
// Add include
#include "configure_engine_snapshot.h"

// In CreateTestData() method, after configuring num_ticks:

// Configure starting_engine_snapshot
if (fbs_test_data->starting_engine_snapshot()) {
  auto snapshot_result = steamrot::data::configure::ConfigureEngineSnapshot(
      test_data.starting_engine_snapshot, 
      fbs_test_data->starting_engine_snapshot());
  if (!snapshot_result)
    return std::unexpected(snapshot_result.error());
}
```

#### 3. TestEngine State Application (Priority: HIGH)

**File to Modify:** `tests/harness/TestEngine.cpp`

**Required Changes:**

Add a method to apply the starting snapshot:

```cpp
// In TestEngine class (TestEngine.h)
private:
  std::expected<std::monostate, FailInfo> 
  ApplyStartingEngineSnapshot();

// In TestEngine.cpp
std::expected<std::monostate, FailInfo> 
TestEngine::ApplyStartingEngineSnapshot() {
  
  const EngineSnapshot &snapshot = m_test_data.starting_engine_snapshot;
  
  // 1. Apply scene_collection_data if present
  if (!snapshot.scene_collection_data.empty()) {
    // Need mutable copy for SceneManager API
    SceneCollectionData scene_data_copy = snapshot.scene_collection_data;
    
    auto result = m_scene_manager.AddScenesFromSceneCollectionData(scene_data_copy);
    if (!result) {
      return std::unexpected(result.error());
    }
  }
  
  // 2. Apply global_event_bus if present
  if (snapshot.global_event_bus.has_value()) {
    // EventHandler doesn't have SetGlobalEventBus()
    // Need to add events individually
    for (const auto &event : snapshot.global_event_bus.value()) {
      m_engine_resources.event_handler.AddEvent(event);
    }
    m_engine_resources.event_handler.ProcessWaitingRoomEventBus();
  }
  
  // 3. Apply scene_manager_data if present
  // TODO: SceneManager needs a method to apply SceneManagerData
  
  return std::monostate{};
}
```

**Call from StartUp():**

```cpp
std::expected<std::monostate, FailInfo> TestEngine::StartUp() {
  // Call base class StartUp
  auto base_startup_result = Engine::StartUp();
  if (!base_startup_result.has_value()) {
    return std::unexpected(base_startup_result.error());
  }

  // Assign TestEngine specific variables from TestData
  m_target_ticks = m_test_data.number_of_ticks;
  
  // Apply starting engine snapshot
  auto apply_snapshot_result = ApplyStartingEngineSnapshot();
  if (!apply_snapshot_result) {
    return std::unexpected(apply_snapshot_result.error());
  }

  return std::monostate{};
}
```

#### 4. Additional Infrastructure Gaps

**EventHandler Missing Methods:**

EventHandler currently has:
- ✅ `AddEvent(const EventPacket &event)` - adds to waiting room
- ✅ `ProcessWaitingRoomEventBus()` - moves to global bus
- ✅ `GetGlobalEventBus()` - returns const reference
- ❌ **Missing:** `SetGlobalEventBus(const EventBus &bus)` - direct setter

**Workaround:** Use existing `AddEvent()` + `ProcessWaitingRoomEventBus()` as shown above.

**SceneManager Missing Methods:**

SceneManager has:
- ✅ `AddScenesFromSceneCollectionData()` - loads scenes from data
- ✅ `CaptureSceneCollectionData()` - captures current state
- ❌ **May be missing:** Methods to apply `SceneManagerData` state

**Investigation Required:** Check if `SceneManager` needs methods to restore its own state (subscriptions, etc.) beyond scene data.

---

## Implementation Requirements

### Phase 1: Core Conversion Infrastructure

**Goal:** Enable conversion from FlatBuffers to C++ structs

**Tasks:**
1. Create `configure_engine_snapshot.h/cpp` with conversion functions
2. Leverage existing configurators where possible (scene data, scene manager data)
3. Implement new configurators only for missing components (EventBus conversion)
4. Add comprehensive unit tests for conversion logic

**Dependencies:**
- Existing configurators in `src/data_providers/configure/`
- FlatBuffers schemas in `src/types/flatbuffers/`
- C++ structs in `src/types/core/`

**Estimated Effort:** Medium
- Some configurators may already exist
- EventBus conversion is straightforward (vector of EventPacket)
- SceneCollectionData conversion may need investigation

### Phase 2: TestData Population

**Goal:** Populate `starting_engine_snapshot` in `TestData` from FlatBuffers

**Tasks:**
1. Add `ConfigureEngineSnapshot()` call to `FlatbuffersTestDataProvider::CreateTestData()`
2. Add include for `configure_engine_snapshot.h`
3. Add error handling for conversion failures
4. Update tests to validate `starting_engine_snapshot` population

**Dependencies:**
- Phase 1 must be complete
- FlatbuffersTestDataProvider exists

**Estimated Effort:** Low
- Simple method call addition
- Follows existing pattern (ConfigureTestMetaData, ConfigureSimulationData)

### Phase 3: TestEngine State Application

**Goal:** Apply `starting_engine_snapshot` to TestEngine during initialization

**Tasks:**
1. Add `ApplyStartingEngineSnapshot()` method to TestEngine
2. Implement scene loading via `SceneManager::AddScenesFromSceneCollectionData()`
3. Implement event bus population via `EventHandler::AddEvent()` + `ProcessWaitingRoomEventBus()`
4. Call from `TestEngine::StartUp()` after base startup
5. Add comprehensive tests for state application

**Dependencies:**
- Phase 2 must be complete
- TestEngine exists
- SceneManager::AddScenesFromSceneCollectionData() exists (✅ confirmed)
- EventHandler::AddEvent() exists (✅ confirmed)

**Estimated Effort:** Medium
- Scene loading is straightforward (API exists)
- Event bus loading requires iteration
- SceneManagerData application may need investigation

**Potential Issues:**
- SceneManager may need additional methods for SceneManagerData application
- Timing: State must be applied after `Engine::StartUp()` but before first tick
- Validation: Need to verify snapshot application doesn't conflict with base Engine initialization

### Phase 4: Testing & Validation

**Goal:** Comprehensive testing of state loading pipeline

**Tasks:**
1. Add unit tests for `configure_engine_snapshot` functions
2. Add integration tests for `FlatbuffersTestDataProvider`
3. Add end-to-end tests for TestEngine initialization
4. Validate against existing test data files
5. Create example test data demonstrating all snapshot fields

**Dependencies:**
- Phases 1-3 complete

**Estimated Effort:** Medium
- Need tests at each layer (conversion, population, application)
- Need both positive and negative test cases
- Need validation that state is correctly applied

---

## Recommendations

### Immediate Actions (Required for Functionality)

1. **Implement Phase 1** - Create FlatBuffers to C++ conversion infrastructure
   - Start with `configure_engine_snapshot.h/cpp`
   - Identify and reuse existing configurators
   - Focus on `EventBus` conversion (likely missing)

2. **Implement Phase 2** - Add snapshot population to TestData
   - Single method call addition to `FlatbuffersTestDataProvider`
   - Low risk, high value

3. **Implement Phase 3** - Add state application to TestEngine
   - Most complex phase
   - Requires careful integration with existing Engine startup
   - May reveal additional infrastructure gaps

### Optional Enhancements (Future Improvements)

1. **EventHandler Enhancement**
   - Add `SetGlobalEventBus(const EventBus &bus)` for direct event bus replacement
   - Current workaround is acceptable but less efficient

2. **SceneManager Enhancement**
   - Add methods to apply `SceneManagerData` if missing
   - Investigate current capabilities first

3. **Validation Framework**
   - Add validation that starting snapshot matches expected structure
   - Warn if snapshot contains data that cannot be applied

4. **Documentation Updates**
   - Update test harness README with state loading workflow
   - Add examples to test data naming conventions
   - Document `starting_engine_snapshot` usage patterns

### Testing Strategy

**Unit Tests:**
- Test each configurator in isolation
- Test TestData population
- Test TestEngine state application methods

**Integration Tests:**
- Test full pipeline: JSON → FlatBuffers → C++ → Applied
- Test with various snapshot field combinations (all, some, none)
- Test error conditions (invalid data, missing data)

**End-to-End Tests:**
- Create test data files with `starting_engine_snapshot`
- Verify TestEngine initializes with correct state
- Validate state is maintained during execution

---

## Related Files

### Core Data Structures

**C++ Structs:**
- `src/types/test_structs/TestData.h` - TestData struct with starting_engine_snapshot field
- `src/types/core/EngineSnapshot.h` - EngineSnapshot struct
- `src/types/core/SceneData.h` - SceneCollectionData type alias
- `src/types/test_structs/SimulationData.h` - SimulationData struct
- `src/types/test_structs/TestMetaData.h` - TestMetaData struct

**FlatBuffers Schemas:**
- `src/types/flatbuffers/testing/test_data.fbs` - TestDataFbs table
- `src/types/flatbuffers/core/engine_snapshot.fbs` - EngineSnapshotFbs table
- `src/types/flatbuffers/testing/simulation_data.fbs` - SimulationDataFbs table

### Data Loading Infrastructure

**Test Harness:**
- `tests/harness/FlatbuffersTestDataLoader.h/cpp` - Loads .bin files
- `tests/harness/FlatbuffersTestDataProvider.h/cpp` - Converts FlatBuffers to C++
- `tests/harness/ITestDataProvider.h` - Provider interface
- `tests/harness/TestEngine.h/cpp` - TestEngine implementation

**Configurators:**
- `src/data_providers/configure/configure_engine_data.h/cpp` - Engine data configuration
- `src/data_providers/configure/configure_scene_data.h/cpp` - Scene data configuration
- `src/data_providers/configure/configure_scene_manager_data.h/cpp` - Scene manager configuration
- `src/data_providers/configure/configure_save_data.h/cpp` - Save data configuration

### Engine Infrastructure

**Engine:**
- `src/engine/Engine.h/cpp` - Base Engine class
- `src/engine/engine_configuration.h` - Engine configuration utilities

**Managers:**
- `src/scenes/SceneManager.h/cpp` - Scene management (has AddScenesFromSceneCollectionData)
- `src/events/EventHandler.h/cpp` - Event handling (has AddEvent, GetGlobalEventBus)

### Test Files

**Unit Tests:**
- `tests/unit/harness/FlatbuffersTestDataLoader.test.cpp` - Tests FlatBuffers loading
- `tests/unit/harness/FlatbuffersTestDataProvider.test.cpp` - Tests data provider
- `tests/unit/harness/TestEngine.test.cpp` - Tests TestEngine

**Test Data:**
- `tests/unit/harness/data/harness.test_data.json` - Example test data with starting_engine_snapshot
- `tests/unit/harness/data/harness.test_data.bin` - Compiled binary

### Documentation

- `tests/harness/README.md` - Test harness documentation (41.7 KB)
- `documentation/testing/TEST_DATA_CONFIGURATION.md` - Test data system documentation
- `documentation/workflows/FILLING_TEST_DATA.md` - Test data creation guide

---

## Conclusion

The infrastructure for loading TestEngine state via `starting_engine_snapshot` is **partially implemented** but **not functional**:

✅ **What Works:**
- FlatBuffers schema definition
- JSON to binary compilation
- Binary file loading
- Data structure accessibility

❌ **What's Missing:**
- FlatBuffers to C++ struct conversion (`ConfigureEngineSnapshot`)
- TestData population (calling the conversion)
- TestEngine state application (applying the snapshot)

**Impact:** Tests cannot initialize TestEngine with custom starting state. All TestEngine tests start with default Engine state from `Engine::StartUp()`.

**Next Steps:** Implement Phases 1-3 in order to enable this functionality. Phase 4 (testing) should run in parallel with implementation.

**Complexity:** Medium - Most infrastructure exists, need to wire it together and handle edge cases.

---

**Analysis by:** GitHub Copilot Agent  
**Repository:** walliscode/SteamRot  
**Branch:** copilot/analyze-testengine-loading-state
