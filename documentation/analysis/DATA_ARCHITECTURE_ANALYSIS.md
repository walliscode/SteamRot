# Data Architecture Analysis - Polymorphic Structs & Scene Loading

**Date**: December 13, 2025  
**Context**: Analysis of data architecture for scene loading, save data, and configuration using polymorphic structs  
**Status**: Complete Architecture Analysis

---

## Executive Summary

This document provides a comprehensive analysis of the SteamRot data architecture, focusing on:
- **Polymorphic struct pattern** (SceneData → FbsSceneData) for multiple data sources
- **Layer stratification** to eliminate circular dependencies
- **Abstract interfaces** for scene loading, save data, and configuration
- **Concrete implementations** for default data, save files, and test data
- **Visualizations** for architecture and data flow

### Key Findings

✅ **Polymorphic struct pattern already exists** (SceneData → FbsSceneData)  
✅ **Provider pattern 75% complete** (ISceneDataProvider, IEngineDataProvider, etc.)  
⚠️ **Circular dependencies exist** (scenes ↔ data_providers, display ↔ scenes ↔ engine)  
⚠️ **SaveData infrastructure partially implemented** (interface exists, entity serialization missing)  
⚠️ **FlatBuffers types still exposed** in some configurators (FlatbuffersDefaultSceneConfigurator)

### Recommendations

1. **Adopt three-layer architecture** (Data/Types → Logic/Providers → Orchestration)
2. **Complete provider pattern** for all data sources
3. **Implement view pattern** for nested data navigation
4. **Add SaveSceneConfigurator** parallel to FlatbuffersDefaultSceneConfigurator
5. **Break circular dependencies** through proper layering

---

## Table of Contents

