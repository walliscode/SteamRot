# Native Bridging Structures Architecture

## Overview

This document describes the architecture for using **native C++ structs** (SaveData, EngineSnapshot, TestData) as bidirectional bridging structures between the game engine and external systems (serialization, testing, save files).

## Problem Statement

Currently, the engine uses format-specific interfaces (FlatBuffers) that create tight coupling:
- **Import**: FlatBuffers → IEntityImporter → EntityMemoryPool (one-way)
- **Export**: EntityMemoryPool → IEntityExporter → Binary (one-way, no implementation)
- **Testing**: Hard to snapshot and restore engine state
- **Serialization**: Cannot easily convert runtime state to serializable structures

**Goal**: Make SaveData/EngineSnapshot/TestData work as format-agnostic bridging structures that can:
1. Import from external formats (JSON, FlatBuffers, etc.) → populate native structs
2. Configure engine from native structs → populate EntityMemoryPool
3. Export engine state to native structs → ready for serialization

## Current Architecture

### Entity Import Flow
```
External Data (FlatBuffers)
    ↓
IEntityImporter (FlatbuffersEntityImporter)
    ↓
IEntityConfigurator (FlatbuffersEntityConfigurator)
    ↓
EntityMemoryPool (game state)
```

**Limitation**: One-way flow, cannot access EMP after configuration

### Entity Export Flow
```
EntityMemoryPool (game state)
    ↓
IEntityExporter (interface only, no implementations)
    ↓
Binary Data (uint8_t[])
```

**Limitation**: No concrete implementations, exports to binary not to structs

### Snapshot Structures

#### EngineSnapshot
```cpp
struct EngineSnapshot {
  std::optional<size_t> tick_number;
  std::optional<EventBus> global_event_bus;
  std::optional<SceneManagerData> scene_manager_data;
  SceneCollectionData scene_collection_data;
};
```

#### SaveData
```cpp
struct SaveData {
  SaveMetaData meta_data;
  EngineSnapshot engine_snapshot;
};
```

#### TestData
```cpp
struct TestData {
  TestMetaData meta_data;
  SimulationData simulation_data;
  uint32_t number_of_ticks;
  EngineSnapshot starting_engine_snapshot;
  std::map<size_t, EngineSnapshot> expected_engine_snapshots;
};
```

## Solution: Three-Tier Architecture

### Tier 1: External Formats (JSON, FlatBuffers, Binary)
Format-specific data representation

### Tier 2: Native Bridging Structures (SaveData, EngineSnapshot, TestData)
Format-agnostic C++ structs that hold complete state

### Tier 3: Runtime Engine (EntityMemoryPool, Scene, EventHandler, etc.)
Active game state

## Proposed Solution: Dual-Mode Interface Pattern

### Approach: Separate Import and Export Interfaces

**Keep existing semantics:**
- **IEntityImporter**: External → EMP (import)
- **IEntityExporter**: EMP → External (export)

**Add new implementations:**
- **NativeEntityImporter**: Native struct → EMP
- **NativeEntityExporter**: EMP → Native struct

### Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    External Formats                          │
│              (FlatBuffers, JSON, Binary)                     │
└─────────────────┬───────────────────────────┬────────────────┘
                  │                           │
        ┌─────────▼─────────┐       ┌────────▼─────────┐
        │ Format-Specific   │       │ Format-Specific  │
        │ Importer          │       │ Exporter         │
        │ (FlatBuffers)     │       │ (FlatBuffers)    │
        └─────────┬─────────┘       └────────▲─────────┘
                  │                           │
┌─────────────────▼───────────────────────────┴────────────────┐
│              Native Bridging Structures                       │
│         (SaveData, EngineSnapshot, TestData)                 │
│           - EntityMemoryPool data                            │
│           - Event bus data                                   │
│           - Scene manager data                               │
└─────────────────┬───────────────────────────▲────────────────┘
                  │                           │
        ┌─────────▼─────────┐       ┌────────┴─────────┐
        │ NativeEntity      │       │ NativeEntity     │
        │ Importer          │       │ Exporter         │
        └─────────┬─────────┘       └────────▲─────────┘
                  │                           │
┌─────────────────▼───────────────────────────┴────────────────┐
│                  Runtime Engine State                         │
│                  (EntityMemoryPool)                          │
└───────────────────────────────────────────────────────────────┘
```

## Implementation Details

### 1. NativeEntityImporter

Reads from a native C++ structure (EntityMemoryPool or equivalent) and configures the target EMP.

```cpp
class NativeEntityImporter : public IEntityImporter {
private:
  EventHandler &m_event_handler;
  const EntityMemoryPool &m_source_emp;  // Source data

public:
  NativeEntityImporter(EventHandler &event_handler,
                      const EntityMemoryPool &source_emp);

