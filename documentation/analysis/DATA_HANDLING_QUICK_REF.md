# Data Handling Architecture - Quick Reference

## TL;DR

**Problem**: SceneConfigurator currently loads FlatBuffers data directly, violating abstraction.

**Solution**: Create ISceneDataProvider to abstract data sources, pass native C++ data to configurator.

---

## Three-Component Pattern

```
┌──────────────────┐
│  Data Provider   │ ← Abstracts data source (FlatBuffers/XML/Save)
└────────┬─────────┘
         │ SceneData (native C++)
         ↓
┌──────────────────┐
│     Factory      │ ← Creates empty Scene objects
└────────┬─────────┘
         │ Scene*
         ↓
┌──────────────────┐
│  Configurator    │ ← Applies data to Scene
└──────────────────┘
```

---

## Component Responsibilities

| Component | Responsibility | Does NOT |
|-----------|---------------|----------|
| **ISceneDataProvider** | Load and convert data to native C++ structs | Create objects, Configure objects |
| **SceneFactory** | Instantiate correct Scene subclass | Load data, Configure Scene |
| **ISceneConfigurator** | Apply native data to Scene | Load data, Create objects |

---

## Current vs Recommended

### Current (Problematic)

```cpp
// ❌ Configurator loads data directly
class FlatbuffersDefaultSceneConfigurator : public ISceneConfigurator {
private:
  FlatbuffersDataLoader m_data_loader;  // Tightly coupled!
public:
  ConfigureSceneInfo(Scene &scene, SceneType type) override {
    auto data = m_data_loader.ProvideDefaultSceneData(type);
    // Configure scene...
  }
};

// Usage
ISceneConfigurator &configurator = GetDefaultSceneConfigurator();
SceneFactory factory(game_context, configurator);
auto scene = factory.CreateAndConfigureScene(scene_type);
```

### Recommended (Clean Abstraction)

```cpp
// ✅ Provider abstracts data loading
class ISceneDataProvider {
  virtual std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType type) const = 0;
};

// ✅ Configurator receives native data
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

## Why This Pattern?

### ✅ Supports Multiple Data Sources Easily

```cpp
// Default data
ISceneDataProvider &provider = GetDefaultSceneDataProvider();

// Save data
ISceneDataProvider &provider = GetSaveSceneDataProvider();

// Test data
ISceneDataProvider &provider = GetTestSceneDataProvider();

// Same factory and configurator for all!
```

### ✅ No Combinatorial Explosion

**Bad (Current path)**: Need configurator per scene × data source:
- FlatbuffersDefaultTitleConfigurator
- FlatbuffersDefaultCraftingConfigurator
- FlatbuffersSaveTitleConfigurator
- FlatbuffersSaveCraftingConfigurator
- XMLDefaultTitleConfigurator
- ... 😱

**Good (Recommended)**: Need provider per data source only:
- FlatbuffersDefaultSceneDataProvider
- FlatbuffersSaveSceneDataProvider
- XMLSceneDataProvider
- One SceneFactory (all scene types)
- One DefaultSceneConfigurator (all data sources)

---

## Key Data Structures

### SceneData (Native C++)

```cpp
struct SceneData {
  SceneInfo info;              // Scene metadata
  SceneResources resources;    // Render textures, etc.
  SceneConfig config;          // Configuration
  EntityCollection entities;   // Entity data (also native!)
  LogicCollectionData logic;   // Logic configuration
};
```

### Provider Interface

```cpp
class ISceneDataProvider {
public:
  virtual ~ISceneDataProvider() = default;
  
