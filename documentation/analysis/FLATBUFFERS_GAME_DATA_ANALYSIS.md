# FlatBuffers Game Data Loading Analysis

## Overview

This document analyzes the FlatBuffers schema organization and data loading patterns in SteamRot, focusing on:
1. **Schema organization** and potential redundancies
2. **Code-level data flow** - how configuration cascades through the system
3. **Identifying what works** and what could be improved
4. **Configuration vs save data** architecture (for future implementation)

---

## Code Flow Analysis

### Engine Startup Flow

The data flows naturally from `main.cpp` through a clear cascade:

```
main.cpp
    │
    └─▶ GameEngine::RunGame()
            │
            └─▶ Engine::StartUp()
                    │
                    ├─▶ FlatbuffersDataLoader::ProvideGameResourcesData()
                    │       └─▶ Loads from: engine_data.json → game_resources
                    │
                    ├─▶ resources::ConfigureGameResources()
                    │       └─▶ Configures: GameResources.game_window, framerate
                    │
                    ├─▶ FlatbuffersDataLoader::ProvideEngineData()
                    │       └─▶ Loads: subscriptions, scene_manager_data
                    │
                    └─▶ ConfigureEngineStateFromData() [virtual - not called in base]
                            └─▶ Would configure: Engine subscriptions
```

**✅ What Works:**
- Clear separation between data loading (`FlatbuffersDataLoader`) and configuration (`resources::ConfigureGameResources`)
- Engine initialization happens in one place (`StartUp()`)
- Error handling via `std::expected` propagates correctly

**⚠️ Issues Identified:**
- `ConfigureEngineStateFromData()` is pure virtual but `StartUp()` calls it - this works but the result isn't used in `Engine.cpp:44-47`
- `ProvideEngineData()` result (line 38-41) is loaded but never used in `Engine::StartUp()`!

### Scene Creation Flow

When a scene is created (e.g., via `SceneManager::AddSceneFromDefault`):

```
SceneManager::AddSceneFromDefault(SceneType)
    │
    └─▶ SceneFactory::CreateDefaultScene(SceneType, GameContext)
            │
            ├─▶ Create Scene Object (TitleScene/CraftingScene)
            │       └─▶ Passes: scene_uuid, game_context
            │
            ├─▶ FlatbuffersDataLoader::ProvideGameResourcesData()  [REDUNDANT!]
            │       └─▶ Already loaded in Engine::StartUp()
            │
            ├─▶ FlatbuffersDataLoader::ProvideSceneResourcesData(SceneType)
            │       └─▶ Loads: scene_data.json → scene_resources
            │
            ├─▶ resources::ConfigureSceneResources()
            │       └─▶ Configures: SceneResources.scene_texture
            │
            ├─▶ Scene::ConfigureFromDefault()
            │       │
            │       └─▶ EntityManager::ConfigureEntitiesFromDefaultData()
            │               │
            │               └─▶ FlatbuffersConfigurator::ConfigureEntitiesFromDefaultData()
            │                       │
            │                       ├─▶ ProvideDefaultSceneData(SceneType)
            │                       │       └─▶ Loads: scene_data.json → entity_collection
            │                       │
            │                       └─▶ ConfigureEntitiesFromCollection()
            │                               └─▶ Configures each component
            │
            ├─▶ EntityManager::GenerateAllArchetypes()
            │       └─▶ Creates: ArchetypeManager mappings
            │
            └─▶ LogicFactory::CreateLogicMap()
                    │
                    ├─▶ ProvideLogicCollectionData(SceneType)  [Optional]
                    │       └─▶ Loads: scene_data.json → logic_collection_data
                    │
                    └─▶ Creates Logic objects per scene type
```

**✅ What Works:**
- Scene creation is centralized in `SceneFactory`
- Entity configuration is delegated to `FlatbuffersConfigurator`
- Logic creation handles null data gracefully (unconfigured but functional)
- Scene type drives which data files are loaded

**⚠️ Issues Identified:**
1. **Redundant Data Loading**: `ProvideGameResourcesData()` called in both `Engine::StartUp()` AND `SceneFactory::CreateDefaultScene()` (line 72-77 in SceneFactory.cpp) - the result isn't even used!
2. **Multiple ProvideDefaultSceneData calls**: Called in `SceneFactory` for resources, then again in `FlatbuffersConfigurator` for entities - could cache.

### Data Flow During Gameplay

```
GameEngine::RunGameLoop()
    │
    └─▶ ExecuteSystemsTick() [per frame]
            │
            ├─▶ EventHandler::ExecuteEventHandlerLevelLogic()
            │       └─▶ Processes SFML events, updates EventBus
            │
            ├─▶ Engine::ExecuteEngineLevelLogic()
            │       └─▶ ProcessSubscriptions() - handles quit events
            │
            ├─▶ SceneManager::ExecuteSceneManagerLevelLogic()
            │       └─▶ ProcessSubscriptions() - handles scene changes
            │
            ├─▶ ExecuteSceneLevelLogic() [virtual]
            │       │
            │       └─▶ SceneManager::UpdateScenes()
            │               │
            │               └─▶ For each Scene:
            │                       ├─▶ sAction()
            │                       ├─▶ sCollision()
            │                       └─▶ sRender()
            │
            └─▶ ExecuteDisplayManagerTick()
                    └─▶ DisplayManager::CallRenderCycle()
```

**✅ What Works:**
- Clear tick execution order: Events → Engine → SceneManager → Scenes → Display
- Subscription-based architecture for engine/scene-level events
- Logic classes execute in defined order per scene

