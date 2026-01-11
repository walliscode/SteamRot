# Data Loading/Exporting Architecture Analysis

## Executive Summary

This document analyzes the current data loading and exporting architecture in SteamRot, focusing on the relationship between `SaveData`, `TestData`, and the `TestEngine`. It identifies areas of overlap, clarifies their distinct purposes, and provides recommendations for a sustainable long-term architecture that maximizes code reuse while maintaining clarity and ease of use.

## Current State Analysis

### 1. SaveData System

**Purpose:** Save and restore game state for production gameplay

**Key Structures:**
- **`SaveData`** (`src/types/core/SaveData.h`)
  - `SaveMetaData meta_data` - Save file metadata (name, UUID)
  - `SceneManagerData scene_manager_data` - SceneManager state (subscriptions)
  - `SceneCollectionData scene_collection_data` - All scene data for the save

- **`SceneCollectionData`** - Vector of `SceneLoadData`
  - `SceneData scene_data` - Scene configuration (info, resources, assets)
  - `std::unique_ptr<IEntityImporter> entity_importer` - Wraps entity data source

**Data Flow:**
1. **Save (Export)**: `GameEngine` → FlatBuffers → File
2. **Load (Import)**: File → FlatBuffers → `SaveData` → `GameEngine`

**Provider:**
- `FlatbuffersSaveDataProvider` implements `ISaveDataProvider`
- `ProvideSaveData()` returns complete `SaveData` struct
- Uses `FlatbuffersEntityImporter` to wrap entity data

**Schema:** `src/types/flatbuffers/configuration/save_data.fbs`
```fbs
table SaveDataFbs {
  save_meta_data: SaveMetaDataFbs;
  scene_collection_data: SceneCollectionDataFbs;
}
```

**Current Limitations:**
- Missing `EngineState` (running, paused, subscriptions)
- Missing `EngineConfig` (user preferences, display settings)
- No support for `EventBus` state
- Only stores scene-level data (SceneManager state is minimal)

### 2. TestData System

**Purpose:** Data-driven testing with the TestEngine

**Key Structures:**
- **`TestData`** (`src/types/test_structs/TestData.h`)
  - `TestMetaData meta_data` - Test metadata (name, description, will_pass)
  - `SimulationData simulation_data` - Logic steps to execute
  - `uint32_t number_of_ticks` - Ticks to run
  - `EngineSnapshot starting_engine_snapshot` - Initial state
  - `std::map<size_t, EngineSnapshot> expected_engine_snapshots` - Expected states per tick

- **`EngineSnapshot`** (`src/types/core/EngineSnapshot.h`)
  - `std::optional<size_t> tick_number` - Tick context
  - `SceneCollectionData scene_collection_data` - Scene entities via importers
  - `std::optional<EventBus> global_event_bus` - Event bus state

**Data Flow:**
1. **Test Setup**: JSON → FlatBuffers → `TestData` → `TestEngine`
2. **Test Execution**: `TestEngine` runs ticks, captures `EngineSnapshot` per tick
3. **Test Validation**: Compare captured snapshots with expected snapshots

**Provider:**
- `FlatbuffersTestDataProvider` implements `ITestDataProvider`
- `ProviderAllTestData()` returns vector of `TestData` structs
- Discovers and loads all `.test_data.bin` files in a directory

**Schema:** `src/types/flatbuffers/testing/test_data.fbs`
```fbs
table TestDataFbs {
  meta_data: TestMetadataFbs (required);
  simulation_data: SimulationDataFbs;
  num_ticks: uint32;
}
```

**TestEngine Workflow:**
1. Construct with `TestData` reference
2. `StartUp()` - Initialize from test data
3. `RunGameLoop()` - Execute ticks, capture snapshots per tick
4. `CaptureSnapShot()` - Deep copy EntityMemoryPool + EventBus per tick
5. Compare snapshots with expected data

**Current Capabilities:**
- Tick-by-tick execution control
- Simulation of Logic classes and free functions
- Input sequence simulation (mouse, keyboard)
- Event sequence injection
- EventBus state capture and validation
- Tick-based snapshot comparison