  virtual std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType type) const = 0;
};
```

### Configurator Interface

```cpp
class ISceneConfigurator {
public:
  virtual ~ISceneConfigurator() = default;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData &data) = 0;
};
```

---

## Migration Checklist

### Phase 1: Create Provider Abstraction
- [ ] Create `ISceneDataProvider.h`
- [ ] Create `SceneData` native struct
- [ ] Implement `FlatbuffersDefaultSceneDataProvider`
- [ ] Add `GetSceneDataProvider()` to `provider_factory.h`

### Phase 2: Update Configurator
- [ ] Change `ConfigureScene()` signature to take `SceneData`
- [ ] Remove `FlatbuffersDataLoader` from configurator
- [ ] Rename `FlatbuffersDefaultSceneConfigurator` → `DefaultSceneConfigurator`
- [ ] Update implementation to use native structs

### Phase 3: Update SceneManager
- [ ] Load data via provider
- [ ] Pass data to configurator
- [ ] Update `AddSceneFromDefault()`

### Phase 4: Add Save Support
- [ ] Implement `FlatbuffersSaveSceneDataProvider`
- [ ] Create `AddSceneFromSave()`
- [ ] Reuse same Factory and Configurator

---

## Code Examples

### Loading Default Scene Data

```cpp
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromDefault(SceneType type) {
  
  // 1. Load data via provider
  ISceneDataProvider &provider = GetDefaultSceneDataProvider();
  auto scene_data_result = provider.LoadSceneData(type);
  if (!scene_data_result.has_value())
    return std::unexpected(scene_data_result.error());
  
  // 2. Create empty scene via factory
  SceneFactory factory(m_game_context);
  auto scene_result = factory.CreateSceneByType(type);
  if (!scene_result.has_value())
    return std::unexpected(scene_result.error());
  
  // 3. Configure scene via configurator
  ISceneConfigurator &configurator = GetSceneConfigurator();
  auto config_result = configurator.ConfigureScene(
      *scene_result.value(), scene_data_result.value());
  if (!config_result.has_value())
    return std::unexpected(config_result.error());
  
  // 4. Add to scene map
  m_scenes.emplace(
      scene_result.value()->GetSceneInfo().id, 
      std::move(scene_result.value()));
  
  return std::monostate{};
}
```

### Loading Scene from Save Data

```cpp
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromSave(const SaveData &save, SceneType type) {
  
  // 1. Load data via DIFFERENT provider
  ISceneDataProvider &provider = GetSaveSceneDataProvider();
  auto scene_data_result = provider.LoadSceneData(type, save);
  if (!scene_data_result.has_value())
    return std::unexpected(scene_data_result.error());
  
  // 2-4. Same as default! (Factory, Configurator, Add)
  SceneFactory factory(m_game_context);
  auto scene_result = factory.CreateSceneByType(type);
  
  ISceneConfigurator &configurator = GetSceneConfigurator();
  auto config_result = configurator.ConfigureScene(
      *scene_result.value(), scene_data_result.value());
  
  m_scenes.emplace(
      scene_result.value()->GetSceneInfo().id, 
      std::move(scene_result.value()));
  
  return std::monostate{};
}
```

**Notice**: Only the provider changes! Factory and Configurator are identical.

---

## Provider Implementation Pattern

```cpp
class FlatbuffersDefaultSceneDataProvider : public ISceneDataProvider {
private:
  FlatbuffersDataLoader m_loader;  // Implementation detail
  
public:
  std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType type) const override {
    
    // 1. Load FlatBuffers data
    auto fbs_result = m_loader.ProvideDefaultSceneData(type);
    if (!fbs_result.has_value())
      return std::unexpected(fbs_result.error());
    
    const SceneDataFbs *fbs = fbs_result.value();
    
    // 2. Convert to native C++ struct
    SceneData data;
    
    if (fbs->scene_info()) {
      data.info.type = fbs->scene_info()->scene_type();
      // ... more conversions
    }
    
    if (fbs->entity_collection()) {
      data.entities.entity_memory_pool_size = 
          fbs->entity_collection()->entity_memory_pool_size();
      // Convert entities...
    }
    
    // 3. Return native data
    return data;
  }
};
```

---

## Questions & Answers

### Q: Should SceneConfigurator load data directly?

**A**: No. Pass data TO configurator, don't load it WITHIN configurator.

### Q: Should we eliminate SceneFactory?

**A**: No. Keep Factory for object creation, Configurator for data application. Separate concerns.

### Q: How do we support both default and save data?

**A**: Different providers, same factory, same configurator.

### Q: Don't we need different configurators for FlatBuffers vs XML?

**A**: No! Configurator receives native C++ structs, so it's data-source agnostic.

### Q: What about nested data (EntityCollectionFbs inside SceneDataFbs)?

**A**: Provider converts entire nested structure to native C++ before passing to configurator.

---

## Benefits Summary

| Benefit | Description |
|---------|-------------|
| **Full Abstraction** | No FlatBuffers leakage into business logic |
| **Extensible** | Add XML, JSON sources without touching configurators |
| **Testable** | Mock providers easily for tests |
| **Reusable** | Same configurator works with all data sources |
| **Maintainable** | Clear responsibilities, no confusion |
| **Scalable** | N data sources = N providers (not N×M configurators) |

---

## Anti-Patterns to Avoid

### ❌ Configurator Loading Data

```cpp
class Configurator {
  FlatbuffersDataLoader m_loader;  // Bad!
  ConfigureScene(Scene &s, SceneType t) {
    auto data = m_loader.Load(t);  // Couples to FlatBuffers
  }
};
```

### ❌ Configurator Creating Objects

```cpp
class Configurator {
  std::unique_ptr<Scene> CreateAndConfigure(SceneType t) {
    auto scene = CreateScene(t);  // Wrong responsibility!
    Configure(scene);
    return scene;
  }
};
```

### ❌ Factory Knowing About Data Format

```cpp
class Factory {
  CreateScene(const SceneDataFbs *fbs) {  // Couples to FlatBuffers!
    auto scene = new TitleScene(...);
    return scene;
  }
};
```

---

## Correct Pattern

✅ **Provider loads and converts** → ✅ **Factory creates empty** → ✅ **Configurator applies data**

Each component has ONE job, uses native types at boundaries, and is independently testable and reusable.
