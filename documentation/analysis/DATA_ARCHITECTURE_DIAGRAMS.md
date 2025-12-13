# Data Architecture Diagrams

**Date**: December 13, 2025  
**Related**: DATA_ARCHITECTURE_ANALYSIS.md

---

## Overview

This document provides visual diagrams for the polymorphic struct data architecture including scene loading, save data, and configuration systems.

---

## Table of Contents

1. [Three-Layer Architecture](#three-layer-architecture)
2. [Polymorphic Struct Hierarchy](#polymorphic-struct-hierarchy)
3. [Provider Pattern](#provider-pattern)
4. [Configurator Pattern](#configurator-pattern)
5. [Default Scene Loading Flow](#default-scene-loading-flow)
6. [Save File Loading Flow](#save-file-loading-flow)
7. [Save Game Flow](#save-game-flow)
8. [Entity Configuration Flow](#entity-configuration-flow)
9. [CMake Dependency Graph](#cmake-dependency-graph)
10. [Migration Path](#migration-path)

---

## Three-Layer Architecture

### Target Architecture

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         LAYER 3: ORCHESTRATION                          │
│                     (High-level game systems)                           │
│                                                                         │
│  ┌──────────────────┐  ┌──────────────────┐  ┌──────────────────┐    │
│  │  engine_core     │  │  display_system  │  │scene_management  │    │
│  │                  │  │                  │  │                  │    │
│  │ • GameEngine     │  │ • DisplayManager │  │ • SceneManager   │    │
│  │ • Engine         │  │ • Window mgmt    │  │ • SceneFactory   │    │
│  │ • GameLoop       │  │ • Rendering      │  │ • Scene          │    │
│  │ • Tick logic     │  │                  │  │ • TitleScene     │    │
│  └──────────────────┘  └──────────────────┘  └──────────────────┘    │
│                                                                         │
│  Dependencies: Layer 1 + Layer 2                                       │
│  Can have circular deps ONLY within Layer 3 (via interfaces)          │
└─────────────────────────────────────────────────────────────────────────┘
                                    ▲
                                    │ depends on
                                    │
┌─────────────────────────────────────────────────────────────────────────┐
│                     LAYER 2: BUSINESS LOGIC                             │
│                  (Implementations & Algorithms)                         │
│                                                                         │
│  ┌─────────────────┐  ┌─────────────────┐  ┌──────────────────────┐  │
│  │   providers     │  │  configurators  │  │logic_implementations │  │
│  │                 │  │                 │  │                      │  │
│  │ • FlatBuffers   │  │ • FlatBuffers   │  │ • UIRenderLogic     │  │
│  │   SceneData     │  │   Scene         │  │ • UICollisionLogic  │  │
│  │   Provider      │  │   Configurator  │  │ • ActionLogic       │  │
│  │ • SaveSceneData │  │ • SaveScene     │  │ • MovementLogic     │  │
│  │   Provider      │  │   Configurator  │  │                      │  │
│  │ • TestSceneData │  │ • TestScene     │  │                      │  │
│  │   Provider      │  │   Configurator  │  │                      │  │
│  └─────────────────┘  └─────────────────┘  └──────────────────────┘  │
│                                                                         │
│  ┌──────────────┐                                                      │
│  │assets_system │                                                      │
│  │              │                                                      │
│  │ • AssetMgr   │                                                      │
│  │ • Font load  │                                                      │
│  │ • Texture    │                                                      │
│  └──────────────┘                                                      │
│                                                                         │
│  Dependencies: Layer 1 ONLY                                            │
│  NO dependencies on Layer 2 peers or Layer 3                           │
└─────────────────────────────────────────────────────────────────────────┘
                                    ▲
                                    │ depends on
                                    │
┌─────────────────────────────────────────────────────────────────────────┐
│                 LAYER 1: DATA, TYPES & INTERFACES                       │
│                    (Pure data structures)                               │
│                                                                         │
│  ┌─────────────┐  ┌──────────────┐  ┌────────────────┐  ┌──────────┐ │
│  │    types    │  │  interfaces  │  │data_structures │  │  events  │ │
│  │             │  │              │  │                │  │          │ │
│  │ • SceneData │  │ • ISceneData │  │ • EntityMemory │  │ • Event  │ │
│  │ • EntityData│  │   Provider   │  │   Pool         │  │   Packet │ │
│  │ • Component │  │ • IScene     │  │ • Archetype    │  │ • Event  │ │
│  │   structs   │  │   Configur   │  │   Manager      │  │   Bus    │ │
│  │ • UIElement │  │ • IEntity    │  │ • Containers   │  │          │ │
│  │   types     │  │   Configur   │  │                │  │          │ │
│  └─────────────┘  └──────────────┘  └────────────────┘  └──────────┘ │
│                                                                         │
│  Dependencies: SFML, flatbuffers (interface only), std                 │
│  ZERO dependencies on other SteamRot packages                          │
└─────────────────────────────────────────────────────────────────────────┘
```

### Layer Rules

**Layer 1 (Foundation)**:
- ✅ Can depend on: SFML, std, external libraries
- ❌ Cannot depend on: Any Layer 2 or Layer 3 packages
- Contains: Pure data structures, interfaces, type definitions

**Layer 2 (Logic)**:
- ✅ Can depend on: Layer 1, SFML, std
- ❌ Cannot depend on: Layer 3, other Layer 2 peers
- Contains: Implementations, algorithms, providers, configurators

**Layer 3 (Orchestration)**:
- ✅ Can depend on: Layer 1, Layer 2, SFML, std
- ⚠️ Can have circular dependencies ONLY within Layer 3 (resolve via interfaces)
- Contains: High-level systems, game loop, managers

---

## Polymorphic Struct Hierarchy

### SceneData Inheritance

```
┌────────────────────────────────────────────────────────────────────┐
│                       SceneData (Abstract Base)                    │
│                                                                    │
│  struct SceneData {                                                │
│    SceneInfo scene_info;  // Common to all sources                │
│  };                                                                │
│                                                                    │
│  Purpose: Base type for polymorphism                              │
│  Location: src/types/core/SceneData.h                             │
└────────────────────────────────────────────────────────────────────┘
                               │
                               │ inherits
                ┌──────────────┴──────────────┬─────────────────────┐
                │                             │                     │
                ▼                             ▼                     ▼
┌──────────────────────────┐  ┌──────────────────────┐  ┌─────────────────────┐
│  FbsSceneData            │  │  SaveSceneData       │  │  TestSceneData      │
│  (Default/Initial)       │  │  (Saved Games)       │  │  (Unit Tests)       │
│                          │  │                      │  │                     │
│  struct FbsSceneData :   │  │  struct SaveScene    │  │  struct TestScene   │
│    public SceneData {    │  │    Data : public     │  │    Data : public    │
│                          │  │    SceneData {       │  │    SceneData {      │
│    const SceneDataFbs    │  │                      │  │                     │
│      *scene_data_fbs;    │  │    const Saved       │  │    const TestScene  │
│  };                      │  │      SceneDataFbs    │  │      DataFbs        │
│                          │  │      *saved_scene    │  │      *test_scene    │
│  Location:               │  │      _data_fbs;      │  │      _data_fbs;     │
│    src/types/core/       │  │                      │  │                     │
│    FbsSceneData.h        │  │    uint64_t play_    │  │    std::string      │
│                          │  │      time_seconds;   │  │      test_name;     │
│  Usage:                  │  │                      │  │                     │
│    - New game            │  │    std::string       │  │  Location:          │
│    - Default scenes      │  │      last_modified;  │  │    src/types/core/  │
│    - Level templates     │  │  };                  │  │    TestSceneData.h  │
│                          │  │                      │  │                     │
│                          │  │  Location:           │  │  Usage:             │
│                          │  │    src/types/core/   │  │    - Unit tests     │
│                          │  │    SaveSceneData.h   │  │    - Test fixtures  │
│                          │  │                      │  │    - Mocking        │
│                          │  │  Usage:              │  │                     │
│                          │  │    - Load saved game │  │                     │
│                          │  │    - Continue game   │  │                     │
└──────────────────────────┘  └──────────────────────┘  └─────────────────────┘
```

### Usage Pattern

```cpp
// SceneFactory doesn't need to know concrete type
std::unique_ptr<Scene> CreateScene(SceneType type, 
                                   ISceneDataProvider *provider,
                                   ISceneConfigurator *configurator) {
  
  // Get polymorphic SceneData (could be any derived type)
  std::unique_ptr<SceneData> data = provider->ProvideData(type);
  
  // Create scene
  std::unique_ptr<Scene> scene = CreateEmptyScene(type);
  
  // Configure (uses dynamic_cast internally)
  configurator->ConfigureScene(*scene, data.get());
  
  return scene;
}
```

---

## Provider Pattern

### Provider Interface Hierarchy

```
┌──────────────────────────────────────────────────────────────────────────┐
│                    ISceneDataProvider (Interface)                        │
│                                                                          │
│  class ISceneDataProvider {                                             │
│  public:                                                                 │
│    virtual std::unique_ptr<SceneData>                                   │
│      ProvideDefaultSceneData(const SceneType) const = 0;               │
│                                                                          │
│    virtual std::unique_ptr<SceneData>                                   │
│      ProvideSceneDataFromSave(const SaveData&,                         │
│                              const SceneType) const = 0;                │
│                                                                          │
│    virtual std::unique_ptr<SceneData>                                   │
│      ProvideTestSceneData(const std::string&) const = 0;               │
│  };                                                                      │
│                                                                          │
│  Location: src/interfaces/ISceneDataProvider.h                          │
└──────────────────────────────────────────────────────────────────────────┘
                                    │
                                    │ implements
                    ┌───────────────┴────────────┬────────────────────┐
                    ▼                            ▼                    ▼
┌──────────────────────────────┐  ┌─────────────────────┐  ┌──────────────────┐
│ FlatbuffersSceneDataProvider │  │ SaveSceneData       │  │ TestSceneData    │
│                              │  │   Provider          │  │   Provider       │
│ Provides default scene data  │  │                     │  │                  │
│ from .bin files              │  │ Provides saved      │  │ Provides test    │
│                              │  │ scene data from     │  │ scene data       │
│ Returns: FbsSceneData        │  │ .save files         │  │                  │
│                              │  │                     │  │ Returns:         │
│ Implementation:              │  │ Returns:            │  │   TestSceneData  │
│  1. Load SceneDataFbs        │  │   SaveSceneData     │  │                  │
│  2. Create FbsSceneData      │  │                     │  │ Implementation:  │
│  3. Set scene_data_fbs ptr   │  │ Implementation:     │  │  1. Load test    │
│  4. Return as SceneData*     │  │  1. Load SaveData   │  │     schema       │
│                              │  │  2. Extract scene   │  │  2. Create       │
│ Location:                    │  │     FlatBuffers     │  │     TestScene    │
│   src/providers/             │  │  3. Create          │  │     Data         │
│   Flatbuffers                │  │     SaveSceneData   │  │  3. Return as    │
│   SceneDataProvider.h        │  │  4. Set UUID, time  │  │     SceneData*   │
│                              │  │  5. Return as       │  │                  │
│ Status: ✅ Exists            │  │     SceneData*      │  │ Location:        │
│         (needs refactor)     │  │                     │  │   src/providers/ │
│                              │  │ Location:           │  │   TestSceneData  │
│                              │  │   src/providers/    │  │   Provider.h     │
│                              │  │   SaveSceneData     │  │                  │
│                              │  │   Provider.h        │  │ Status:          │
│                              │  │                     │  │   ❌ To create   │
│                              │  │ Status:             │  │                  │
│                              │  │   ❌ To create      │  │                  │
└──────────────────────────────┘  └─────────────────────┘  └──────────────────┘
```

### Provider Responsibilities

```
┌─────────────────────────────────────────────────────────────────┐
│                    PROVIDER RESPONSIBILITIES                    │
└─────────────────────────────────────────────────────────────────┘

1. DATA LOADING
   ├─ Load raw data from storage (files, network, memory)
   ├─ Parse FlatBuffers binary data
   └─ Handle file I/O errors

2. DATA CONVERSION
   ├─ Convert FlatBuffers types → Native C++ structs
   ├─ Extract simple fields (strings, ints, floats)
   └─ Create polymorphic data structs (FbsSceneData, SaveSceneData)

3. MEMORY MANAGEMENT
   ├─ Own FlatBuffers buffer lifetime
   ├─ Ensure buffer outlives views/pointers
   └─ Return smart pointers (std::unique_ptr<SceneData>)

4. ERROR HANDLING
   ├─ Return std::expected<T, FailInfo>
   ├─ Provide detailed error messages
   └─ Handle missing files gracefully

5. ABSTRACTION
   ├─ Hide FlatBuffers implementation details
   ├─ Expose clean interface (ISceneDataProvider)
   └─ Return polymorphic base pointers

❌ NOT RESPONSIBLE FOR:
   ├─ Configuring game objects (that's configurator's job)
   ├─ Business logic
   └─ Object creation (beyond data structs)
```

---

## Configurator Pattern

### Configurator Interface Hierarchy

```
┌──────────────────────────────────────────────────────────────────────────┐
│                   ISceneConfigurator (Interface)                         │
│                                                                          │
│  class ISceneConfigurator {                                             │
│  public:                                                                 │
│    virtual std::expected<std::monostate, FailInfo>                      │
│      ConfigureScene(Scene &scene, const SceneData *data) = 0;          │
│                                                                          │
│    virtual std::expected<std::monostate, FailInfo>                      │
│      ConfigureSceneInfo(Scene &scene, const SceneData *data) = 0;      │
│                                                                          │
│    virtual std::expected<std::monostate, FailInfo>                      │
│      ConfigureSceneResources(Scene &scene,                             │
│                             const SceneData *data) = 0;                │
│                                                                          │
│    virtual std::expected<std::monostate, FailInfo>                      │
│      ConfigureLogicMap(Scene &scene) = 0;                              │
│  };                                                                      │
│                                                                          │
│  Location: src/interfaces/ISceneConfigurator.h                          │
└──────────────────────────────────────────────────────────────────────────┘
                                    │
                                    │ implements
                    ┌───────────────┴────────────┬────────────────────┐
                    ▼                            ▼                    ▼
┌──────────────────────────────┐  ┌─────────────────────┐  ┌──────────────────┐
│ FlatbuffersScene             │  │ SaveScene           │  │ TestScene        │
│   Configurator               │  │   Configurator      │  │   Configurator   │
│                              │  │                     │  │                  │
│ Configures Scene from        │  │ Configures Scene    │  │ Configures Scene │
│ default FlatBuffers data     │  │ from saved data     │  │ from test data   │
│                              │  │                     │  │                  │
│ Expects: FbsSceneData*       │  │ Expects:            │  │ Expects:         │
│                              │  │   SaveSceneData*    │  │   TestSceneData* │
│ Implementation:              │  │                     │  │                  │
│  1. dynamic_cast to          │  │ Implementation:     │  │ Implementation:  │
│     FbsSceneData*            │  │  1. dynamic_cast to │  │  1. dynamic_cast │
│  2. Extract SceneDataFbs*    │  │     SaveSceneData*  │  │  2. Extract test │
│  3. Configure SceneInfo      │  │  2. Extract Saved   │  │     data         │
│  4. Configure Entity pool    │  │     SceneDataFbs*   │  │  3. Configure    │
│  5. Configure Logic map      │  │  3. Restore UUID    │  │     for test     │
│                              │  │  4. Restore entity  │  │                  │
│ Location:                    │  │     states          │  │ Location:        │
│   src/configurators/         │  │  5. Recreate logic  │  │   src/           │
│   FlatbuffersScene           │  │     map             │  │   configurators/ │
│   Configurator.h             │  │                     │  │   TestScene      │
│                              │  │ Location:           │  │   Configurator.h │
│ Status: ✅ Exists            │  │   src/              │  │                  │
│         (needs refactor      │  │   configurators/    │  │ Status:          │
│         to accept data*)     │  │   SaveScene         │  │   ❌ To create   │
│                              │  │   Configurator.h    │  │                  │
│                              │  │                     │  │                  │
│                              │  │ Status:             │  │                  │
│                              │  │   ❌ To create      │  │                  │
└──────────────────────────────┘  └─────────────────────┘  └──────────────────┘
```

### Configurator Responsibilities

```
┌─────────────────────────────────────────────────────────────────┐
│                 CONFIGURATOR RESPONSIBILITIES                   │
└─────────────────────────────────────────────────────────────────┘

1. TYPE CHECKING
   ├─ Receive polymorphic SceneData* base pointer
   ├─ dynamic_cast to expected concrete type
   └─ Return error if wrong type

2. OBJECT CONFIGURATION
   ├─ Apply data to game objects (Scene, EntityMemoryPool)
   ├─ Set member variables
   ├─ Configure component values
   └─ Initialize object state

3. DEPENDENCY INJECTION
   ├─ Receive configurators for nested objects (IEntityConfigurator)
   ├─ Delegate configuration to specialized configurators
   └─ Coordinate configuration order

4. VALIDATION
   ├─ Validate data before applying
   ├─ Check for null pointers
   ├─ Verify required fields exist
   └─ Return std::expected<std::monostate, FailInfo>

5. STATIC LOGIC SETUP
   ├─ Configure LogicMap (not data-driven)
   ├─ Create Logic instances
   └─ Set up static relationships

❌ NOT RESPONSIBLE FOR:
   ├─ Loading data (that's provider's job)
   ├─ Owning data buffers
   └─ File I/O
```

---

## Default Scene Loading Flow

### Current State (Has Issues)

```
┌──────────────────────────────────────────────────────────────────┐
│                     CURRENT STATE (PROBLEMS)                     │
└──────────────────────────────────────────────────────────────────┘

SceneFactory::CreateScene(SceneType)
    │
    ├─ Creates empty Scene
    │
    ▼
FlatbuffersDefaultSceneConfigurator::ConfigureScene(Scene&)
    │
    ├─ Problem: Configurator has FlatbuffersDataLoader member
    ├─ Problem: Configurator loads data (violates SRP)
    ├─ Problem: Can't swap data sources
    │
    ▼
FlatbuffersDataLoader::ProvideDefaultSceneData()
    │
    ├─ Loads SceneDataFbs from .bin file
    ├─ Returns const SceneDataFbs*
    │
    ▼
FlatbuffersDefaultSceneConfigurator::ApplyData(Scene&)
    │
    ├─ Directly uses FlatBuffers types
    ├─ Configures Scene
    │
    ▼
Scene configured ✅

❌ ISSUES:
  - Configurator loads data (tight coupling)
  - Can't swap data sources without changing configurator
  - Hard to test (need real .bin files)
  - Violates Single Responsibility Principle
```

### Target State (Clean)

```
┌──────────────────────────────────────────────────────────────────┐
│                      TARGET STATE (CLEAN)                        │
└──────────────────────────────────────────────────────────────────┘

SceneFactory::CreateScene(SceneType)
    │
    ├─ Get ISceneDataProvider (via factory)
    ├─ Get ISceneConfigurator (via factory)
    │
    ▼
ISceneDataProvider::ProvideDefaultSceneData(SceneType)
    │
    ├─ Implementation: FlatbuffersSceneDataProvider
    │
    ▼
FlatbuffersSceneDataProvider
    │
    ├─ 1. Load SceneDataFbs from .bin file
    ├─ 2. Create FbsSceneData (polymorphic)
    ├─ 3. Set scene_data_fbs pointer
    ├─ 4. Return std::unique_ptr<SceneData>
    │
    ▼
SceneFactory receives SceneData* (base pointer)
    │
    ├─ Creates empty Scene
    │
    ▼
ISceneConfigurator::ConfigureScene(Scene&, SceneData*)
    │
    ├─ Implementation: FlatbuffersSceneConfigurator
    │
    ▼
FlatbuffersSceneConfigurator
    │
    ├─ 1. dynamic_cast<FbsSceneData*>(data)
    ├─ 2. Validate cast succeeded
    ├─ 3. Extract const SceneDataFbs* from FbsSceneData
    ├─ 4. Configure Scene from FlatBuffers
    │   ├─ ConfigureSceneInfo()
    │   ├─ ConfigureSceneResources()
    │   │   ├─ EntityMemoryPool configuration
    │   │   ├─ LogicMap configuration
    │   ├─ ConfigureSceneConfig()
    │
    ▼
Scene configured ✅

✅ BENEFITS:
  - Provider loads data (SRP)
  - Configurator receives data (SRP)
  - Easy to swap data sources
  - Easy to test (mock providers)
  - Clean separation of concerns
```

---

## Save File Loading Flow

```
┌──────────────────────────────────────────────────────────────────┐
│                    SAVE FILE LOADING FLOW                        │
└──────────────────────────────────────────────────────────────────┘

User clicks "Load Game" (slot 2)
    │
    ▼
ISaveDataProvider::LoadSave(2)
    │
    ├─ Implementation: FlatbuffersSaveDataProvider
    │
    ▼
FlatbuffersSaveDataProvider
    │
    ├─ 1. Load save_slot_2.save (FlatBuffers binary)
    ├─ 2. Parse SaveDataFbs
    ├─ 3. Create SaveData struct
    ├─ 4. Extract metadata (name, time, version)
    ├─ 5. Extract current_scene_type
    ├─ 6. Return std::expected<SaveData, FailInfo>
    │
    ▼
SceneFactory::CreateScene(SceneType, DataSource::SaveFile, &SaveData)
    │
    ├─ Get SaveSceneDataProvider
    ├─ Get SaveSceneConfigurator
    │
    ▼
SaveSceneDataProvider::ProvideSceneDataFromSave(SaveData, SceneType)
    │
    ├─ 1. Extract scene UUID from SaveData
    ├─ 2. Load SavedSceneDataFbs for this scene
    ├─ 3. Create SaveSceneData (polymorphic)
    ├─ 4. Set saved_scene_data_fbs pointer
    ├─ 5. Set play_time_seconds
    ├─ 6. Set last_modified
    ├─ 7. Return std::unique_ptr<SceneData>
    │
    ▼
SceneFactory receives SceneData* (base pointer)
    │
    ├─ Creates empty Scene
    │
    ▼
SaveSceneConfigurator::ConfigureScene(Scene&, SceneData*)
    │
    ├─ 1. dynamic_cast<SaveSceneData*>(data)
    ├─ 2. Validate cast succeeded
    ├─ 3. Extract const SavedSceneDataFbs* from SaveSceneData
    ├─ 4. Restore Scene from saved data
    │   ├─ Restore Scene UUID (important!)
    │   ├─ Restore SceneType
    │   │
    │   ├─ Restore EntityMemoryPool
    │   │   ├─ For each saved entity:
    │   │   │   ├─ Restore entity index
    │   │   │   ├─ Restore component states
    │   │   │   ├─ Preserve archetypes
    │   │   │
    │   ├─ Recreate LogicMap (static - not saved)
    │
    ▼
Scene restored to saved state ✅

NOTES:
  - Scene UUID preserved (essential for save files)
  - Entity indices preserved (for references)
  - Component states fully restored
  - Logic recreated (not serialized)
```

---

## Save Game Flow

```
┌──────────────────────────────────────────────────────────────────┐
│                       SAVE GAME FLOW                             │
└──────────────────────────────────────────────────────────────────┘

User clicks "Save Game" (slot 1)
    │
    ▼
SaveManager::CaptureGameState()
    │
    ├─ 1. Create SaveData struct
    ├─ 2. Fill metadata
    │   ├─ save_name = "Player Save 1"
    │   ├─ created_at = current timestamp
    │   ├─ last_modified = current timestamp
    │   ├─ play_time_seconds = accumulated play time
    │   ├─ slot_index = 1
    │   ├─ game_version = "0.1.0"
    │
    ├─ 3. Get current_scene_type from SceneManager
    │
    ├─ 4. Capture current scene state
    │
    ▼
SceneSerializer::CaptureSceneState(Scene&)
    │
    ├─ 1. Capture Scene UUID
    ├─ 2. Capture SceneType
    ├─ 3. Capture EntityMemoryPool state
    │
    ▼
EntitySerializer::SerializeEntityPool(EntityMemoryPool&)
    │
    ├─ 1. Iterate all active entities
    │   │
    │   ├─ For each entity:
    │   │   ├─ Capture entity index
    │   │   ├─ Capture active components
    │   │   │   │
    │   │   │   ├─ If CUserInterface active:
    │   │   │   │   ├─ Serialize ui_name
    │   │   │   │   ├─ Serialize visibility
    │   │   │   │   ├─ Serialize UIElement tree
    │   │   │   │
    │   │   │   ├─ If CGrimoireMachina active:
    │   │   │   │   ├─ Serialize state
    │   │   │   │   ├─ Serialize data
    │   │   │   │
    │   │   │   └─ ... other components
    │   │   │
    │   │   ├─ Create SavedEntityData (FlatBuffers)
    │
    ├─ 2. Create SavedEntityCollection (FlatBuffers)
    │
    ▼
SceneSerializer creates SavedSceneDataFbs
    │
    ├─ Contains:
    │   ├─ scene_uuid
    │   ├─ scene_type
    │   ├─ entity_collection (all entities)
    │   ├─ timestamp
    │
    ▼
SaveManager::SaveGameState(SaveData)
    │
    ▼
ISaveDataProvider::SaveGame(SaveData)
    │
    ├─ Implementation: FlatbuffersSaveDataProvider
    │
    ▼
FlatbuffersSaveDataProvider::SaveGame(SaveData)
    │
    ├─ 1. Create FlatBuffers builder
    ├─ 2. Build SaveMetadata
    ├─ 3. Build SavedSceneDataFbs for current scene
    ├─ 4. Build SaveDataFbs (root)
    ├─ 5. Finish builder
    ├─ 6. Write to save_slot_1.save
    │
    ▼
Save complete ✅

NOTES:
  - Entity indices preserved (for stability)
  - Only active components serialized
  - UIElement trees fully serialized (polymorphic)
  - Logic NOT serialized (recreated on load)
  - Scene UUID saved (essential for references)
```

---

## Entity Configuration Flow

```
┌──────────────────────────────────────────────────────────────────┐
│              ENTITY CONFIGURATION WITH POLYMORPHIC DATA          │
└──────────────────────────────────────────────────────────────────┘

Scene has SceneData* (polymorphic - could be any derived type)
    │
    ▼
IEntityConfigurator::ConfigureEntityMemoryPool(EMP&, SceneData*)
    │
    ├─ Check SceneData type via dynamic_cast
    │
    ├─────────────┬─────────────────────┬───────────────────┐
    │             │                     │                   │
    ▼             ▼                     ▼                   ▼
FbsSceneData  SaveSceneData      TestSceneData      (Future types)
    │             │                     │
    │             │                     │
    ▼             ▼                     ▼
┌─────────────────────────────────────────────────────────────────┐
│         FlatbuffersEntityConfigurator                           │
│                                                                 │
│  1. Extract EntityCollection from FlatBuffers                  │
│  2. For each entity in collection:                             │
│     ├─ Create entity at specified index                        │
│     ├─ Configure first-layer components                        │
│     │   ├─ CMeta                                               │
│     │   ├─ CUserInterface (if present)                         │
│     │   ├─ CGrimoireMachina (if present)                       │
│     │                                                           │
│     ├─ Configure second-layer components                       │
│     │   ├─ CUIState (depends on CUserInterface)                │
│     │   ├─ CMachinaForm (depends on CGrimoireMachina)          │
│     │                                                           │
│     └─ Set default values for components                       │
│                                                                 │
│  3. Validate archetypes                                        │
│  4. Update ArchetypeManager                                    │
└─────────────────────────────────────────────────────────────────┘
    │
    ▼
┌─────────────────────────────────────────────────────────────────┐
│            SaveEntityConfigurator (for saved data)              │
│                                                                 │
│  1. Extract SavedEntityCollection from save file FlatBuffers   │
│  2. For each saved entity:                                     │
│     ├─ Restore entity at SAME index as saved                   │
│     ├─ Restore first-layer components                          │
│     │   ├─ Restore saved values (not defaults)                 │
│     │   ├─ Preserve entity UUID                                │
│     │                                                           │
│     ├─ Restore second-layer components                         │
│     │   ├─ Restore CUIState (preserved state)                  │
│     │   ├─ Restore relationships                               │
│     │                                                           │
│     └─ Restore component state (active/inactive)               │
│                                                                 │
│  3. Rebuild archetypes from saved entities                     │
│  4. Update ArchetypeManager                                    │
└─────────────────────────────────────────────────────────────────┘
    │
    ▼
┌─────────────────────────────────────────────────────────────────┐
│          TestEntityConfigurator (for test data)                 │
│                                                                 │
│  1. Extract test entity configuration                          │
│  2. Create minimal entity setup for testing                    │
│  3. Set specific test conditions                               │
│  4. Inject test doubles/mocks if needed                        │
└─────────────────────────────────────────────────────────────────┘
    │
    ▼
EntityMemoryPool fully configured ✅

KEY DIFFERENCES:
  - FlatBuffers: Sets DEFAULT values
  - Save: Restores SAVED values, preserves indices
  - Test: Creates MINIMAL setup for testing
```

---

## CMake Dependency Graph

### Current State (Circular Dependencies)

```
┌────────────────────────────────────────────────────────────────┐
│                   CURRENT CMAKE DEPENDENCIES                   │
│                      (HAS CIRCULAR DEPS)                       │
└────────────────────────────────────────────────────────────────┘

Layer 3:
  ┌──────────┐
  │  engine  │─────┐
  └──────────┘     │
       │           │
       ├───────────┼────────────┐
       │           │            │
       ▼           ▼            ▼
  ┌─────────┐ ┌─────────┐ ┌────────────────┐
  │ display │ │ scenes  │ │ data_providers │
  └─────────┘ └─────────┘ └────────────────┘
       │           │            │
       └───────────┴────────────┘
              ▲   │   ▲
              │   │   │
              └───┴───┘  ❌ CIRCULAR!

  display → scenes → display  ❌
  scenes → data_providers → scenes  ❌


Layer 2:
  ┌─────────┐
  │  logic  │
  └─────────┘
       │
       ├──────────┬─────────────┐
       ▼          ▼             ▼
  ┌────────┐ ┌────────┐  ┌───────────────┐
  │ entity │ │ engine │  │user_interface │
  └────────┘ └────────┘  └───────────────┘
       │                       │
       └───────────────────────┘
                  │
                  ▼
             ┌────────┐
             │ engine │  ❌ CIRCULAR!
             └────────┘

  logic → engine → logic  ❌
  user_interface → engine  ❌ (heavy coupling)


Layer 1:
  ┌──────────┐
  │  types   │ (Good - no circular deps)
  └──────────┘
       │
       ▼
  ┌──────────┐
  │  events  │
  └──────────┘
```

### Target State (Clean Layers)

```
┌────────────────────────────────────────────────────────────────┐
│                    TARGET CMAKE DEPENDENCIES                   │
│                      (NO CIRCULAR DEPS)                        │
└────────────────────────────────────────────────────────────────┘

Layer 3 (Orchestration):
  ┌─────────────┐   ┌──────────────┐   ┌──────────────────┐
  │ engine_core │   │display_system│   │scene_management  │
  └─────────────┘   └──────────────┘   └──────────────────┘
        │                  │                     │
        │                  │                     │
        └──────────────────┴─────────────────────┘
                          │ (only downward deps)
                          ▼
  ┌────────────────────────────────────────────────────────┐
  │                    Layer 2                             │
  └────────────────────────────────────────────────────────┘

Layer 2 (Business Logic):
  ┌───────────┐   ┌───────────────┐   ┌────────────────────┐
  │ providers │   │ configurators │   │logic_implementations│
  └───────────┘   └───────────────┘   └────────────────────┘
        │                  │                     │
        │                  │                     │
        └──────────────────┴─────────────────────┘
                          │ (only downward deps)
                          ▼
  ┌────────────────────────────────────────────────────────┐
  │                    Layer 1                             │
  └────────────────────────────────────────────────────────┘

Layer 1 (Data & Interfaces):
  ┌──────────┐   ┌────────────┐   ┌────────────────┐   ┌────────┐
  │  types   │   │ interfaces │   │data_structures │   │ events │
  └──────────┘   └────────────┘   └────────────────┘   └────────┘
        │              │                  │                  │
        └──────────────┴──────────────────┴──────────────────┘
                          │ (no deps on other packages)
                          ▼
                ┌──────────────────────┐
                │  External libraries  │
                │  (SFML, flatbuffers) │
                └──────────────────────┘

✅ NO CIRCULAR DEPENDENCIES
✅ CLEAN LAYER SEPARATION
✅ DEPENDENCIES FLOW DOWNWARD ONLY
```

---

## Migration Path

### Visualization of Migration Phases

```
┌──────────────────────────────────────────────────────────────────────────┐
│                         MIGRATION TIMELINE                               │
└──────────────────────────────────────────────────────────────────────────┘

Phase 1: Extract Interfaces (Week 1)
┌────────────────────────────────────────────────────────┐
│ Create src/interfaces/                                 │
│ └─ ISceneDataProvider.h                               │
│ └─ ISceneConfigurator.h                               │
│ └─ IEntityConfigurator.h                              │
│ └─ ISaveDataProvider.h                                │
│                                                        │
│ Result: interfaces package with ZERO circular deps    │
└────────────────────────────────────────────────────────┘
                         │
                         ▼
Phase 2: Refactor Configurators (Week 2)
┌────────────────────────────────────────────────────────┐
│ Change ISceneConfigurator signature:                  │
│   ConfigureScene(Scene&, SceneData*)                  │
│                                                        │
│ Update FlatbuffersDefaultSceneConfigurator:           │
│   - Remove FlatbuffersDataLoader member               │
│   - Accept SceneData* parameter                       │
│   - Downcast to FbsSceneData*                         │
│                                                        │
│ Update SceneFactory:                                  │
│   - Get provider separately                           │
│   - Provider loads data                               │
│   - Configurator receives data                        │
│                                                        │
│ Result: Provider/Configurator separation complete     │
└────────────────────────────────────────────────────────┘
                         │
                         ▼
Phase 3: Implement Save Infrastructure (Weeks 3-4)
┌────────────────────────────────────────────────────────┐
│ Create SaveSceneData struct                           │
│                                                        │
│ Implement SaveSceneDataProvider                       │
│                                                        │
│ Implement SaveSceneConfigurator                       │
│                                                        │
│ Implement EntitySerializer                            │
│   - SerializeEntityPool()                             │
│   - Save to FlatBuffers format                        │
│                                                        │
│ Update ISaveDataProvider                              │
│   - Add scene state capture methods                   │
│                                                        │
│ Test save/load round-trip                             │
│                                                        │
│ Result: Complete save/load workflow                   │
└────────────────────────────────────────────────────────┘
                         │
                         ▼
Phase 4: Break Circular Dependencies (Weeks 5-6)
┌────────────────────────────────────────────────────────┐
│ Move SceneType enum → types package                   │
│                                                        │
│ Ensure data_providers only depends on:               │
│   - types                                             │
│   - interfaces                                        │
│                                                        │
│ Move scene types → types/scene/                       │
│                                                        │
│ Use linker flag: -Wl,--no-undefined                   │
│   (catches circular deps at link time)                │
│                                                        │
│ Result: No circular dependencies                      │
└────────────────────────────────────────────────────────┘
                         │
                         ▼
Phase 5: Reorganize Packages (Weeks 7-8)
┌────────────────────────────────────────────────────────┐
│ Rename packages:                                       │
│   data_providers → providers                          │
│   scenes → scene_management                           │
│   engine → engine_core                                │
│   display → display_system                            │
│                                                        │
│ Create configurators package:                         │
│   Extract from scenes/ and entity/                    │
│                                                        │
│ Create logic_implementations package:                 │
│   Rename from logic                                   │
│                                                        │
│ Update all CMakeLists.txt                             │
│                                                        │
│ Result: Clean three-layer architecture                │
└────────────────────────────────────────────────────────┘
```

### Before and After Comparison

```
BEFORE:                              AFTER:

┌─────────────┐                      ┌──────────────────┐
│   scenes    │                      │scene_management  │
│ ┌─────────┐ │                      │  (Layer 3)       │
│ │ Config  │ │                      └──────────────────┘
│ │Loader   │ │                               │
│ └─────────┘ │                               │ uses
│      │      │                               ▼
│      └──────┼────┐                 ┌──────────────────┐
│             │    │                 │  configurators   │
└─────────────┘    │                 │  (Layer 2)       │
      ▲            │                 └──────────────────┘
      │            │                          │
      │    ┌───────▼─────┐                   │ uses
      │    │data_providers│                   ▼
      └────│             │          ┌──────────────────┐
           └─────────────┘          │    providers     │
                 ▲                  │   (Layer 2)      │
                 │                  └──────────────────┘
                 │                           │
         ❌ CIRCULAR!                        │ uses
                                             ▼
                                    ┌──────────────────┐
                                    │   interfaces     │
                                    │   (Layer 1)      │
                                    └──────────────────┘
                                             │
                                             │ uses
                                             ▼
                                    ┌──────────────────┐
                                    │      types       │
                                    │   (Layer 1)      │
                                    └──────────────────┘
                                    
                                    ✅ NO CIRCULAR DEPS!
```

---

## Summary

This document provides comprehensive visual diagrams for the polymorphic struct data architecture. Key takeaways:

1. **Three-layer architecture** eliminates circular dependencies
2. **Polymorphic struct pattern** (SceneData → FbsSceneData, SaveSceneData) enables multiple data sources
3. **Provider pattern** isolates data loading from business logic
4. **Configurator pattern** applies data to game objects in type-safe manner
5. **Migration path** is incremental and testable at each phase

**Next Steps**:
1. Review diagrams with team
2. Approve architecture design
3. Begin Phase 1 implementation (extract interfaces)

---

**Document Status**: ✅ Complete  
**Date**: December 13, 2025  
**Related**: DATA_ARCHITECTURE_ANALYSIS.md, DATA_ARCHITECTURE_QUICK_REF.md
