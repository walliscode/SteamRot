# EngineSnapshot Integration: Analysis and Implementation

## Executive Summary

This document provides a comprehensive analysis and implementation strategy for using `EngineSnapshot` as the primary container for setting up a `GameEngine` (or `TestEngine`) and exporting data for saving or testing purposes.

## Current State Analysis

### Existing EngineSnapshot Structure

The `EngineSnapshot` struct currently exists in `src/types/core/EngineSnapshot.h` with the following capabilities:

```cpp
struct EngineSnapshot {
  std::optional<size_t> tick_number;
  SceneCollectionData scene_collection_data;
  std::optional<EventBus> global_event_bus;
  bool HasData() const;
};
```

**Current Usage:**
- Used by `TestEngine` to capture and store engine state at specific ticks
- Stores scene entity state (EntityMemoryPool per scene, keyed by UUID)
- Captures global event bus state
- Used in `TestData` for test simulation validation

### Current Engine Initialization

**Engine Base Class (`Engine`):**
- Initialized via `StartUp()` method
- Loads `EngineData` through `IEngineDataProvider` interface
- `EngineData` contains:
  - `EngineResourcesConfig` (window, display settings)
  - `EngineState` (runtime state, subscriptions, flags)
  - `EngineConfig` (user preferences, display config)
  - `AssetConfig` (initial assets to load)

**Data Flow:**
```
IEngineDataProvider::LoadEngineData()
    ↓
EngineData (config + state + resources)
    ↓
Engine::StartUp() configures:
    - EngineResources (window, event handler, asset manager)
    - EngineConfig (display, preferences)
    - EngineState (subscriptions, flags)
    - SceneManager (loads initial scenes)
```

### Current Save/Load Architecture

**SaveData Structure:**
- `SaveMetaData` (save name, file ID, timestamp)
- `SceneManagerData` (scene manager state)
- `SceneCollectionData` (serialized scene data)

**Missing:** No direct connection between `EngineSnapshot` and the save/load system.

### Current Testing Architecture

**TestEngine:**
- Uses `TestData` which contains `EngineSnapshot` for starting state and expected states
- Captures snapshots after each tick via `CaptureSnapShot()`
- Stores snapshots in `m_data_bank` for validation

## Problem Statement

The user wants `EngineSnapshot` to serve as:
1. **Input**: A container that can configure and initialize a `GameEngine` or `TestEngine`
2. **Output**: A container that can export current engine state for:
   - Saving game state to disk
   - Testing and validation
   - Serialization for network transmission (future)

## Requirements Analysis

### Functional Requirements

1. **FR1: Engine Initialization from Snapshot**
   - Engine must be configurable from an `EngineSnapshot`
   - Should support partial snapshots (optional fields)
   - Must handle both cold start (new game) and restore (load save)

2. **FR2: Engine State Export to Snapshot**
   - Engine must be able to export current state to `EngineSnapshot`
   - Should capture all runtime state needed for save/restore
   - Must support selective export (only specific data)

3. **FR3: Save/Load Integration**
   - `EngineSnapshot` should be convertible to/from `SaveData`
   - Must support FlatBuffers serialization
   - Should maintain backward compatibility with existing save format

4. **FR4: Testing Integration**
   - Maintain current `TestEngine` snapshot usage
   - Support data-driven testing workflows
   - Enable snapshot-based test assertions

### Non-Functional Requirements

1. **NFR1: Extensibility**
   - Easy to add new snapshot fields without breaking existing code
   - Optional fields allow incremental adoption

2. **NFR2: Performance**
   - Snapshot capture should be efficient (acceptable for per-tick capture in tests)
   - No performance degradation for normal game loop

3. **NFR3: Maintainability**
   - Clear separation of concerns
   - Consistent with existing architecture patterns
   - Well-documented interfaces

## Design Analysis

### Option 1: Provider/Consumer Pattern (RECOMMENDED)

