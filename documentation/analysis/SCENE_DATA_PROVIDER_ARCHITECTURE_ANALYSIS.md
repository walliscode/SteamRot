# Scene Data Loading Architecture - End-to-End Analysis

**Date**: December 7, 2025  
**Context**: Comprehensive analysis of scene data loading from Engine startup to fully configured Scene  
**Status**: Analysis Only - Not Yet Implemented

---

## Table of Contents

1. [Executive Summary](#executive-summary)
2. [Current State Analysis](#current-state-analysis)
3. [The Problem Statement](#the-problem-statement)
4. [Architectural Principles](#architectural-principles)
5. [The Two Access Patterns](#the-two-access-patterns)
6. [End-to-End Flow Analysis](#end-to-end-flow-analysis)
7. [Polymorphism vs Function Overloads Strategy](#polymorphism-vs-function-overloads-strategy)
8. [Recommended Approach](#recommended-approach)
9. [Implementation Roadmap](#implementation-roadmap)
10. [Future Extensibility](#future-extensibility)

---

## Executive Summary

### The Vision

Create a cohesive, extensible architecture for loading and configuring Scenes that:
1. ✅ Abstracts FlatBuffers away from game code (interface pattern)
2. ✅ Supports both **default scene data** (new game) and **save scene data** (loaded game)
3. ✅ Handles nested data structures without coupling or wasteful copying
4. ✅ Uses consistent patterns throughout the codebase
5. ✅ Allows for future data format additions (JSON, XML, Lua)

### The Solution

**Two-Provider Strategy with View Pattern**:
- `ISceneDataProvider` → Loads default/template scene data
- `ISaveDataProvider` → Loads saved game data (including scene states)
- **View/Extractor Pattern** → Converts nested save data to flat scene data
- **Factory + Configurator Pattern** → Creates and configures Scene objects

### Key Insight

> "The interface pattern you've already established with `ISaveDataProvider` and `ISceneDataProvider` is **exactly the right approach**. We don't need to mix systems - we extend the existing pattern consistently."

---

## Current State Analysis

### What Currently Works ✅

#### 1. Engine Data Loading (Recently Implemented)
```cpp
// Engine::StartUp() - WORKING
IEngineDataProvider& provider = GetEngineDataProvider();
auto resources_config = provider.LoadEngineResourcesConfig();  // Native C++ struct
auto engine_config = provider.LoadEngineConfig();               // Native C++ struct
auto engine_state = provider.LoadEngineState();                 // Native C++ struct
```

**Pattern**: Interface → Implementation → Native Struct
- ✅ FlatBuffers abstracted away
- ✅ Game code uses native C++ structs
- ✅ Extensible to other data formats

#### 2. Scene Metadata Loading (Partially Implemented)
```cpp
// SceneFactory::CreateDefaultScene() - WORKING
ISceneDataProvider& provider = GetSceneDataProvider();
auto scene_data = provider.LoadSceneData(scene_type);  // Native SceneData struct

// Configure render texture
sf::Vector2u texture_size(scene_data.render_texture_width, 
                         scene_data.render_texture_height);
scene_ptr->m_scene_resources.scene_texture = sf::RenderTexture(texture_size);
```

**Status**: ✅ Works for simple metadata (dimensions, IDs)

#### 3. Save Data Interface (Skeleton Exists)
```cpp
// ISaveDataProvider - INTERFACE EXISTS
class ISaveDataProvider {
  virtual std::expected<SaveData, FailInfo> LoadSave(uint32_t slot) = 0;
  virtual std::expected<std::monostate, FailInfo> SaveGame(const SaveData&) = 0;
};

// SaveData struct - DEFINED BUT INCOMPLETE
struct SaveData {
  Metadata metadata;
  SceneType current_scene_type;
  uint32_t version;
  // NOTE: Scene state data will be added when entity serialization is implemented
};
```

**Status**: ⚠️ Interface exists, but scene state data not yet defined

### What Doesn't Work Yet ❌

#### 1. Entity Loading Still Uses FlatBuffers Directly
```cpp
// Scene::ConfigureFromDefault() - NEEDS IMPROVEMENT
auto emp_configure_result = 
    m_scene_resources.entity_manager.ConfigureEntitiesFromDefaultData(
        m_scene_info.type, DataType::Flatbuffers);  // ❌ Hardcoded FlatBuffers

// EntityManager::ConfigureEntitiesFromDefaultData() - NEEDS IMPROVEMENT
FlatbuffersConfigurator configurator{m_event_handler};  // ❌ Concrete class
configurator.ConfigureEntitiesFromDefaultData(m_entity_memory_pool, scene_type);

// FlatbuffersConfigurator::ConfigureEntitiesFromDefaultData() - NEEDS IMPROVEMENT
const SceneDataData* scene_data = 
    m_data_loader.ProvideDefaultSceneData(scene_type).value();  // ❌ FlatBuffers type!
```

**Problems**:
- ❌ FlatBuffers types leak into configuration code
- ❌ `DataType` enum is a code smell (hardcoded format)
- ❌ Can't swap implementations easily
- ❌ Tightly coupled to FlatBuffers

#### 2. No Save Scene Loading Implementation
```cpp
// MISSING: How do we load a scene from a save file?
// MISSING: How do we convert SaveData scene state → Scene configuration?
// MISSING: SceneDataExtractor or equivalent pattern
```

#### 3. Mixed Responsibilities
```cpp
// FlatbuffersDataLoader - TOO MANY RESPONSIBILITIES
class FlatbuffersDataLoader {
  ProvideDefaultSceneData();      // Scene data
  ProvideAssetData();             // Asset data
  ProvideUIStylesData();          // UI data
  ProvideContextData();           // Context data
  ProvideEngineResourcesConfig(); // Engine data
  ProvideFragment();              // Fragment data
  // ... 10+ methods!
};
```

**Problem**: God object with too many responsibilities

---

## The Problem Statement

### User's Vision

> "We will have **default scene data** (fetched via enum switch) and **save scene data**. They should have exactly the same structure but not exactly the same data."

> "**ISaveDataProvider** allows us access to the save scene data (via some viewer or other mechanism)."

> "We need to turn scene data into Scenes themselves. Either turn the SceneDataFbs into some intermediate struct OR create the Scene in the provider via SceneFactory."

> "Use **polymorphism vs function overloads** to allow for different data types."

> "Tie this all into a **cohesive approach** so we are not mixing too many systems."

### The Challenge

How do we design a system that:

1. **Loads default scenes** (new game) from default data files
2. **Loads saved scenes** (continue game) from save files
3. **Uses the same configuration pipeline** regardless of source
4. **Abstracts data formats** (FlatBuffers, JSON, etc.) from game code
5. **Handles nested data** (SaveData → SceneState → EntityCollection) efficiently
6. **Maintains consistency** with existing patterns (IEngineDataProvider, etc.)

---

## Architectural Principles

### Principle 1: Interface-Based Data Access

**Pattern**: Game code depends on interfaces, not concrete implementations

```cpp
// ✅ GOOD - Interface dependency
ISceneDataProvider& GetSceneDataProvider();
ISaveDataProvider& GetSaveDataProvider();

// ❌ BAD - Concrete class dependency
FlatbuffersDataLoader loader;
```

**Benefits**:
- Swap implementations without changing game code
- Easy to test (mock interfaces)
- Format-agnostic (FlatBuffers, JSON, XML, Lua)

### Principle 2: Native Structs as API Boundary

**Pattern**: Interfaces return native C++ structs, not format-specific types

```cpp
// ✅ GOOD - Native struct
struct SceneData {
  SceneType scene_type;
  std::string scene_id;
  uint32_t render_texture_width;
  uint32_t render_texture_height;
};

// ❌ BAD - FlatBuffers type
const SceneDataFbs* LoadSceneData();
```

**Benefits**:
- No FlatBuffers dependency in game code
- Format changes don't affect game code
- Easy to serialize to other formats

### Principle 3: Separation of Concerns

**Pattern**: One interface per data domain

```cpp
IEngineDataProvider      → Engine configuration
ISceneDataProvider       → Scene configuration (default)
ISaveDataProvider        → Save/load game state (includes saved scenes)
IAssetDataProvider       → Asset lists
IFragmentDataProvider    → Fragment geometries
```

**Anti-Pattern**: Don't create format-specific interfaces
```cpp
// ❌ BAD
IFlatbuffersProvider
IJsonProvider
```

### Principle 4: Configurators for Complex Transformations

**When to use Configurators**:
- Complex transformation logic needed
- Requires dependencies (EventHandler, AssetManager)
- Modifying existing objects
- Order of operations matters

**Example**: EntityConfigurator (complex, needs EventHandler for Subscribers)

**When to use Providers**:
- Loading external data
- Format might vary
- Returns complete data structures
- Stateless operations

**Example**: ISceneDataProvider (loads scene metadata)

### Principle 5: View/Extractor Pattern for Nested Data

**Problem**: SaveData has nested scene data, but we want flat SceneData for configuration

**Solution**: Extractor/Adapter pattern
```cpp
// View pattern - no copies, just provides access
class SceneDataExtractor {
  SceneData ExtractSceneData(const SaveData& save, uint32_t scene_index);
};
```

---

## The Two Access Patterns

### Pattern 1: Default Scene Data (New Game)

**Flow**: Load default scene template → Create Scene → Configure from default

```
User Starts New Game
        ↓
ISceneDataProvider::LoadSceneData(SceneType)
        ↓
Returns SceneData (metadata: type, dimensions, id)
        ↓
SceneFactory::CreateDefaultScene()
        ↓
Scene::ConfigureFromDefault()
        ↓
[Entity Configurator loads default entities]
        ↓
Fully Configured Scene Ready
```

**Key Files**:
- `data/scenes/title.scene_data.bin` (default scene template)
- `data/scenes/crafting.scene_data.bin` (default scene template)

**Implementation**:
```cpp
// 1. Get provider
ISceneDataProvider& provider = GetSceneDataProvider();

// 2. Load scene metadata
auto scene_data_result = provider.LoadSceneData(SceneType::SceneType_TITLE);
const SceneData& scene_data = scene_data_result.value();

// 3. Create scene
SceneFactory factory;
auto scene_result = factory.CreateDefaultScene(scene_type, game_context);
std::unique_ptr<Scene> scene = std::move(scene_result.value());

// 4. Configure from default
scene->ConfigureFromDefault();
```

### Pattern 2: Saved Scene Data (Continue Game)

**Flow**: Load save file → Extract scene data → Create Scene → Configure from save

```
User Loads Save File
        ↓
ISaveDataProvider::LoadSave(slot_index)
        ↓
Returns SaveData (includes scene states)
        ↓
SceneDataExtractor::ExtractSceneData(save, scene_index)
        ↓
Returns SceneData (extracted from save)
        ↓
SceneFactory::CreateSceneFromSave()
        ↓
Scene::ConfigureFromSave(scene_state)
        ↓
[Entity Configurator loads saved entities]
        ↓
Fully Configured Scene Ready
```

**Key Files**:
- `saves/slot_0.save.bin` (saved game state)
- `saves/slot_1.save.bin` (saved game state)

**Implementation** (Future):
```cpp
// 1. Get save provider
ISaveDataProvider& save_provider = GetSaveDataProvider();

// 2. Load save file
auto save_result = save_provider.LoadSave(slot_index);
const SaveData& save_data = save_result.value();

// 3. Extract scene data for current scene
SceneDataExtractor extractor;
SceneData scene_data = extractor.ExtractSceneData(
    save_data, save_data.current_scene_type);

// 4. Create scene
SceneFactory factory;
auto scene_result = factory.CreateSceneFromSave(
    scene_data, save_data.scene_states[0], game_context);
std::unique_ptr<Scene> scene = std::move(scene_result.value());

// 5. Configure from save data
scene->ConfigureFromSave(save_data.scene_states[0]);
```

---

## End-to-End Flow Analysis

### Phase 1: Engine Startup (Already Working ✅)

```cpp
// main.cpp
GameEngine engine;
engine.RunGame();

// Engine::RunGame()
auto start_up_result = StartUp();  // ← Configure engine-level resources
RunGameLoop();

// Engine::StartUp() - WORKING
IEngineDataProvider& provider = GetEngineDataProvider();
auto resources_config = provider.LoadEngineResourcesConfig();
engine::ConfigureEngineResources(m_engine_resources, resources_config.value());
auto engine_config = provider.LoadEngineConfig();
m_engine_config = engine_config.value();
auto engine_state = provider.LoadEngineState();
m_engine_state = engine_state.value();
```

**Status**: ✅ **COMPLETE** - Engine-level resources configured via interface pattern

### Phase 2: Scene Creation (Partially Working ⚠️)

```cpp
// Engine startup typically loads a scene
m_scene_manager.LoadTitleScene();

// SceneManager::LoadTitleScene()
AddSceneFromDefault(SceneType::SceneType_TITLE);

// SceneManager::AddSceneFromDefault()
SceneFactory factory;
auto scene_result = factory.CreateDefaultScene(scene_type, m_game_context);
```

**Status**: ⚠️ **PARTIAL** - Scene created but entity configuration still uses FlatBuffers

### Phase 3: Scene Metadata Configuration (Working ✅)

```cpp
// SceneFactory::CreateDefaultScene()

// 3a. Load scene metadata via interface
ISceneDataProvider& data_provider = GetSceneDataProvider();
auto scene_data_result = data_provider.LoadSceneData(scene_type);
const SceneData& scene_data = scene_data_result.value();

// 3b. Configure render texture
sf::Vector2u texture_size(scene_data.render_texture_width,
                         scene_data.render_texture_height);
scene_ptr->m_scene_resources.scene_texture = sf::RenderTexture(texture_size);
```

**Status**: ✅ **COMPLETE** - Scene metadata (dimensions, type, ID) configured via interface

### Phase 4: Entity Configuration (Needs Improvement ❌)

```cpp
// SceneFactory::CreateDefaultScene()
auto configure_result = scene_ptr->ConfigureFromDefault();

// Scene::ConfigureFromDefault()
auto emp_configure_result =
    m_scene_resources.entity_manager.ConfigureEntitiesFromDefaultData(
        m_scene_info.type, DataType::Flatbuffers);  // ❌ Hardcoded!

// EntityManager::ConfigureEntitiesFromDefaultData()
FlatbuffersConfigurator configurator{m_event_handler};  // ❌ Concrete class
configurator.ConfigureEntitiesFromDefaultData(m_entity_memory_pool, scene_type);

// FlatbuffersConfigurator::ConfigureEntitiesFromDefaultData()
const SceneDataData* scene_data =
    m_data_loader.ProvideDefaultSceneData(scene_type).value();  // ❌ FlatBuffers type!
return ConfigureEntitiesFromCollection(entity_memory_pool,
                                       scene_data->entity_collection());
```

**Problems**:
- ❌ `DataType` enum hardcoded (not extensible)
- ❌ FlatBuffers types leak into configuration
- ❌ Tightly coupled to FlatBuffers implementation
- ❌ Can't easily load from save files

**Desired State**:
```cpp
// Scene::ConfigureFromDefault() - DESIRED
IEntityDataProvider& entity_provider = GetEntityDataProvider(m_scene_info.type);
auto entity_data_result = entity_provider.LoadDefaultEntities();
const EntityCollectionData& entities = entity_data_result.value();

EntityConfigurator configurator{m_event_handler};
configurator.ConfigureEntities(m_entity_memory_pool, entities);
```

### Phase 5: Archetype Generation (Working ✅)

```cpp
// SceneFactory::CreateDefaultScene()
auto archetype_result =
    scene_ptr->m_scene_resources.entity_manager.GenerateAllArchetypes();
```

**Status**: ✅ **COMPLETE** - Archetypes generated from entity components

### Phase 6: Logic Configuration (Working ✅)

```cpp
// SceneFactory::CreateDefaultScene()
LogicFactory logic_factory(scene_type, scene_ptr->GetSceneContext());
auto logic_map_result = logic_factory.CreateLogicMap();
scene_ptr->SetLogicMap(std::move(logic_map_result.value()));
```

**Status**: ✅ **COMPLETE** - Logic systems created for scene

### Phase 7: Scene Ready (Working ✅)

```cpp
// SceneFactory::CreateDefaultScene()
return scene_ptr;

// SceneManager stores scene
m_scenes[scene_uuid] = std::move(scene_ptr);
```

**Status**: ✅ **COMPLETE** - Scene added to manager and ready to run

---

## Polymorphism vs Function Overloads Strategy

### The Question

> "Use polymorphism vs function overloads to allow for different data types"

### Answer: Use Both, But for Different Purposes

#### Use Polymorphism (Virtual Functions) For:

**1. Data Format Abstraction**

```cpp
// Interface - Format-agnostic
class ISceneDataProvider {
public:
  virtual ~ISceneDataProvider() = default;
  virtual std::expected<SceneData, FailInfo> 
  LoadSceneData(SceneType scene_type) const = 0;
};

// FlatBuffers implementation
class FlatbuffersSceneDataProvider : public ISceneDataProvider {
  std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType scene_type) const override;
};

// Future JSON implementation
class JsonSceneDataProvider : public ISceneDataProvider {
  std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType scene_type) const override;
};
```

**Why**: Allows swapping implementations at runtime without changing game code

#### Use Function Overloads For:

**2. Data Source Differentiation**

```cpp
// SceneFactory - Different configuration sources
class SceneFactory {
public:
  // Create from default data (new game)
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateDefaultScene(const SceneType& scene_type, 
                    const GameContext& game_context);
  
  // Create from save data (loaded game)
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateSceneFromSave(const SceneData& scene_metadata,
                     const SceneStateData& scene_state,
                     const GameContext& game_context);
};

// EntityConfigurator - Different entity sources
class EntityConfigurator {
public:
  // Configure from default entity data
  std::expected<std::monostate, FailInfo>
  ConfigureEntities(EntityMemoryPool& pool,
                   const EntityCollectionData& default_entities);
  
  // Configure from saved entity data
  std::expected<std::monostate, FailInfo>
  ConfigureEntities(EntityMemoryPool& pool,
                   const SavedEntityCollectionData& saved_entities);
};
```

**Why**: Makes intent clear at call site, allows different parameter types

#### Use Templates For:

**3. Type-Safe Generic Operations**

```cpp
// Provider factory - Type-safe provider retrieval
template<typename ProviderInterface>
ProviderInterface& GetProvider();

// Usage:
auto& scene_provider = GetProvider<ISceneDataProvider>();
auto& save_provider = GetProvider<ISaveDataProvider>();
```

**Why**: Compile-time type safety, no runtime overhead

### Recommended Pattern Breakdown

| Concern | Solution | Example |
|---------|----------|---------|
| **Data Format** (FlatBuffers vs JSON) | Polymorphism (interfaces) | `ISceneDataProvider` |
| **Data Source** (default vs save) | Function Overloads | `CreateDefaultScene()` vs `CreateSceneFromSave()` |
| **Provider Access** | Templates + Factory | `GetProvider<T>()` |
| **Configuration Logic** | Classes (Configurator) | `EntityConfigurator` |
| **Simple Conversions** | Free Functions | `ConvertFbToNative()` |

---

## Recommended Approach

### Solution 1: Separate Providers (Recommended ✅)

**Principle**: Keep `ISceneDataProvider` and `ISaveDataProvider` separate with distinct responsibilities

```cpp
// ISceneDataProvider - Loads default/template scenes
class ISceneDataProvider {
  virtual std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType scene_type) const = 0;
};

// ISaveDataProvider - Loads saved games
class ISaveDataProvider {
  virtual std::expected<SaveData, FailInfo>
  LoadSave(uint32_t slot_index) const = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  SaveGame(const SaveData& save_data) = 0;
};

// SceneDataExtractor - Adapts SaveData → SceneData
class SceneDataExtractor {
public:
  SceneData ExtractSceneMetadata(const SaveData& save) const;
  SceneStateData ExtractSceneState(const SaveData& save, 
                                   uint32_t scene_index) const;
};
```

**Why Separate**:
- ✅ Single Responsibility Principle (one purpose per interface)
- ✅ Clear which provider to use (default vs save)
- ✅ Different lifecycle (save files vs default data)
- ✅ Different error handling (save file corruption vs missing defaults)

**File Structure**:
```
data/scenes/                   ← Default scene templates
  title.scene_data.bin
  crafting.scene_data.bin

saves/                         ← Saved games
  slot_0.save.bin
  slot_1.save.bin
```

### Solution 2: Adapter/Extractor Pattern (Recommended ✅)

**Problem**: SaveData contains nested scene data, but we want flat SceneData

**Solution**: SceneDataExtractor adapts between the two

```cpp
// SaveData structure (nested)
struct SaveData {
  Metadata metadata;
  SceneType current_scene_type;
  std::vector<SceneStateData> scene_states;  // Multiple scenes!
  uint32_t version;
};

// SceneStateData (nested in SaveData)
struct SceneStateData {
  SceneType scene_type;
  std::string scene_id;
  uint32_t render_texture_width;
  uint32_t render_texture_height;
  EntityCollectionData entity_data;  // Entity states
  // ... other scene state
};

// SceneData (flat, used for configuration)
struct SceneData {
  SceneType scene_type;
  std::string scene_id;
  uint32_t render_texture_width;
  uint32_t render_texture_height;
};

// Extractor converts nested → flat
class SceneDataExtractor {
public:
  // Extract metadata only (for scene creation)
  SceneData ExtractSceneMetadata(const SaveData& save) const {
    const auto& scene_state = save.scene_states[0];  // Current scene
    
    SceneData data;
    data.scene_type = scene_state.scene_type;
    data.scene_id = scene_state.scene_id;
    data.render_texture_width = scene_state.render_texture_width;
    data.render_texture_height = scene_state.render_texture_height;
    return data;
  }
  
  // Extract full state (for entity configuration)
  SceneStateData ExtractSceneState(const SaveData& save, 
                                   uint32_t scene_index) const {
    return save.scene_states[scene_index];
  }
};
```

**Benefits**:
- ✅ No wasteful copying (extracts only what's needed)
- ✅ Single source of truth (SaveData owns the data)
- ✅ Clear transformation (nested → flat)
- ✅ Reusable (any code can extract scene data from saves)

### Solution 3: Unified Configuration Pipeline (Recommended ✅)

**Principle**: Same configuration flow regardless of data source

```cpp
// Unified pipeline for default scene
std::unique_ptr<Scene> LoadDefaultScene(SceneType scene_type) {
  // 1. Load metadata
  ISceneDataProvider& provider = GetSceneDataProvider();
  auto scene_data = provider.LoadSceneData(scene_type).value();
  
  // 2. Create scene
  SceneFactory factory;
  auto scene = factory.CreateDefaultScene(scene_type, game_context).value();
  
  // 3. Configure from default
  scene->ConfigureFromDefault();
  
  return scene;
}

// Unified pipeline for saved scene
std::unique_ptr<Scene> LoadSavedScene(uint32_t slot_index) {
  // 1. Load save data
  ISaveDataProvider& save_provider = GetSaveDataProvider();
  auto save_data = save_provider.LoadSave(slot_index).value();
  
  // 2. Extract scene metadata
  SceneDataExtractor extractor;
  SceneData scene_data = extractor.ExtractSceneMetadata(save_data);
  
  // 3. Extract scene state
  SceneStateData scene_state = extractor.ExtractSceneState(save_data, 0);
  
  // 4. Create scene
  SceneFactory factory;
  auto scene = factory.CreateSceneFromSave(
      scene_data, scene_state, game_context).value();
  
  // 5. Configure from save
  scene->ConfigureFromSave(scene_state);
  
  return scene;
}
```

**Key Insight**: Both paths converge at Scene creation, just with different data sources

### Solution 4: Avoid Intermediate Structs (Recommended ✅)

**Anti-Pattern**: Don't create intermediate structs that contain FlatBuffers pointers

```cpp
// ❌ BAD - Intermediate struct with FlatBuffers
struct SceneConfigData {
  const SceneDataFbs* fb_data;  // FlatBuffers pointer!
  SceneType scene_type;
};

// ❌ BAD - Passing FlatBuffers around
void ConfigureScene(const SceneDataFbs* fb_data);
```

**Good Pattern**: Convert to native structs at provider boundary

```cpp
// ✅ GOOD - Provider converts to native struct
class FlatbuffersSceneDataProvider : public ISceneDataProvider {
  std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType scene_type) const override {
    // 1. Load FlatBuffers data (internal only)
    auto fb_result = m_loader.ProvideDefaultSceneData(scene_type);
    const SceneDataFbs* fb_data = fb_result.value();
    
    // 2. Convert to native struct
    SceneData native;
    native.scene_type = fb_data->scene_type();
    native.scene_id = fb_data->scene_id() ? fb_data->scene_id()->str() : "";
    native.render_texture_width = fb_data->render_texture_width();
    native.render_texture_height = fb_data->render_texture_height();
    
    // 3. Return native struct (no FlatBuffers!)
    return native;
  }
};
```

---

## Implementation Roadmap

### Phase 1: Complete SceneData Provider (Low Risk ⬜)

**Goal**: Finish ISceneDataProvider implementation with full scene metadata

**Tasks**:
1. ✅ ISceneDataProvider interface exists
2. ✅ SceneData struct defined
3. ⬜ FlatbuffersSceneDataProvider fully implemented
4. ⬜ Tests for scene data loading

**Files to Change**:
- `src/data_providers/FlatbuffersSceneDataProvider.cpp` (implement conversion)
- `tests/unit/data_providers/FlatbuffersSceneDataProvider.test.cpp` (add tests)

**Estimated Effort**: 1-2 days

### Phase 2: Add Extractor Pattern (Low Risk ⬜)

**Goal**: Add SceneDataExtractor for converting SaveData → SceneData

**Tasks**:
1. ⬜ Define SceneStateData struct (in SaveData)
2. ⬜ Create SceneDataExtractor class
3. ⬜ Implement ExtractSceneMetadata()
4. ⬜ Implement ExtractSceneState()
5. ⬜ Add tests

**New Files**:
- `src/configuration/SceneDataExtractor.h`
- `src/configuration/SceneDataExtractor.cpp`
- `tests/unit/configuration/SceneDataExtractor.test.cpp`

**Estimated Effort**: 2-3 days

### Phase 3: Create IEntityDataProvider (Medium Risk 🟡)

**Goal**: Abstract entity loading away from FlatBuffers

**Tasks**:
1. ⬜ Define EntityCollectionData struct (native C++)
2. ⬜ Create IEntityDataProvider interface
3. ⬜ Implement FlatbuffersEntityDataProvider
4. ⬜ Update EntityConfigurator to use interface
5. ⬜ Remove DataType enum
6. ⬜ Add tests

**New Files**:
- `src/data_providers/IEntityDataProvider.h`
- `src/data_providers/FlatbuffersEntityDataProvider.h/cpp`
- `tests/unit/data_providers/FlatbuffersEntityDataProvider.test.cpp`

**Files to Change**:
- `src/entity/EntityManager.h/cpp` (remove DataType enum)
- `src/entity/EntityConfigurator.h/cpp` (use interface)
- `src/scenes/Scene.cpp` (update ConfigureFromDefault)

**Estimated Effort**: 1-2 weeks

### Phase 4: Implement SaveData Scene States (High Risk 🔴)

**Goal**: Add scene state serialization to save files

**Tasks**:
1. ⬜ Define SceneStateData FlatBuffers schema
2. ⬜ Update SaveData FlatBuffers schema
3. ⬜ Implement scene state capture
4. ⬜ Implement scene state restoration
5. ⬜ Update FlatbuffersSaveDataProvider
6. ⬜ Add comprehensive tests

**Files to Change**:
- `src/flatbuffers_headers/save_data.fbs` (add scene_states)
- `src/configuration/ISaveDataProvider.h` (update SaveData struct)
- `src/configuration/FlatbuffersSaveDataProvider.cpp` (implement save/load)

**Estimated Effort**: 2-3 weeks

### Phase 5: Add SceneFactory Overloads (Low Risk ⬜)

**Goal**: Support creating scenes from save data

**Tasks**:
1. ⬜ Add CreateSceneFromSave() overload
2. ⬜ Add Scene::ConfigureFromSave() method
3. ⬜ Wire up in SceneManager
4. ⬜ Add tests

**Files to Change**:
- `src/scenes/SceneFactory.h/cpp`
- `src/scenes/Scene.h/cpp`
- `src/scenes/SceneManager.h/cpp`

**Estimated Effort**: 3-5 days

---

## Future Extensibility

### Adding New Data Formats

**Example**: Adding JSON support

```cpp
// 1. Create JSON implementation
class JsonSceneDataProvider : public ISceneDataProvider {
  std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType scene_type) const override {
    // Load from JSON file
    auto json_doc = LoadJsonFile(GetScenePath(scene_type));
    
    // Parse to native struct
    SceneData data;
    data.scene_type = ParseSceneType(json_doc["scene_type"]);
    data.scene_id = json_doc["scene_id"];
    data.render_texture_width = json_doc["render_texture_width"];
    data.render_texture_height = json_doc["render_texture_height"];
    
    return data;
  }
};

// 2. Register with factory
void RegisterProviders() {
  if (use_json) {
    RegisterProvider<ISceneDataProvider>(
        std::make_unique<JsonSceneDataProvider>());
  } else {
    RegisterProvider<ISceneDataProvider>(
        std::make_unique<FlatbuffersSceneDataProvider>());
  }
}

// 3. Game code unchanged!
ISceneDataProvider& provider = GetSceneDataProvider();
auto scene_data = provider.LoadSceneData(scene_type);
```

**No Changes Required**:
- ❌ SceneFactory
- ❌ Scene
- ❌ SceneManager
- ❌ EntityConfigurator
- ❌ Any game code

### Adding New Scene Types

**Example**: Adding "Shop" scene

```cpp
// 1. Add to enum
enum class SceneType {
  SceneType_TITLE,
  SceneType_CRAFTING,
  SceneType_SHOP,  // New!
};

// 2. Create scene class
class ShopScene : public Scene {
  // Implementation
};

// 3. Add to SceneFactory
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateDefaultScene(const SceneType& scene_type,
                                const GameContext& game_context) {
  switch (scene_type) {
  case SceneType::SceneType_SHOP: {
    std::unique_ptr<ShopScene> shop_scene(
        new ShopScene(scene_uuid, game_context));
    scene_ptr = std::move(shop_scene);
    break;
  }
  // ... other cases
  }
}

// 4. Add data file
data/scenes/shop.scene_data.bin

// 5. Provider automatically supports it!
ISceneDataProvider& provider = GetSceneDataProvider();
auto data = provider.LoadSceneData(SceneType::SceneType_SHOP);
```

### Adding Save Slots

**Already Designed**:
```cpp
ISaveDataProvider& provider = GetSaveDataProvider();

// List all slots
auto slots = provider.GetSaveSlots();
for (const auto& slot : slots.value()) {
  std::cout << slot.save_name << " - " 
            << slot.play_time_seconds << "s\n";
}

// Load specific slot
auto save = provider.LoadSave(3);  // Slot 3

// Save to slot
SaveData new_save = CaptureSaveData();
new_save.metadata.slot_index = 5;
provider.SaveGame(new_save);
```

---

## Cohesive System Summary

### The Complete Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                        Game Engine                          │
│  ┌─────────────────────────────────────────────────────┐   │
│  │              Engine::StartUp()                      │   │
│  │  IEngineDataProvider → EngineResources             │   │
│  │  IEngineDataProvider → EngineConfig                │   │
│  │  IEngineDataProvider → EngineState                 │   │
│  └─────────────────────────────────────────────────────┘   │
│                          ↓                                   │
│  ┌─────────────────────────────────────────────────────┐   │
│  │           SceneManager::LoadScene()                 │   │
│  │                                                      │   │
│  │  ┌──────────────────┐  ┌─────────────────────┐    │   │
│  │  │ New Game Path    │  │ Load Game Path      │    │   │
│  │  │                  │  │                     │    │   │
│  │  │ ISceneData       │  │ ISaveData           │    │   │
│  │  │ Provider         │  │ Provider            │    │   │
│  │  │     ↓            │  │     ↓               │    │   │
│  │  │ SceneData        │  │ SaveData            │    │   │
│  │  │                  │  │     ↓               │    │   │
│  │  │                  │  │ SceneData           │    │   │
│  │  │                  │  │ Extractor           │    │   │
│  │  │                  │  │     ↓               │    │   │
│  │  │                  │  │ SceneData           │    │   │
│  │  └──────────────────┘  └─────────────────────┘    │   │
│  │           ↓                       ↓                │   │
│  │  ┌─────────────────────────────────────────┐     │   │
│  │  │      SceneFactory                        │     │   │
│  │  │  CreateDefaultScene() or                │     │   │
│  │  │  CreateSceneFromSave()                  │     │   │
│  │  │         ↓                                │     │   │
│  │  │  Scene (created, dimensions set)        │     │   │
│  │  └─────────────────────────────────────────┘     │   │
│  │                    ↓                              │   │
│  │  ┌─────────────────────────────────────────┐     │   │
│  │  │   Scene::ConfigureFromDefault() or      │     │   │
│  │  │   Scene::ConfigureFromSave()            │     │   │
│  │  │         ↓                                │     │   │
│  │  │  IEntityDataProvider                    │     │   │
│  │  │         ↓                                │     │   │
│  │  │  EntityConfigurator                     │     │   │
│  │  │         ↓                                │     │   │
│  │  │  EntityMemoryPool (configured)          │     │   │
│  │  └─────────────────────────────────────────┘     │   │
│  │                    ↓                              │   │
│  │  ┌─────────────────────────────────────────┐     │   │
│  │  │   ArchetypeManager::Generate            │     │   │
│  │  │   Archetypes (from components)          │     │   │
│  │  └─────────────────────────────────────────┘     │   │
│  │                    ↓                              │   │
│  │  ┌─────────────────────────────────────────┐     │   │
│  │  │   LogicFactory::CreateLogicMap          │     │   │
│  │  │   (scene-specific logic systems)        │     │   │
│  │  └─────────────────────────────────────────┘     │   │
│  │                    ↓                              │   │
│  │         Scene (fully configured)                  │   │
│  └─────────────────────────────────────────────────────┘   │
│                          ↓                                   │
│                   Game Loop Running                          │
└─────────────────────────────────────────────────────────────┘
```

### Key Interfaces

| Interface | Responsibility | Returns | Used By |
|-----------|---------------|---------|---------|
| `IEngineDataProvider` | Engine configuration | Native structs | Engine::StartUp() |
| `ISceneDataProvider` | Default scene metadata | SceneData | SceneFactory |
| `ISaveDataProvider` | Save/load game state | SaveData | SceneManager |
| `IEntityDataProvider` | Entity configurations | EntityCollectionData | EntityConfigurator |
| `IAssetDataProvider` | Asset lists | AssetData | AssetManager |
| `IFragmentDataProvider` | Fragment geometries | Fragment | (existing) |

### Design Principles Applied

1. ✅ **Interface-based access** - All data through interfaces
2. ✅ **Native struct boundaries** - No FlatBuffers in game code
3. ✅ **Single Responsibility** - One interface per data domain
4. ✅ **Separation of Concerns** - Providers ≠ Configurators ≠ Factories
5. ✅ **Extractor Pattern** - Nested data → flat data without coupling
6. ✅ **Unified Pipelines** - Same flow for default and save scenes
7. ✅ **Polymorphism for Formats** - Swap FlatBuffers/JSON/XML
8. ✅ **Overloads for Sources** - Clear intent (default vs save)
9. ✅ **Future Extensibility** - Add formats/scenes without breaking existing code

### No Mixed Systems

**Consistent Pattern Throughout**:
- Data files → Provider Interface → Native Struct → Configurator → Configured Object
- Same pattern for: Engine, Scene, Entities, Assets, Fragments
- No mixing of FlatBuffers, intermediates, or god objects

---

## Conclusion

### What We Have

A **cohesive, extensible architecture** for scene data loading that:
- ✅ Separates concerns (providers vs configurators vs factories)
- ✅ Abstracts data formats (FlatBuffers hidden behind interfaces)
- ✅ Supports both default and save scenes (unified pipeline)
- ✅ Handles nested data efficiently (extractor pattern)
- ✅ Uses polymorphism appropriately (format abstraction)
- ✅ Uses overloads appropriately (source differentiation)
- ✅ Maintains consistency with existing patterns (IEngineDataProvider, etc.)

### Implementation Status

| Phase | Status | Risk | Effort |
|-------|--------|------|--------|
| Engine Data Loading | ✅ Complete | ⬜ Low | Done |
| Scene Metadata Loading | ✅ Complete | ⬜ Low | Done |
| Extractor Pattern | ⬜ Not Started | ⬜ Low | 2-3 days |
| Entity Data Provider | ⬜ Not Started | 🟡 Medium | 1-2 weeks |
| Save Scene States | ⬜ Not Started | 🔴 High | 2-3 weeks |
| Scene Factory Overloads | ⬜ Not Started | ⬜ Low | 3-5 days |

### Next Steps

1. **Review this analysis** with team
2. **Validate architecture** aligns with vision
3. **Prioritize phases** based on immediate needs
4. **Implement Phase 2** (Extractor Pattern) - low risk, high value
5. **Implement Phase 3** (Entity Data Provider) - moderate effort, completes abstraction

---

**Document Status**: Analysis Complete - Ready for Review and Implementation Planning