### 3. Key Overlaps

**Shared Concepts:**

1. **Scene Entity Data**
   - Both systems need to import/export `EntityMemoryPool` data
   - Both use `SceneCollectionData` with `IEntityImporter`
   - Both use FlatBuffers for serialization
   - **Overlap: Entity import/export mechanism is identical**

2. **Scene Configuration**
   - Both systems need `SceneData` (info, resources, assets)
   - Both organize data per scene
   - **Overlap: Scene metadata and configuration**

3. **Metadata**
   - Both have metadata (SaveMetaData vs TestMetaData)
   - Different fields but similar purpose (identification, description)
   - **Overlap: High-level descriptive data**

4. **EventBus State**
   - `EngineSnapshot` captures EventBus (optional)
   - `SaveData` currently doesn't, but should for complete state
   - **Overlap: Runtime event state**

**Distinct Aspects:**

1. **SaveData Specific:**
   - Focused on production save/load
   - Single point-in-time snapshot
   - Includes SceneManager state (subscriptions)
   - Missing: Engine-level state (EngineState, EngineConfig)
   - Missing: EventBus state

2. **TestData Specific:**
   - Focused on testing and validation
   - Multiple snapshots (starting + expected per tick)
   - Simulation configuration (LogicClasses, Functions, Inputs, Events)
   - Tick-based execution model
   - Extensive state capture (entities + events per tick)

### 4. Architecture Differences

**SaveData Approach:**
- **Monolithic snapshot** - Single complete game state at save time
- **Production-focused** - Used by GameEngine for save/load
- **Provider pattern** - `ISaveDataProvider` abstracts data source
- **Importer pattern** - `IEntityImporter` defers entity loading

**TestData Approach:**
- **Multi-snapshot** - Starting state + expected states per tick
- **Test-focused** - Used by TestEngine for validation
- **Simulation-driven** - Specifies logic to execute between states
- **Provider pattern** - `ITestDataProvider` abstracts data source
- **Importer pattern** - Uses same `IEntityImporter` as SaveData

## Areas of Concern

### 1. SaveData Incompleteness

**Issue:** `SaveData` doesn't use EngineSnapshot as common structure

**Current State:**
- SaveData has its own fields duplicating what EngineSnapshot provides
- TestData already uses EngineSnapshot
- Creates duplication and potential inconsistency

**Better Approach:**
- Use EngineSnapshot as the common structure for engine state
- SaveData = SaveMetaData + EngineSnapshot
- Single source of truth for "engine state at a point in time"

**NOT Included (Intentionally Separate):**
- `EngineConfig` (user preferences, display settings) - These are user-specific global settings, NOT per-save data. Already handled separately via default.preferences.bin and user preference files.

**Recommendation:** Refactor SaveData to use EngineSnapshot:
```cpp
struct SaveData {
  SaveMetaData meta_data;
  EngineSnapshot snapshot;  // Common structure with TestData!
  
  // EngineSnapshot will contain all engine state:
  // - SceneCollectionData
  // - EventBus (non-optional, empty if unused)
  // - std::optional<EngineState>
  // - std::optional<SceneManagerData>
};
```

### 2. TestData vs SaveData Overlap

**Issue:** Both systems need to import/export EntityMemoryPool data

**Current State:** 
- Both use `SceneCollectionData` + `IEntityImporter`
- Both use FlatBuffers for serialization
- Implementation is already shared via importer pattern

**Evaluation:** ✅ **This overlap is good!**
- Code reuse via `IEntityImporter` interface
- Same entity import mechanism for testing and production
- Ensures tests use same data path as production

**Recommendation:** Maintain current approach, no changes needed

### 3. EngineSnapshot vs SaveData Alignment

**Issue:** `EngineSnapshot` has evolved beyond `SaveData` capabilities

**EngineSnapshot Advantages:**
- Captures EventBus state (optional)
- Extensible design with optional fields
- Clear documentation of future extensions
- Used successfully in TestEngine