  std::expected<std::monostate, FailInfo>
  ImportEntities(EntityMemoryPool &target_emp) override;
};
```

**Usage:**
```cpp
// For testing: create entities in a native structure
EntityMemoryPool source_emp = CreateTestEntities();

// Import into engine
NativeEntityImporter importer(event_handler, source_emp);
importer.ImportEntities(scene.GetEntityMemoryPool());
```

### 2. NativeEntityExporter

Exports runtime EMP to a native C++ structure.

```cpp
class NativeEntityExporter : public IEntityExporter {
public:
  // Export to binary (existing interface)
  std::expected<std::unique_ptr<uint8_t[]>, FailInfo>
  ExportEntities(const EntityMemoryPool &emp, size_t &out_size) override;
  
  // Export to native structure (new method)
  std::expected<EntityMemoryPool, FailInfo>
  ExportToNativeStructure(const EntityMemoryPool &source_emp);
};
```

**Usage:**
```cpp
// Export runtime state
NativeEntityExporter exporter;
auto emp_snapshot = exporter.ExportToNativeStructure(scene.GetEntityMemoryPool());

// Store in EngineSnapshot
engine_snapshot.scene_collection_data[0].entity_data = emp_snapshot.value();
```

### 3. Enhanced SceneData

Add optional EntityMemoryPool storage for native workflows.

```cpp
struct SceneData {
  SceneInfo scene_info;
  SceneResourcesConfig scene_resources_config;
  AssetConfig scene_asset_config;
  
  // Format-agnostic entity source (for import)
  std::unique_ptr<IEntityImporter> entity_importer{nullptr};
  
  // Optional: Direct entity data (for native workflows)
  std::optional<EntityMemoryPool> entity_data;
};
```

### 4. Snapshot Utilities

Helper functions for capturing and restoring state.

```cpp
namespace steamrot::snapshot {

// Capture current engine state
EngineSnapshot CaptureEngineState(
    const Engine& engine,
    size_t tick_number);

// Restore engine state from snapshot
std::expected<std::monostate, FailInfo>
RestoreEngineState(
    Engine& engine,
    const EngineSnapshot& snapshot);

// Create importer from snapshot
std::unique_ptr<IEntityImporter>
CreateImporterFromSnapshot(
    EventHandler& event_handler,
    const SceneData& scene_data);

} // namespace steamrot::snapshot
```

## Use Cases

### Use Case 1: Save Game (Engine → File)

```cpp
// 1. Export runtime state to native structure
NativeEntityExporter exporter;
SaveData save_data;
save_data.meta_data = CreateMetaData();

// 2. Capture engine snapshot
save_data.engine_snapshot = snapshot::CaptureEngineState(engine, tick);

// 3. Serialize native structure to format
FlatbuffersSaveDataExporter fb_exporter;
auto binary = fb_exporter.ExportToFlatBuffers(save_data);

// 4. Write to file
WriteToFile("save.dat", binary);
```

### Use Case 2: Load Game (File → Engine)

```cpp
// 1. Read from file
auto binary = ReadFromFile("save.dat");

// 2. Deserialize to native structure
FlatbuffersSaveDataProvider fb_provider(binary);
auto save_data = fb_provider.ProvideSaveData();

// 3. Restore engine state
snapshot::RestoreEngineState(engine, save_data.value().engine_snapshot);
```

### Use Case 3: Testing (Setup → Run → Validate)

```cpp
// 1. Create test data in native structures
TestData test_data;
test_data.starting_engine_snapshot.scene_collection_data[0].entity_data = 
    CreateTestEntities();

// 2. Configure engine from native structures
NativeEntityImporter importer(
    event_handler, 
    *test_data.starting_engine_snapshot.scene_collection_data[0].entity_data);
importer.ImportEntities(scene.GetEntityMemoryPool());

// 3. Run simulation
test_engine.Run();

// 4. Export results to native structure
auto result_snapshot = snapshot::CaptureEngineState(test_engine, tick);

// 5. Compare native structures
REQUIRE(result_snapshot == test_data.expected_engine_snapshots[tick]);
```

### Use Case 4: Snapshot Diff (Debugging)

```cpp
// Capture state at tick 1
auto snapshot1 = snapshot::CaptureEngineState(engine, 1);

