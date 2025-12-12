# Data Handling Architecture - Executive Summary

**Created**: December 12, 2025  
**Purpose**: Address user concerns about data handling abstraction and architecture

---

## Your Questions Answered

### Q1: "Does this mean we don't actually want the FlatbuffersDefaultSceneConfigurator to be loading the data directly?"

**Answer**: ✅ Correct. The configurator should NOT load data.

**Recommendation**: Create `ISceneDataProvider` to load and convert data to native C++ structs, then pass those structs to the configurator.

**Why**: This maintains abstraction and allows the same configurator to work with data from any source (default files, save files, XML, test data).

---

### Q2: "Do we want to be passing it a strategy or just have the FlatbuffersSaveDataSceneConfigurator take in the SaveDataFbs?"

**Answer**: Pass native C++ data structs to the configurator. Don't have it load FlatBuffers internally.

**Better approach**: You don't need separate `FlatbuffersSaveDataSceneConfigurator` at all! Use ONE `DefaultSceneConfigurator` that works with native data from ANY provider (default, save, test, etc.).

**Why**: Avoids combinatorial explosion of configurator classes (N scene types × M data sources = disaster).

---

### Q3: "Maybe it's better to have the ISceneConfigurator just produce a Scene object itself rather than the SceneFactory?"

**Answer**: ❌ No. Keep Factory and Configurator separate.

**Reason**: 
- **Factory** knows about scene types (Title, Crafting) - creates empty objects
- **Configurator** knows how to apply data - works with ANY scene type
- Merging them creates the same combinatorial explosion problem

**Keep the separation**: It follows Single Responsibility Principle and makes both components reusable.

---

## The Solution: Three-Component Pattern

```
┌──────────────────┐
│  Data Provider   │ ← Abstracts data source (FlatBuffers/XML/Save)
│  ISceneDataProvider
└────────┬─────────┘
         │ SceneData (native C++)
         ↓
┌──────────────────┐
│     Factory      │ ← Creates empty Scene objects
│  SceneFactory    │
└────────┬─────────┘
         │ Scene*
         ↓
┌──────────────────┐
│  Configurator    │ ← Applies data to Scene
│  ISceneConfigurator
└──────────────────┘
```

### Component Responsibilities

| Component | Purpose | Knows About | Does NOT |
|-----------|---------|-------------|----------|
| **ISceneDataProvider** | Load and convert data | Data formats (FlatBuffers, XML) | Create objects, Configure |
| **SceneFactory** | Create objects | Scene types (Title, Crafting) | Load data, Configure |
| **ISceneConfigurator** | Apply data | Configuration logic | Load data, Create objects |

---

## How This Solves Your Concerns

### 1. Nested Data (EntityCollectionFbs inside SceneDataFbs)

**Current concern**: How do we pass EntityCollectionFbs down to FlatbuffersEntityConfigurator without creating EntityMemoryPool at the wrong level?

**Solution**: 
- Provider converts **entire nested structure** to native C++
- `SceneData` contains native `EntityCollection` (not FlatBuffers pointer)
- SceneConfigurator passes native `EntityCollection` to EntityConfigurator
- No FlatBuffers pointers pass between configuration layers

**Example**:
```cpp
// Provider converts everything to native
struct SceneData {
  SceneInfo info;             // Native
  SceneResources resources;   // Native
  EntityCollection entities;  // Native (not FlatBuffers!)
  LogicCollectionData logic;  // Native
};

// Configurator receives and passes native data
void ConfigureScene(Scene &scene, const SceneData &data) {
  ConfigureSceneInfo(scene, data.info);
  ConfigureSceneResources(scene, data.resources);
  
  // Pass native EntityCollection to entity configurator
  FlatbuffersEntityConfigurator entity_config(event_handler, data.entities);
  entity_config.ConfigureEntityMemoryPool(scene.GetEntityMemoryPool());
}
```

### 2. Multiple Data Sources (Default vs Save)

**Current concern**: How do we support both default data and save data without duplicating code?

**Solution**: Use different providers, same factory, same configurator.

**Default Scene**:
```cpp
ISceneDataProvider &provider = GetDefaultSceneDataProvider();
auto scene_data = provider.LoadSceneData(scene_type);

SceneFactory factory(game_context);
auto scene = factory.CreateSceneByType(scene_type);

ISceneConfigurator &configurator = GetSceneConfigurator();
configurator.ConfigureScene(*scene, scene_data.value());
```

**Save Scene**:
```cpp
ISceneDataProvider &provider = GetSaveSceneDataProvider();  // ← Only change!
auto scene_data = provider.LoadSceneData(scene_type, save_data);

// Factory and Configurator are IDENTICAL
SceneFactory factory(game_context);
auto scene = factory.CreateSceneByType(scene_type);

ISceneConfigurator &configurator = GetSceneConfigurator();
configurator.ConfigureScene(*scene, scene_data.value());
```

**Key insight**: Only the provider changes between default and save data.

### 3. Keeping Abstraction Clean

**Current concern**: We're coupling to FlatBuffers when we should be abstract.

**Solution**: FlatBuffers only appear in provider implementations (`.cpp` files), never in interfaces or business logic.

