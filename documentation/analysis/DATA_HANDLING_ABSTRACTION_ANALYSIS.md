# Data Handling Abstraction Analysis

## Executive Summary

This document analyzes the current data handling architecture in SteamRot and addresses concerns about maintaining proper abstraction between data sources, configuration strategies, and object creation patterns. It provides recommendations for clarifying responsibilities and improving the architecture.

### Key Findings

1. **Current Architecture is Mostly Sound**: The existing provider pattern and configurator strategy pattern work well together
2. **Responsibility Confusion**: SceneFactory vs SceneConfigurator responsibilities need clarification
3. **Data Passing Needs Standardization**: FlatBuffers data should be passed through configuration layers consistently
4. **Missing Provider Abstraction**: Need ISceneDataProvider to fully abstract FlatBuffers from Scene configuration

### Recommendations

1. **Keep SceneFactory**: Factory should instantiate empty Scene objects
2. **Enhance SceneConfigurator**: Configurators should handle ALL data loading and configuration
3. **Create ISceneDataProvider**: Complete the provider pattern for scene data
4. **Separate Default vs Save Configurators**: Different configurators for different data sources

---

## Problem Statement Analysis

The user raises several important concerns:

> "we have data sources, such as .bin files or .json files or .xml files which are loaded into specific buffers and the different c++ packages provide ways of dealing with them"

**Status**: ✅ Well handled via provider pattern (75% complete)

> "we then need to take that non-native data struct and use to configure an object that we have created"

**Status**: ✅ Working via configurators (IEntityConfigurator, ISceneConfigurator)

> "however the data for the object could also be nested inside another object of the same data type"

**Status**: ⚠️ This is where confusion arises - addressed below

> "EntityCollectionFbs data is used to configure an EntityMemoryPool (native struct) and we could feasibly have EntityCollectionXML in the future"

**Status**: ✅ Abstraction exists via IEntityConfigurator interface

> "this data is contained inside SceneDataFbs which is used to configure a Scene"

**Status**: ⚠️ Current issue - SceneConfigurator loads SceneDataFbs directly, not abstracted

> "so when configuring the scene we don't want to create an EntityMemoryPool from the data, we want to pass the EntityCollectionFbs down to the FlatbuffersEntityConfigurator"

**Status**: ✅ This is exactly what happens currently - it works!

> "at the moment the SceneFactory takes the ISceneConfigurator as a strategy for how to configure a Scene, and we are using the SceneManager to dictate where the data source comes from. Does this mean we don't actually want the FlatbuffersDefaultSceneConfigurator to be loading the data directly?"

**Status**: ⚠️ Key architectural question - addressed in recommendations

> "maybe its better to have the ISceneConfigurator just produce a Scene object itself rather than the SceneFactory"

**Status**: 🤔 This is an option but may conflate responsibilities - analyzed below

---

## Current Architecture Overview

### Data Flow Layers

```
┌─────────────────────────────────────────────────────────┐
│                  Layer 1: Data Sources                   │
│  .bin files, .json files (FlatBuffers binary format)    │
└──────────────────┬──────────────────────────────────────┘
                   │
                   ↓
┌─────────────────────────────────────────────────────────┐
│            Layer 2: Data Loader (Concrete)               │
│  FlatbuffersDataLoader - loads and returns FlatBuffers   │
│  pointers (const SceneDataFbs*, etc.)                    │
└──────────────────┬──────────────────────────────────────┘
                   │
                   ↓
┌─────────────────────────────────────────────────────────┐
│         Layer 3: Configurators (Strategy Pattern)        │
│  ISceneConfigurator, IEntityConfigurator                 │
│  - FlatbuffersDefaultSceneConfigurator                   │
│  - FlatbuffersEntityConfigurator                         │
└──────────────────┬──────────────────────────────────────┘
                   │
                   ↓
┌─────────────────────────────────────────────────────────┐
│              Layer 4: Object Creation                    │
│  SceneFactory, Scene constructors                        │
└─────────────────────────────────────────────────────────┘
```

### Current Scene Creation Flow