**✅ Natural Cascade:**
The system properly cascades from Engine → SceneManager → Scene → Logic:
- GameContext created from GameResources (owned by Engine)
- SceneContext created from SceneResources + GameResources + EntityManager
- Logic classes receive SceneContext with all necessary references

### Context Propagation

```
Engine (owns GameResources)
    │
    └─▶ GameContext(GameResources&)
            │     Contains refs to: game_window, event_handler, 
            │                        mouse_position, loop_number, asset_manager
            │
            └─▶ SceneManager receives GameContext
                    │
                    └─▶ Scene::GetSceneContext()
                            │     Creates SceneContext from:
                            │     - SceneResources (owned by Scene)
                            │     - GameResources (via GameContext)
                            │     - EntityManager (owned by Scene)
                            │
                            └─▶ Logic classes receive SceneContext
                                    Contains refs to: scene_entities, archetypes,
                                                       scene_texture, game_window,
                                                       asset_manager, event_handler,
                                                       mouse_position
```

**✅ What Works Excellently:**
- Context objects are lightweight (references only)
- Clear ownership: GameResources→Engine, SceneResources→Scene
- No circular dependencies
- Easy to trace data flow

---

## Default vs Saved Data Routes

### Current Architecture Assessment

**Question:** Is there a clear route from default and saved data? Will this system allow starting with defaults and then loading saved/cached data when needed?

### ✅ YES - The System is Well-Positioned for Default + Saved Data

The current architecture provides clear separation points that support both default and saved data routes:

```
                        ┌─────────────────────────────────────────┐
                        │            DATA SOURCE LAYER            │
                        └─────────────────────────────────────────┘
                                          │
                    ┌─────────────────────┼─────────────────────┐
                    ▼                     ▼                     ▼
          ┌─────────────────┐   ┌─────────────────┐   ┌─────────────────┐
          │ Default Data    │   │ Saved Data      │   │ Test Data       │
          │ (scene_data.json)│  │ (save_slot.bin) │   │ (test_data.bin) │
          └─────────────────┘   └─────────────────┘   └─────────────────┘
                    │                     │                     │
                    └─────────────────────┼─────────────────────┘
                                          ▼
          ┌─────────────────────────────────────────────────────────────────┐
          │                  FlatbuffersDataLoader                          │
          ├─────────────────────────────────────────────────────────────────┤
          │ ProvideDefaultSceneData(SceneType)  ← Default route             │
          │ ProvideSaveData(slot) [PROPOSED]    ← Saved route               │
          │ ProvideTestData(name) [EXISTS]      ← Test route                │
          └─────────────────────────────────────────────────────────────────┘
                                          │
                                          ▼
          ┌─────────────────────────────────────────────────────────────────┐
          │                  FlatbuffersConfigurator                        │
          ├─────────────────────────────────────────────────────────────────┤
          │ ConfigureEntitiesFromDefaultData()  ← Default route             │
          │ ConfigureEntitiesFromCollection()   ← Universal route (reusable)│
          └─────────────────────────────────────────────────────────────────┘
                                          │
                                          ▼
          ┌─────────────────────────────────────────────────────────────────┐
          │                  EntityMemoryPool                               │
          │        (Same destination regardless of data source)             │
          └─────────────────────────────────────────────────────────────────┘
```

### Key Architectural Strengths for Default + Saved Data

| Aspect | How It Supports Dual Routes |
|--------|----------------------------|
| **EntityCollection** | Already reusable - same format works for default, saved, and test data |
| **ConfigureEntitiesFromCollection()** | Universal method that takes any EntityCollection regardless of source |
| **SceneType routing** | Clean dispatch based on scene type - easily extensible |
| **FlatbuffersDataLoader** | Central data provider - just add new `ProvideSaveData()` method |
| **Test data infrastructure** | Proves the pattern works - tests already load EntityCollections from different sources |

### Current Default Data Route (Working)

```cpp
// SceneFactory.cpp - CreateDefaultScene()
Scene::ConfigureFromDefault()
    └─▶ EntityManager::ConfigureEntitiesFromDefaultData(scene_type, DataType::Flatbuffers)
            └─▶ FlatbuffersConfigurator::ConfigureEntitiesFromDefaultData(pool, scene_type)
                    └─▶ m_data_loader.ProvideDefaultSceneData(scene_type)  // Loads scene_data.json
                    └─▶ ConfigureEntitiesFromCollection(pool, entity_collection)
```

### Proposed Saved Data Route (Easy to Add)

```cpp
// Proposed: SceneFactory.cpp - CreateSceneFromSave()
Scene::ConfigureFromSaveData(save_data)
    └─▶ EntityManager::ConfigureEntitiesFromSaveData(save_data)
            └─▶ FlatbuffersConfigurator::ConfigureEntitiesFromCollection(pool, save_data->entity_collection())
                    // ✅ Reuses same ConfigureEntitiesFromCollection() - no duplication!
```

### What Already Exists That Enables This

1. **`ConfigureEntitiesFromCollection(EntityMemoryPool&, const EntityCollection*)`**
   - Location: `FlatbuffersConfigurator.h:103-105`
   - This method takes ANY EntityCollection - it doesn't care about the source
   - Both default and saved data can use this same code path

2. **`EntityCollection` schema is already save-ready**
   - Contains: `entities: [EntityData]` and `entity_memory_pool_size: int`
   - All component data is already serializable
   - Test data system proves this works

3. **Clear DataType enum in EntityManager**
   - Location: `EntityManager.h:25-28`
   - Currently just has `Flatbuffers`, but designed to be extensible
   - Could add `SavedFlatbuffers` or similar if needed

### What Would Need to be Added for Save Data

