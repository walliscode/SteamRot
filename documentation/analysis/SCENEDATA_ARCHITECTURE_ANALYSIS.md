# SceneData Architecture Analysis

## Overview

This document analyzes the current `SceneData` architecture and addresses concerns about using derived instances to pass heavy data structures, particularly for data import/export operations.

## Current Architecture

### SceneData Base Structure

```cpp
// src/types/core/SceneData.h
struct SceneData {
  virtual ~SceneData() = default;
  
  SceneInfo scene_info;
  SceneResourcesConfig scene_resources_config;
  AssetConfig scene_asset_config;
};

using SceneCollectionData = std::vector<std::unique_ptr<SceneData>>;
```

**Purpose**: Abstract base containing configuration data needed to set up a Scene.

**Contents**:
- `scene_info` - Scene identification (UUID, type)
- `scene_resources_config` - Resource configuration (texture dimensions, entity pool size)
- `scene_asset_config` - Asset loading configuration (fonts, textures, audio)

### FbsSceneData Derived Structure

```cpp
// src/types/core/FbsSceneData.h
struct FbsSceneData : public SceneData {
  const EntityCollectionFbs *entity_collection = nullptr;
};
```

**Purpose**: Extends `SceneData` with FlatBuffers-specific data for scene loading.

**Contents**:
- Inherits all base `SceneData` members
- **Adds**: `entity_collection` - Pointer to FlatBuffers entity data

### Data Flow: Import (Loading)

The current architecture handles data import (loading scenes) as follows:

1. **Data Loading**: `FlatbuffersDataLoader` reads binary FlatBuffers files
2. **Data Provision**: `FlatbuffersSceneDataProvider::ProvideDefaultSceneData()`
   - Creates `FbsSceneData` instance
   - Populates base `SceneData` fields
   - **Attaches pointer** to FlatBuffers `EntityCollectionFbs`
   - Returns as `std::unique_ptr<SceneData>`

3. **Scene Configuration**: `FlatbuffersSceneConfigurator::ConfigureEntities()`
   - Receives `const SceneData*` parameter
   - **Dynamic cast** to `FbsSceneData*` to access `entity_collection`
   - Creates `FlatbuffersEntityConfigurator` with entity collection
   - Configures runtime `EntityMemoryPool`

```cpp
// From FlatbuffersSceneConfigurator.cpp
FbsSceneData *fbs_scene_data =
    dynamic_cast<FbsSceneData *>(const_cast<SceneData *>(scene_data));

if (!fbs_scene_data)
  return std::unexpected(
      FailInfo(FailMode::InvalidCast, "SceneData is not FbsSceneData"));

FlatbuffersEntityConfigurator entity_configurator(
    scene.GetSceneContext().event_handler,
    *fbs_scene_data->entity_collection);
```

### Key Observations

1. **Polymorphism Pattern**: Uses inheritance and runtime polymorphism
2. **Type-Specific Data**: Derived classes add data source-specific pointers
3. **Dynamic Casting**: Configurators must downcast to access derived data
4. **Pointer Semantics**: Heavy data (FlatBuffers) passed by pointer, not copied

## The Problem: Export (Saving) Direction

### Current Gap

The architecture handles **import** (FlatBuffers → Runtime) but has no clear path for **export** (Runtime → Serializable Data).

### Export Requirements

For saving game state, we need:

1. **Runtime Data Access**: Access to current `EntityMemoryPool` state
2. **Data Transformation**: Convert runtime entities to serializable format
3. **Save File Generation**: Write to FlatBuffers binary or other format

### Architectural Asymmetry

**Import Path**:
```
FlatBuffers File → FbsSceneData (pointer to EntityCollectionFbs)
                → FlatbuffersEntityConfigurator
                → EntityMemoryPool (runtime)
```

**Export Path** (missing clear structure):
```
EntityMemoryPool (runtime) → ??? (NativeSceneData?)
                           → EntityExporter?
                           → FlatBuffers File
```

### The Specific Concern

**Question**: "Do we have a NativeSceneData struct that contains a ref/pointer to the EntityMemoryPool then for exporting?"

**Current Answer**: No. There is no `NativeSceneData` or equivalent structure.

**Implications**:
- Export functionality would need a different pattern than import
- Asymmetric architecture (import uses derived SceneData, export would need different approach)
- No clear place to "attach" runtime data references for export operations

## Analysis of Current Approach

### Advantages of Derived SceneData Pattern

1. **Type Safety**: Compile-time knowledge of data source
2. **No Data Copying**: Pointers avoid expensive copies of large structures
3. **Polymorphic Interface**: Common base for different data sources
4. **Extensibility**: Easy to add new derived types (e.g., `SaveFileSceneData`)