**Recommendation:** Align `SaveData` with `EngineSnapshot` concepts:
- Add `std::optional<EventBus> event_bus` to SaveData
- Add `EngineState` and `EngineConfig` to SaveData
- Consider making SaveData fields optional for partial saves/loads

### 4. TestEngine Usability

**Current State:** TestEngine is well-designed and easy to use
- Simple construction from TestData
- Automatic tick execution via `RunGameLoop()`
- Automatic snapshot capture per tick
- Clear separation from SaveData workflow

**Evaluation:** ✅ **TestEngine usability is good!**
- Not overly coupled to SaveData concepts
- SimulationData provides test-specific functionality
- Tick-based model is appropriate for testing

**Recommendation:** No changes to TestEngine workflow needed

### 5. Export Functionality Duplication

**Issue:** Need to export engine state for both saves and test generation

**Current State:**
- `TestEngine::CaptureSnapShot()` exports to `EngineSnapshot`
- No equivalent export function for `SaveData` from GameEngine
- Both need similar "export current state" functionality

**Analysis:**
- Both need to deep copy EntityMemoryPool
- Both need to capture EventBus state
- Both need to collect data from active scenes
- Different metadata (SaveMetaData vs TestMetaData)
- Different container structures (SaveData vs EngineSnapshot)

**Recommendation:** Create shared utility functions:
```cpp
namespace steamrot::engine::export {
  
// Export all scene entity data
std::expected<SceneCollectionData, FailInfo> 
ExportSceneCollectionData(const SceneManager& scene_manager);

// Export EventBus state
EventBus ExportEventBus(const EventHandler& event_handler);

// Export EngineState
EngineState ExportEngineState(const Engine& engine);

// Export EngineConfig
EngineConfig ExportEngineConfig(const Engine& engine);

} // namespace steamrot::engine::export
```

Then use in both contexts:
- `GameEngine::SaveGame()` calls export functions → SaveData
- `TestEngine::CaptureSnapShot()` calls export functions → EngineSnapshot

## Long-Term Architecture Recommendations

### Recommendation 1: Extend SaveData to Full Engine State

**Goal:** Enable complete save/load of game state

**Changes:**
```cpp
// src/types/core/SaveData.h
struct SaveData {
  SaveMetaData meta_data;
  EngineSnapshot snapshot;  // NEW: Use EngineSnapshot as common structure!
  
  // EngineSnapshot contains:
  // - SceneCollectionData scene_collection_data
  // - EventBus global_event_bus (non-optional, empty if unused)
  // - std::optional<EngineState> engine_state
  // - std::optional<SceneManagerData> scene_manager_data
  // - std::optional<size_t> tick_number (not used for saves)
  
  // Note: EngineConfig NOT included in EngineSnapshot - user preferences
  // and display settings are global user settings, not per-save game data.
  // They're managed separately via the user preferences system
  // (default.preferences.bin and user-specific preference files).
};
```

**FlatBuffers Schema:**
```fbs
// src/types/flatbuffers/configuration/save_data.fbs
include "../engine/engine_snapshot.fbs";

table SaveDataFbs {
  save_meta_data: SaveMetaDataFbs;
  snapshot: EngineSnapshotFbs;  // Use EngineSnapshot as common structure!
  
  // EngineSnapshotFbs contains all engine state:
  // - scene_collection_data
  // - global_event_bus (vector, empty if unused)
  // - engine_state (optional)
  // - scene_manager_data (optional)
  
  // Note: EngineConfig NOT included in EngineSnapshot - user preferences/
  // display settings are global user settings, not per-save. Handled 
  // separately via default.preferences.bin and user preference system.
}
```

**Benefits:**
- Complete game state restoration
- Aligns with EngineSnapshot capabilities
- Enables save file migration/debugging
- Better data export for test generation

**Migration Path:**
1. Add new fields to SaveData struct
2. Update FlatBuffers schema
3. Update FlatbuffersSaveDataProvider
4. Make fields optional for backward compatibility
5. Update GameEngine save/load methods