**Description:** Create separate interfaces for reading from and writing to `EngineSnapshot`, following the existing provider pattern used throughout the codebase.

**Interfaces:**

```cpp
// Interface for consuming snapshot data (input)
class IEngineSnapshotConsumer {
public:
  virtual ~IEngineSnapshotConsumer() = default;
  
  // Configure engine from snapshot
  virtual std::expected<std::monostate, FailInfo>
  ConsumeSnapshot(const EngineSnapshot& snapshot) = 0;
  
  // Apply partial snapshot (update specific fields)
  virtual std::expected<std::monostate, FailInfo>
  ApplySnapshotUpdate(const EngineSnapshot& snapshot) = 0;
};

// Interface for producing snapshot data (output)
class IEngineSnapshotProvider {
public:
  virtual ~IEngineSnapshotProvider() = default;
  
  // Capture complete snapshot
  virtual std::expected<EngineSnapshot, FailInfo>
  CaptureSnapshot() const = 0;
  
  // Capture partial snapshot (only specified fields)
  virtual std::expected<EngineSnapshot, FailInfo>
  CapturePartialSnapshot(SnapshotFields fields) const = 0;
};
```

**Integration with Engine:**

```cpp
class Engine : public IEngineSnapshotProvider, public IEngineSnapshotConsumer {
protected:
  // Implement provider interface
  std::expected<EngineSnapshot, FailInfo> CaptureSnapshot() const override;
  
  // Implement consumer interface
  std::expected<std::monostate, FailInfo> 
  ConsumeSnapshot(const EngineSnapshot& snapshot) override;
  
  std::expected<std::monostate, FailInfo>
  ApplySnapshotUpdate(const EngineSnapshot& snapshot) override;
  
  // Helper methods
  std::expected<std::monostate, FailInfo>
  ConfigureFromSnapshot(const EngineSnapshot& snapshot);
  
  std::expected<std::monostate, FailInfo>
  RestoreStateFromSnapshot(const EngineSnapshot& snapshot);
};
```

**Pros:**
- ✅ Consistent with existing codebase patterns (IEngineDataProvider, IEntityImporter/Exporter)
- ✅ Clear separation of concerns (read vs write)
- ✅ Testable in isolation
- ✅ Supports polymorphism and mocking
- ✅ Explicit interface contracts

**Cons:**
- ⚠️ More boilerplate code
- ⚠️ Two interfaces to maintain

### Option 2: Direct Methods on Engine

**Description:** Add snapshot methods directly to `Engine` class without interfaces.

**Implementation:**

```cpp
class Engine {
public:
  // Capture current state to snapshot
  std::expected<EngineSnapshot, FailInfo> ExportSnapshot() const;
  
  // Initialize from snapshot
  std::expected<std::monostate, FailInfo> 
  InitializeFromSnapshot(const EngineSnapshot& snapshot);
  
  // Update state from snapshot
  std::expected<std::monostate, FailInfo>
  RestoreFromSnapshot(const EngineSnapshot& snapshot);
};
```

**Pros:**
- ✅ Simpler implementation
- ✅ Less code to maintain
- ✅ Direct access, no indirection

**Cons:**
- ❌ Inconsistent with codebase patterns
- ❌ Harder to test in isolation
- ❌ No polymorphism support
- ❌ Tight coupling

### Option 3: Snapshot as Constructor Parameter

**Description:** Add constructor overload accepting `EngineSnapshot`.

**Implementation:**

```cpp
class Engine {
public:
  // Existing default constructor
  Engine();
  
  // New: construct from snapshot
  explicit Engine(const EngineSnapshot& snapshot);
};
```

**Pros:**
- ✅ Initialization guaranteed before use
- ✅ Immutable initialization pattern

**Cons:**
- ❌ Cannot change snapshot after construction
- ❌ Complex two-phase initialization required
- ❌ Doesn't support runtime state restore
- ❌ Breaks existing constructor patterns