### Disadvantages of Derived SceneData Pattern

1. **Runtime Casting Required**: Configurators must `dynamic_cast` to access specific data
2. **Tight Coupling**: Configurators need knowledge of derived types
3. **Asymmetry**: Works well for import, unclear for export
4. **Const-Casting**: `const SceneData*` requires const-cast for dynamic_cast (see code above)

### Heavy Struct Concerns

**Definition**: "Heavy structs" = Large data structures expensive to copy
- `EntityCollectionFbs` - FlatBuffers table with all entity data
- `EntityMemoryPool` - Runtime entity storage with component vectors

**Current Handling**:
- Import: Passes `const EntityCollectionFbs*` (pointer to FlatBuffers data)
- Runtime: Passes `EntityMemoryPool&` (reference to runtime data)

**Concern**: Using inheritance to smuggle pointers feels like a workaround rather than a clear architectural pattern.

## Alternative Architectures Considered

### Option 1: Separate Import/Export Types

**Import**: Keep current `FbsSceneData` approach
**Export**: Create new `NativeSceneData` or `RuntimeSceneData`

```cpp
struct NativeSceneData : public SceneData {
  EntityMemoryPool *entity_memory_pool = nullptr;
};
```

**Pros**:
- Symmetric architecture for import/export
- Clear ownership and semantics
- Familiar pattern (mirrors FbsSceneData)

**Cons**:
- Still uses inheritance for data smuggling
- Dynamic casting still required
- Doesn't solve fundamental architectural concern

### Option 2: Context Objects Instead of Derived Data

Replace derived `SceneData` with context objects passed separately.

**Import**:
```cpp
struct ImportContext {
  const EntityCollectionFbs *entity_collection;
  EventHandler &event_handler;
};

// Configuration signature
ConfigureEntities(Scene &scene, const SceneData *scene_data, 
                 const ImportContext &context);
```

**Export**:
```cpp
struct ExportContext {
  const EntityMemoryPool &entity_memory_pool;
};

// Exporter signature
ExportEntities(const SceneData *scene_data, const ExportContext &context);
```

**Pros**:
- No inheritance for data passing
- No dynamic casting
- Symmetric import/export patterns
- Clearer separation of config data vs. runtime data

**Cons**:
- More parameters to pass around
- Breaks current interface pattern
- Requires refactoring existing code

### Option 3: Separate Provider and Exporter Hierarchies

Keep `SceneData` lightweight, use separate hierarchies for import/export.

**SceneData**: Contains only configuration, no pointers to heavy data

**Import**: 
```cpp
class IEntityImporter {
  virtual std::expected<std::monostate, FailInfo>
  ImportEntities(EntityMemoryPool &emp, const SceneData &scene_data) = 0;
};

class FlatbuffersEntityImporter : public IEntityImporter {
  FlatbuffersEntityImporter(const EntityCollectionFbs *entity_collection);
  // ...
};
```

**Export**:
```cpp
class IEntityExporter {
  virtual std::expected<std::unique_ptr<uint8_t[]>, FailInfo>
  ExportEntities(const EntityMemoryPool &emp, const SceneData &scene_data) = 0;
};

class FlatbuffersEntityExporter : public IEntityExporter {
  // Converts EntityMemoryPool to FlatBuffers binary
};
```

**Pros**:
- Clean separation: data (SceneData) vs. operations (Importer/Exporter)
- Symmetric import/export operations
- No inheritance for data passing
- Follows Single Responsibility Principle
- Easy to add new formats (JSON, XML, etc.)

**Cons**:
- More classes and interfaces
- Requires refactoring existing architecture
- Configuration interfaces need update

### Option 4: Data Transfer Objects (DTOs)

Use separate DTOs for import and export, keep SceneData minimal.

```cpp
// SceneData stays lightweight - just configuration
struct SceneData {
  SceneInfo scene_info;
  SceneResourcesConfig scene_resources_config;
  AssetConfig scene_asset_config;
};

// Import DTO
struct SceneImportData {
  SceneData scene_data;
  const EntityCollectionFbs *entity_collection;
};

// Export DTO  
struct SceneExportData {
  SceneData scene_data;
  const EntityMemoryPool &entity_memory_pool;
};
```

**Pros**:
- Clear data structures for each operation
- No inheritance required
- Symmetric approach
- Explicit data requirements

**Cons**:
- More types to manage
- Requires changing configurator interfaces
- Wrapper overhead (minimal)

## Recommendations

### Short-Term: Document Current Pattern

**Status Quo**: Keep current architecture, document it clearly.