### Recommendation 2: Create Shared Export Utilities

**Goal:** Reuse state export logic between GameEngine and TestEngine

**Implementation:**
```cpp
// src/engine/engine_export.h
namespace steamrot::engine::export {

/////////////////////////////////////////////////
/// @brief Export EntityMemoryPool data from all active scenes
/////////////////////////////////////////////////
std::expected<SceneCollectionData, FailInfo>
ExportActiveScenes(const SceneManager& scene_manager, 
                   EventHandler& event_handler);

/////////////////////////////////////////////////
/// @brief Export EventBus state
/////////////////////////////////////////////////
EventBus ExportEventBus(const EventHandler& event_handler);

/////////////////////////////////////////////////
/// @brief Export EngineState
/////////////////////////////////////////////////
EngineState ExportEngineState(const EngineState& engine_state);

/////////////////////////////////////////////////
// Note: ExportEngineConfig NOT needed for SaveData since EngineConfig
// contains user preferences (volume, language, display settings) which
// are global user settings, not per-save game data.

/////////////////////////////////////////////////
/// @brief Export complete EngineSnapshot
/////////////////////////////////////////////////
std::expected<EngineSnapshot, FailInfo>
ExportEngineSnapshot(const Engine& engine, 
                     std::optional<size_t> tick_number = std::nullopt);

} // namespace steamrot::engine::export
```

**Usage in GameEngine:**
```cpp
std::expected<SaveData, FailInfo> GameEngine::SaveGame() {
  SaveData save_data;
  
  // Export to EngineSnapshot using shared utilities
  auto snapshot_result = engine::export::ExportEngineSnapshot(*this);
  if (!snapshot_result.has_value()) return std::unexpected(snapshot_result.error());
  
  save_data.snapshot = std::move(snapshot_result.value());
  
  // SaveData is just metadata + snapshot - clean and simple!
  // Note: EngineConfig NOT in snapshot - user preferences and display 
  // settings are global user settings managed separately by the preference system.
  
  return save_data;
}
```

**Usage in TestEngine:**
```cpp
std::expected<std::monostate, FailInfo>
TestEngine::CaptureSnapShot(size_t tick) {
  // Use shared export utility
  auto snapshot = engine::export::ExportEngineSnapshot(*this, tick);
  if (!snapshot.has_value()) return std::unexpected(snapshot.error());
  
  m_data_bank[tick] = std::move(snapshot.value());
  return std::monostate{};
}
```

**Benefits:**
- Eliminates code duplication
- Ensures consistent export behavior
- Makes TestEngine snapshots match SaveData format
- Easier to maintain and extend
- Facilitates test data generation from real gameplay

### Recommendation 3: Maintain Separate Metadata Types

**Goal:** Keep SaveMetaData and TestMetaData distinct

**Rationale:**
- Different purposes (production save vs test validation)
- Different fields (save_name/file_id vs test_name/will_pass)
- SaveMetaData may evolve with player statistics, achievements
- TestMetaData may evolve with test categorization, priorities

**Recommendation:** ✅ **Keep separate, no unification needed**

### Recommendation 4: Keep TestData Test-Specific Features

**Goal:** Don't force SaveData to support test-specific features

**Test-Specific Features to Keep in TestData:**
- `SimulationData` - Logic execution configuration
- `number_of_ticks` - Tick count for test execution
- `expected_engine_snapshots` - Multi-tick validation
- Input sequences - Mouse/keyboard simulation
- Event sequences - Event injection

**Rationale:**
- These features are testing concerns, not production save/load
- Adding them to SaveData would complicate production usage
- TestEngine already has good separation of concerns

**Recommendation:** ✅ **Keep test features in TestData only**

### Recommendation 5: Align EngineSnapshot with SaveData

**Goal:** Make EngineSnapshot a superset of SaveData capabilities

**Current State:**
- EngineSnapshot has optional EventBus ✅
- EngineSnapshot has SceneCollectionData ✅
- EngineSnapshot is missing EngineConfig ❌
- EngineSnapshot is missing EngineState ❌
- EngineSnapshot is missing SceneManagerData ❌

