# EngineSnapshot Integration - Quick Reference

This is a quick reference companion to the comprehensive analysis document (`ENGINE_SNAPSHOT_INTEGRATION.md`).

## TL;DR

**Goal**: Make `EngineSnapshot` the primary container for both:
- **Input**: Initializing Engine (GameEngine/TestEngine)
- **Output**: Exporting engine state (for saving/testing)

**Recommended Solution**: Provider/Consumer Pattern with two interfaces:
- `IEngineSnapshotProvider` (output/export)
- `IEngineSnapshotConsumer` (input/import)

## Quick Architecture Overview

```
                    ╔═══════════════════════════════╗
                    ║     EngineSnapshot            ║
                    ║  (Central State Container)    ║
                    ╚═══════════════════════════════╝
                              ↑         ↓
                    ┌─────────┴─────────┴─────────┐
                    │                              │
         ┌──────────▼──────────┐        ┌─────────▼──────────┐
         │ IEngineSnapshot     │        │ IEngineSnapshot    │
         │ Consumer            │        │ Provider           │
         │ (Input/Import)      │        │ (Output/Export)    │
         └──────────┬──────────┘        └─────────┬──────────┘
                    │                              │
                    └──────────┬───────────────────┘
                               │
                    ┌──────────▼──────────┐
                    │     Engine          │
                    │  (implements both)  │
                    └─────────────────────┘
```

## Interface Signatures

### IEngineSnapshotProvider (Export)

```cpp
class IEngineSnapshotProvider {
  virtual std::expected<EngineSnapshot, FailInfo>
  CaptureSnapshot() const = 0;
  
  virtual std::expected<EngineSnapshot, FailInfo>
  CapturePartialSnapshot(SnapshotFields fields) const = 0;
};
```

**Use Cases:**
- Save game state to disk
- Capture test snapshots
- Network state sync (future)

### IEngineSnapshotConsumer (Import)

```cpp
class IEngineSnapshotConsumer {
  virtual std::expected<std::monostate, FailInfo>
  ConsumeSnapshot(const EngineSnapshot& snapshot) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ApplySnapshotUpdate(const EngineSnapshot& snapshot, 
                      SnapshotMode mode) = 0;
};
```

**Use Cases:**
- Load saved game
- Initialize from test data
- Restore checkpoints

## Extended EngineSnapshot Fields

### Current (Limited)
```cpp
struct EngineSnapshot {
  std::optional<size_t> tick_number;
  SceneCollectionData scene_collection_data;
  std::optional<EventBus> global_event_bus;
};
```

### Proposed (Complete)
```cpp
struct EngineSnapshot {
  // Existing
  std::optional<size_t> tick_number;
  SceneCollectionData scene_collection_data;
  std::optional<EventBus> global_event_bus;
  
  // New for complete state
  std::optional<EngineConfig> engine_config;
  std::optional<EngineState> engine_state;
  std::optional<EngineResourcesConfig> engine_resources_config;
  std::optional<AssetConfig> loaded_assets;
  std::optional<SceneManagerData> scene_manager_data;
  
  // Validation helpers
  bool HasMinimalData() const;    // For testing
  bool HasCompleteData() const;   // For saving
};
```

## Key Design Decisions

### 1. Why Provider/Consumer Pattern?

| Aspect | Benefit |
|--------|---------|
| **Consistency** | Matches `IEngineDataProvider`, `IEntityImporter/Exporter` |
| **Separation of Concerns** | Read and write operations separated |
| **Testability** | Easy to mock and test in isolation |
| **Polymorphism** | Supports different engine types |
| **Extensibility** | Easy to add new snapshot sources/destinations |

### 2. Why Not Direct Methods on Engine?

**Problem with direct methods:**
```cpp
// Less flexible
class Engine {
  EngineSnapshot ExportSnapshot();        // Harder to mock
  void ImportSnapshot(EngineSnapshot);    // Tight coupling
};
```