1. [Current State](#current-state)
2. [Dependency Analysis](#dependency-analysis)
3. [Polymorphic Struct Pattern](#polymorphic-struct-pattern)
4. [Layered Architecture Design](#layered-architecture-design)
5. [Abstract Interfaces](#abstract-interfaces)
6. [Concrete Implementations](#concrete-implementations)
7. [Data Flow Diagrams](#data-flow-diagrams)
8. [Migration Path](#migration-path)
9. [Implementation Checklist](#implementation-checklist)

---

## Current State

### Existing Patterns

#### 1. Polymorphic Struct Pattern

Already implemented for SceneData:

```cpp
// Base struct (abstract data type)
struct SceneData {
  SceneInfo scene_info;
};

// FlatBuffers implementation (for default data from .bin files)
struct FbsSceneData : public SceneData {
  const SceneDataFbs *scene_data_fbs;
};

// Note: SaveData contains a vector of SceneData objects
// No separate SaveSceneData type needed - SaveData is the container
struct SaveData {
  Metadata metadata;
  SceneType current_scene_type;
  std::vector<std::unique_ptr<SceneData>> scenes;  // Can hold multiple scenes
};
```

**Status**: ✅ Pattern established, ready for expansion

#### 2. Provider Pattern

Existing provider interfaces:

```cpp
// Scene data provider
class ISceneDataProvider {
public:
  virtual std::unique_ptr<SceneData>
  ProvideDefaultSceneData(const SceneType scene_type) const = 0;
};

// Engine data provider
class IEngineDataProvider {
public:
  virtual std::expected<EngineCoreData, FailInfo>
  LoadEngineCoreData() const = 0;
};

// Asset data provider
class IAssetDataProvider {
public:
  virtual std::expected<AssetData, FailInfo>
  LoadAssetData() const = 0;
};

// Scene manager data provider
class ISceneManagerDataProvider {
public:
  virtual std::expected<SceneManagerCoreData, FailInfo>
  LoadSceneManagerCoreData() const = 0;
};

// Save data provider (partially implemented)
class ISaveDataProvider {
public:
  virtual std::expected<SaveData, FailInfo>
  LoadSave(uint32_t slot_index) const = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  SaveGame(const SaveData &save_data) = 0;
};
```

**Status**: ✅ 5/6 providers implemented, SaveData needs scene serialization

#### 3. Configurator Pattern

Existing configurators:

```cpp
// Scene configurator interface
class ISceneConfigurator {
public:
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene, const SceneType scene_type) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene, const SceneType scene_type) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneConfig(Scene &scene, const SceneType scene_type) = 0;
};

// Entity configurator interface
class IEntityConfigurator {
public:
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool &emp) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureCUserInterface(CUserInterface &component) = 0;
  // ... other component methods
};
```

**Implementations**:
- `FlatbuffersDefaultSceneConfigurator` - for default scene data
- `FlatbuffersEntityConfigurator` - for entity/component data

**Status**: ✅ Pattern established, needs SaveSceneConfigurator

---

## Dependency Analysis

### Current CMake Package Dependencies

#### Layer Issues (Circular Dependencies)

**Problem 1: scenes ↔ data_providers**
```cmake
# scenes/CMakeLists.txt
target_link_libraries(scenes PUBLIC
  data_providers  # scenes depends on data_providers
)

# data_providers/CMakeLists.txt
target_link_libraries(data_providers PUBLIC
  scenes  # data_providers depends on scenes (for SceneType?)
)
```

**Problem 2: display ↔ scenes ↔ engine**
```cmake
# display/CMakeLists.txt
target_link_libraries(display PUBLIC
  scenes
  engine
)

# scenes/CMakeLists.txt
target_link_libraries(scenes PUBLIC
  display  # Circular!
)

# engine/CMakeLists.txt
target_link_libraries(engine PUBLIC
  display
)
```

**Problem 3: user_interface → engine (heavyweight)**
```cmake
# user_interface/CMakeLists.txt
target_link_libraries(user_interface PUBLIC
  engine  # UI depends on engine (heavy coupling)
)
```

#### Dependency Graph (Current)

```
Layer 3 (Orchestration):
  engine → display, entity, events, context, logic, assets, configuration, data_providers
  display → scenes, engine, logic
  
Layer 2 (Business Logic):
  scenes → display, entity, context, logic, data_providers  ❌ Circular
  logic → components, entity, engine, user_interface
  assets → data_providers, user_interface
  
Layer 1.5 (Providers):
  data_providers → types, events, scenes  ❌ Circular
  configuration → types, data_providers
  
Layer 1 (Data/Types):
  entity → components, types, events, data_providers
  components → types
  events → types
  types → (leaf node)
```

**Analysis**: Multiple circular dependencies prevent clean layering

---

## Polymorphic Struct Pattern

### Design Principle

**Concept**: Use inheritance to create data structs that can represent multiple sources:

```cpp
// Base struct (interface - minimal data)
struct SceneData {
  SceneInfo scene_info;  // Common to all sources
};

// FlatBuffers implementation (default/initial data from .bin files)
struct FbsSceneData : public SceneData {
  const SceneDataFbs *scene_data_fbs;
};

// Test data implementation (for unit tests)
struct TestSceneData : public SceneData {
  const TestSceneDataFbs *test_scene_data_fbs;
  std::string test_name;
};

// SaveData is a CONTAINER, not a polymorphic SceneData type
struct SaveData {
  struct Metadata {
    std::string save_name;
    std::string created_at;
    std::string last_modified;
    uint64_t play_time_seconds{0};
  } metadata;
  
  SceneType current_scene_type;
  std::vector<std::unique_ptr<SceneData>> scenes;  // Contains scene data
  uint32_t version{1};
};
```

### Usage in Configurators

Configurators work with the base `SceneData*` type (regardless of source):

```cpp
class ISceneConfigurator {
public:
  // Takes base SceneData pointer (polymorphic)
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData *data) = 0;
};

// FlatBuffers configurator (for default data)
class FlatbuffersSceneConfigurator : public ISceneConfigurator {
public:
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData *data) override {
    // Downcast to FbsSceneData
    const FbsSceneData *fbs_data = dynamic_cast<const FbsSceneData*>(data);
    if (!fbs_data) {
      return std::unexpected(FailInfo{"Not FbsSceneData"});
    }
    
    // Use FlatBuffers data
    const SceneDataFbs *fb = fbs_data->scene_data_fbs;
    // ... configure from FlatBuffers
  }
};

// Note: For save data, use the SAME configurator (FlatbuffersSceneConfigurator)
// because SaveData.scenes contains SceneData objects loaded from FlatBuffers
// No separate SaveSceneConfigurator needed!
```

### Benefits

✅ **Single interface** for SceneFactory (doesn't need to know about data sources)  
✅ **Type-safe** (dynamic_cast ensures correct type)  
✅ **Extensible** (add new data sources without changing existing code)  
✅ **Testable** (TestSceneData for mocking)  
✅ **Simpler** (SaveData is just a container, not a polymorphic type)

---

## Layered Architecture Design

### Three-Layer Strategy

To eliminate circular dependencies and create clean architecture:

```
┌──────────────────────────────────────────────────────────────┐
│  Layer 3: Orchestration (Top-level systems)                  │
│  - engine_core                                               │
│  - display_system                                            │
│  - scene_management                                          │
│  Dependencies: Layer 2 + Layer 1                             │
└──────────────────────────────────────────────────────────────┘
                           ▲
                           │
┌──────────────────────────────────────────────────────────────┐
│  Layer 2: Business Logic & Implementations                   │
│  - logic (rendering, collision, actions)                     │
│  - providers (FlatBuffers*, Save*, Test*)                    │
│  - configurators (FlatbuffersSceneConfigurator, etc.)        │
│  - assets_system                                             │
│  Dependencies: Layer 1 only                                  │
└──────────────────────────────────────────────────────────────┘
                           ▲
                           │
┌──────────────────────────────────────────────────────────────┐
│  Layer 1: Data, Types & Abstract Interfaces                  │
│  - types (SceneData, EntityData, Components)                 │
│  - interfaces (ISceneDataProvider, ISceneConfigurator)       │
│  - data_structures (EntityMemoryPool, Archetype)             │
│  - events (EventPacket, EventHandler interface)              │
│  Dependencies: None (leaf nodes) or SFML/std only            │
└──────────────────────────────────────────────────────────────┘
```

### CMake Package Reorganization

#### Layer 1: Data & Types (Zero Circular Dependencies)

**Package: `types`**
- Contents: SceneData, EntityData, Component structs, UIElement types
- Dependencies: SFML, flatbuffers (interface only), stduuid
- **Must NOT depend on**: Any Layer 2 or Layer 3 packages

**Package: `interfaces`** (NEW - extract from existing packages)
- Contents: ISceneDataProvider, ISceneConfigurator, IEntityConfigurator, IAssetDataProvider
- Dependencies: types only
- **Purpose**: Break circular dependencies by providing abstract contracts

**Package: `data_structures`**
- Contents: EntityMemoryPool, ArchetypeManager, containers
- Dependencies: types, interfaces
- **Must NOT depend on**: providers, configurators, or logic

**Package: `events`**
- Contents: EventPacket, EventBus, IEventHandler interface
- Dependencies: types
- **Must NOT depend on**: engine, scenes, or other Layer 2+

#### Layer 2: Business Logic & Providers (Depends on Layer 1)

**Package: `providers`** (rename from `data_providers`)
- Contents: FlatbuffersSceneDataProvider, FlatbuffersSaveDataProvider, TestDataProvider
- Dependencies: types, interfaces, data_structures
- **Must NOT depend on**: scenes, engine, display

**Package: `configurators`** (NEW - extract from scenes/entity)
- Contents: FlatbuffersSceneConfigurator, SaveSceneConfigurator, FlatbuffersEntityConfigurator
- Dependencies: types, interfaces, data_structures, providers
- **Must NOT depend on**: scenes, engine

**Package: `logic_implementations`** (rename from `logic`)
- Contents: UIRenderLogic, UICollisionLogic, movement logic
- Dependencies: types, data_structures, interfaces
- **Must NOT depend on**: engine, scenes

**Package: `assets_system`** (rename from `assets`)
- Contents: AssetManager, font/texture loading
- Dependencies: types, providers, interfaces
- **Must NOT depend on**: engine, scenes

#### Layer 3: Orchestration (Depends on Layer 1 + 2)

**Package: `scene_management`** (rename from `scenes`)
- Contents: SceneManager, SceneFactory, Scene, CraftingScene, TitleScene
- Dependencies: types, interfaces, data_structures, configurators, logic_implementations
- **Can depend on**: providers (for factory pattern)

**Package: `engine_core`** (rename from `engine`)
- Contents: GameEngine, Engine, GameLoop
- Dependencies: scene_management, assets_system, providers, configurators

**Package: `display_system`** (rename from `display`)
- Contents: DisplayManager, window management
- Dependencies: scene_management, engine_core (carefully - may need interface)

### Dependency Rules

**Layer 1 Rules**:
- ✅ Can depend on: SFML, std, external libraries
- ❌ Cannot depend on: Any other layer

**Layer 2 Rules**:
- ✅ Can depend on: Layer 1, SFML, std
- ❌ Cannot depend on: Layer 3

**Layer 3 Rules**:
- ✅ Can depend on: Layer 1, Layer 2, SFML, std
- ⚠️ Circular dependencies within Layer 3 must be resolved via interfaces

---

## Abstract Interfaces

### Required Interfaces for Clean Architecture

#### 1. Scene Data Loading

```cpp
// Location: src/interfaces/ISceneDataProvider.h
namespace steamrot {

class ISceneDataProvider {
public:
  virtual ~ISceneDataProvider() = default;
  
  // Provide default scene data (from .bin files)
  virtual std::unique_ptr<SceneData>
  ProvideDefaultSceneData(const SceneType scene_type) const = 0;
  
  // NEW: Provide scene data from save file
  virtual std::unique_ptr<SceneData>
  ProvideSceneDataFromSave(const SaveData &save, 
                          const SceneType scene_type) const = 0;
  
  // NEW: Provide scene data for testing
  virtual std::unique_ptr<SceneData>
  ProvideTestSceneData(const std::string &test_name) const = 0;
};

} // namespace steamrot
```

**Implementations**:
- `FlatbuffersSceneDataProvider` - default scene data from .bin files
- `SaveSceneDataProvider` (NEW) - scene data from save files
- `TestSceneDataProvider` (NEW) - scene data for unit tests

#### 2. Scene Configuration

```cpp
// Location: src/interfaces/ISceneConfigurator.h
namespace steamrot {

class ISceneConfigurator {
public:
  virtual ~ISceneConfigurator() = default;
  
  // Configure scene from polymorphic SceneData
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData *data) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene, const SceneData *data) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene, const SceneData *data) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneConfig(Scene &scene, const SceneData *data) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureLogicMap(Scene &scene) = 0;
};

} // namespace steamrot
```

**Implementations**:
- `FlatbuffersSceneConfigurator` - configure from FbsSceneData
- `SaveSceneConfigurator` (NEW) - configure from SaveSceneData
- `TestSceneConfigurator` (NEW) - configure from TestSceneData

#### 3. Save Data Management

```cpp
// Location: src/interfaces/ISaveDataProvider.h
namespace steamrot {

class ISaveDataProvider {
public:
  virtual ~ISaveDataProvider() = default;
  
  // Load save metadata
  virtual std::expected<std::vector<SaveSlotInfo>, FailInfo>
  GetSaveSlots() const = 0;
  
  // Load complete save data
  virtual std::expected<SaveData, FailInfo>
  LoadSave(uint32_t slot_index) const = 0;
  
  // NEW: Save complete game state
  virtual std::expected<std::monostate, FailInfo>
  SaveGame(const SaveData &save_data) = 0;
  
  // NEW: Capture current scene state
  virtual std::expected<SceneData*, FailInfo>
  CaptureSceneState(const Scene &scene) const = 0;
};

} // namespace steamrot
```

**Implementations**:
- `FlatbuffersSaveDataProvider` - save/load from .save files

#### 4. Entity Data Configuration

```cpp
// Location: src/interfaces/IEntityConfigurator.h
namespace steamrot {

class IEntityConfigurator {
public:
  virtual ~IEntityConfigurator() = default;
  
  // Configure entire entity memory pool
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool &emp, 
                           const SceneData *data) = 0;
  
  // Configure individual components
  virtual std::expected<std::monostate, FailInfo>
  ConfigureCUserInterface(CUserInterface &component,
                         const void *component_data) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureCGrimoireMachina(CGrimoireMachina &component,
                           const void *component_data) = 0;
  // ... other components
};

} // namespace steamrot
```

**Implementations**:
- `FlatbuffersEntityConfigurator` - configure from FlatBuffers entity data
- `SaveEntityConfigurator` (NEW) - configure from saved entity data

---

## Concrete Implementations

### Implementation Matrix

| Data Source | Provider | Configurator | Status |
|-------------|----------|--------------|--------|
| **Default Scene Data** | FlatbuffersSceneDataProvider | FlatbuffersSceneConfigurator | ✅ Exists (rename needed) |
| **Save File Data** | SaveSceneDataProvider | SaveSceneConfigurator | ❌ Need to create |
| **Test Data** | TestSceneDataProvider | TestSceneConfigurator | ❌ Need to create |
| **Engine Config** | FlatbuffersEngineDataProvider | N/A (direct use) | ✅ Exists |
| **Asset Data** | FlatbuffersAssetDataProvider | N/A (direct use) | ✅ Exists |

### 1. Default Scene Data (FlatBuffers)

**Provider**: `FlatbuffersSceneDataProvider`
```cpp
// Location: src/providers/FlatbuffersSceneDataProvider.cpp
namespace steamrot {

std::unique_ptr<SceneData>
FlatbuffersSceneDataProvider::ProvideDefaultSceneData(
    const SceneType scene_type) const {
  
  // Load FlatBuffers data
  FlatbuffersDataLoader loader;
  auto fb_result = loader.ProvideDefaultSceneData(scene_type);
  if (!fb_result.has_value()) {
    return nullptr;
  }
  
  // Create FbsSceneData (polymorphic)
  auto fbs_data = std::make_unique<FbsSceneData>();
  fbs_data->scene_info.type = scene_type;
  fbs_data->scene_data_fbs = fb_result.value();
  
  return fbs_data;
}

} // namespace steamrot
```

**Configurator**: `FlatbuffersSceneConfigurator`
```cpp
// Location: src/configurators/FlatbuffersSceneConfigurator.cpp
namespace steamrot {

std::expected<std::monostate, FailInfo>
FlatbuffersSceneConfigurator::ConfigureScene(Scene &scene, 
                                            const SceneData *data) {
  // Downcast to FbsSceneData
  const FbsSceneData *fbs_data = dynamic_cast<const FbsSceneData*>(data);
  if (!fbs_data) {
    return std::unexpected(FailInfo{"Expected FbsSceneData"});
  }
  
  // Configure from FlatBuffers
  const SceneDataFbs *fb = fbs_data->scene_data_fbs;
  
  // Configure SceneInfo
  scene.m_scene_info.type = fbs_data->scene_info.type;
  scene.m_scene_info.uuid = GenerateUUID();
  
  // Configure SceneResources (entities, logic map)
  auto entity_config = ConfigureEntityPool(scene, fb->entity_collection());
  if (!entity_config.has_value()) {
    return std::unexpected(entity_config.error());
  }
  
  auto logic_config = ConfigureLogicMap(scene);
  if (!logic_config.has_value()) {
    return std::unexpected(logic_config.error());
  }
  
  return std::monostate{};
}

} // namespace steamrot
```

### 2. Save File Data (NEW)

**Provider**: `SaveSceneDataProvider` (NEW)
```cpp
// Location: src/providers/FlatbuffersSceneDataProvider.h
namespace steamrot {

class FlatbuffersSceneDataProvider : public ISceneDataProvider {
public:
  // For save data: Extract SceneData from SaveData's vector
  std::unique_ptr<SceneData>
  ProvideSceneDataFromSave(const SaveData &save, 
                          const SceneType scene_type) const override {
    
    // SaveData already contains SceneData objects in its vector
    // Find the matching scene
    for (const auto& scene_data : save.scenes) {
      if (scene_data->scene_info.type == scene_type) {
        // Return a copy (or clone) of the SceneData
        // The SceneData in save.scenes is already in FlatBuffers format
        return std::make_unique<SceneData>(*scene_data);
      }
    }
    
    return nullptr;  // Scene not found in save
  }
};

} // namespace steamrot
```

**Configurator**: Use the same `FlatbuffersSceneConfigurator`!

No separate SaveSceneConfigurator needed because SaveData.scenes contains regular SceneData objects (likely FbsSceneData from FlatBuffers). The configurator doesn't care whether the data came from a .bin file or a .save file - it just configures from SceneData.

```cpp
// The SAME configurator works for both default and save data
class FlatbuffersSceneConfigurator : public ISceneConfigurator {
public:
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData *data) override {
    // Works for data from .bin files OR save files
    const FbsSceneData *fbs_data = dynamic_cast<const FbsSceneData*>(data);
    if (!fbs_data) {
      return std::unexpected(FailInfo{"Expected FbsSceneData"});
    }
    
    // Configure from FlatBuffers (same logic regardless of source)
    const SceneDataFbs *fb = fbs_data->scene_data_fbs;
    
    // Configure scene info, resources, config, logic
    // ... implementation
    
    return std::monostate{};
  }
};
```

### 3. Test Data (NEW)

**Provider**: `TestSceneDataProvider` (NEW)
```cpp
// Location: src/providers/TestSceneDataProvider.h
namespace steamrot {

class TestSceneDataProvider : public ISceneDataProvider {
public:
  std::unique_ptr<SceneData>
  ProvideTestSceneData(const std::string &test_name) const override {
    
    // Load test FlatBuffers data
    FlatbuffersDataLoader loader;
    auto fb_result = loader.LoadTestSceneData(test_name);
    if (!fb_result.has_value()) {
      return nullptr;
    }
    
    // Create TestSceneData (polymorphic)
    auto test_data = std::make_unique<TestSceneData>();
    test_data->scene_info.type = SceneType::SceneType_TEST;
    test_data->test_scene_data_fbs = fb_result.value();
    test_data->test_name = test_name;
    
    return test_data;
  }
};

} // namespace steamrot
```

### Usage in SceneFactory

#### Pattern 1: Default Scene Loading

```cpp
// Location: src/scene_management/SceneFactory.cpp
namespace steamrot {

std::unique_ptr<Scene> SceneFactory::CreateSceneFromDefault(
    const SceneType scene_type) {
  
  // Get provider and configurator for default data
  ISceneDataProvider& provider = GetFlatbuffersSceneDataProvider();
  ISceneConfigurator& configurator = GetFlatbuffersSceneConfigurator();
  
  // Provider loads default data
  std::unique_ptr<SceneData> data = provider.ProvideDefaultSceneData(scene_type);
  if (!data) {
    return nullptr;
  }
  
  // Create empty scene
  std::unique_ptr<Scene> scene = CreateEmptyScene(scene_type);
  
  // Configurator applies data
  auto config_result = configurator.ConfigureScene(*scene, data.get());
  if (!config_result.has_value()) {
    return nullptr;
  }
  
  return scene;
}

} // namespace steamrot
```

#### Pattern 2: Save File Loading (Two-Step Process)

**Key Insight**: SaveData is a container that holds scene data. The process requires:
1. Load `SaveData` (contains metadata + scene data)
2. Extract `SceneData` from `SaveData`
3. Configure scene from `SceneData`

```cpp
// Location: src/scene_management/SceneFactory.cpp
namespace steamrot {

std::unique_ptr<Scene> SceneFactory::CreateSceneFromSave(
    uint32_t save_slot) {
  
  // STEP 1: Load SaveData (contains scene data)
  ISaveDataProvider& save_provider = GetSaveDataProvider();
  auto save_result = save_provider.LoadSave(save_slot);
  if (!save_result.has_value()) {
    return nullptr;
  }
  
  const SaveData& save_data = save_result.value();
  SceneType scene_type = save_data.current_scene_type;
  
  // STEP 2: Extract SceneData from SaveData.scenes vector
  ISceneDataProvider& scene_provider = GetFlatbuffersSceneDataProvider();
  std::unique_ptr<SceneData> scene_data = 
      scene_provider.ProvideSceneDataFromSave(save_data, scene_type);
  if (!scene_data) {
    return nullptr;
  }
  
  // STEP 3: Configure scene from SceneData (same configurator as default!)
  ISceneConfigurator& configurator = GetFlatbuffersSceneConfigurator();
  std::unique_ptr<Scene> scene = CreateEmptyScene(scene_type);
  
  auto config_result = configurator.ConfigureScene(*scene, scene_data.get());
  if (!config_result.has_value()) {
    return nullptr;
  }
  
  return scene;
}

} // namespace steamrot
```

**Why Two Steps?**

1. **SaveData is a container**: SaveData contains metadata + vector of SceneData objects
2. **Separation of concerns**: 
   - `ISaveDataProvider` handles save file I/O and metadata
   - `ISceneDataProvider` extracts SceneData from SaveData.scenes vector
   - `ISceneConfigurator` applies scene data to Scene objects (same as default!)
3. **Reusability**: Same `FlatbuffersSceneDataProvider` and `FlatbuffersSceneConfigurator` work for both default and save data

#### Pattern 3: Alternative - Unified Interface (Optional)

If you prefer a single-step API, you can create a convenience method:

```cpp
// Location: src/scene_management/SceneFactory.cpp
namespace steamrot {

std::unique_ptr<Scene> SceneFactory::CreateScene(
    DataSource source,
    const void *source_data) {
  
  switch (source) {
    case DataSource::Default: {
      const SceneType* type = static_cast<const SceneType*>(source_data);
      return CreateSceneFromDefault(*type);
    }
      
    case DataSource::SaveFile: {
      const uint32_t* slot = static_cast<const uint32_t*>(source_data);
      return CreateSceneFromSave(*slot);
    }
      
    case DataSource::Test: {
      const std::string* test_name = static_cast<const std::string*>(source_data);
      return CreateSceneFromTest(*test_name);
    }
  }
  
  return nullptr;
}

} // namespace steamrot
```

**Recommended**: Use the explicit methods (`CreateSceneFromDefault`, `CreateSceneFromSave`) for clarity.

---

## Data Flow Diagrams

### Diagram 1: Default Scene Loading (Current & Target)

```
┌─────────────────────────────────────────────────────────────────┐
│                     CURRENT STATE                               │
└─────────────────────────────────────────────────────────────────┘

SceneFactory
    ↓
    ├─ Creates empty Scene
    ↓
FlatbuffersDefaultSceneConfigurator
    ↓
    ├─ Has FlatbuffersDataLoader member
    ├─ Loads FlatBuffers directly
    ↓
Scene configured

❌ Issues:
- Configurator loads data (violates SRP)
- Can't swap data sources
- Hard to test


┌─────────────────────────────────────────────────────────────────┐
│                     TARGET STATE                                │
└─────────────────────────────────────────────────────────────────┘

SceneFactory
    ↓
    ├─ Gets ISceneDataProvider
    ├─ Gets ISceneConfigurator
    ↓
ISceneDataProvider (FlatbuffersSceneDataProvider)
    ↓
    ├─ Loads SceneDataFbs from .bin file
    ├─ Creates FbsSceneData (polymorphic)
    ├─ Returns std::unique_ptr<SceneData>
    ↓
ISceneConfigurator (FlatbuffersSceneConfigurator)
    ↓
    ├─ Receives SceneData* (base pointer)
    ├─ dynamic_cast to FbsSceneData*
    ├─ Configures Scene from FlatBuffers
    ↓
Scene configured

✅ Benefits:
- Configurator doesn't load data
- Provider isolates FlatBuffers
- Easy to swap data sources
- Testable (mock providers)
```

### Diagram 2: Save File Loading (Target)

```
┌─────────────────────────────────────────────────────────────────┐
│                  SAVE FILE LOADING FLOW                         │
└─────────────────────────────────────────────────────────────────┘

User clicks "Load Game" (slot 2)
    ↓
ISaveDataProvider.LoadSave(2)
    ↓
    ├─ Loads save_slot_2.save (FlatBuffers)
    ├─ Creates SaveData struct
    ├─ Extracts metadata, current_scene_type
    ↓
SceneFactory.CreateScene(SceneType, DataSource::SaveFile, &SaveData)
    ↓
    ├─ Gets SaveSceneDataProvider
    ├─ Gets SaveSceneConfigurator
    ↓
SaveSceneDataProvider.ProvideSceneDataFromSave(SaveData, SceneType)
    ↓
    ├─ Loads SavedSceneDataFbs from save file
    ├─ Creates SaveSceneData (polymorphic)
    ├─ Includes: UUID, play_time, last_modified
    ├─ Returns std::unique_ptr<SceneData>
    ↓
SaveSceneConfigurator.ConfigureScene(Scene&, SceneData*)
    ↓
    ├─ dynamic_cast to SaveSceneData*
    ├─ Restores Scene UUID (important!)
    ├─ Restores EntityMemoryPool state
    ├─ Restores component values
    ├─ Recreates LogicMap (static)
    ↓
Scene restored to saved state
```

### Diagram 3: Save Game Flow (Target)

```
┌─────────────────────────────────────────────────────────────────┐
│                    SAVE GAME FLOW                               │
└─────────────────────────────────────────────────────────────────┘

User clicks "Save Game" (slot 1)
    ↓
SaveManager.CaptureGameState()
    ↓
    ├─ Create SaveData struct
    ├─ Capture metadata (name, time, version)
    ├─ Capture current_scene_type
    ↓
    ├─ For current scene:
    │   ↓
    │   SceneSerializer.CaptureSceneState(Scene&)
    │       ↓
    │       ├─ Capture Scene UUID
    │       ├─ Capture SceneType
    │       ├─ Capture EntityMemoryPool state
    │       │   ↓
    │       │   EntitySerializer.SerializeEntityPool(EMP)
    │       │       ↓
    │       │       ├─ Iterate all active entities
    │       │       ├─ Serialize each component
    │       │       ├─ Preserve entity indices
    │       │       ├─ Create SavedEntityData
    │       ↓
    │       ├─ Create SavedSceneDataFbs
    │       ├─ Return SaveSceneData
    ↓
ISaveDataProvider.SaveGame(SaveData)
    ↓
    ├─ Build FlatBuffers from SaveData
    ├─ Build SavedSceneDataFbs
    ├─ Build SaveMetadata
    ├─ Write to save_slot_1.save
    ↓
Save complete
```

### Diagram 4: Entity Configuration with Polymorphic Data

```
┌─────────────────────────────────────────────────────────────────┐
│            ENTITY CONFIGURATION FLOW                            │
└─────────────────────────────────────────────────────────────────┘

Scene has SceneData* (polymorphic)
    ↓
IEntityConfigurator.ConfigureEntityMemoryPool(EMP, SceneData*)
    ↓
    ├─ Downcast SceneData* to concrete type
    │
    ├─ If FbsSceneData:
    │   ↓
    │   FlatbuffersEntityConfigurator
    │       ↓
    │       ├─ Extract EntityCollection from FlatBuffers
    │       ├─ Iterate entities
    │       ├─ Configure each component
    │       ├─ Set default values
    │
    ├─ If SaveSceneData:
    │   ↓
    │   SaveEntityConfigurator
    │       ↓
    │       ├─ Extract SavedEntityCollection from save file
    │       ├─ Iterate entities
    │       ├─ Restore each component state
    │       ├─ Preserve entity indices
    │
    ├─ If TestSceneData:
        ↓
        TestEntityConfigurator
            ↓
            ├─ Extract test entities
            ├─ Configure for test scenario
    ↓
EntityMemoryPool configured
```

### Diagram 5: Layer Architecture

```
┌──────────────────────────────────────────────────────────────────┐
│                      LAYER 3: ORCHESTRATION                      │
│                                                                  │
│  ┌─────────────┐  ┌──────────────┐  ┌─────────────────┐       │
│  │ engine_core │  │display_system│  │scene_management │       │
│  └─────────────┘  └──────────────┘  └─────────────────┘       │
│         │                │                    │                 │
│         └────────────────┴────────────────────┘                 │
│                         ▼                                       │
└──────────────────────────────────────────────────────────────────┘
                          │
┌──────────────────────────────────────────────────────────────────┐
│                   LAYER 2: BUSINESS LOGIC                        │
│                                                                  │
│  ┌──────────┐  ┌──────────────┐  ┌─────────────────────┐      │
│  │providers │  │configurators  │  │logic_implementations │      │
│  └──────────┘  └──────────────┘  └─────────────────────┘      │
│                                                                  │
│  ┌─────────────┐  ┌──────────────────────────────────┐         │
│  │assets_system│  │ FlatbuffersSceneDataProvider    │         │
│  └─────────────┘  │ SaveSceneDataProvider           │         │
│                   │ FlatbuffersSceneConfigurator    │         │
│                   │ SaveSceneConfigurator           │         │
│                   └──────────────────────────────────┘         │
│                         │                                       │
│                         ▼                                       │
└──────────────────────────────────────────────────────────────────┘
                          │
┌──────────────────────────────────────────────────────────────────┐
│              LAYER 1: DATA, TYPES & INTERFACES                   │
│                                                                  │
│  ┌─────────┐  ┌────────────────┐  ┌──────────────────┐        │
│  │  types  │  │  interfaces    │  │data_structures   │        │
│  └─────────┘  └────────────────┘  └──────────────────┘        │
│                                                                  │
│  ┌────────────────────────────────────────────────┐            │
│  │ SceneData, FbsSceneData, SaveSceneData        │            │
│  │ ISceneDataProvider, ISceneConfigurator        │            │
│  │ EntityMemoryPool, ArchetypeManager            │            │
│  │ Component, CUserInterface, etc.               │            │
│  └────────────────────────────────────────────────┘            │
│                                                                  │
│  Dependencies: SFML, flatbuffers (interface), std              │
└──────────────────────────────────────────────────────────────────┘
```

---

## Migration Path

### Phase 1: Extract Interfaces (Week 1)

**Goal**: Create `interfaces` package with zero dependencies

**Tasks**:
1. Create `src/interfaces/` directory
2. Move interface declarations:
   - `ISceneDataProvider.h`
   - `ISceneConfigurator.h`
   - `IEntityConfigurator.h`
   - `IAssetDataProvider.h`
   - `ISaveDataProvider.h`
3. Update CMakeLists.txt:
   ```cmake
   add_library(interfaces INTERFACE)
   target_include_directories(interfaces INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
   target_link_libraries(interfaces INTERFACE types)
   ```
4. Update all existing code to include from `interfaces/`

**Validation**: Builds successfully, no new dependencies introduced

### Phase 2: Refactor Configurators (Week 2)

**Goal**: Configurators receive polymorphic SceneData*, don't load data

**Tasks**:
1. Modify `ISceneConfigurator`:
   ```cpp
   virtual std::expected<std::monostate, FailInfo>
   ConfigureScene(Scene &scene, const SceneData *data) = 0;
   ```
2. Update `FlatbuffersDefaultSceneConfigurator`:
   - Remove `FlatbuffersDataLoader` member
   - Accept `SceneData*` parameter
   - Downcast to `FbsSceneData*`
3. Update `SceneFactory`:
   - Get provider and configurator separately
   - Provider loads data, configurator receives it

**Validation**: Default scene loading still works

### Phase 3: Implement Save Infrastructure (Weeks 3-4)

**Goal**: Complete save/load workflow

**Tasks**:
1. Create `SaveSceneData` struct:
   ```cpp
   struct SaveSceneData : public SceneData {
     const SavedSceneDataFbs *saved_scene_data_fbs;
     uint64_t play_time_seconds;
     std::string last_modified;
   };
   ```
2. Implement `SaveSceneDataProvider`
3. Implement `SaveSceneConfigurator`
4. Implement entity serialization:
   - `EntitySerializer::SerializeEntityPool()`
   - Save to FlatBuffers format
5. Update `ISaveDataProvider` with scene state methods
6. Test save/load round-trip

**Validation**: Can save and load game state

### Phase 4: Break Circular Dependencies (Weeks 5-6)

**Goal**: Eliminate scenes ↔ data_providers circular dependency

**Tasks**:
1. Move SceneType enum to `types` package
2. Ensure `data_providers` only depends on `types` and `interfaces`
3. Move any scene-specific types to `types/scene/`
4. Update CMakeLists.txt to enforce layer rules
5. Use `-Wl,--no-undefined` linker flag to catch violations

**Validation**: No circular dependencies, clean layer architecture

### Phase 5: Reorganize Packages (Weeks 7-8)

**Goal**: Full three-layer architecture

**Tasks**:
1. Rename `data_providers` → `providers`
2. Create `configurators` package (extract from scenes/entity)
3. Create `scene_management` package (rename from scenes)
4. Create `engine_core` package (rename from engine)
5. Update all CMakeLists.txt
6. Update documentation

**Validation**: Clean build, all tests pass

---

## Implementation Checklist

### Immediate Actions (Phase 1)

- [ ] Create `src/interfaces/` directory
- [ ] Extract interface headers to `interfaces/`
- [ ] Create `interfaces` CMake package
- [ ] Update all includes to use `interfaces/`
- [ ] Verify build succeeds

### Short-term (Phases 2-3)

- [ ] Refactor `ISceneConfigurator` to accept `SceneData*`
- [ ] Update `FlatbuffersDefaultSceneConfigurator` (remove data loader)
- [ ] Update `SceneFactory` (separate provider and configurator)
- [ ] Create `SaveSceneData` struct
- [ ] Implement `SaveSceneDataProvider`
- [ ] Implement `SaveSceneConfigurator`
- [ ] Implement entity serialization
- [ ] Test save/load workflow

### Medium-term (Phases 4-5)

- [ ] Move SceneType to `types` package
- [ ] Break scenes ↔ data_providers circular dependency
- [ ] Reorganize into three-layer architecture
- [ ] Rename packages (providers, configurators, scene_management)
- [ ] Update all CMakeLists.txt
- [ ] Enable strict linker checks (`--no-undefined`)
- [ ] Update documentation

### Documentation

- [ ] Update architecture diagrams
- [ ] Create migration guide for developers
- [ ] Update README with new architecture
- [ ] Document provider pattern usage
- [ ] Document configurator pattern usage

---

## Conclusion

This analysis provides a complete roadmap for implementing clean data architecture with:

✅ **Polymorphic struct pattern** (SceneData → FbsSceneData, SaveSceneData)  
✅ **Layer stratification** (Data/Types → Logic/Providers → Orchestration)  
✅ **Abstract interfaces** (ISceneDataProvider, ISceneConfigurator, ISaveDataProvider)  
✅ **Concrete implementations** (FlatBuffers, Save, Test data sources)  
✅ **Visual diagrams** showing data flow and architecture

The migration path is incremental, testable at each phase, and maintains backward compatibility throughout.

**Next Steps**: Review with team, approve architecture, begin Phase 1 implementation.

---

**Document Status**: ✅ Complete  
**Date**: December 13, 2025  
**Related Documents**:
- `DATA_ARCHITECTURE_DIAGRAMS.md` - Visual reference
- `DATA_ARCHITECTURE_QUICK_REF.md` - Quick reference guide
- `SAVE_LOAD_WORKFLOW_ANALYSIS.md` - Existing save/load analysis