```cpp
// In SceneManager::AddSceneFromDefault
ISceneConfigurator &configurator = GetDefaultSceneConfigurator();
                                   // Returns FlatbuffersDefaultSceneConfigurator

SceneFactory factory(game_context, configurator);

auto scene = factory.CreateAndConfigureScene(scene_type);
    │
    ├─> CreateSceneByType(scene_type)
    │   └─> Creates empty TitleScene or CraftingScene
    │
    └─> configurator.ConfigureScene(scene, scene_type)
        ├─> ConfigureSceneInfo()
        │   └─> Loads SceneDataFbs via m_data_loader
        ├─> ConfigureSceneResources()  
        │   └─> Loads SceneDataFbs again (redundant!)
        ├─> ConfigureSceneConfig()
        └─> ConfigureLogicMap()
```

### Current Entity Configuration Flow

```cpp
// Inside Scene (called by configurator or derived scene)
EntityManager &entity_manager = scene.GetEntityManager();

// Create configurator with FlatBuffers data
const EntityCollectionFbs &entity_data = ...; // From SceneDataFbs
FlatbuffersEntityConfigurator configurator(event_handler, entity_data);

// Configure the entity memory pool
configurator.ConfigureEntityMemoryPool(entity_manager.GetEntityMemoryPool());
    │
    └─> ConfigureFirstLayerComponents()
        └─> For each entity in entity_data:
            ├─> ConfigureComponent(CUserInterface)
            └─> ConfigureComponent(CGrimoireMachina)
```

---

## Problem Areas Identified

### 1. SceneConfigurator Loading Data Directly

**Issue**: `FlatbuffersDefaultSceneConfigurator` has a `FlatbuffersDataLoader` member and loads data directly:

```cpp
class FlatbuffersDefaultSceneConfigurator : public ISceneConfigurator {
private:
  FlatbuffersDataLoader m_data_loader;  // ❌ Violates abstraction!
  
public:
  std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene, const SceneType scene_type) override {
    // Loads data directly
    auto load_result = m_data_loader.ProvideDefaultSceneData(scene_type);
    // ...
  }
};
```

**Problem**: This tightly couples the configurator to FlatBuffers implementation. To support XML or other formats, we'd need entirely new configurator classes.

**Better Approach**: Pass data TO the configurator, don't load it WITHIN the configurator.

### 2. Redundant Data Loading

**Issue**: `ProvideDefaultSceneData()` is called multiple times:
- Once in `ConfigureSceneInfo()`
- Again in `ConfigureSceneResources()`
- Potentially again elsewhere

**Problem**: Wasteful and creates opportunities for inconsistency.

### 3. Unclear Factory vs Configurator Responsibilities

**Current State**:
- `SceneFactory`: Creates empty Scene objects, delegates to configurator
- `ISceneConfigurator`: Loads data and configures the Scene

**Question**: Why have both? Could configurator just create the Scene?

### 4. Missing SceneData Provider Abstraction

**Issue**: While we have:
- ✅ `IEngineDataProvider`
- ✅ `ISceneManagerDataProvider`
- ✅ `IAssetDataProvider`
- ❌ **Missing**: `ISceneDataProvider`

**Problem**: Scene configuration is not fully abstracted from FlatBuffers.

---

## Architectural Options

### Option 1: Status Quo (Keep Both Factory and Configurator)

**Keep SceneFactory** for object creation and **keep SceneConfigurator** for configuration, but fix the data loading issues.

**Pros**:
- Maintains separation of concerns: Factory creates, Configurator configures
- Follows existing patterns in the codebase
- Minimal changes required

**Cons**:
- Two classes involved in scene creation (some may see as overly complex)
- Need to clarify responsibilities

**Implementation**:
```cpp
// Factory creates empty objects
class SceneFactory {
  std::unique_ptr<Scene> CreateSceneByType(SceneType type);
};

// Configurator receives data and configures
class ISceneConfigurator {
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneDataFbs &scene_data) = 0;
};

// Usage
ISceneDataProvider &provider = GetSceneDataProvider();
auto scene_data = provider.LoadDefaultSceneData(scene_type);

SceneFactory factory(game_context);
auto scene = factory.CreateSceneByType(scene_type);

ISceneConfigurator &configurator = GetSceneConfigurator();
configurator.ConfigureScene(*scene, *scene_data);
```