**Recommended Extensions:**
```cpp
// src/types/core/EngineSnapshot.h
struct EngineSnapshot {
  // Existing fields
  std::optional<size_t> tick_number;
  SceneCollectionData scene_collection_data;
  EventBus global_event_bus;  // Non-optional, empty if unused
  
  // NEW: Add these for completeness
  std::optional<EngineState> engine_state;
  std::optional<SceneManagerData> scene_manager_data;
  
  // Note: EngineConfig intentionally NOT included - user preferences and
  // display settings are global user settings, not per-snapshot state.
  // Tests that need to verify preference-dependent behavior should set
  // up preferences in test fixtures, not in snapshots.
  
  // Vector fields like EventBus are non-optional but can be empty.
  // This is simpler than using std::optional for collections.
  
  bool HasData() const {
    return !global_event_bus.empty() || 
           engine_state.has_value() ||
           scene_manager_data.has_value();
  }
};
```

**Benefits:**
- EngineSnapshot can represent complete engine state
- Tests can validate any aspect of engine state
- SaveData export can reuse EngineSnapshot export logic
- Clear superset relationship: EngineSnapshot ⊇ SaveData data

### Recommendation 6: SaveData Contains EngineSnapshot

**Goal:** Use EngineSnapshot as common structure for engine state

**Concept:**
```
SaveData = SaveMetaData + EngineSnapshot
Both SaveData and TestData use EngineSnapshot to represent engine state
```

**Implementation (RECOMMENDED):**
```cpp
struct SaveData {
  SaveMetaData meta_data;
  EngineSnapshot snapshot; // Common structure for engine state!
};
```

**This approach is better because:**
- Single source of truth for "engine state at a point in time"
- Eliminates duplication between SaveData and EngineSnapshot
- SaveData is conceptually just "metadata + snapshot"
- TestData already uses EngineSnapshot this way
- Export utilities work the same for both (export to EngineSnapshot)
- Simpler architecture with less code to maintain

**Handling Required vs Optional Fields:**
- EngineSnapshot uses optional fields for testing flexibility
- SaveData validation enforces that required fields are populated at save/load time
- Vector fields (like EventBus) can be non-optional empty vectors instead of optional
- This keeps the structure flexible while ensuring complete saves

### Recommendation 7: Enable SaveData → TestData Conversion

**Goal:** Generate test data from real gameplay saves

**Use Case:** 
- Play game → Save → Convert save to test data
- Use real game states for regression testing
- Validate game behavior at specific saved points

**Utility Function:**
```cpp
// src/data_providers/SaveDataToTestDataConverter.h
namespace steamrot::conversion {

/////////////////////////////////////////////////
/// @brief Convert SaveData to TestData for testing
///
/// Creates a TestData instance from a SaveData save file,
/// useful for generating test cases from real gameplay.
///
/// @param save_data SaveData to convert
/// @param test_name Name for the generated test
/// @param test_description Description for the test
/// @return TestData instance for testing
/////////////////////////////////////////////////
std::expected<TestData, FailInfo>
ConvertSaveDataToTestData(const SaveData& save_data,
                          const std::string& test_name,
                          const std::string& test_description);

} // namespace steamrot::conversion
```

**Implementation:**
```cpp
std::expected<TestData, FailInfo>
ConvertSaveDataToTestData(const SaveData& save_data,
                          const std::string& test_name,
                          const std::string& test_description) {
  TestData test_data;
  
  // Convert metadata
  test_data.meta_data.test_name = test_name;
  test_data.meta_data.test_description = test_description;
  test_data.meta_data.will_pass = true;
  
  // Convert engine state to starting snapshot
  EngineSnapshot starting_snapshot;
  starting_snapshot.scene_collection_data = save_data.scene_collection_data;
  starting_snapshot.global_event_bus = save_data.event_bus;
  starting_snapshot.engine_state = save_data.engine_state;
  starting_snapshot.scene_manager_data = save_data.scene_manager_data;
  
  // Note: EngineConfig not converted from SaveData since it's not stored there.
  // User preferences are global settings managed separately.
  
  test_data.starting_engine_snapshot = std::move(starting_snapshot);
  test_data.number_of_ticks = 1; // Default: single tick validation
  
  return test_data;
}
```