### Recommendation: Option 1 (Provider/Consumer Pattern)

**Rationale:**
1. Consistent with existing architecture (IEngineDataProvider, IEntityImporter/Exporter)
2. Follows Single Responsibility Principle (separate read/write concerns)
3. Testable and mockable
4. Extensible for future needs
5. Clear interface contracts

## Detailed Design: Provider/Consumer Implementation

### Phase 1: Extend EngineSnapshot

**Enhance `EngineSnapshot` to capture all necessary state:**

```cpp
struct EngineSnapshot {
  // Existing fields
  std::optional<size_t> tick_number;
  SceneCollectionData scene_collection_data;
  std::optional<EventBus> global_event_bus;
  
  // New fields for complete state capture
  std::optional<EngineConfig> engine_config;
  std::optional<EngineState> engine_state;
  std::optional<EngineResourcesConfig> engine_resources_config;
  std::optional<AssetConfig> loaded_assets;
  std::optional<SceneManagerData> scene_manager_data;
  
  // Enhanced helper
  bool HasData() const;
  bool HasMinimalData() const;  // For initialization
  bool HasCompleteData() const; // For save/restore
};
```

### Phase 2: Create Interfaces

**File: `src/types/interfaces/IEngineSnapshotProvider.h`**

```cpp
namespace steamrot {

/////////////////////////////////////////////////
/// @enum SnapshotFields
/// @brief Flags for selective snapshot capture
/////////////////////////////////////////////////
enum class SnapshotFields : uint32_t {
  None = 0,
  TickNumber = 1 << 0,
  SceneData = 1 << 1,
  EventBus = 1 << 2,
  EngineConfig = 1 << 3,
  EngineState = 1 << 4,
  Resources = 1 << 5,
  Assets = 1 << 6,
  All = 0xFFFFFFFF
};

/////////////////////////////////////////////////
/// @class IEngineSnapshotProvider
/// @brief Interface for capturing engine state as snapshot
/////////////////////////////////////////////////
class IEngineSnapshotProvider {
public:
  virtual ~IEngineSnapshotProvider() = default;
  
  /////////////////////////////////////////////////
  /// @brief Capture complete engine state snapshot
  ///
  /// @return EngineSnapshot with all available data or FailInfo
  /////////////////////////////////////////////////
  virtual std::expected<EngineSnapshot, FailInfo>
  CaptureSnapshot() const = 0;
  
  /////////////////////////////////////////////////
  /// @brief Capture partial snapshot with selected fields
  ///
  /// @param fields Bitfield of SnapshotFields to capture
  /// @return EngineSnapshot with selected data or FailInfo
  /////////////////////////////////////////////////
  virtual std::expected<EngineSnapshot, FailInfo>
  CapturePartialSnapshot(SnapshotFields fields) const = 0;
};

} // namespace steamrot
```

**File: `src/types/interfaces/IEngineSnapshotConsumer.h`**

```cpp
namespace steamrot {

/////////////////////////////////////////////////
/// @enum SnapshotMode
/// @brief Mode for applying snapshot to engine
/////////////////////////////////////////////////
enum class SnapshotMode {
  Initialize,  // Cold start, configure from scratch
  Restore,     // Hot restore, update existing state
  Merge        // Merge snapshot data with current state
};

/////////////////////////////////////////////////
/// @class IEngineSnapshotConsumer
/// @brief Interface for configuring engine from snapshot
/////////////////////////////////////////////////
class IEngineSnapshotConsumer {
public:
  virtual ~IEngineSnapshotConsumer() = default;
  
  /////////////////////////////////////////////////
  /// @brief Configure engine from snapshot
  ///
  /// Used for cold start initialization. Configures engine
  /// resources, state, and scenes from snapshot data.
  ///
  /// @param snapshot Complete or partial snapshot data
  /// @return Success or FailInfo with error details
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ConsumeSnapshot(const EngineSnapshot& snapshot) = 0;
  
  /////////////////////////////////////////////////
  /// @brief Apply snapshot update to running engine
  ///
  /// Used for hot restore during runtime. Updates engine
  /// state from snapshot without full reinitialization.
  ///
  /// @param snapshot Snapshot with data to update
  /// @param mode How to apply the snapshot
  /// @return Success or FailInfo with error details
  /////////////////////////////////////////////////
  virtual std::expected<std::monostate, FailInfo>
  ApplySnapshotUpdate(const EngineSnapshot& snapshot, 
                      SnapshotMode mode = SnapshotMode::Merge) = 0;
};

} // namespace steamrot
```