**Actions**:
1. Document that `FbsSceneData` is import-specific
2. For export, create separate `IEntityExporter` interface
3. Export uses different pattern (not derived SceneData)
4. Accept architectural asymmetry as pragmatic choice

**When to Use**: Project is in active development, major refactoring not feasible.

### Medium-Term: Option 3 (Separate Hierarchies)

**Recommended Approach**: Implement separate Importer/Exporter hierarchies.

**Migration Path**:
1. Create `IEntityImporter` interface
2. Implement `FlatbuffersEntityImporter` (wraps current logic)
3. Update configurators to use importer
4. Create `IEntityExporter` interface with `FlatbuffersEntityExporter`
5. Implement save/load functionality with exporters
6. Eventually deprecate `FbsSceneData` inheritance pattern

**Benefits**:
- Clear architectural separation
- Supports both import and export
- Extensible for future formats
- Gradual migration path

### Long-Term: Option 2 (Context Objects)

**Future Architecture**: If major refactoring is planned.

**Considerations**:
- Requires comprehensive refactoring
- Changes all configurator interfaces
- Most "clean" architecture but highest cost
- Consider if rewriting large portions anyway

## Implementation Guidelines

### If Keeping Current Pattern

If the team decides to keep the derived `SceneData` pattern:

**For Export**, create:

```cpp
// src/types/core/NativeSceneData.h
struct NativeSceneData : public SceneData {
  /////////////////////////////////////////////////
  /// @brief Pointer to runtime EntityMemoryPool for export operations
  /////////////////////////////////////////////////
  const EntityMemoryPool *entity_memory_pool = nullptr;
};
```

**Usage**:
```cpp
// In save/export code
NativeSceneData native_scene_data;
native_scene_data.scene_info = scene.GetSceneInfo();
native_scene_data.scene_resources_config = /* ... */;
native_scene_data.entity_memory_pool = &scene.GetEntityMemoryPool();

// Pass to exporter
auto result = entity_exporter.ExportEntities(&native_scene_data);
```

**Document Clearly**:
- `FbsSceneData` is for **import only** (loading from FlatBuffers)
- `NativeSceneData` is for **export only** (saving to any format)
- Base `SceneData` is for **configuration** (when heavy data not needed)

### If Migrating to Importer/Exporter Pattern

See Option 3 implementation plan above. Key interfaces:

```cpp
// Import interface
class IEntityImporter {
public:
  virtual ~IEntityImporter() = default;
  
  virtual std::expected<std::monostate, FailInfo>
  ImportEntities(EntityMemoryPool &emp) = 0;
};

// Export interface
class IEntityExporter {
public:
  virtual ~IEntityExporter() = default;
  
  virtual std::expected<std::unique_ptr<uint8_t[]>, FailInfo>
  ExportEntities(const EntityMemoryPool &emp, size_t &out_size) = 0;
};
```

## Conclusion

**The Current Problem**: The architecture uses derived `SceneData` types to pass pointers to heavy data structures during import, but has no clear equivalent for export operations.

**Root Cause**: Mixing data definition (SceneData) with data source identification (FbsSceneData) creates confusion about the purpose and usage of these types.

**Recommended Solution**: 
1. **Short-term**: Create `NativeSceneData` for symmetry, document the pattern clearly
2. **Medium-term**: Migrate to separate `IEntityImporter`/`IEntityExporter` hierarchies
3. **Long-term**: Consider context objects if major refactoring happens

**Key Insight**: The concern is valid. Using inheritance to pass pointers is a code smell indicating that data structures (SceneData) are being conflated with operational concerns (how to load/save that data). Separating these concerns through dedicated Import/Export interfaces is the cleanest long-term solution.

## Related Files

- `src/types/core/SceneData.h` - Base SceneData definition
- `src/types/core/FbsSceneData.h` - FlatBuffers-specific derived type
- `src/scenes/FlatbuffersSceneConfigurator.cpp` - Uses FbsSceneData for configuration
- `src/entity/FlatbuffersEntityConfigurator.h` - Configures entities from FlatBuffers
- `src/interfaces/ISceneConfigurator.h` - Scene configuration interface
- `src/data_providers/FlatbuffersSceneDataProvider.h` - Creates FbsSceneData instances

## Next Steps

1. **Discuss with team**: Which approach to take (status quo, add NativeSceneData, or refactor)
2. **Prioritize**: Is export functionality needed now or can it wait?
3. **Document decision**: Record architectural decision and rationale
4. **Plan migration**: If refactoring, create detailed migration plan
5. **Update documentation**: Keep architecture docs in sync with decisions

---

**Document Version**: 1.0
**Date**: 2026-01-06
**Author**: Architecture Analysis
**Status**: Draft for Review