### Option 2: Configurator Creates Scene (Eliminate Factory)

**Eliminate SceneFactory** and have configurators handle both creation and configuration.

**Pros**:
- Fewer classes involved
- One-stop-shop for scene creation
- Configurator has full control

**Cons**:
- Conflates object creation with configuration
- Harder to support multiple data sources (would need configurator per scene type AND data source)
- Goes against Single Responsibility Principle

**Implementation**:
```cpp
class ISceneConfigurator {
  virtual std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateAndConfigureScene(SceneType type) = 0;
};

// Usage
ISceneConfigurator &configurator = GetDefaultSceneConfigurator();
auto scene = configurator.CreateAndConfigureScene(SceneType::TITLE);
```

**Problem with this approach**: If we have multiple scene types (Title, Crafting, Battle) and multiple data sources (Default, SaveFile, Test), we'd need:
- `FlatbuffersDefaultTitleSceneConfigurator`
- `FlatbuffersDefaultCraftingSceneConfigurator`
- `FlatbuffersSaveTitleSceneConfigurator`
- `FlatbuffersSaveCraftingSceneConfigurator`
- ... combinatorial explosion!

### Option 3: Data Provider + Configurator (Recommended)

**Create ISceneDataProvider** to abstract data loading, keep Factory for creation, pass data to Configurator.

**Pros**:
- Full abstraction: `ISceneDataProvider` abstracts FlatBuffers
- Clear responsibilities:
  - Provider: Load data
  - Factory: Create objects
  - Configurator: Apply data to objects
- Supports multiple data sources easily
- Follows existing patterns (Provider pattern is 75% complete)

**Cons**:
- Three components involved (but each has clear purpose)
- More classes to understand

**Implementation**:
```cpp
// Provider abstracts data source
class ISceneDataProvider {
  virtual std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType type) const = 0;
};

// SceneData is a native C++ struct
struct SceneData {
  SceneInfo info;
  SceneResources resources;
  EntityCollection entities;  // Also native C++
  LogicCollectionData logic;
};

// Factory creates empty objects
class SceneFactory {
  std::unique_ptr<Scene> CreateSceneByType(SceneType type);
};

// Configurator configures from native data
class ISceneConfigurator {
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData &data) = 0;
};

// Usage
ISceneDataProvider &provider = GetSceneDataProvider();
auto scene_data = provider.LoadSceneData(scene_type);

SceneFactory factory(game_context);
auto scene = factory.CreateSceneByType(scene_type);

ISceneConfigurator &configurator = GetSceneConfigurator();
configurator.ConfigureScene(*scene, scene_data.value());
```

---

## Recommended Architecture

### Recommendation: Adopt Option 3 with Provider Pattern

This is the most scalable and maintainable approach.

### Component Responsibilities

#### 1. ISceneDataProvider (NEW)

**Purpose**: Abstract data sources (FlatBuffers, XML, JSON)

**Interface**:
```cpp
class ISceneDataProvider {
public:
  virtual ~ISceneDataProvider() = default;
  
  // Load complete scene data
  virtual std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType type) const = 0;
};
```

**Implementations**:
- `FlatbuffersDefaultSceneDataProvider` - Loads from default .bin files
- `FlatbuffersSaveSceneDataProvider` - Loads from save files
- `XMLSceneDataProvider` - Future: Loads from XML
- `TestSceneDataProvider` - Loads test data

#### 2. SceneFactory

**Purpose**: Create empty Scene objects of the correct type

**Responsibilities**:
- Switch on `SceneType` and instantiate correct derived class
- Pass `GameContext` to Scene constructor
- Return `std::unique_ptr<Scene>`

**Does NOT**:
- Load any data
- Configure the Scene
- Know about FlatBuffers, XML, or any data format

**Interface**:
```cpp
class SceneFactory {
private:
  const GameContext &m_game_context;
  
public:
  SceneFactory(const GameContext &game_context);
  
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateSceneByType(SceneType type);
};
```

#### 3. ISceneConfigurator