### Phase 3: Implement in Engine Base Class

**File: `src/engine/Engine.h`** (additions)

```cpp
class Engine : public IEngineSnapshotProvider, 
               public IEngineSnapshotConsumer {
protected:
  /////////////////////////////////////////////////
  /// @brief Capture complete snapshot (implements IEngineSnapshotProvider)
  /////////////////////////////////////////////////
  std::expected<EngineSnapshot, FailInfo> 
  CaptureSnapshot() const override;
  
  /////////////////////////////////////////////////
  /// @brief Capture partial snapshot (implements IEngineSnapshotProvider)
  /////////////////////////////////////////////////
  std::expected<EngineSnapshot, FailInfo>
  CapturePartialSnapshot(SnapshotFields fields) const override;
  
  /////////////////////////////////////////////////
  /// @brief Configure from snapshot (implements IEngineSnapshotConsumer)
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConsumeSnapshot(const EngineSnapshot& snapshot) override;
  
  /////////////////////////////////////////////////
  /// @brief Apply snapshot update (implements IEngineSnapshotConsumer)
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ApplySnapshotUpdate(const EngineSnapshot& snapshot,
                      SnapshotMode mode = SnapshotMode::Merge) override;
  
  /////////////////////////////////////////////////
  /// @brief Helper: Configure engine resources from snapshot
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureResourcesFromSnapshot(const EngineSnapshot& snapshot);
  
  /////////////////////////////////////////////////
  /// @brief Helper: Configure engine state from snapshot
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureStateFromSnapshot(const EngineSnapshot& snapshot);
  
  /////////////////////////////////////////////////
  /// @brief Helper: Configure scenes from snapshot
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureScenesFromSnapshot(const EngineSnapshot& snapshot);

public:
  /////////////////////////////////////////////////
  /// @brief New constructor: Initialize engine from snapshot
  ///
  /// @param snapshot Initial engine configuration and state
  /////////////////////////////////////////////////
  explicit Engine(const EngineSnapshot& snapshot);
  
  // Existing default constructor
  explicit Engine();
};
```

### Phase 4: Update TestEngine

**Maintain backward compatibility while using new interfaces:**

```cpp
class TestEngine : public Engine {
private:
  /////////////////////////////////////////////////
  /// @brief Override to use new snapshot capture interface
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  CaptureSnapShot(size_t tick) {
    auto snapshot_result = CaptureSnapshot();
    if (!snapshot_result) {
      return std::unexpected(snapshot_result.error());
    }
    
    EngineSnapshot snapshot = std::move(snapshot_result.value());
    snapshot.tick_number = tick;
    m_data_bank[tick] = std::move(snapshot);
    return std::monostate{};
  }
  
public:
  /////////////////////////////////////////////////
  /// @brief New: Initialize TestEngine from snapshot
  ///
  /// @param test_data Test configuration
  /// @param initial_snapshot Optional initial state
  /////////////////////////////////////////////////
  TestEngine(const TestData& test_data, 
             const std::optional<EngineSnapshot>& initial_snapshot = std::nullopt);
};
```

### Phase 5: Save/Load Integration

**Create conversion utilities between `EngineSnapshot` and `SaveData`:**