// Run some ticks
engine.Tick();
engine.Tick();

// Capture state at tick 3
auto snapshot3 = snapshot::CaptureEngineState(engine, 3);

// Compare differences
auto diff = snapshot::DiffSnapshots(snapshot1, snapshot3);
LOG("Entities changed: {}", diff.entities_changed);
LOG("Components modified: {}", diff.components_modified);
```

## Benefits

### 1. Format Independence
- Native structs don't care about FlatBuffers, JSON, or binary format
- Easy to add new serialization formats
- Testing doesn't depend on file formats

### 2. Testability
- Direct C++ struct manipulation (no FlatBuffers schemas)
- Easy to create test data programmatically
- Can validate intermediate states

### 3. Debugging
- Can snapshot state at any tick
- Compare snapshots to find differences
- Export to human-readable formats

### 4. Separation of Concerns
- Import/Export logic separated
- Native structures are clean data containers
- Format-specific code isolated

### 5. Flexibility
- Can work with both formatted data (FlatBuffers) and native structures
- Choose the right tool for the job (testing vs. production)
- Gradual migration path

## Migration Path

### Phase 1: Core Implementation (Current)
- ✅ Document architecture
- ✅ Design interfaces
- ⏳ Implement NativeEntityImporter
- ⏳ Implement NativeEntityExporter
- ⏳ Add EntityMemoryPool to SceneData

### Phase 2: Snapshot Utilities
- Create snapshot capture utilities
- Create snapshot restore utilities
- Create snapshot diff utilities

### Phase 3: Integration
- Update TestEngine to use native structures
- Update SaveData workflows
- Add snapshot-based testing infrastructure

### Phase 4: Optional Format Exporters
- Implement FlatBuffers → Native converters
- Implement Native → FlatBuffers converters
- Implement JSON → Native converters
- Implement Native → JSON converters

## Design Principles

### 1. Single Responsibility
- **IEntityImporter**: Only imports (External → EMP)
- **IEntityExporter**: Only exports (EMP → External)
- **Native structures**: Only hold data (no logic)

### 2. Dependency Inversion
- Engine depends on interfaces (IEntityImporter/Exporter)
- Implementations depend on interfaces
- Native structs depend on nothing

### 3. Open/Closed Principle
- Easy to add new importers (JSON, Binary, etc.)
- Easy to add new exporters
- No changes to existing interfaces

### 4. Least Surprise
- Import means: External → Internal
- Export means: Internal → External
- Native structs are just data

## Alternative Approaches Considered

### Alternative 1: Bidirectional IEntityBridge
Create a new interface that handles both import and export:

```cpp
class IEntityBridge {
  virtual std::expected<std::monostate, FailInfo>
  ImportEntities(EntityMemoryPool &emp) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ExportEntities(const EntityMemoryPool &emp) = 0;
  
  virtual EntityMemoryPool& GetEntityMemoryPool() = 0;
};
```

**Rejected because:**
- Violates Single Responsibility Principle
- Forces all implementations to support both directions
- Confusing semantics (what does "get" mean?)

### Alternative 2: Modify IEntityImporter to Hold EMP
Make IEntityImporter hold a reference to both source and target:

```cpp
class IEntityImporter {
  virtual std::expected<std::monostate, FailInfo>
  ImportEntities(EntityMemoryPool &emp) = 0;
  
  virtual EntityMemoryPool& GetSourceData() = 0; // NEW
};
```

**Rejected because:**
- Breaks existing abstraction (importers should be stateless)
- Not all importers have EMP as source (FlatBuffers doesn't)
- Confusing to have getters on an "Importer"

### Alternative 3: Direct EMP in Snapshot
Store EntityMemoryPool directly in EngineSnapshot:

```cpp
struct EngineSnapshot {
  std::optional<EntityMemoryPool> entity_memory_pool;
};
```

**Rejected because:**
- Loses scene organization (EMP is per-scene)
- No way to handle multiple scenes
- Doesn't solve import/export problem

## Conclusion

The **Dual-Mode Interface Pattern** (Solution 1) is the recommended approach because:
- ✅ Clean separation of concerns
- ✅ Maintains existing interface semantics
- ✅ Easy to understand and use
- ✅ Flexible for future additions
- ✅ Supports both formatted and native workflows

The architecture enables SaveData, EngineSnapshot, and TestData to truly function as **native bridging structures** that can be populated from any format, used to configure the engine, and exported back to any format.