**Abstraction layers**:
```
┌─────────────────────────────────────────────────┐
│  Business Logic (Scene, SceneManager, etc.)     │
│  - Only sees: ISceneDataProvider, SceneData     │
│  - Never sees: FlatBuffers, .bin files          │
└─────────────────────────────────────────────────┘
                      │
                      ↓ Interface
┌─────────────────────────────────────────────────┐
│  Provider Interface (ISceneDataProvider)         │
│  - Returns: Native C++ structs                   │
└─────────────────────────────────────────────────┘
                      │
                      ↓ Implementation
┌─────────────────────────────────────────────────┐
│  Provider Impl (FlatbuffersDefaultSceneDataProvider) │
│  - Loads: .bin files                             │
│  - Uses: FlatbuffersDataLoader                   │
│  - Converts: FlatBuffers → Native C++            │
└─────────────────────────────────────────────────┘
```

---

## What Needs to Change

### Current Architecture Issues

1. ❌ `FlatbuffersDefaultSceneConfigurator` has `FlatbuffersDataLoader` member
2. ❌ Configurator loads data in `ConfigureSceneInfo()` and `ConfigureSceneResources()`
3. ❌ No `ISceneDataProvider` abstraction
4. ❌ Redundant data loading (same data loaded multiple times)

### Migration Steps

#### Phase 1: Create ISceneDataProvider
```cpp
// 1. Define interface
class ISceneDataProvider {
  virtual std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType type) const = 0;
};

// 2. Define native struct
struct SceneData {
  SceneInfo info;
  SceneResources resources;
  EntityCollection entities;
  LogicCollectionData logic;
};

// 3. Implement FlatBuffers provider
class FlatbuffersDefaultSceneDataProvider : public ISceneDataProvider {
  std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType type) const override;
};

// 4. Add to provider factory
ISceneDataProvider &GetSceneDataProvider();
```

#### Phase 2: Update Configurator
```cpp
// 1. Change interface to receive data
class ISceneConfigurator {
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData &data) = 0;
  // Note: Takes SceneData&, not SceneType
};

// 2. Remove FlatbuffersDataLoader from configurator
// 3. Rename: FlatbuffersDefaultSceneConfigurator → DefaultSceneConfigurator
// 4. Update implementation to use native structs
```

#### Phase 3: Update SceneManager
```cpp
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromDefault(SceneType type) {
  
  // Load via provider
  ISceneDataProvider &provider = GetSceneDataProvider();
  auto scene_data = provider.LoadSceneData(type);
  
  // Create via factory
  SceneFactory factory(m_game_context);
  auto scene = factory.CreateSceneByType(type);
  
  // Configure via configurator
  ISceneConfigurator &configurator = GetSceneConfigurator();
  configurator.ConfigureScene(*scene, scene_data.value());
  
  // Add to scenes
  m_scenes.emplace(scene->GetSceneInfo().id, std::move(scene));
  
  return std::monostate{};
}
```

#### Phase 4: Add Save Support
```cpp
// Implement save provider
class FlatbuffersSaveSceneDataProvider : public ISceneDataProvider {
  std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType type, const SaveData &save) const;
};

// Add save method (reuses factory and configurator!)
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromSave(const SaveData &save, SceneType type) {
  ISceneDataProvider &provider = GetSaveSceneDataProvider();
  // Rest is identical to AddSceneFromDefault!
}
```

---

## Key Principles

### 1. Providers Provide Data (Native C++)
- Load from any source (.bin, XML, save files)
- Convert to native C++ structs
- Hide implementation details

### 2. Factories Create Objects (Empty)
- Know about scene types
- Instantiate correct derived class
- Return empty, unconfigured objects

### 3. Configurators Apply Data (Generic)
- Receive native C++ data
- Apply to objects
- Work with ANY data source

### 4. Never Pass FlatBuffers Between Layers
- Convert at provider boundary
- Use native types everywhere else
- Maintain abstraction

---

## Benefits

### ✅ Maintainability
- Clear responsibilities
- Each component has ONE job
- Easy to understand data flow

### ✅ Extensibility
- Add new data sources: Create new provider (XML, JSON, procedural)
- Add new scene types: Update factory switch
- Configuration logic: Reuse same configurator

### ✅ Testability
- Mock providers easily
- Test components independently
- No FlatBuffers coupling in tests

### ✅ Reusability
- ONE configurator for all data sources
- ONE factory for all scenes
- Providers per data source only

### ✅ Scalability
- N data sources = N providers (linear growth)
- NOT N scene types × M data sources (exponential growth)

---

## Documentation References

For more details, see:

- **Comprehensive Analysis**: `documentation/analysis/DATA_HANDLING_ABSTRACTION_ANALYSIS.md`
- **Quick Reference**: `documentation/analysis/DATA_HANDLING_QUICK_REF.md`
- **Visual Diagrams**: `documentation/analysis/DATA_HANDLING_DIAGRAMS.md`

---

## Summary

**Current State**: Configurators load data directly → tightly coupled to FlatBuffers

**Recommended State**: Providers load and convert → Configurators receive native data

**Key Change**: Create `ISceneDataProvider` interface and refactor configurators to receive data instead of loading it.

**Result**: Clean abstraction, reusable components, easy to extend with new data sources (save files, XML, test data).

---

## Final Recommendation

**Do NOT** make SceneConfigurator create Scene objects. Keep the three-component pattern:

1. **ISceneDataProvider** - Load and convert data
2. **SceneFactory** - Create empty objects  
3. **ISceneConfigurator** - Apply data to objects

This is the cleanest, most maintainable, and most extensible architecture for your use case.

The current confusion arises from configurators loading data directly. Fix this by introducing the provider abstraction, and everything else falls into place naturally.