| Component | Status | What's Needed |
|-----------|--------|---------------|
| `SaveData` schema | ❌ Missing | Create `save_data.fbs` with SaveMetadata + EntityCollections |
| `ProvideSaveData()` | ❌ Missing | Add to FlatbuffersDataLoader |
| `ConfigureFromSaveData()` | ❌ Missing | Add to Scene class (calls existing `ConfigureEntitiesFromCollection`) |
| File I/O for saves | ❌ Missing | Save/load binary files to user data directory |
| UI for save/load | ❌ Missing | Menu integration |

### Conclusion

**The current system naturally supports the default + saved data pattern.** The key insight is that `ConfigureEntitiesFromCollection()` is already data-source-agnostic. You can:

1. ✅ Start with defaults: `ProvideDefaultSceneData()` → `ConfigureEntitiesFromCollection()`
2. ✅ Load saved data later: `ProvideSaveData()` [new] → `ConfigureEntitiesFromCollection()` [same method]
3. ✅ Test data also works: `ProvideTestData()` → `ConfigureEntitiesFromCollection()` [same method]

The architecture is sound for this use case - you just need to add the save data loading path.

---

## Engine Data Provisioning Options

### Current Situation

**Question:** Should Engine data routes be baked into `StartUp()`, or should data be provided externally (e.g., passed from `main()`)?

### Current Implementation

```cpp
// Engine.cpp - StartUp() bakes in the data source
std::expected<std::monostate, FailInfo> Engine::StartUp() {
  FlatbuffersDataLoader data_loader;  // Creates loader internally
  auto result = data_loader.ProvideGameResourcesData();  // Hardcoded source
  // ...
}

// GameEngine.h - Constructor takes EngineData but doesn't use it
GameEngine(const EngineData engine_data);  // Parameter is unused!

// main.cpp - Currently creates GameEngine without providing data
steamrot::GameEngine steam_rot;  // No data passed
steam_rot.RunGame();
```

### The Problem

1. **GameEngine/TestEngine must share the same route** - Both inherit `Engine::StartUp()` which internally creates `FlatbuffersDataLoader`
2. **TestEngine needs different data source** - It uses `TestDataConfig*` instead of default files
3. **GameEngine constructor takes `EngineData` but ignores it** - Dead parameter
4. **No flexibility for user configuration cache** - Can't override defaults

### Architectural Options

#### Option A: Pass Data from main() (Dependency Injection)

```cpp
// main.cpp provides the data
FlatbuffersDataLoader loader;
auto engine_data = loader.ProvideEngineData().value();
steamrot::GameEngine steam_rot(engine_data);
steam_rot.RunGame();

// Engine uses provided data (no internal loading)
class Engine {
  virtual void StartUp(const EngineData& data) = 0;  // Data comes from outside
};
```

**Pros:**
- ✅ Clear separation: main() handles data loading, Engine handles logic
- ✅ Easy to test: can inject mock data
- ✅ Flexible: main() can load from defaults, cache, or command-line override

**Cons:**
- ❌ More boilerplate in main()
- ❌ Caller must understand what data Engine needs

#### Option B: Keep StartUp() Self-Contained, Use Virtual Method for Data Source

```cpp
// Engine base class
class Engine {
protected:
  virtual const EngineData* GetEngineData() = 0;  // Derived classes provide data
  
  std::expected<std::monostate, FailInfo> StartUp() {
    const EngineData* data = GetEngineData();  // Call virtual method
    // Configure using data...
  }
};

// GameEngine - loads from default files
class GameEngine : public Engine {
  FlatbuffersDataLoader m_data_loader;
  const EngineData* GetEngineData() override {
    return m_data_loader.ProvideEngineData().value();
  }
};

// TestEngine - uses injected test config
class TestEngine : public Engine {
  TestDataConfig* m_config;
  const EngineData* GetEngineData() override {
    return m_config->starting_engine_state();  // Already doing this!
  }
};
```

**Pros:**
- ✅ StartUp() stays clean - doesn't need parameters
- ✅ Each Engine type owns its data source strategy
- ✅ Consistent with current TestEngine approach

**Cons:**
- ❌ Data loading is scattered (each derived class loads differently)
- ❌ Harder to share caching logic

#### Option C: Keep Engine Data Separate, Cache User Changes

```cpp
// GameEngine always loads from engine_data.json (static defaults)
// User preferences/cache stored separately

// Two data sources:
// 1. engine_data.json → Static game config (never changes)
// 2. user_prefs.json → User overrides (window size, volume, etc.)

class GameEngine {
  void StartUp() override {
    auto defaults = m_loader.ProvideEngineData();  // Static defaults
    auto user_prefs = m_loader.ProvideUserPrefs(); // User overrides
    MergeAndConfigure(defaults, user_prefs);       // Apply overrides
  }
};
```

**Pros:**
- ✅ Clear separation: game defaults vs user preferences
- ✅ Defaults always work (user can reset)
- ✅ User changes don't affect base game

**Cons:**
- ❌ Two data sources to manage
- ❌ Need merge logic

### Recommendation

**Option B (Virtual Method)** is the cleanest fit for the current architecture because:

1. **TestEngine already does this** - It overrides `ConfigureEngineStateFromData()` to use `m_test_config`
2. **Minimal change** - Just move data loading into a virtual method
3. **Each Engine owns its strategy** - GameEngine loads from files, TestEngine from config
4. **StartUp() stays consistent** - Same flow in base class, just different data sources

### Proposed Refactor