**Better with interfaces:**
```cpp
// More flexible
class Engine : public IEngineSnapshotProvider, 
               public IEngineSnapshotConsumer {
  // Polymorphic, testable, mockable
};
```

### 3. Optional Fields Strategy

**Benefits:**
- Non-breaking additions (add new fields anytime)
- Partial snapshots for testing (only capture what you need)
- Clear intent (presence = relevant, absence = irrelevant)

**Example:**
```cpp
// Testing: Only need scene data
EngineSnapshot test_snapshot;
test_snapshot.scene_collection_data = test_scenes;
test_snapshot.tick_number = 0;
// Other fields left empty - engine uses defaults

// Saving: Need complete state
EngineSnapshot save_snapshot = engine.CaptureSnapshot();
// All fields populated for full restore
```

## Critical Issues and Solutions

### Issue 1: Two Initialization Paths
**Problem:** Engine has `StartUp()` (existing) and snapshot constructor (new)

**Solution:**
```cpp
// Common initialization helper used by both
Engine::Engine() {
  InitializeDefaults();
}

Engine::Engine(const EngineSnapshot& snapshot) {
  InitializeDefaults();
  ConsumeSnapshot(snapshot);  // Reuse interface
}

std::expected<std::monostate, FailInfo> Engine::StartUp() {
  // Convert EngineData to EngineSnapshot internally
  auto engine_data = data_provider.LoadEngineData();
  EngineSnapshot snapshot = ConvertEngineDataToSnapshot(engine_data);
  return ConsumeSnapshot(snapshot);  // Reuse interface
}
```

### Issue 2: Partial Snapshots
**Problem:** Not all snapshots have all data

**Solution:**
- Use `std::optional` for all fields ✅
- Add validation: `HasMinimalData()`, `HasCompleteData()` ✅
- Document requirements per use case ✅

### Issue 3: SceneManager State
**Problem:** SceneManager has its own state management

**Solution:**
```cpp
class SceneManager {
  std::expected<SceneManagerData, FailInfo> ExportData() const;
  std::expected<std::monostate, FailInfo> ImportData(const SceneManagerData&);
};

// Engine delegates to SceneManager
EngineSnapshot Engine::CaptureSnapshot() const {
  EngineSnapshot snapshot;
  snapshot.scene_manager_data = m_scene_manager.ExportData();
  // ...
  return snapshot;
}
```

### Issue 4: Non-Serializable Resources
**Problem:** SFML window, OpenGL contexts can't be serialized

**Solution:**
- Store **config**, not resources ✅
- Recreate resources from config on restore ✅

```cpp
std::expected<std::monostate, FailInfo>
Engine::RecreateResourcesFromConfig(const EngineResourcesConfig& config) {
  if (m_engine_resources.game_window.isOpen()) {
    m_engine_resources.game_window.close();
  }
  return engine::ConfigureEngineResources(m_engine_resources, config);
}
```

## Implementation Phases (5 weeks)

| Phase | Duration | Deliverables |
|-------|----------|--------------|
| **1. Foundation** | Week 1 | Interfaces, enums, extended snapshot, tests |
| **2. Engine Integration** | Weeks 1-2 | Implement in Engine, refactor StartUp, TestEngine update |
| **3. Save/Load** | Week 2 | FlatBuffers schema, converters, serialization |
| **4. SceneManager** | Weeks 2-3 | Export/Import methods, integration |
| **5. Documentation** | Week 3 | Migration guide, examples, deprecation notices |

## Code Examples

### Example 1: Initialize Engine from Snapshot

```cpp
// Load snapshot (from file, test data, or network)
EngineSnapshot snapshot = LoadSnapshotFromFile("save_001.bin");

// Direct initialization
Engine engine(snapshot);
engine.RunGameLoop();
```

### Example 2: Save Current Engine State