**Purpose**: Apply data to an existing Scene object

**Responsibilities**:
- Receive native C++ data structures (not FlatBuffers)
- Configure Scene members (SceneInfo, SceneResources, etc.)
- Delegate entity configuration to IEntityConfigurator
- Configure LogicMap

**Does NOT**:
- Create Scene objects
- Load data from files
- Know about FlatBuffers, XML, or any data format

**Interface**:
```cpp
class ISceneConfigurator {
public:
  virtual ~ISceneConfigurator() = default;
  
  // Configure from complete scene data
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData &data) = 0;
  
protected:
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneInfo(Scene &scene, const SceneInfo &info) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneResources(Scene &scene, const SceneResources &resources) = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureSceneConfig(Scene &scene, const SceneConfig &config) = 0;
  
  std::expected<std::monostate, FailInfo>
  ConfigureLogicMap(Scene &scene);  // Non-virtual, same for all
};
```

**Implementations**:
- `DefaultSceneConfigurator` - Standard configuration logic
- `TestSceneConfigurator` - Test-specific configuration (if needed)

Note: Since configuration logic is generally the same regardless of data source, we likely only need ONE configurator implementation, not one per data source!

### Data Flow Diagram (Recommended)

```
SceneManager::AddSceneFromDefault(SceneType::TITLE)
    │
    ├─> GetSceneDataProvider() → ISceneDataProvider&
    │   └─> Returns: FlatbuffersDefaultSceneDataProvider
    │
    ├─> provider.LoadSceneData(SceneType::TITLE)
    │   └─> Returns: SceneData (native C++ struct)
    │       ├─ info: SceneInfo
    │       ├─ resources: SceneResources
    │       ├─ entities: EntityCollection
    │       └─ logic: LogicCollectionData
    │
    ├─> SceneFactory(game_context).CreateSceneByType(TITLE)
    │   └─> Returns: std::unique_ptr<TitleScene> (empty)
    │
    ├─> GetSceneConfigurator() → ISceneConfigurator&
    │   └─> Returns: DefaultSceneConfigurator
    │
    └─> configurator.ConfigureScene(*scene, scene_data)
        ├─> ConfigureSceneInfo(scene, scene_data.info)
        ├─> ConfigureSceneResources(scene, scene_data.resources)
        ├─> ConfigureEntities(scene, scene_data.entities)
        │   └─> Creates IEntityConfigurator with entity data
        │       └─> Configures EntityMemoryPool
        └─> ConfigureLogicMap(scene)
```

### Supporting Save Data

With this architecture, supporting save data is trivial:

```cpp
// In SceneManager
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromSave(const SaveData &save_data, SceneType type) {
  
  // Different provider!
  ISceneDataProvider &provider = GetSaveSceneDataProvider();
  auto scene_data = provider.LoadSceneData(type, save_data);
  
  // Same factory!
  SceneFactory factory(m_game_context);
  auto scene = factory.CreateSceneByType(type);
  
  // Same configurator!
  ISceneConfigurator &configurator = GetSceneConfigurator();
  configurator.ConfigureScene(*scene, scene_data.value());
  
  // Add to scenes map
  m_scenes.emplace(scene->GetSceneInfo().id, std::move(scene));
  
  return std::monostate{};
}
```

**Key insight**: Only the Provider changes between default and save data! Factory and Configurator remain the same.

---

## Native Data Structures

### SceneData Struct

```cpp
namespace steamrot {

struct SceneInfo {
  uuids::uuid id;
  SceneType type;
};

struct SceneResources {
  uint32_t render_texture_width{800};
  uint32_t render_texture_height{600};
  // Other resource data
};

struct SceneConfig {
  // Configuration data
};

struct EntityCollection {
  uint32_t entity_memory_pool_size{100};
  std::vector<EntityData> entities;
};

struct SceneData {
  SceneInfo info;
  SceneResources resources;
  SceneConfig config;
  EntityCollection entities;
  LogicCollectionData logic;
};

} // namespace steamrot
```

### Provider Implementation