```cpp
// Engine.h - Add virtual data provider
class Engine {
protected:
  // NEW: Each derived class provides its data
  virtual std::expected<const EngineData*, FailInfo> ProvideEngineConfiguration() = 0;
  
  std::expected<std::monostate, FailInfo> StartUp() {
    auto engine_data_result = ProvideEngineConfiguration();  // Virtual call
    if (!engine_data_result) return std::unexpected(engine_data_result.error());
    
    // Use the data...
    const EngineData* engine_data = engine_data_result.value();
    // Configure from engine_data...
  }
};

// GameEngine.cpp - Loads from default files
std::expected<const EngineData*, FailInfo> GameEngine::ProvideEngineConfiguration() {
  FlatbuffersDataLoader loader;
  return loader.ProvideEngineData();  // Default files
}

// TestEngine.cpp - Uses injected config
std::expected<const EngineData*, FailInfo> TestEngine::ProvideEngineConfiguration() {
  return m_test_config->starting_engine_state();  // Injected test data
}
```

This matches the existing pattern where `ConfigureEngineStateFromData()` is virtual and each Engine implements its own strategy.

---

## What Works Well vs What Needs Improvement

### ✅ What Works Well

| Area | Why It Works | Code Location |
|------|--------------|---------------|
| **Context Cascade** | GameContext→SceneContext provides clean reference propagation | `GameContext.cpp`, `SceneContext.cpp` |
| **Resource/Context Separation** | Resources own objects, Contexts hold references | `GameResources.h`, `SceneResources.h` |
| **FlatbuffersDataLoader Design** | Single class for all data loading, clear method naming | `FlatbuffersDataLoader.h` |
| **Entity Configuration** | `FlatbuffersConfigurator` handles all component setup | `FlatbuffersConfigurator.cpp` |
| **Scene Type Routing** | SceneType enum drives which files load | `ProvideDefaultSceneData()` |
| **Error Propagation** | `std::expected` used consistently throughout | All loader/configurator code |
| **Tick Execution Order** | Clear sequence: Events→Engine→SceneManager→Scenes | `Engine::ExecuteSystemsTick()` |
| **Logic Factory Pattern** | Scene-specific logic configuration per SceneType | `LogicFactory::CreateLogicMap()` |

### ⚠️ What Needs Improvement

| Issue | Location | Impact | Suggested Fix |
|-------|----------|--------|---------------|
| **Unused ProvideEngineData result** | `Engine.cpp:38-41` | Dead code, wasted data load | Remove call or use the data |
| **Redundant ProvideGameResourcesData** | `SceneFactory.cpp:72-77` | Loads data already in Engine, result unused | Remove the call |
| **Multiple SceneData loads** | `SceneFactory` + `FlatbuffersConfigurator` | Same file loaded twice per scene creation | Cache the result or pass it down |
| **Duplicate GameResourcesData schemas** | `engine_data.fbs` vs `context_data.fbs` | Confusion about source of truth | Pick one source |
| **Duplicate SceneResourcesData** | `scene_data.fbs` vs `context_data.fbs` | Risk of inconsistency | Use scene_data.json only |
| **No data caching** | Throughout `FlatbuffersDataLoader` | Reloads same files multiple times | Add optional caching |

### Code Issues Identified

#### Issue 1: Dead Code in Engine::StartUp()

```cpp
// Engine.cpp lines 38-47
auto load_engine_data_result = data_loader.ProvideEngineData();
if (!load_engine_data_result) {
  return std::unexpected(load_engine_data_result.error());
}
// ❌ Result is never used! load_engine_data_result.value() goes unused
```

#### Issue 2: Unused Data Load in SceneFactory

```cpp
// SceneFactory.cpp lines 72-77
FlatbuffersDataLoader data_loader;
auto game_resources_result = data_loader.ProvideGameResourcesData();
if (!game_resources_result) {
  return std::unexpected(game_resources_result.error());
}
// ❌ game_resources_result.value() is never used!
// This data was already loaded in Engine::StartUp()
```

#### Issue 3: Duplicate Scene Data Loads

```cpp
// SceneFactory.cpp loads scene resources:
auto scene_resources_result = data_loader.ProvideSceneResourcesData(scene_type);

// Then FlatbuffersConfigurator loads the same file again:
const SceneDataData *scene_data = 
    m_data_loader.ProvideDefaultSceneData(scene_type).value();
// Note: ProvideSceneResourcesData() internally calls ProvideDefaultSceneData().
```

---

## Summary of Findings

### Key Findings

| Finding | Severity | Location | Recommendation |
|---------|----------|----------|----------------|
| **Clear separation exists for Resource vs Context data** | Good | `ResourceConfigurator`, `ContextConfigurator` | Current design is sound |
| **GameResourcesData duplicated in EngineData and ContextData** | Medium | `engine_data.fbs`, `context_data.fbs` | Consolidate or clarify purposes |
| **No distinct "Save Data" schema** | Medium | N/A | Add SaveData schema for game progress |
| **SceneResourcesData in multiple locations** | Low | `scene_data.fbs`, `context_data.fbs` | Clarify primary source of truth |
| **Test data system is well-designed and extensible** | Good | `test_data.fbs` | Use as model for save data |
| **EntityCollection supports both config and runtime** | Good | `entities.fbs` | Can be reused for save data |

---

## Current Architecture Overview

### Data Loading Layers

