# Scene Factory and Configurator Architecture Analysis

## Executive Summary

This document analyzes the Scene configuration workflow from data files to fully configured Scene objects. It proposes a strategy-pattern-based solution that decouples FlatBuffers (and other data sources) from game code while avoiding intermediate native structs.

**Date**: December 9, 2025  
**Status**: Planning Phase

---

## Table of Contents

1. [Problem Statement](#problem-statement)
2. [Current Architecture](#current-architecture)
3. [Requirements Analysis](#requirements-analysis)
4. [Proposed Solution](#proposed-solution)
5. [Key Components](#key-components)
6. [Data Flow Diagrams](#data-flow-diagrams)
7. [Implementation Strategy](#implementation-strategy)
8. [Migration Path](#migration-path)

---

## Problem Statement

### Requirements

1. **Decouple Data Sources**: Avoid coupling FlatBuffers structs (or other data sources) directly into game code
2. **No Intermediate Structs**: Go straight from `SceneDataFbs` to `Scene` without native intermediate structs like `SceneData`
3. **Multiple Data Sources**: Support both default scene data and save data (SaveDataFbs contains SceneDataFbs)
4. **Strategy Pattern**: Use strategy pattern in SceneFactory (not abstract factory)
5. **Unified Configuration**: Marry multiple data types with multiple data sources

### Current Issues

```cpp
// Current: Direct FlatBuffers coupling
class FlatbuffersSceneFactory : public ISceneFactory {
private:
  const SceneDataFbs *m_scene_data_fbs;  // ❌ Direct coupling to FlatBuffers
  
public:
  FlatbuffersSceneFactory(const GameContext &game_context,
                          const SceneDataFbs *scene_data_fbs);
};

// Current: Entity configurator also coupled
class FlatbuffersEntityConfigurator : public IEntityConfigurator {
private:
  const EntityCollectionFbs &m_entity_collection_data;  // ❌ Direct coupling
};
```

### Why No Intermediate Native Struct?

The `ISceneDataProvider` currently returns a native `SceneData` struct:

```cpp
struct SceneData {
  SceneType scene_type{SceneType::SceneType_UNKNOWN};
  std::string scene_id;
  uint32_t render_texture_width{800};
  uint32_t render_texture_height{600};
};
```

**Problem**: This struct only covers basic metadata. Full scene configuration includes:
- Entity collection (complex nested data)
- Logic collection (system configuration)
- Asset collection (resource references)
- Scene resources (render texture, managers)

Creating a native struct for ALL this data would:
1. Duplicate the complex configuration logic
2. Create maintenance burden (two parallel structures)
3. Provide no real benefit since we're just converting between formats

**Solution**: Keep the FlatBuffers data as the source of truth and configure Scene directly from it.

---

## Current Architecture

### Current Scene Loading Flow

```
SceneManager::AddSceneFromDefault(SceneType)
    ↓
ISceneFactory::CreateDefaultScene(SceneType, GameContext)
    ↓ (creates)
FlatbuffersSceneFactory(GameContext, SceneDataFbs*)
    ↓ (has direct reference)
SceneDataFbs* ❌ Direct coupling
    ↓ (used by)
FlatbuffersSceneFactory::ConfigureSceneResources(Scene&)
FlatbuffersEntityConfigurator::ConfigureEntityMemoryPool(EMP&)
```

### Current Issues

1. **SceneFactory is FlatBuffers-specific**: `FlatbuffersSceneFactory` is hardcoded to use `SceneDataFbs*`
2. **No abstraction for data source**: Cannot switch between default data and save data
3. **No configurator pattern**: Configuration logic is in factory, not separate configurators
4. **Direct FlatBuffers coupling**: Game code sees FlatBuffers types

---

## Requirements Analysis

### Functional Requirements

| ID | Requirement | Priority |
|----|-------------|----------|
| FR1 | Load Scene from default data files | Critical |
| FR2 | Load Scene from save data files | Critical |
| FR3 | Configure Scene directly from FlatBuffers (no intermediate struct) | Critical |
| FR4 | Support multiple data sources without code duplication | High |
| FR5 | Decouple FlatBuffers from game code | High |
| FR6 | Use strategy pattern for data source selection | High |

### Non-Functional Requirements

| ID | Requirement | Priority |
|----|-------------|----------|
| NFR1 | Minimize code duplication | High |
| NFR2 | Maintain type safety | Critical |
| NFR3 | Clear separation of concerns | High |
| NFR4 | Easy to extend with new data sources (JSON, Lua, etc.) | Medium |

---

## Proposed Solution

### Architecture Overview

Use **Strategy Pattern** with configurator interfaces that encapsulate data source access:

```
┌─────────────────────────────────────────────────────────────┐
│                      SceneManager                           │
│  - Orchestrates Scene loading                              │
│  - Chooses data source (Default vs Save)                   │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ↓ Creates configurator based on source
┌────────────────────────────────────────────────────────────┐
│              ISceneConfigurator (Strategy)                 │
│  - GetSceneData() → const SceneDataFbs*                   │
│  - CreateEntityConfigurator() → IEntityConfigurator       │
└────────────────────────┬───────────────────────────────────┘
                         │
         ┌───────────────┴───────────────┐
         │                               │
         ↓                               ↓
┌──────────────────────┐      ┌──────────────────────┐
│DefaultSceneConfigurator│    │SavedSceneConfigurator│
│  - Uses ISceneData-  │      │  - Uses ISaveData-   │
│    Provider          │      │    Provider          │
│  - Loads default     │      │  - Loads save data   │
│    scene data        │      │  - Extracts scene    │
└──────────────────────┘      └──────────────────────┘
         │                               │
         ↓ Provides to                  ↓
┌─────────────────────────────────────────────────────────┐
│                   SceneFactory                          │
│  - Receives ISceneConfigurator in constructor          │
│  - Calls GetSceneData() to get SceneDataFbs*          │
│  - Configures Scene from FlatBuffers data             │
└─────────────────────────────────────────────────────────┘
```

### Key Design Decisions

1. **ISceneConfigurator Interface**: Strategy interface that abstracts data source
2. **No Data Provider in Factory**: Factory receives configurator, not provider
3. **Configurator Owns Data Access**: Configurator encapsulates provider access
4. **Direct FlatBuffers Usage**: Factory works with `SceneDataFbs*`, not native structs
5. **Entity Configurator Creation**: Scene configurator creates appropriate entity configurator

---

## Key Components

### 1. ISceneConfigurator Interface

```cpp
namespace steamrot {

/////////////////////////////////////////////////
/// @class ISceneConfigurator
/// @brief Strategy interface for Scene configuration from different data sources.
///
/// Implementations handle specific data sources (default data, save data, etc.)
/// and provide unified access to FlatBuffers data structures.
/////////////////////////////////////////////////
class ISceneConfigurator {
public:
  virtual ~ISceneConfigurator() = default;

  /////////////////////////////////////////////////
  /// @brief Get the FlatBuffers scene data for configuration.
  ///
  /// @return Pointer to const SceneDataFbs, or nullptr on error
  /////////////////////////////////////////////////
  virtual const SceneDataFbs* GetSceneData() const = 0;

  /////////////////////////////////////////////////
  /// @brief Create an appropriate entity configurator for this data source.
  ///
  /// @param event_handler Reference to the EventHandler
  /// @return Unique pointer to entity configurator
  /////////////////////////////////////////////////
  virtual std::unique_ptr<IEntityConfigurator> 
  CreateEntityConfigurator(EventHandler &event_handler) const = 0;

  /////////////////////////////////////////////////
  /// @brief Get the scene type being configured.
  ///
  /// @return SceneType enum value
  /////////////////////////////////////////////////
  virtual SceneType GetSceneType() const = 0;
};

} // namespace steamrot
```

### 2. DefaultSceneConfigurator

```cpp
namespace steamrot {

/////////////////////////////////////////////////
/// @class DefaultSceneConfigurator
/// @brief Configurator for loading Scenes from default data files.
///
/// Uses ISceneDataProvider internally to access default scene data.
/////////////////////////////////////////////////
class DefaultSceneConfigurator : public ISceneConfigurator {
private:
  SceneType m_scene_type;
  ISceneDataProvider &m_scene_data_provider;
  
  // Cache FlatBuffers data after first load
  mutable const SceneDataFbs *m_cached_scene_data{nullptr};

public:
  DefaultSceneConfigurator(SceneType scene_type,
                          ISceneDataProvider &scene_data_provider);

  const SceneDataFbs* GetSceneData() const override;

  std::unique_ptr<IEntityConfigurator> 
  CreateEntityConfigurator(EventHandler &event_handler) const override;

  SceneType GetSceneType() const override;
};

} // namespace steamrot
```

### 3. SavedSceneConfigurator

```cpp
namespace steamrot {

/////////////////////////////////////////////////
/// @class SavedSceneConfigurator
/// @brief Configurator for loading Scenes from save data files.
///
/// Uses ISaveDataProvider internally to access saved game data.
/// Extracts SceneDataFbs from SaveDataFbs.
/////////////////////////////////////////////////
class SavedSceneConfigurator : public ISceneConfigurator {
private:
  uint32_t m_save_slot_index;
  ISaveDataProvider &m_save_data_provider;
  
  // Cache FlatBuffers data after first load
  mutable const SceneDataFbs *m_cached_scene_data{nullptr};

public:
  SavedSceneConfigurator(uint32_t save_slot_index,
                        ISaveDataProvider &save_data_provider);

  const SceneDataFbs* GetSceneData() const override;

  std::unique_ptr<IEntityConfigurator> 
  CreateEntityConfigurator(EventHandler &event_handler) const override;

  SceneType GetSceneType() const override;
};

} // namespace steamrot
```

### 4. Updated SceneFactory

```cpp
namespace steamrot {

class ISceneFactory {
protected:
  const GameContext &m_game_context;
  
  // Strategy: configurator provides data access
  const ISceneConfigurator &m_scene_configurator;
  
  std::unique_ptr<IEntityConfigurator> m_entity_configurator{nullptr};
  
  SceneType scene_type{SceneType::SceneType_UNKNOWN};

  std::expected<std::unique_ptr<Scene>, FailInfo> CreateSceneByType();

public:
  // Constructor now takes configurator instead of data
  ISceneFactory(const GameContext &game_context,
                const ISceneConfigurator &scene_configurator);

  virtual ~ISceneFactory() = default;

  std::expected<std::unique_ptr<Scene>, FailInfo> CreateScene();

  std::expected<std::monostate, FailInfo> ConfigureSceneInfo(Scene &scene);

  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene) = 0;

  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneConfig(Scene &scene) = 0;

  std::expected<std::monostate, FailInfo> ConfigureLogicMap(Scene &scene);
};

} // namespace steamrot
```

---

## Data Flow Diagrams

### Pattern 1: Loading Default Scene

```
┌──────────────┐
│ SceneManager │
└──────┬───────┘
       │
       │ 1. LoadSceneFromDefault(SceneType::TITLE)
       │
       ↓
┌──────────────────────┐
│ Get Scene Data       │
│ Provider (singleton) │
└──────┬───────────────┘
       │
       │ 2. Create DefaultSceneConfigurator
       │
       ↓
┌─────────────────────────────┐
│ DefaultSceneConfigurator    │
│  - scene_type: TITLE        │
│  - provider: ISceneData     │
│    Provider                 │
└──────┬──────────────────────┘
       │
       │ 3. Create SceneFactory with configurator
       │
       ↓
┌─────────────────────────────┐
│ SceneFactory                │
│  - configurator: &config    │
└──────┬──────────────────────┘
       │
       │ 4. CreateScene()
       │
       ├─→ configurator.GetSceneData()
       │       │
       │       ↓ Loads from provider
       │   ┌─────────────────────┐
       │   │ SceneDataFbs* (FB) │
       │   └─────────────────────┘
       │
       ├─→ configurator.CreateEntityConfigurator()
       │       │
       │       ↓ Creates
       │   ┌──────────────────────────────┐
       │   │ FlatbuffersEntity           │
       │   │ Configurator                │
       │   └──────────────────────────────┘
       │
       └─→ ConfigureSceneResources(scene)
               │
               ↓ Uses SceneDataFbs* directly
           ┌───────────────────┐
           │ Configured Scene  │
           └───────────────────┘
```

### Pattern 2: Loading Saved Scene

```
┌──────────────┐
│ SceneManager │
└──────┬───────┘
       │
       │ 1. LoadSceneFromSave(slot_index: 0)
       │
       ↓
┌──────────────────────┐
│ Get Save Data        │
│ Provider (singleton) │
└──────┬───────────────┘
       │
       │ 2. Create SavedSceneConfigurator
       │
       ↓
┌─────────────────────────────┐
│ SavedSceneConfigurator      │
│  - save_slot: 0             │
│  - provider: ISaveData      │
│    Provider                 │
└──────┬──────────────────────┘
       │
       │ 3. Create SceneFactory with configurator
       │
       ↓
┌─────────────────────────────┐
│ SceneFactory                │
│  - configurator: &config    │
└──────┬──────────────────────┘
       │
       │ 4. CreateScene()
       │
       ├─→ configurator.GetSceneData()
       │       │
       │       ↓ Loads SaveDataFbs from provider
       │   ┌─────────────────────┐
       │   │ SaveDataFbs* (FB)   │
       │   └─────────┬───────────┘
       │             │
       │             ↓ Extracts
       │   ┌─────────────────────┐
       │   │ SceneDataFbs* (FB) │
       │   └─────────────────────┘
       │
       ├─→ configurator.CreateEntityConfigurator()
       │       │
       │       ↓ Creates
       │   ┌──────────────────────────────┐
       │   │ FlatbuffersEntity           │
       │   │ Configurator                │
       │   └──────────────────────────────┘
       │
       └─→ ConfigureSceneResources(scene)
               │
               ↓ Uses SceneDataFbs* directly
           ┌───────────────────┐
           │ Configured Scene  │
           └───────────────────┘
```

---

## Implementation Strategy

### Phase 1: Create Configurator Interfaces

**Files to create:**
- `src/scenes/ISceneConfigurator.h`
- `src/scenes/DefaultSceneConfigurator.h`
- `src/scenes/DefaultSceneConfigurator.cpp`
- `src/scenes/SavedSceneConfigurator.h`
- `src/scenes/SavedSceneConfigurator.cpp`

**Key points:**
1. `ISceneConfigurator` is the strategy interface
2. Implementations wrap data providers internally
3. Caching of FlatBuffers data to avoid repeated loads

### Phase 2: Update SceneFactory

**Files to modify:**
- `src/scenes/ISceneFactory.h`
- `src/scenes/ISceneFactory.cpp`
- `src/scenes/FlatbuffersSceneFactory.h`
- `src/scenes/FlatbuffersSceneFactory.cpp`

**Changes:**
1. Constructor takes `const ISceneConfigurator&` instead of data pointer
2. Use `m_scene_configurator.GetSceneData()` to access FlatBuffers
3. Use `m_scene_configurator.CreateEntityConfigurator()` for entity config
4. Remove direct FlatBuffers member variable

### Phase 3: Update SceneManager

**Files to modify:**
- `src/scenes/SceneManager.h`
- `src/scenes/SceneManager.cpp`

**Changes:**
1. `LoadSceneFromDefault()` creates `DefaultSceneConfigurator`
2. `LoadSceneFromSave()` creates `SavedSceneConfigurator`
3. Passes configurator to SceneFactory
4. Manages configurator lifetime

### Phase 4: Update Entity Configurator

**Files to modify:**
- `src/entity/IEntityConfigurator.h`
- `src/entity/FlatbuffersEntityConfigurator.h`
- `src/entity/FlatbuffersEntityConfigurator.cpp`

**Note**: Entity configurator pattern already exists and works well. Main change is how it's created (via scene configurator).

---

## Migration Path

### Step 1: Create New Interfaces (No Breaking Changes)

Create new configurator classes alongside existing code:
- `ISceneConfigurator`
- `DefaultSceneConfigurator`
- `SavedSceneConfigurator`

### Step 2: Update SceneFactory (Breaking Change - Controlled)

Update `ISceneFactory` and `FlatbuffersSceneFactory` to use configurators.

**Before:**
```cpp
FlatbuffersSceneFactory(const GameContext &game_context,
                        const SceneDataFbs *scene_data_fbs);
```

**After:**
```cpp
FlatbuffersSceneFactory(const GameContext &game_context,
                        const ISceneConfigurator &scene_configurator);
```

### Step 3: Update SceneManager (Controlled Breaking Change)

Update `SceneManager` methods:

**Add new methods:**
```cpp
std::expected<std::monostate, FailInfo>
LoadSceneFromDefault(SceneType scene_type);

std::expected<std::monostate, FailInfo>
LoadSceneFromSave(uint32_t save_slot_index);
```

**Deprecate (then remove):**
```cpp
std::expected<std::monostate, FailInfo>
AddSceneFromDefault(const SceneType &scene_type);  // Old API
```

### Step 4: Testing

Create tests for:
1. `DefaultSceneConfigurator` with mock provider
2. `SavedSceneConfigurator` with mock provider
3. SceneFactory with configurators
4. End-to-end Scene loading from both sources

---

## Benefits of This Approach

### ✅ Decoupling
- Game code never sees FlatBuffers types directly
- SceneFactory is agnostic to data source
- Easy to add new data sources (JSON, Lua, etc.)

### ✅ No Intermediate Structs
- Configure Scene directly from FlatBuffers
- Avoid duplication of complex configuration logic
- Single source of truth

### ✅ Strategy Pattern
- Clear separation of concerns
- Runtime selection of data source
- No conditionals in factory

### ✅ Type Safety
- Compiler enforces correct usage
- `std::expected` for error handling
- Strong interfaces

### ✅ Maintainability
- Each class has single responsibility
- Easy to understand flow
- Easy to extend

---

## Open Questions

1. **Configurator Lifetime**: Who owns the configurator? SceneManager or SceneFactory?
   - **Recommendation**: SceneManager creates and owns, passes by reference to Factory

2. **Caching Strategy**: How long do we cache FlatBuffers data in configurators?
   - **Recommendation**: Cache for the lifetime of the configurator (Scene creation)

3. **Error Handling**: What happens if data load fails during configuration?
   - **Recommendation**: Return `std::expected` from `GetSceneData()`, propagate up

4. **Multiple Scenes**: Can one configurator configure multiple Scenes?
   - **Recommendation**: No, one configurator per Scene (matches lifecycle)

5. **Testing Strategy**: How do we test configurators without real data files?
   - **Recommendation**: Mock providers in tests, test configurator logic separately

---

## Conclusion

This design provides a clean, maintainable architecture for Scene configuration that:
- Decouples data sources from game code
- Avoids unnecessary intermediate structs
- Uses strategy pattern for flexibility
- Maintains type safety and clarity

The key insight is that **configurators encapsulate data access**, allowing SceneFactory to remain agnostic to data sources while still working directly with FlatBuffers data.

---

## Related Documents

- [Scene Factory Quick Reference](SCENE_FACTORY_QUICK_REF.md) - Quick reference guide
- [Scene Factory Visuals](SCENE_FACTORY_VISUALS.md) - Diagrams and visual aids
- `src/data_providers/ISceneDataProvider.h` - Default data provider interface
- `src/configuration/ISaveDataProvider.h` - Save data provider interface
- `src/scenes/ISceneFactory.h` - Current factory interface
- `src/entity/IEntityConfigurator.h` - Entity configurator pattern