```cpp
class FlatbuffersDefaultSceneDataProvider : public ISceneDataProvider {
private:
  FlatbuffersDataLoader m_loader;
  
public:
  std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType type) const override {
    
    // Load FlatBuffers data
    auto fbs_result = m_loader.ProvideDefaultSceneData(type);
    if (!fbs_result.has_value())
      return std::unexpected(fbs_result.error());
    
    const SceneDataFbs *fbs_data = fbs_result.value();
    
    // Convert to native C++ struct
    SceneData scene_data;
    
    // Convert scene info
    if (fbs_data->scene_info()) {
      scene_data.info.type = fbs_data->scene_info()->scene_type();
      // ... other conversions
    }
    
    // Convert scene resources
    if (fbs_data->scene_resources()) {
      scene_data.resources.render_texture_width = 
          fbs_data->scene_resources()->render_texture_width();
      // ... other conversions
    }
    
    // Convert entity collection
    if (fbs_data->entity_collection()) {
      scene_data.entities.entity_memory_pool_size = 
          fbs_data->entity_collection()->entity_memory_pool_size();
      // Convert entities...
    }
    
    return scene_data;
  }
};
```

---

## Migration Path

### Phase 1: Create ISceneDataProvider

1. Create `ISceneDataProvider.h` interface
2. Create `SceneData` native struct
3. Implement `FlatbuffersDefaultSceneDataProvider`
4. Add to `provider_factory.h/cpp`

### Phase 2: Update SceneConfigurator

1. Change `ISceneConfigurator` to take `SceneData` instead of `SceneType`
2. Remove `FlatbuffersDataLoader` member from `FlatbuffersDefaultSceneConfigurator`
3. Rename `FlatbuffersDefaultSceneConfigurator` to just `DefaultSceneConfigurator`
4. Update configurator methods to use native structs

### Phase 3: Update SceneManager

1. Use provider to load data
2. Pass data to configurator
3. Update `AddSceneFromDefault()` to new pattern

### Phase 4: Add Save Data Support

1. Implement `FlatbuffersSaveSceneDataProvider`
2. Create `AddSceneFromSave()` method
3. Use same Factory and Configurator

---

## Answers to Specific Questions

### "Does this mean we don't actually want the FlatbuffersDefaultSceneConfigurator to be loading the data directly?"

**Answer**: Correct! The configurator should receive data, not load it.

**Reason**: This maintains abstraction and allows the same configurator to work with data from any source.

### "Do we want to be passing it a strategy or just have the FlatbuffersSaveDataSceneConfigurator take in the SaveDataFbs?"

**Answer**: Pass the data to the configurator, not have it load internally.

**Better**: Don't need separate save data configurator at all! Use the same configurator with different data providers.

### "Maybe it's better to have the ISceneConfigurator just produce a Scene object itself rather than the SceneFactory?"

**Answer**: No, keep them separate for these reasons:

1. **Single Responsibility**: Factory creates, Configurator configures
2. **Combinatorial Explosion**: With N scene types and M data sources, we'd need N×M configurator classes
3. **Reusability**: Same configurator works with all data sources when data is passed in
4. **Testability**: Easier to test creation and configuration separately

---

## Summary

### Current Problems

1. ❌ SceneConfigurator loads data directly (couples to FlatBuffers)
2. ❌ Missing ISceneDataProvider abstraction
3. ❌ Redundant data loading
4. ❌ Unclear responsibilities between Factory and Configurator

### Recommended Solution

1. ✅ Create **ISceneDataProvider** with native C++ `SceneData` struct
2. ✅ Keep **SceneFactory** for object creation (separate concern)
3. ✅ Update **ISceneConfigurator** to receive data (not load it)
4. ✅ Use provider pattern consistently (completes 75% → 100%)

### Benefits

- ✅ Full abstraction from FlatBuffers
- ✅ Easy to add new data sources (XML, JSON, test data)
- ✅ Clear separation of concerns
- ✅ Reusable configurator across data sources
- ✅ Supports both default and save data naturally
- ✅ Consistent with existing provider pattern
- ✅ Testable components

### Key Principle

**"Providers provide data, Factories create objects, Configurators apply data to objects"**

Each component has ONE job, and the data flows cleanly through the system.