```
┌────────────────────────────────────────────────────────────────────────────────┐
│                          FLATBUFFERS SCHEMAS                                   │
├────────────────────────────────────────────────────────────────────────────────┤
│                                                                                │
│  ┌──────────────────────┐  ┌──────────────────────┐  ┌────────────────────┐   │
│  │   Configuration      │  │    Runtime Data      │  │    Test Data       │   │
│  │      Schemas         │  │      Schemas         │  │     Schemas        │   │
│  ├──────────────────────┤  ├──────────────────────┤  ├────────────────────┤   │
│  │ context_data.fbs     │  │ entities.fbs         │  │ test_data.fbs      │   │
│  │ resource_data.fbs    │  │ scene_data.fbs       │  │ simulation.fbs     │   │
│  │ logic_data.fbs       │  │ engine_data.fbs      │  │ event_test_data.fbs│   │
│  │ scene_manager_data.fbs│ │ event_bus_data.fbs   │  │ input_test_data.fbs│   │
│  └──────────────────────┘  └──────────────────────┘  └────────────────────┘   │
│                                                                                │
└────────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌────────────────────────────────────────────────────────────────────────────────┐
│                          DATA LOADERS                                          │
├────────────────────────────────────────────────────────────────────────────────┤
│                                                                                │
│  ┌──────────────────────────────────────────────────────────────────────────┐ │
│  │                    FlatbuffersDataLoader                                 │ │
│  │  - ProvideContextData()                                                  │ │
│  │  - ProvideEngineData()                                                   │ │
│  │  - ProvideDefaultSceneData(SceneType)                                    │ │
│  │  - ProvideGameResourcesData()                                            │ │
│  │  - ProvideSceneResourcesData(SceneType)                                  │ │
│  │  - ProvideLogicCollectionData(SceneType)                                 │ │
│  │  - ProvideAssetData()                                                    │ │
│  │  - ProvideUIStylesData()                                                 │ │
│  │  - ProvideFragment()                                                     │ │
│  │  - ProvideSceneManagerData()                                             │ │
│  └──────────────────────────────────────────────────────────────────────────┘ │
│                                                                                │
└────────────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌────────────────────────────────────────────────────────────────────────────────┐
│                          CONFIGURATORS                                         │
├────────────────────────────────────────────────────────────────────────────────┤
│                                                                                │
│  ┌─────────────────────────┐  ┌──────────────────────────┐                    │
│  │  FlatbuffersConfigurator │  │  resources_configuration │                    │
│  │  (Entity Configuration)  │  │  (Resource Configuration)│                    │
│  ├─────────────────────────┤  ├──────────────────────────┤                    │
│  │ ConfigureEntities        │  │ ConfigureGameResources   │                    │
│  │   FromDefaultData()      │  │ ConfigureSceneResources  │                    │
│  │ ConfigureEntities        │  └──────────────────────────┘                    │
│  │   FromCollection()       │                                                  │
│  └─────────────────────────┘                                                   │
│                                                                                │
└────────────────────────────────────────────────────────────────────────────────┘
```

---

## Schema Categories Analysis

### 1. Configuration Data (Static at Runtime)

**Purpose:** Data that defines the initial game setup, UI layout, resource paths, and system configuration. Typically loaded once at startup and does not change during gameplay.

**Current Schemas:**

| Schema | Purpose | Location |
|--------|---------|----------|
| `context_data.fbs` | Window size, framerate, scene pool sizes | `data/context/context_data.json` |
| `resource_data.fbs` | GameResourcesData, SceneResourcesData | Embedded in `engine_data` and `scene_data` |
| `logic_data.fbs` | Logic class subscriptions per scene | Embedded in `scene_data` |
| `scene_manager_data.fbs` | Scene manager subscriptions | `data/scene_manager/` |
| `ui_style.fbs` | UI visual styling | `data/ui_styles/` |
| `assets.fbs` | Asset collection metadata | `data/asset_manager/` |

**Data Files:**
```
data/
├── context/context_data.json           # Game and scene context config
├── game_engine/engine_data.json        # Engine-level config and resources
├── scene_manager/scene_manager_data.json # Scene manager subscriptions
├── scenes/
│   ├── title.scene_data.json           # Title scene entities and config
│   └── crafting.scene_data.json        # Crafting scene entities and config
├── asset_manager/                       # Asset metadata
└── ui_styles/                          # UI styling definitions
```

---

### 2. Default Scene Data (Initial Game State)

**Purpose:** Defines the default entity configuration for each scene when the game starts fresh. This is the "new game" state.

**Current Schemas:**

| Schema | Purpose | Contents |
|--------|---------|----------|
| `scene_data.fbs` | Root table for scene data | EntityCollection, Assets, Resources, SceneType |
| `entities.fbs` | Entity definitions | EntityData array with component data |
| `user_interface.fbs` | UI component data | Panel, Button, DropDown hierarchies |
| `grimoire_machina.fbs` | Game-specific component | Fragment references |
| `ui_state.fbs` | UI state mappings | Visibility state configurations |

**Example Flow:**
```
Scene Load (New Game)
         │
         ▼
┌────────────────────────┐
│ ProvideDefaultSceneData│
│    (SceneType)         │
└────────────────────────┘
         │
         ▼
┌────────────────────────┐
│ ConfigureEntities      │
│   FromDefaultData()    │
└────────────────────────┘
         │
         ▼
┌────────────────────────┐
│ EntityMemoryPool with  │
│ configured components  │
└────────────────────────┘
```

---

### 3. Test Data (Development/Testing)

**Purpose:** Provides data-driven test scenarios with entity states, simulation steps, inputs, and expected outcomes.

**Current Schemas:**

| Schema | Purpose | Contents |
|--------|---------|----------|
| `test_data.fbs` | Test configuration root | Metadata, entity collections, simulation data |
| `simulation.fbs` | Simulation steps | Logic classes, functions, execution order |
| `event_test_data.fbs` | Event testing | Event sequences for injection |
| `input_test_data.fbs` | Input testing | Input sequences for injection |
| `event_bus_data.fbs` | Event bus state | EventPacket snapshots |
| `engine_data.fbs` | Engine state snapshot | Full engine state for testing |