```cpp
namespace steamrot {

/////////////////////////////////////////////////
/// @brief Convert EngineSnapshot to SaveData format
///
/// @param snapshot Source snapshot
/// @param meta_data Save metadata (name, timestamp, etc.)
/// @return SaveData ready for serialization
/////////////////////////////////////////////////
std::expected<SaveData, FailInfo>
ConvertSnapshotToSaveData(const EngineSnapshot& snapshot,
                          const SaveMetaData& meta_data);

/////////////////////////////////////////////////
/// @brief Convert SaveData to EngineSnapshot format
///
/// @param save_data Source save data
/// @return EngineSnapshot ready for engine initialization
/////////////////////////////////////////////////
std::expected<EngineSnapshot, FailInfo>
ConvertSaveDataToSnapshot(const SaveData& save_data);

} // namespace steamrot
```

### Phase 6: FlatBuffers Schema

**Create `engine_snapshot.fbs` schema:**

```fbs
include "entities.fbs";
include "event_bus_data.fbs";
include "engine_config.fbs";
include "engine_state.fbs";
include "engine_resources_config.fbs";
include "asset_config.fbs";
include "scene_collection_data.fbs";

namespace steamrot;

table EngineSnapshotFbs {
  tick_number: uint64;
  scene_collection_data: SceneCollectionDataFbs;
  global_event_bus: EventBusDataFbs;
  engine_config: EngineConfigFbs;
  engine_state: EngineStateFbs;
  engine_resources_config: EngineResourcesConfigFbs;
  loaded_assets: AssetConfigFbs;
  scene_manager_data: SceneManagerDataFbs;
}

root_type EngineSnapshotFbs;
```

## Potential Issues and Solutions

### Issue 1: Two-Phase Initialization Complexity

**Problem:** Engine currently uses `StartUp()` for initialization after construction. Adding snapshot-based construction creates two initialization paths.

**Solution:**
1. Make `Engine(const EngineSnapshot&)` constructor call a common initialization helper
2. Both default constructor + `StartUp()` and snapshot constructor use the same helpers
3. Deprecate (but maintain) the two-phase initialization for backward compatibility

```cpp
Engine::Engine() {
  InitializeDefaults();
}

Engine::Engine(const EngineSnapshot& snapshot) {
  InitializeDefaults();
  auto result = ConsumeSnapshot(snapshot);
  if (!result) {
    throw EngineInitializationException(result.error());
  }
}

std::expected<std::monostate, FailInfo> Engine::StartUp() {
  // Load from data provider and call ConsumeSnapshot internally
  auto data_provider_result = /* ... */;
  EngineSnapshot snapshot = ConvertEngineDataToSnapshot(engine_data);
  return ConsumeSnapshot(snapshot);
}
```

### Issue 2: Partial vs Complete Snapshots

**Problem:** Not all snapshots have all data. Engine needs to handle partial snapshots gracefully.

**Solution:**
1. Use `std::optional` for all snapshot fields (already done)
2. Add validation methods: `HasMinimalData()`, `HasCompleteData()`
3. Clearly document which fields are required for each use case:
   - **Testing:** SceneData, EventBus (optional others)
   - **Saving:** All fields required
   - **Network sync:** Selective fields based on what changed

```cpp
bool EngineSnapshot::HasMinimalData() const {
  return scene_collection_data.size() > 0;
}

bool EngineSnapshot::HasCompleteData() const {
  return tick_number.has_value() &&
         !scene_collection_data.empty() &&
         engine_config.has_value() &&
         engine_state.has_value() &&
         engine_resources_config.has_value();
}
```

### Issue 3: Scene Manager State Synchronization

**Problem:** `SceneManager` maintains its own state separate from snapshot. Need to ensure consistency.

**Solution:**
1. Capture `SceneManagerData` in snapshot (already in `SaveData`)
2. Add methods to `SceneManager` for snapshot integration:
   ```cpp
   std::expected<SceneManagerData, FailInfo> 
   SceneManager::ExportData() const;
   
   std::expected<std::monostate, FailInfo>
   SceneManager::ImportData(const SceneManagerData& data);
   ```
