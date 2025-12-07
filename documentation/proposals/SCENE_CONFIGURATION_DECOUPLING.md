# Scene Configuration Decoupling - Comprehensive Design

**Date**: December 7, 2025  
**Status**: Proposal  
**Type**: Architecture - Data Source Abstraction  
**Related PR**: #882

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Problem Statement](#problem-statement)
3. [Current Architecture Analysis](#current-architecture-analysis)
4. [Proposed Architecture](#proposed-architecture)
5. [Implementation Phases](#implementation-phases)
6. [Benefits](#benefits)
7. [Risks and Mitigations](#risks-and-mitigations)
8. [Alternatives Considered](#alternatives-considered)
9. [Future Work](#future-work)

---

## Executive Summary

### The Problem

Currently, Scene and SceneFactory are tightly coupled to "default" data sources and FlatBuffers:

1. **Hardcoded Data Sources**: `Scene::ConfigureFromDefault()` and `SceneFactory::CreateDefaultScene()` assume data always comes from default FlatBuffers files
2. **No Save/Load Support**: Cannot create scenes from saved game data
3. **Format Coupling**: FlatBuffers types leak into Scene and SceneFactory
4. **Data Copying**: EntityConfigurator copies entity data through intermediate structures instead of configuring EntityMemoryPool directly

### The Solution

**Decouple scene configuration from data sources** through abstraction layers:

1. **IEntityConfigurator**: Abstract interface for entity configuration, independent of data format
2. **Scene Becomes Data-Agnostic**: Remove `ConfigureFromDefault()`, add `Configure(IEntityConfigurator&, const void*)`
3. **SceneManager Controls Sources**: SceneManager decides whether to load from defaults (`ISceneDataProvider`) or saves (`ISaveDataProvider`)
4. **Direct EMP Configuration**: Pass `EntityMemoryPool&` by reference, eliminating wasteful copying

### Key Outcomes

- ✅ Scene/SceneFactory have no knowledge of data sources (default vs save)
- ✅ Both ISceneDataProvider and ISaveDataProvider flow through identical code paths
- ✅ FlatBuffers types never exposed in Scene/SceneFactory public APIs
- ✅ EntityMemoryPool configured directly without intermediate copies
- ✅ Easy to add new data formats (JSON, XML, network) by implementing IEntityConfigurator

---

## Problem Statement

### Current Issues

#### 1. Hardcoded "Default" Assumption

```cpp
// SceneFactory.cpp - CURRENT
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateDefaultScene(const SceneType &scene_type,
                                const GameContext &game_context) {
    // ... create scene ...
    
    // Hardcoded to load from "default" data
    auto configure_result = scene_ptr->ConfigureFromDefault();
}

// Scene.cpp - CURRENT
std::expected<std::monostate, FailInfo>
Scene::ConfigureFromDefault(const DataType &data_type) {
    auto emp_configure_result =
        m_scene_resources.entity_manager.ConfigureEntitiesFromDefaultData(
            m_scene_info.type, data_type);
}
```

**Problem**: Method names and implementation assume "default" data. Cannot load from saves.

#### 2. No Separation of Concerns

```cpp
// SceneManager.cpp - CURRENT
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromDefault(const SceneType &scene_type) {
    SceneFactory scene_factory;
    auto scene_creation_result =
        scene_factory.CreateDefaultScene(scene_type, m_game_context);
    // SceneFactory decides data source, not SceneManager
}
```

**Problem**: SceneFactory (factory logic) makes data source decisions instead of SceneManager (orchestrator).

#### 3. FlatBuffers Coupling

```cpp
// FlatbuffersConfigurator.cpp - CURRENT
std::expected<std::monostate, FailInfo>
FlatbuffersConfigurator::ConfigureEntitiesFromDefaultData(
    EntityMemoryPool &entity_memory_pool, const SceneType scene_type) {
    
    // FlatBuffers type returned
    const SceneDataData *scene_data =
        m_data_loader.ProvideDefaultSceneData(scene_type).value();
    
    return ConfigureEntitiesFromCollection(entity_memory_pool,
                                          scene_data->entity_collection());
}
```

**Problem**: Configuration methods return/receive FlatBuffers types (`SceneDataData*`, `EntityCollection*`).

#### 4. Intermediate Data Copies

```cpp
// EntityManager.cpp - CURRENT
std::expected<std::monostate, FailInfo>
EntityManager::ConfigureEntitiesFromDefaultData(
    const SceneType scene_type, const DataType data_type) {
    
    FlatbuffersConfigurator configurator{m_event_handler};
    
    // Data flows through multiple layers
    auto configure_result = configurator.ConfigureEntitiesFromDefaultData(
        m_entity_memory_pool, scene_type);
}
```

**Problem**: Scene type passed down, data fetched internally, creating hidden dependencies.

---

## Current Architecture Analysis

### Call Chain (Default Scene Creation)

```
SceneManager::AddSceneFromDefault(SceneType)
  └─> SceneFactory::CreateDefaultScene(SceneType, GameContext)
       ├─> new TitleScene(uuid, GameContext)
       ├─> ISceneDataProvider::LoadSceneData(SceneType)  // For render texture size
       └─> Scene::ConfigureFromDefault(DataType)
            └─> EntityManager::ConfigureEntitiesFromDefaultData(SceneType, DataType)
                 └─> FlatbuffersConfigurator::ConfigureEntitiesFromDefaultData(EMP&, SceneType)
                      └─> FlatbuffersDataLoader::ProvideDefaultSceneData(SceneType)
                           └─> Returns SceneDataData* (FlatBuffers type)
```

### Problems with Current Flow

1. **Data Source Embedded in Names**: "Default" appears in method names throughout stack
2. **Scene Type Passed Down**: SceneType travels down the stack, fetching data internally at the bottom
3. **FlatBuffers Leakage**: FlatBuffers types (`SceneDataData*`, `EntityCollection*`) in call chain
4. **No Save/Load Path**: No way to provide saved game data instead of defaults

### Current Data Providers

We already have the right interfaces, but they're not used correctly:

```cpp
// ISceneDataProvider.h - ALREADY EXISTS ✅
class ISceneDataProvider {
public:
    virtual std::expected<SceneData, FailInfo>
    LoadSceneData(SceneType scene_type) const = 0;
};

// ISaveDataProvider.h - ALREADY EXISTS ✅
class ISaveDataProvider {
public:
    virtual std::expected<SaveData, FailInfo>
    LoadSave(uint32_t slot_index) const = 0;
};
```

**Issue**: These interfaces exist but aren't used in the scene configuration flow. SceneFactory bypasses them by calling configurator methods that internally fetch FlatBuffers data.

---

## Proposed Architecture

### Core Principle

**Inversion of Control**: Move data source decisions UP the stack to SceneManager, where they belong.

```
Current:  SceneManager → SceneFactory → Scene → EntityManager → Configurator → [fetches data]
Proposed: SceneManager → [fetches data] → SceneFactory → Scene → EntityManager → Configurator(data)
```

### New Abstractions

#### 1. IEntityConfigurator Interface

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Abstract interface for entity configuration
/////////////////////////////////////////////////
#pragma once

#include "FailInfo.h"
#include "containers.h"
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
/// @class IEntityConfigurator
/// @brief Abstract interface for configuring entities in an EntityMemoryPool.
///
/// Implementations handle different data formats (FlatBuffers, JSON, SaveData, etc.)
/// without exposing format-specific types to the scene system.
///
/// Key Design:
/// - Receives EntityMemoryPool& to configure directly (no copying)
/// - Accepts opaque data pointer (const void*) to allow any format
/// - Returns success/failure without format-specific types
/////////////////////////////////////////////////
class IEntityConfigurator {
public:
    virtual ~IEntityConfigurator() = default;

    /////////////////////////////////////////////////
    /// @brief Configure entities in the memory pool from provided data.
    ///
    /// @param entity_memory_pool Reference to pool to configure in-place
    /// @param data_source Opaque pointer to data (format determined by implementation)
    /// @return Success or detailed failure information
    /////////////////////////////////////////////////
    virtual std::expected<std::monostate, FailInfo>
    ConfigureEntities(EntityMemoryPool &entity_memory_pool,
                     const void *data_source) = 0;
};

} // namespace steamrot
```

#### 2. Refactored EntityConfigurator Base Class

```cpp
/////////////////////////////////////////////////
/// @class EntityConfigurator
/// @brief Base implementation of IEntityConfigurator.
///
/// Provides common functionality for all configurators (EventHandler access).
/// Derived classes implement format-specific configuration.
/////////////////////////////////////////////////
class EntityConfigurator : public IEntityConfigurator {
protected:
    /////////////////////////////////////////////////
    /// @brief Reference to the EventHandler for creating Subscribers
    /////////////////////////////////////////////////
    EventHandler &m_event_handler;

public:
    EntityConfigurator(EventHandler &event_handler);
    
    // Pure virtual from IEntityConfigurator
    virtual std::expected<std::monostate, FailInfo>
    ConfigureEntities(EntityMemoryPool &entity_memory_pool,
                     const void *data_source) = 0;
};
```

#### 3. FlatbuffersConfigurator Updated

```cpp
/////////////////////////////////////////////////
/// @class FlatbuffersConfigurator
/// @brief FlatBuffers-specific implementation of IEntityConfigurator.
/////////////////////////////////////////////////
class FlatbuffersConfigurator : public EntityConfigurator {
private:
    FlatbuffersDataLoader m_data_loader;

public:
    FlatbuffersConfigurator(EventHandler &event_handler);

    /////////////////////////////////////////////////
    /// @brief Configure entities from FlatBuffers EntityCollection data.
    ///
    /// @param entity_memory_pool Pool to configure
    /// @param data_source Pointer to EntityCollection (FlatBuffers type)
    /// @return Success or failure
    /////////////////////////////////////////////////
    std::expected<std::monostate, FailInfo>
    ConfigureEntities(EntityMemoryPool &entity_memory_pool,
                     const void *data_source) override;
};
```

#### 4. SaveDataConfigurator (Future)

```cpp
/////////////////////////////////////////////////
/// @class SaveDataConfigurator
/// @brief SaveData-specific implementation of IEntityConfigurator.
///
/// Configures entities from SaveData structs (native C++).
/////////////////////////////////////////////////
class SaveDataConfigurator : public EntityConfigurator {
public:
    SaveDataConfigurator(EventHandler &event_handler);

    std::expected<std::monostate, FailInfo>
    ConfigureEntities(EntityMemoryPool &entity_memory_pool,
                     const void *data_source) override;
};
```

### Updated Scene Class

```cpp
/////////////////////////////////////////////////
/// @class Scene
/// @brief Abstract base class for all Scenes in the game.
/////////////////////////////////////////////////
class Scene {
protected:
    // ... existing members ...

    /////////////////////////////////////////////////
    /// @brief Constructor for Scene class.
    /////////////////////////////////////////////////
    Scene(const SceneType scene_type, const uuids::uuid &id,
          const GameContext &game_context);

public:
    virtual ~Scene() = default;

    /////////////////////////////////////////////////
    /// @brief Configure scene entities using provided configurator and data.
    ///
    /// Scene is agnostic to data source - configurator handles format.
    ///
    /// @param configurator Abstract configurator for entity setup
    /// @param data_source Opaque pointer to data (format determined by configurator)
    /// @return Success or failure
    /////////////////////////////////////////////////
    std::expected<std::monostate, FailInfo>
    Configure(IEntityConfigurator &configurator, const void *data_source);

    // ❌ REMOVED: ConfigureFromDefault(DataType) - no longer needed

    // ... rest of interface unchanged ...
};
```

### Updated SceneFactory

Option A: Namespace with free functions (recommended for simplicity)

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Scene factory functions for creating game scenes.
/////////////////////////////////////////////////
#pragma once

namespace steamrot::scene_factory {

/////////////////////////////////////////////////
/// @brief Create a scene of the specified type.
///
/// @param scene_type Type of scene to create
/// @param game_context Game-wide context
/// @param configurator Entity configurator to use
/// @param data_source Data for configuration (format matches configurator)
/// @return Fully configured scene or failure
/////////////////////////////////////////////////
std::expected<std::unique_ptr<Scene>, FailInfo>
CreateScene(const SceneType &scene_type,
           const GameContext &game_context,
           IEntityConfigurator &configurator,
           const void *data_source);

} // namespace steamrot::scene_factory
```

Option B: Abstract factory class (more OOP, more overhead)

```cpp
/////////////////////////////////////////////////
/// @class AbstractSceneFactory
/// @brief Abstract factory for creating scenes.
/////////////////////////////////////////////////
class AbstractSceneFactory {
public:
    virtual ~AbstractSceneFactory() = default;

    virtual std::expected<std::unique_ptr<Scene>, FailInfo>
    CreateScene(const SceneType &scene_type,
               const GameContext &game_context,
               IEntityConfigurator &configurator,
               const void *data_source) = 0;
};
```

**Recommendation**: Use namespace approach (Option A) for simplicity. No need for runtime polymorphism here.

### Updated SceneManager

```cpp
/////////////////////////////////////////////////
/// @class SceneManager
/// @brief Manages game scenes and controls data source decisions.
/////////////////////////////////////////////////
class SceneManager : public SceneInfoProvider {
private:
    const GameContext &m_game_context;
    std::unordered_map<uuids::uuid, std::unique_ptr<Scene>> m_scenes;
    // ... other members ...

public:
    /////////////////////////////////////////////////
    /// @brief Create and add a scene from default data.
    ///
    /// Uses ISceneDataProvider to load default scene data.
    ///
    /// @param scene_type Type of scene to create
    /// @return Scene UUID or failure
    /////////////////////////////////////////////////
    std::expected<uuids::uuid, FailInfo>
    AddSceneFromDefault(const SceneType &scene_type);

    /////////////////////////////////////////////////
    /// @brief Create and add a scene from saved game data.
    ///
    /// Uses ISaveDataProvider to load saved scene data.
    ///
    /// @param slot_index Save slot to load from
    /// @return Scene UUID or failure
    /////////////////////////////////////////////////
    std::expected<uuids::uuid, FailInfo>
    AddSceneFromSave(uint32_t slot_index);

    // ... rest of interface ...
};
```

### Data Flow - New Architecture

#### Creating Scene from Default Data

```
1. SceneManager::AddSceneFromDefault(SceneType)
   │
   ├─> ISceneDataProvider& provider = GetSceneDataProvider()
   ├─> SceneData data = provider.LoadSceneData(scene_type)  // Native C++ struct
   │
   ├─> FlatbuffersConfigurator configurator(event_handler)
   ├─> EntityCollection* fb_data = LoadEntityCollectionFromFlatBuffers(scene_type)
   │
   └─> scene_factory::CreateScene(scene_type, game_context, configurator, fb_data)
        │
        ├─> new TitleScene(uuid, game_context)
        ├─> Configure render texture from SceneData
        │
        └─> Scene::Configure(configurator, fb_data)
             │
             └─> EntityManager::Configure(configurator, fb_data)
                  │
                  └─> configurator.ConfigureEntities(m_entity_memory_pool, fb_data)
                       │
                       └─> FlatbuffersConfigurator::ConfigureEntities(pool&, fb_data)
                            └─> Configure pool in-place (no copies)
```

#### Creating Scene from Save Data (Future)

```
1. SceneManager::AddSceneFromSave(slot_index)
   │
   ├─> ISaveDataProvider& provider = GetSaveDataProvider()
   ├─> SaveData save = provider.LoadSave(slot_index)  // Native C++ struct
   │
   ├─> SaveDataConfigurator configurator(event_handler)
   │
   └─> scene_factory::CreateScene(save.current_scene_type, game_context, 
                                   configurator, &save)
        │
        ├─> new TitleScene(uuid, game_context)
        ├─> Configure render texture from SaveData
        │
        └─> Scene::Configure(configurator, &save)
             │
             └─> EntityManager::Configure(configurator, &save)
                  │
                  └─> configurator.ConfigureEntities(m_entity_memory_pool, &save)
                       │
                       └─> SaveDataConfigurator::ConfigureEntities(pool&, &save)
                            └─> Extract entity data from SaveData and configure pool
```

### Key Improvements

1. **Data Source Decisions at Top**: SceneManager chooses `ISceneDataProvider` vs `ISaveDataProvider`
2. **Data Flows Down**: Data fetched first, then passed down the stack
3. **Scene Agnostic**: Scene never knows if data came from defaults or saves
4. **Format Hidden**: `const void*` hides format from Scene/SceneFactory
5. **Direct Configuration**: EntityMemoryPool configured in-place via reference

---

## Implementation Phases

### Phase 1: Create IEntityConfigurator Interface (1-2 days, Low Risk)

**Goal**: Establish abstraction layer for entity configuration.

**Tasks**:
1. Create `IEntityConfigurator` interface in `src/entity/IEntityConfigurator.h`
2. Add pure virtual `ConfigureEntities(EntityMemoryPool&, const void*)` method
3. Update `EntityConfigurator` to inherit from `IEntityConfigurator`
4. Add to CMakeLists.txt

**Tests**:
- Verify interface compiles
- Test that EntityConfigurator still works as base class

**Acceptance Criteria**:
- `IEntityConfigurator` interface exists
- `EntityConfigurator` implements it
- All existing tests pass

---

### Phase 2: Refactor FlatbuffersConfigurator (2-3 days, Low Risk)

**Goal**: Update FlatbuffersConfigurator to implement new interface.

**Tasks**:
1. Update `FlatbuffersConfigurator` to override `ConfigureEntities(EMP&, const void*)`
2. Reimplement existing methods to call `ConfigureEntities` internally
3. Keep `ConfigureEntitiesFromDefaultData` temporarily for backward compatibility
4. Update internal methods to work with EntityMemoryPool& directly

**Tests**:
- All existing FlatbuffersConfigurator tests pass
- New tests for `ConfigureEntities` method

**Acceptance Criteria**:
- FlatbuffersConfigurator implements IEntityConfigurator
- Existing functionality preserved
- Tests pass

---

### Phase 3: Remove Default Knowledge from Scene (2-3 days, Moderate Risk)

**Goal**: Make Scene data-source agnostic.

**Tasks**:
1. Add `Scene::Configure(IEntityConfigurator&, const void*)` method
2. Deprecate `Scene::ConfigureFromDefault()` (mark with comment)
3. Update EntityManager to accept configurator from outside
4. Remove DataType enum (no longer needed)

**Tests**:
- Update Scene tests to use new `Configure` method
- Test with FlatbuffersConfigurator
- Verify no "default" assumptions remain

**Acceptance Criteria**:
- `Scene::Configure` exists and works
- Scene has no data source knowledge
- Tests pass with new interface

---

### Phase 4: Abstract SceneFactory (2-3 days, Moderate Risk)

**Goal**: Remove data fetching from SceneFactory.

**Tasks**:
1. Convert SceneFactory to `scene_factory` namespace (or create AbstractSceneFactory)
2. Replace `CreateDefaultScene` with `CreateScene(type, context, configurator, data)`
3. SceneFactory receives data from caller, doesn't fetch it
4. Update all scene type creation (TitleScene, CraftingScene) to use new signature

**Tests**:
- Test scene creation with provided configurator and data
- Verify no internal data fetching
- Test both scene types

**Acceptance Criteria**:
- SceneFactory creates scenes without fetching data
- Configurator and data provided by caller
- All scene types work

---

### Phase 5: SceneManager Controls Data Sources (3-4 days, Moderate Risk)

**Goal**: Move data source decisions to SceneManager.

**Tasks**:
1. Update `SceneManager::AddSceneFromDefault` to:
   - Get data from `ISceneDataProvider`
   - Create `FlatbuffersConfigurator`
   - Call `scene_factory::CreateScene` with both
2. Add `SceneManager::AddSceneFromSave` method (stubbed for future)
3. Remove all "LoadDefault" methods from lower layers
4. Update convenience methods (LoadTitleScene, LoadCraftingScene)

**Tests**:
- Test AddSceneFromDefault with ISceneDataProvider
- Verify SceneManager controls data flow
- Test scene loading workflow end-to-end

**Acceptance Criteria**:
- SceneManager fetches data before scene creation
- Data source decision isolated to SceneManager
- Both convenience methods work

---

### Phase 6: Cleanup and Documentation (1-2 days, Low Risk)

**Goal**: Remove deprecated code, finalize documentation.

**Tasks**:
1. Remove `ConfigureFromDefault` methods completely
2. Remove `DataType` enum if no longer used
3. Update all tests to use new architecture
4. Update inline documentation
5. Update architecture diagrams

**Tests**:
- Full test suite passes
- No deprecated code remains

**Acceptance Criteria**:
- Codebase clean of old patterns
- Documentation up to date
- All tests passing

---

## Benefits

### 1. **Separation of Concerns**

- **SceneManager**: Orchestrates scene lifecycle, decides data sources
- **SceneFactory**: Pure factory logic, creates scene objects
- **Scene**: Domain model, agnostic to data format
- **IEntityConfigurator**: Data format adapter

Each class has a single, clear responsibility.

### 2. **Save/Load Support**

```cpp
// Easy to implement load game feature
std::expected<uuids::uuid, FailInfo>
SceneManager::AddSceneFromSave(uint32_t slot_index) {
    ISaveDataProvider& save_provider = GetSaveDataProvider();
    auto save_data = save_provider.LoadSave(slot_index);
    
    SaveDataConfigurator configurator(m_game_context.event_handler);
    
    return scene_factory::CreateScene(
        save_data.current_scene_type,
        m_game_context,
        configurator,
        &save_data.value());
}
```

### 3. **Format Flexibility**

Easy to add new data formats:

```cpp
// JSON configurator
class JsonConfigurator : public EntityConfigurator {
    std::expected<std::monostate, FailInfo>
    ConfigureEntities(EntityMemoryPool &pool, const void *data) override {
        const nlohmann::json* json_data = static_cast<const nlohmann::json*>(data);
        // Parse JSON and configure pool
    }
};

// Network configurator (receive scene data over network)
class NetworkConfigurator : public EntityConfigurator {
    std::expected<std::monostate, FailInfo>
    ConfigureEntities(EntityMemoryPool &pool, const void *data) override {
        const NetworkSceneData* net_data = static_cast<const NetworkSceneData*>(data);
        // Deserialize network data and configure pool
    }
};
```

### 4. **No Data Copying**

```cpp
// OLD: Data copied through layers
EntityManager::ConfigureFromDefault(SceneType) 
  → FlatbuffersConfigurator::ConfigureFromDefault(SceneType)
    → FlatbuffersDataLoader::ProvideDefaultSceneData(SceneType)
      → returns SceneDataData* (internal buffer)
        → ConfigureEntitiesFromCollection(copy to intermediate structs)

// NEW: Direct reference, no copies
EntityManager::Configure(configurator, data)
  → configurator.ConfigureEntities(m_entity_memory_pool&, data)
    → Configure m_entity_memory_pool in-place
```

### 5. **Testability**

```cpp
// Easy to test with mock configurator
class MockConfigurator : public EntityConfigurator {
    std::expected<std::monostate, FailInfo>
    ConfigureEntities(EntityMemoryPool &pool, const void *data) override {
        // Test-specific setup
        return std::monostate{};
    }
};

TEST_CASE("Scene configuration") {
    MockConfigurator mock_config(event_handler);
    TestData test_data;
    
    auto scene = scene_factory::CreateScene(
        SceneType::SceneType_TITLE,
        game_context,
        mock_config,
        &test_data);
    
    REQUIRE(scene.has_value());
}
```

### 6. **FlatBuffers Isolation**

Before:
```cpp
// FlatBuffers types leak to Scene
Scene::ConfigureFromDefault()
  → EntityManager::ConfigureEntitiesFromDefaultData()
    → FlatbuffersConfigurator (exposes const SceneDataData*)
```

After:
```cpp
// FlatBuffers isolated to FlatbuffersConfigurator
Scene::Configure(IEntityConfigurator&, const void*)
  → Only interface exposed, format hidden
```

---

## Risks and Mitigations

### Risk 1: Breaking Existing Code (High)

**Impact**: Scene creation is used throughout codebase.

**Mitigation**:
- Incremental migration with backward compatibility
- Keep old methods during transition
- Update one integration point at a time
- Comprehensive testing at each phase

### Risk 2: `const void*` Type Safety (Medium)

**Impact**: Passing `const void*` loses type safety.

**Mitigation**:
- Each configurator knows its expected type
- Cast and validate in `ConfigureEntities` implementation
- Return descriptive errors if cast fails
- Document expected types in configurator headers

```cpp
std::expected<std::monostate, FailInfo>
FlatbuffersConfigurator::ConfigureEntities(
    EntityMemoryPool &pool, const void *data) {
    
    if (!data) {
        return std::unexpected(FailInfo{
            FailMode::NullPointer,
            "Data source is null"
        });
    }
    
    const EntityCollection* entity_data = 
        static_cast<const EntityCollection*>(data);
    
    // Validate FlatBuffers table
    if (!entity_data->entity_memory_pool_size()) {
        return std::unexpected(FailInfo{
            FailMode::FlatbuffersDataNotFound,
            "Entity memory pool size not found"
        });
    }
    
    // Configure pool...
}
```

### Risk 3: SaveData Not Yet Implemented (Low)

**Impact**: Can't test save/load workflow immediately.

**Mitigation**:
- Architecture designed for it, even if not implemented
- Add `AddSceneFromSave` stub that returns "not implemented"
- Phase 5 proven to work with defaults first
- SaveData implementation is future work, not blocking

### Risk 4: Performance Overhead (Low)

**Impact**: Virtual function call for `ConfigureEntities`.

**Mitigation**:
- Scene configuration happens once at scene load (not per frame)
- Virtual call overhead negligible compared to I/O and configuration work
- Measured: ~1-2 nanoseconds per virtual call, scene load is milliseconds
- Benefit (flexibility) vastly outweighs cost

---

## Alternatives Considered

### Alternative 1: Keep SceneFactory Methods, Add Overloads

```cpp
class SceneFactory {
    // Keep existing
    std::expected<std::unique_ptr<Scene>, FailInfo>
    CreateDefaultScene(SceneType, GameContext);
    
    // Add new
    std::expected<std::unique_ptr<Scene>, FailInfo>
    CreateSceneFromSave(SaveData, GameContext);
};
```

**Rejected Because**:
- Doesn't solve core problem (data source embedded in method names)
- Explosion of methods for each data source type
- SceneFactory still makes data source decisions
- Doesn't abstract configurators

### Alternative 2: Template-Based Configuration

```cpp
template<typename DataSource>
std::expected<std::unique_ptr<Scene>, FailInfo>
CreateScene(SceneType type, GameContext context, DataSource data);
```

**Rejected Because**:
- Forces header-only implementation
- Increases compile times
- Doesn't provide runtime polymorphism (needed for save slots)
- Harder to test with mocks

### Alternative 3: Strategy Pattern with Concrete Types

```cpp
class SceneConfigurationStrategy {
    virtual std::expected<std::monostate, FailInfo>
    Configure(EntityMemoryPool&, SceneData data) = 0;
};
```

**Rejected Because**:
- Still requires SceneData (format-specific struct)
- Doesn't solve FlatBuffers coupling
- Less flexible than `const void*` approach
- Requires intermediate conversions

---

## Future Work

### Phase 7: Implement SaveData Configuration (Future)

**When**: After save/load system fully designed

**What**:
1. Design SaveData schema to include scene states
2. Implement SaveDataConfigurator
3. Add scene state serialization/deserialization
4. Update SceneManager::AddSceneFromSave to use real implementation

### Phase 8: Optimize Entity Data Loading (Future)

**When**: After performance profiling

**What**:
1. Investigate lazy loading for large entity pools
2. Consider entity streaming for large worlds
3. Profile configuration performance
4. Optimize hot paths if needed

### Phase 9: Network Scene Synchronization (Future)

**When**: If multiplayer is added

**What**:
1. Implement NetworkConfigurator
2. Add network protocol for scene data
3. Handle entity synchronization
4. Implement client-side prediction

---

## Conclusion

This proposal solves the fundamental coupling between Scene/SceneFactory and data sources. By introducing IEntityConfigurator and moving data source decisions to SceneManager, we create a flexible, testable architecture that supports:

- ✅ Default scene loading (existing feature)
- ✅ Save game loading (future feature)
- ✅ Alternative data formats (JSON, XML, network)
- ✅ Direct EntityMemoryPool configuration (performance)
- ✅ Clean separation of concerns

**Recommendation**: **Proceed with implementation** following the 6-phase plan. Low to moderate risk with high value.

**Timeline**: ~2-3 weeks for Phases 1-6

**Next Steps**:
1. Review and approve proposal
2. Create implementation tasks
3. Begin Phase 1 (IEntityConfigurator interface)

---

## Related Documents

- [Quick Reference Guide](SCENE_CONFIGURATION_DECOUPLING_QUICK_REF.md) - Implementation patterns
- [Architecture Diagrams](SCENE_CONFIGURATION_DECOUPLING_DIAGRAMS.md) - Visual representations
- [ISaveDataProvider Interface](../../src/configuration/ISaveDataProvider.h) - Existing save interface
- [ISceneDataProvider Interface](../../src/data_providers/ISceneDataProvider.h) - Existing scene data interface