**Key Design Pattern:**
```fbs
table TestDataConfig {
  metadata: TestMetadata (required);         # Test identification
  starting_engine_state: EngineData;         # Initial state
  simulation_data: SimulationData;           # Steps to execute
  num_ticks: uint32;                         # Duration
  tick_snapshots: [TickSnapshot];            # Intermediate validation
}
```

**Strengths:**
- ✅ Extensible design with optional fields
- ✅ Metadata support for categorization
- ✅ Supports both start and expected states
- ✅ Tick-based simulation modeling
- ✅ Clean separation from production data

---

## Identified Redundancies and Gaps

### Redundancy 1: GameResourcesData Duplication

**Location:**
- `engine_data.fbs` includes `game_resources: GameResourcesData`
- `context_data.fbs` has `GameContextConfig` with overlapping fields

**Current State:**
```fbs
// resource_data.fbs
table GameResourcesData {
  window_width: uint32 = 800;
  window_height: uint32 = 600;
  window_title: string;
  framerate_limit: uint32 = 60;
}

// context_data.fbs
table GameContextConfig {
  window_width: uint32 = 800;
  window_height: uint32 = 600;
  window_title: string;
  framerate_limit: uint32 = 60;
}
```

**Data Files:**
```json
// engine_data.json
{
  "game_resources": {
    "window_width": 800,
    "window_height": 600,
    "window_title": "SteamRot",
    "framerate_limit": 60
  }
}

// context_data.json
{
  "game_context": {
    "window_width": 800,
    "window_height": 600,
    "window_title": "SteamRot",
    "framerate_limit": 60
  }
}
```

**Impact:**
- Same data defined in two places
- Risk of inconsistency if one is updated but not the other
- Unclear which is the source of truth

**Recommendation:**
Choose one as the authoritative source:
- **Option A:** Keep `context_data.json` as the primary configuration source, remove `game_resources` from `engine_data.json`
- **Option B:** Keep `engine_data.json` as the primary source (since EngineData is more comprehensive), have `ContextConfigurator` read from it

---

### Redundancy 2: SceneResourcesData Multiple Sources

**Location:**
- `scene_data.fbs` → `SceneDataData.scene_resources`
- `context_data.fbs` → `SceneContextConfig` (render_texture_width/height)

**Current State:**
```json
// title.scene_data.json
{
  "scene_resources": {
    "render_texture_width": 800,
    "render_texture_height": 600
  }
}

// context_data.json
{
  "scene_contexts": [{
    "scene_type": "TITLE",
    "render_texture_width": 800,
    "render_texture_height": 600
  }]
}
```

**Impact:**
- Same render texture dimensions in two places
- `ProvideSceneResourcesData()` loads from `scene_data.json`
- But `ContextConfigurator` might use `context_data.json`

**Recommendation:**
- Document clearly which source is authoritative
- Consider removing one source to prevent inconsistency
- Suggest keeping scene-specific resources in `scene_data.json` and removing from `context_data.json`

---

### Gap 1: No Save Data Schema

**Current State:**
- No dedicated schema for saving/loading game progress
- EntityCollection can represent entity state but lacks:
  - Player progress metadata
  - Current scene identifier
  - Game time/timestamps
  - Achievement/unlock state

**Recommendation:**
Create a new `save_data.fbs` schema:

```fbs
// Proposed: save_data.fbs
namespace steamrot;

include "entities.fbs";
include "scene_types.fbs";
include "event_bus_data.fbs";

table SaveMetadata {
  save_name: string;
  created_at: string;          // ISO 8601 timestamp
  last_modified: string;
  game_version: string;
  play_time_seconds: uint64;
}

table SaveData {
  metadata: SaveMetadata (required);
  
  current_scene_type: SceneType;
  
  // Full scene states (only populated scenes)
  scene_states: [SceneStateData];
  
  // Global game state
  global_event_bus: EventBusData;
}

table SceneStateData {
  scene_type: SceneType;
  entity_collection: EntityCollection;
}

root_type SaveData;
```

**Benefits:**
- Clear separation from default/configuration data
- Reuses existing EntityCollection schema
- Supports multiple scenes' states
- Includes metadata for save management
- Follows existing test_data.fbs patterns

---

### Gap 2: No Distinction Between "Default" and "Runtime" Entity Data

**Current State:**
- `EntityCollection` used for both:
  - Default scene data (loaded from JSON)
  - Test data (starting/expected states)
- No explicit marker distinguishing "default" vs "saved" data

**Recommendation:**
Consider adding a flag or wrapper:

```fbs
// Option: Add to EntityCollection
table EntityCollection {
  entities: [EntityData] (required);
  entity_memory_pool_size: int;
  data_source: DataSourceType;  // NEW: Default, Saved, Test
}

enum DataSourceType : byte {
  Default = 0,    // From scene_data JSON
  Saved = 1,      // From save file
  Test = 2        // From test_data
}
```

This helps debugging and ensures proper handling of different data sources.

---

## Architecture Recommendations