**Benefits:**
- Leverage real gameplay for test generation
- Regression testing of specific game states
- Debug problematic saves by converting to tests
- Validate save/load round-trip correctness

## Summary of Recommendations

### ✅ Good as-is (No changes needed)

1. **Entity Import/Export Overlap** - Shared `IEntityImporter` pattern works well
2. **TestEngine Usability** - Already easy to use, good separation of concerns
3. **Separate Metadata Types** - SaveMetaData and TestMetaData serve different purposes
4. **Test-Specific Features** - Keep SimulationData, tick logic, etc. in TestData only

### 📝 Recommended Improvements

1. **Refactor SaveData** - Use EngineSnapshot as common structure (NOT separate fields)
2. **Extend EngineSnapshot** - Add EngineState, SceneManagerData (NOT EngineConfig - global settings)
3. **Shared Export Utilities** - Create `engine::export` namespace exporting to EngineSnapshot
4. **Vector Fields Non-Optional** - Use empty vectors instead of optional (simpler for collections like EventBus)
5. **SaveData ↔ TestData Conversion** - Enable test generation from saves

### 🎯 Implementation Priority

**Phase 1: Core State Capture** (High Priority)
- Refactor SaveData to contain EngineSnapshot instead of separate fields
- Extend EngineSnapshot with EngineState, SceneManagerData (NOT EngineConfig - global settings)
- Make EventBus non-optional (empty vector if unused)
- Update FlatBuffers schema
- Update FlatbuffersSaveDataProvider

**Phase 2: Code Reuse** (Medium Priority)
- Create `engine::export` namespace with utilities:
  - `ExportEngineSnapshot()` - Main export function
  - `ExportActiveScenes()`
  - `ExportEventBus()`
  - `ExportEngineState()`
- Both TestEngine::CaptureSnapShot and GameEngine::SaveGame export to EngineSnapshot
- Single source of truth for state export

**Phase 3: Testing Integration** (Low Priority)
- Implement SaveData → TestData conversion utility
- Extend EngineSnapshot with optional engine-level fields
- Add tools for generating test data from gameplay

## Architectural Principles

1. **Separation of Concerns**
   - SaveData: Production save/load
   - TestData: Testing and validation
   - Shared: Entity import/export via IEntityImporter

2. **Code Reuse via Utilities**
   - Export functions in `engine::export` namespace
   - Used by both GameEngine and TestEngine
   - Ensures consistent behavior

3. **Extensibility**
   - Optional fields in EngineSnapshot for selective capture
   - SaveData can grow to include more engine state
   - TestData maintains test-specific features

4. **No Forced Unification**
   - Don't make SaveData support test features
   - Don't make TestData mimic SaveData exactly
   - Share implementation, not interfaces

5. **Clear Data Flow**
   - GameEngine → SaveData → File (production)
   - File → TestData → TestEngine (testing)
   - SaveData → TestData (optional conversion)

## Conclusion

The current architecture has good bones:
- ✅ Entity import/export is properly shared via `IEntityImporter`
- ✅ TestEngine is well-designed and easy to use
- ✅ Clear separation between production and testing concerns

Areas for improvement:
- ❌ SaveData is incomplete (missing engine-level state)
- ❌ Code duplication in state export (TestEngine vs potential SaveGame)
- ❌ EngineSnapshot and SaveData not aligned

Recommended path forward:
1. Extend SaveData to capture complete engine state
2. Create shared export utilities to eliminate duplication
3. Align EngineSnapshot capabilities with SaveData needs
4. Optional: Add SaveData → TestData conversion for test generation

This approach maximizes code reuse while maintaining clear boundaries and preserving the excellent usability of the TestEngine system.