3. Engine snapshot methods delegate to SceneManager

### Issue 4: Resource Ownership and Lifetime

**Problem:** Some engine resources (SFML window, OpenGL contexts) cannot be serialized or transferred between engine instances.

**Solution:**
1. `EngineResourcesConfig` stores configuration, not resources
2. On restore, recreate resources from config
3. Document non-serializable resources clearly
4. Add resource recreation helpers

```cpp
std::expected<std::monostate, FailInfo>
Engine::RecreateResourcesFromConfig(const EngineResourcesConfig& config) {
  // Close existing window
  if (m_engine_resources.game_window.isOpen()) {
    m_engine_resources.game_window.close();
  }
  
  // Recreate with new config
  return engine::ConfigureEngineResources(m_engine_resources, config);
}
```

### Issue 5: Backward Compatibility

**Problem:** Existing code uses `IEngineDataProvider` and `StartUp()`. Need smooth migration path.

**Solution:**
1. Keep existing interfaces functional
2. Internally convert `EngineData` to `EngineSnapshot`
3. Provide migration guide
4. Use deprecation warnings for old patterns
5. Add adapter utility:

```cpp
EngineSnapshot ConvertEngineDataToSnapshot(const EngineData& engine_data) {
  EngineSnapshot snapshot;
  snapshot.engine_config = engine_data.engine_config;
  snapshot.engine_state = engine_data.engine_state;
  snapshot.engine_resources_config = engine_data.engine_resources_config;
  snapshot.loaded_assets = engine_data.initial_asset_config;
  return snapshot;
}
```

### Issue 6: Performance Impact

**Problem:** Creating complete snapshots every tick (as TestEngine does) could be expensive.

**Solution:**
1. Use `CapturePartialSnapshot()` when full snapshot not needed
2. Implement copy-on-write or move semantics where possible
3. Profile snapshot capture and optimize hot paths
4. Consider lazy capture (defer until actually needed)

```cpp
// Example: TestEngine only needs scene data most of the time
auto snapshot = CapturePartialSnapshot(
  SnapshotFields::SceneData | SnapshotFields::TickNumber);
```

## Implementation Phases

### Phase 1: Foundation (Week 1)
- [ ] Extend `EngineSnapshot` with new optional fields
- [ ] Create `IEngineSnapshotProvider` interface
- [ ] Create `IEngineSnapshotConsumer` interface
- [ ] Add `SnapshotFields` and `SnapshotMode` enums
- [ ] Write unit tests for interfaces

### Phase 2: Engine Integration (Week 1-2)
- [ ] Implement provider interface in `Engine`
- [ ] Implement consumer interface in `Engine`
- [ ] Add snapshot-based constructor
- [ ] Refactor `StartUp()` to use snapshot internally
- [ ] Update `TestEngine` to use new interfaces
- [ ] Write integration tests

### Phase 3: Save/Load Integration (Week 2)
- [ ] Create `EngineSnapshotFbs` FlatBuffers schema
- [ ] Implement `ConvertSnapshotToSaveData()`
- [ ] Implement `ConvertSaveDataToSnapshot()`
- [ ] Update `FlatbuffersSaveDataProvider` to use snapshots
- [ ] Write serialization tests

### Phase 4: SceneManager Integration (Week 2-3)
- [ ] Add `ExportData()` to `SceneManager`
- [ ] Add `ImportData()` to `SceneManager`
- [ ] Integrate with Engine snapshot capture
- [ ] Write SceneManager snapshot tests

### Phase 5: Documentation and Migration (Week 3)
- [ ] Update architecture documentation
- [ ] Write migration guide from `EngineData` to `EngineSnapshot`
- [ ] Add code examples
- [ ] Update workflow documentation
- [ ] Add deprecation notices