### 1. Configuration vs Runtime Data Flow

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                        RECOMMENDED DATA FLOW                                    │
└─────────────────────────────────────────────────────────────────────────────────┘

                    ┌─────────────────────────────────────────┐
                    │           CONFIGURATION DATA            │
                    │  (Loaded once, immutable at runtime)    │
                    ├─────────────────────────────────────────┤
                    │ • context_data.json → Window, framerate │
                    │ • ui_styles/*.json → UI themes          │
                    │ • scene_manager_data.json → Subs        │
                    └─────────────────────────────────────────┘
                                        │
                                        │ Load at startup
                                        ▼
                    ┌─────────────────────────────────────────┐
                    │          DEFAULT SCENE DATA             │
                    │  (Templates for new game/scene load)    │
                    ├─────────────────────────────────────────┤
                    │ • scenes/title.scene_data.json          │
                    │ • scenes/crafting.scene_data.json       │
                    │ • Contains default EntityCollection     │
                    └─────────────────────────────────────────┘
                                        │
                      ┌─────────────────┴─────────────────┐
                      ▼                                   ▼
        ┌───────────────────────┐          ┌───────────────────────┐
        │      NEW GAME         │          │     LOAD GAME         │
        ├───────────────────────┤          ├───────────────────────┤
        │ Load default entities │          │ Load from save_data   │
        │ from scene_data.json  │          │ Restore entity states │
        └───────────────────────┘          └───────────────────────┘
                      │                                   │
                      └─────────────────┬─────────────────┘
                                        ▼
                    ┌─────────────────────────────────────────┐
                    │           RUNTIME STATE                 │
                    │  (EntityMemoryPool, EventBus, etc.)     │
                    ├─────────────────────────────────────────┤
                    │ • Entities modified during gameplay     │
                    │ • Events processed and cleared          │
                    │ • Can be saved to save_data             │
                    └─────────────────────────────────────────┘
                                        │
                                        │ Save action
                                        ▼
                    ┌─────────────────────────────────────────┐
                    │            SAVE DATA                    │
                    │  (Serialized runtime state)             │
                    ├─────────────────────────────────────────┤
                    │ • saves/slot1.save_data.json            │
                    │ • Contains modified EntityCollection    │
                    │ • Includes save metadata                │
                    └─────────────────────────────────────────┘
```

### 2. Consolidate Resource Configuration

**Recommended Source of Truth:**

| Data Type | Primary Source | Remove From |
|-----------|----------------|-------------|
| Window/Framerate | `context_data.json` | `engine_data.json.game_resources` |
| Scene Render Texture | `scenes/*.scene_data.json` | `context_data.json.scene_contexts` |
| Entity Pool Size | `context_data.json.scene_contexts` | Keep (no duplication) |

**Updated engine_data.json:**
```json
{
  "subscriptions": [{ "event_type_data": "EVENT_QUIT_GAME" }]
  // REMOVE: game_resources (now in context_data.json)
}
```

**Updated context_data.json:**
```json
{
  "game_context": {
    "window_width": 800,
    "window_height": 600,
    "window_title": "SteamRot",
    "framerate_limit": 60
  },
  "scene_contexts": [
    {
      "scene_type": "TITLE",
      "entity_pool_size": 50
      // REMOVE: render_texture dimensions (in scene_data.json)
    }
  ]
}
```

### 3. Implement Save/Load System

**Phase 1: Schema Definition**
1. Create `save_data.fbs` with SaveData table
2. Define SaveMetadata for save management
3. Reuse EntityCollection for scene states

**Phase 2: Loader Implementation**
1. Add `ProvideSaveData(slot)` to FlatbuffersDataLoader
2. Add `SaveGameData(slot)` for serialization
3. Implement SaveManager class

**Phase 3: Integration**
1. Add save/load UI in game
2. Hook into scene transitions
3. Add autosave support

---

## Schema Relationship Diagram

```
                    ┌─────────────────────────────────┐
                    │         ROOT SCHEMAS            │
                    └─────────────────────────────────┘
                                    │
        ┌───────────────────────────┼───────────────────────────┐
        │                           │                           │
        ▼                           ▼                           ▼
┌───────────────┐         ┌─────────────────┐         ┌─────────────────┐
│ engine_data   │         │  scene_data     │         │   test_data     │
│     .fbs      │         │      .fbs       │         │      .fbs       │
├───────────────┤         ├─────────────────┤         ├─────────────────┤
│ subscriptions │         │ entity_collection│        │ metadata        │
│ scene_manager │──┐      │ assets          │         │ start_entities  │
│ event_buses   │  │      │ scene_resources │         │ expected_entities│
└───────────────┘  │      │ scene_type      │         │ simulation_data │
                   │      └─────────────────┘         │ tick_snapshots  │
                   │                │                 └─────────────────┘
                   │                │                         │
                   ▼                ▼                         ▼
           ┌─────────────────────────────────────────────────────────────┐
           │                    SHARED SCHEMAS                           │
           ├─────────────────────────────────────────────────────────────┤
           │                                                             │
           │  ┌───────────────┐  ┌─────────────┐  ┌───────────────────┐ │
           │  │ entities.fbs  │  │ events.fbs  │  │ subscriber_data   │ │
           │  ├───────────────┤  ├─────────────┤  │      .fbs         │ │
           │  │EntityData     │  │EventType    │  ├───────────────────┤ │
           │  │EntityCollection│ │EventDataData│  │SubscriberData     │ │
           │  └───────────────┘  └─────────────┘  └───────────────────┘ │
           │         │                │                   │             │
           │         ▼                ▼                   │             │
           │  ┌─────────────────────────────┐             │             │
           │  │     COMPONENT SCHEMAS       │◀────────────┘             │
           │  ├─────────────────────────────┤                           │
           │  │ user_interface.fbs          │                           │
           │  │ grimoire_machina.fbs        │                           │
           │  │ ui_state.fbs                │                           │
           │  └─────────────────────────────┘                           │
           │                                                             │
           └─────────────────────────────────────────────────────────────┘
```

---

## Recommendations Summary

### Immediate Actions (Low Risk)

1. **Document Source of Truth**
   - Update RESOURCE_CONFIGURATION.md to clarify primary sources
   - Add comments in JSON files indicating purpose

2. **Standardize Naming**
   - `game_context` vs `game_resources` - pick one naming convention
   - Document the distinction between "config" and "resource"

### Short-Term Actions (Medium Risk)

3. **Remove GameResourcesData from EngineData**
   - Keep window/framerate config only in `context_data.json`
   - Update `FlatbuffersDataLoader::ProvideGameResourcesData()` to use ContextData

4. **Remove render texture dimensions from SceneContextConfig**
   - Keep scene resource dimensions only in `scene_data.json`
   - Update ContextConfigurator accordingly

### Long-Term Actions (New Features)

5. **Create Save Data System**
   - Define `save_data.fbs` schema
   - Implement SaveManager class
   - Add serialization for runtime entity state

6. **Add Data Source Tracking**
   - Add `DataSourceType` enum to EntityCollection
   - Helps debugging and ensures proper data handling

---

## Code-Level Recommendations

### Immediate Code Fixes

#### 1. Remove Dead Code in Engine::StartUp()

**File:** `src/systems/Engine.cpp`

```cpp
// Current (lines 38-47):
auto load_engine_data_result = data_loader.ProvideEngineData();
if (!load_engine_data_result) {
  return std::unexpected(load_engine_data_result.error());
}
// Result is never used.

// FIX OPTION 1: Remove the call entirely if data isn't needed

// FIX OPTION 2: Use the data to configure Engine subscriptions
// Note: Engine already has ConfigureSubscribersFromData() method (inherited)
auto engine_data = load_engine_data_result.value();
if (engine_data->subscriptions()) {
  auto sub_result = ConfigureSubscribersFromData(engine_data->subscriptions());
  if (!sub_result) return std::unexpected(sub_result.error());
}
```

#### 2. Remove Redundant Data Load in SceneFactory

**File:** `src/scenes/SceneFactory.cpp`

```cpp
// Current (lines 72-77):
FlatbuffersDataLoader data_loader;
auto game_resources_result = data_loader.ProvideGameResourcesData();
if (!game_resources_result) {
  return std::unexpected(game_resources_result.error());
}
// ❌ Result unused and already loaded in Engine::StartUp()

// FIX: Simply remove these lines - they serve no purpose
```

#### 3. Cache Scene Data to Avoid Duplicate Loads

**File:** `src/scenes/SceneFactory.cpp`

```cpp
// Current: Scene data loaded multiple times
auto scene_resources_result = data_loader.ProvideSceneResourcesData(scene_type);
// Later, FlatbuffersConfigurator loads scene_data again

// PROPOSED FIX: Load once and pass to configurator
// This would require adding a new overload to Scene::ConfigureFromDefault()
// that accepts pre-loaded SceneDataData*

auto scene_data_result = data_loader.ProvideDefaultSceneData(scene_type);
if (!scene_data_result) {
  return std::unexpected(scene_data_result.error());
}
const SceneDataData* scene_data = scene_data_result.value();

// Use scene_data->scene_resources() for configuration
auto configure_resources_result = resources::ConfigureSceneResources(
    scene_ptr->m_scene_resources, scene_data->scene_resources());

// PROPOSED: Add new method Scene::ConfigureFromData(const SceneDataData*)
// that passes pre-loaded data to FlatbuffersConfigurator
// Currently, ConfigureFromDefault() reloads the same file internally
auto configure_result = scene_ptr->ConfigureFromData(scene_data);
```

### Data Flow Improvements

#### Option A: Pass Data Down the Chain

Instead of each layer loading its own data, pass loaded data through the configuration chain:

```
Engine::StartUp()
    │
    └─▶ Load EngineData once
            │
            └─▶ Pass to SceneManager::ConfigureFromData(EngineData)
                    │
                    └─▶ SceneFactory receives required data
                            │
                            └─▶ Scene receives its data subset
```

#### Option B: Add Data Caching to FlatbuffersDataLoader

```cpp
class FlatbuffersDataLoader {
private:
  mutable std::unordered_map<SceneType, const SceneDataData*> m_scene_data_cache;
  mutable const EngineData* m_engine_data_cache = nullptr;
  
public:
  // Returns cached data if available
  std::expected<const SceneDataData*, FailInfo>
  ProvideDefaultSceneData(const SceneType scene_type) const {
    if (m_scene_data_cache.contains(scene_type)) {
      return m_scene_data_cache[scene_type];
    }
    // Load and cache...
  }
};
```

---

## File Reference

### Schema Files
- `src/flatbuffers_headers/context_data.fbs`
- `src/flatbuffers_headers/resource_data.fbs`
- `src/flatbuffers_headers/engine_data.fbs`
- `src/flatbuffers_headers/scene_data.fbs`
- `src/flatbuffers_headers/entities.fbs`
- `src/flatbuffers_headers/test_data.fbs`

### Data Files
- `data/context/context_data.json`
- `data/game_engine/engine_data.json`
- `data/scenes/*.scene_data.json`

### Loader/Configurator Code
- `src/data_handlers/FlatbuffersDataLoader.h/cpp`
- `src/entity/FlatbuffersConfigurator.h/cpp`
- `src/resources/resources_configuration.h/cpp`

### Documentation
- `documentation/configuration/CONTEXT_CONFIGURATION.md`
- `documentation/configuration/RESOURCE_CONFIGURATION.md`
- `documentation/configuration/RESOURCE_CONTEXT_ARCHITECTURE.md`
- `documentation/testing/TEST_DATA_CONFIGURATION.md`

---

## Related Analysis Documents

- `EVENT_PACKET_CREATION_ANALYSIS.md` - Event system analysis
- `PATH_PROVIDER_NAMESPACE_ANALYSIS.md` - Path provider analysis
- `TEST_DATA_VISUALIZATION_REPORT.md` - Test data system analysis