```cpp
// Capture complete state
auto snapshot_result = engine.CaptureSnapshot();
if (!snapshot_result) {
  // Handle error
}

EngineSnapshot snapshot = snapshot_result.value();

// Convert to SaveData
SaveMetaData meta{"My Save", "save_001", GetTimestamp()};
auto save_data = ConvertSnapshotToSaveData(snapshot, meta);

// Serialize and write
SerializeToFile(save_data, "save_001.bin");
```

### Example 3: Partial Snapshot for Testing

```cpp
// Only capture scene data and event bus (not full state)
auto snapshot = engine.CapturePartialSnapshot(
  SnapshotFields::SceneData | SnapshotFields::EventBus);

// Use for test validation
REQUIRE(snapshot.scene_collection_data.size() == expected_count);
REQUIRE(snapshot.global_event_bus.has_value());
REQUIRE_FALSE(snapshot.engine_config.has_value());  // Not captured
```

### Example 4: Hot-Restore During Runtime

```cpp
// Load checkpoint snapshot
EngineSnapshot checkpoint = LoadCheckpoint();

// Apply to running engine (merge mode - only update specified fields)
auto result = engine.ApplySnapshotUpdate(checkpoint, SnapshotMode::Merge);
if (!result) {
  // Handle restore error
}
```

## Migration Path

### Existing Code (No Changes Needed)

```cpp
// This continues to work
Engine engine;
auto result = engine.RunGame();  // Uses StartUp() internally
```

### New Code (Optional Adoption)

```cpp
// New snapshot-based approach
EngineSnapshot snapshot = CreateOrLoadSnapshot();
Engine engine(snapshot);
engine.RunGameLoop();
```

### Internal Changes (Transparent to Users)

```cpp
// Engine::StartUp() internally converts to snapshot
std::expected<std::monostate, FailInfo> Engine::StartUp() {
  auto engine_data = LoadEngineData();  // Existing provider
  EngineSnapshot snapshot = Convert(engine_data);  // New adapter
  return ConsumeSnapshot(snapshot);  // New interface
}
```

## Testing Strategy Summary

### Unit Tests
- Interface contract validation
- Snapshot field validation
- Conversion utilities

### Integration Tests
- Engine initialization from snapshot
- Save/load round trips
- Partial vs complete snapshots

### System Tests
- TestEngine with snapshots
- Full game save/restore
- Performance benchmarks

## Benefits Summary

| Benefit | Description |
|---------|-------------|
| **Unified Model** | Single container for all state (no EngineData vs EngineSnapshot confusion) |
| **Extensibility** | Add fields without breaking changes (optional fields) |
| **Testing** | Easy to create test scenarios with partial snapshots |
| **Save/Load** | Direct path from engine state to save file |
| **Debugging** | Capture snapshots for issue reproduction |
| **Network Play** | Foundation for state synchronization (future) |

## Potential Drawbacks

| Concern | Mitigation |
|---------|-----------|
| **Complexity** | Clear interfaces and documentation |
| **Performance** | Partial snapshots, move semantics, profiling |
| **Migration** | Backward compatible, optional adoption |
| **Two Paths** | Common helpers, internal conversion |

## When to Use Each Interface

### Use IEngineSnapshotProvider (Export) when:
- ✅ Saving game state
- ✅ Capturing test data
- ✅ Creating checkpoints
- ✅ Network state sync
- ✅ Debugging/profiling

### Use IEngineSnapshotConsumer (Import) when:
- ✅ Loading saved games
- ✅ Initializing from test data
- ✅ Restoring checkpoints
- ✅ Network state sync
- ✅ Hot-reloading content

## Summary

The Provider/Consumer pattern provides a **clean, extensible, and architecturally consistent** approach to making `EngineSnapshot` the central container for engine state.

**Bottom Line:**
- Two simple interfaces
- One unified data model
- Backward compatible
- Future-proof design
- Clear separation of concerns

For complete details, see: `ENGINE_SNAPSHOT_INTEGRATION.md`