## Testing Strategy

### Unit Tests

**Interface Tests:**
```cpp
TEST_CASE("IEngineSnapshotProvider interface contract", "[engine][snapshot]") {
  // Test that implementations fulfill interface contract
}

TEST_CASE("IEngineSnapshotConsumer interface contract", "[engine][snapshot]") {
  // Test that implementations fulfill interface contract
}
```

**Snapshot Tests:**
```cpp
TEST_CASE("EngineSnapshot captures complete state", "[engine][snapshot]") {
  // Test full snapshot capture
}

TEST_CASE("EngineSnapshot captures partial state", "[engine][snapshot]") {
  // Test selective snapshot capture
}

TEST_CASE("EngineSnapshot validation methods", "[engine][snapshot]") {
  // Test HasMinimalData(), HasCompleteData()
}
```

### Integration Tests

**Engine Initialization:**
```cpp
TEST_CASE("Engine initializes from complete snapshot", "[engine][snapshot][integration]") {
  EngineSnapshot snapshot = CreateTestSnapshot();
  Engine engine(snapshot);
  // Verify engine configured correctly
}

TEST_CASE("Engine initializes from partial snapshot", "[engine][snapshot][integration]") {
  EngineSnapshot snapshot = CreateMinimalSnapshot();
  Engine engine(snapshot);
  // Verify engine uses defaults for missing fields
}
```

**Save/Load Round Trip:**
```cpp
TEST_CASE("Engine state survives save/load cycle", "[engine][snapshot][save][integration]") {
  // Capture snapshot -> Convert to SaveData -> Serialize -> Deserialize -> 
  // Convert to snapshot -> Initialize engine -> Compare states
}
```

### System Tests

**TestEngine Workflow:**
```cpp
TEST_CASE("TestEngine uses snapshot for simulation", "[engine][testengine][system]") {
  // Create TestData with EngineSnapshot
  // Run TestEngine
  // Verify captured snapshots match expected
}
```

## Migration Guide

### For Existing Code Using IEngineDataProvider

**Before:**
```cpp
Engine engine;
auto result = engine.RunGame(); // Calls StartUp() internally
```

**After (Option 1 - No changes needed):**
```cpp
// Existing code continues to work
Engine engine;
auto result = engine.RunGame(); // Still works, uses snapshot internally
```

**After (Option 2 - Use snapshot directly):**
```cpp
// Load snapshot from provider
auto snapshot_result = snapshot_provider.LoadSnapshot();
if (!snapshot_result) { /* handle error */ }

// Initialize directly from snapshot
Engine engine(snapshot_result.value());
engine.RunGameLoop();
```

### For TestEngine Usage

**Before:**
```cpp
TestEngine engine(test_data);
auto result = engine.RunGame();
const auto& snapshots = engine.GetDataBank();
```

**After:**
```cpp
// Can optionally provide initial snapshot
EngineSnapshot initial_snapshot = CreateInitialState();
TestEngine engine(test_data, initial_snapshot);
auto result = engine.RunGame();
const auto& snapshots = engine.GetDataBank();
```

## Conclusion

The Provider/Consumer pattern provides a robust, extensible, and architecturally consistent approach to integrating `EngineSnapshot` as the primary container for engine initialization and state export.

**Key Benefits:**
1. ✅ Unified data model for initialization, save/load, and testing
2. ✅ Extensible via optional fields (non-breaking changes)
3. ✅ Consistent with existing codebase patterns
4. ✅ Clear separation of concerns
5. ✅ Testable and maintainable
6. ✅ Backward compatible migration path

**Trade-offs:**
1. ⚠️ More initial implementation complexity
2. ⚠️ Requires refactoring existing initialization path
3. ⚠️ Need to maintain both snapshot and legacy paths during migration

**Recommendation:** Proceed with implementation using the phased approach outlined above, starting with Phase 1 (Foundation) to establish interfaces and extended snapshot structure.
