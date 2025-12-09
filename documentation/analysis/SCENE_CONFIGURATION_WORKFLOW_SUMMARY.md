# Scene Configuration Workflow - Planning Summary

**Addresses problem statement: workflow from file data to fully configured Scene**

**Date**: December 9, 2025  
**Status**: Planning Complete - Implementation Not Started

---

## Problem Statement Addressed

### Original Requirements

From the problem statement:
1. ✅ **Avoid coupling FlatBuffers**: No direct FlatBuffers references in game code
2. ✅ **No intermediate native struct**: Go straight from SceneDataFbs to Scene
3. ✅ **Two data sources**: Default data and save data (both contain SceneDataFbs)
4. ✅ **Strategy pattern**: Use configurator strategy, not abstract factory in wrong place
5. ✅ **Marry data types with sources**: Unified approach for multiple configurations

### Key Insight

> "I'm assuming this prevents us from actually using an IDataProvider interface as there is not structure to provide."

**Answer**: We DO use IDataProvider interfaces, but configurators wrap them internally!

```
ISceneDataProvider (provides SceneDataFbs) 
    ↓ wrapped by
DefaultSceneConfigurator (strategy)
    ↓ used by
SceneFactory (consumer)
```

The provider still provides data, but the **configurator** encapsulates provider access and converts it to what the factory needs.

---

## Solution Architecture

### Three-Layer Approach

```
Layer 1: Data Providers (source abstraction)
    • ISceneDataProvider
    • ISaveDataProvider

Layer 2: Configurators (strategy pattern)
    • ISceneConfigurator (interface)
    • DefaultSceneConfigurator (concrete strategy)
    • SavedSceneConfigurator (concrete strategy)

Layer 3: Factory (consumer)
    • ISceneFactory
    • FlatbuffersSceneFactory
```

### How It Solves The Problem

#### 1. Avoid Coupling FlatBuffers

**Before (❌):**
```cpp
class FlatbuffersSceneFactory {
  const SceneDataFbs *m_scene_data_fbs;  // Direct coupling!
};
```

**After (✅):**
```cpp
class FlatbuffersSceneFactory {
  const ISceneConfigurator &m_scene_configurator;  // Interface!
  // Factory never stores FlatBuffers pointers
};
```

#### 2. No Intermediate Struct

**We DON'T do this:**
```cpp
struct SceneData {
  std::string scene_id;
  EntityCollection entities;  // Complex nested data
  // ... duplicates FlatBuffers structure
};

SceneData ConvertFromFlatBuffers(const SceneDataFbs*);  // ❌ Duplication
Scene ConfigureFromNative(const SceneData&);  // ❌ Extra step
```

**We DO this:**
```cpp
// Configurator configures Scene directly - FlatBuffers encapsulated inside
m_scene_configurator.ConfigureScene(scene, game_context);

// NO FlatBuffers types visible to factory!
// Configurator handles all FlatBuffers access internally
```

#### 3. Two Data Sources

Both sources work through the same interface:

```cpp
// Default scene loading
ISceneDataProvider &provider = GetSceneDataProvider();
DefaultSceneConfigurator config(SceneType::TITLE, provider);
SceneFactory factory(game_context, config);  // Same factory!
auto scene = factory.CreateScene();  // Configurator.ConfigureScene() called internally

// Saved scene loading  
ISaveDataProvider &save_provider = GetSaveDataProvider();
SavedSceneConfigurator config(0, save_provider);
SceneFactory factory(game_context, config);  // Same factory!
auto scene = factory.CreateScene();  // Configurator.ConfigureScene() called internally
```

#### 4. Strategy Pattern

Configurators ARE the strategy:

```cpp
class ISceneConfigurator {  // Strategy interface
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const GameContext &game_context) = 0;
  // NO FlatBuffers types in interface!
};

class DefaultSceneConfigurator : public ISceneConfigurator {
  // Strategy for default data - FlatBuffers encapsulated internally
};

class SavedSceneConfigurator : public ISceneConfigurator {
  // Strategy for save data - FlatBuffers encapsulated internally
};
```

SceneFactory uses the strategy without knowing which concrete implementation:

```cpp
class ISceneFactory {
  const ISceneConfigurator &m_scene_configurator;  // Strategy reference
  
  std::expected<std::unique_ptr<Scene>, FailInfo> CreateScene() {
    auto scene = CreateSceneByType();
    // Configurator does all the configuration work
    m_scene_configurator.ConfigureScene(*scene, m_game_context);
    return scene;
    // Factory never sees FlatBuffers!
  }
};
```

#### 5. Marry Data Types With Sources

The configurator pattern naturally handles this:

| Data Source | Provider Type | Configurator | Configurator Calls |
|-------------|---------------|--------------|-------------------|
| Default files | ISceneDataProvider | DefaultSceneConfigurator | ConfigureScene(Scene&, GameContext&) |
| Save files | ISaveDataProvider | SavedSceneConfigurator | ConfigureScene(Scene&, GameContext&) |
| Test data | (Mock providers) | (Test configurator) | ConfigureScene(Scene&, GameContext&) |
| JSON (future) | IJSONSceneProvider | JSONSceneConfigurator | ConfigureScene(Scene&, GameContext&) |

All sources → All configurators → Same factory interface → NO FlatBuffers exposure

---

## Complete Workflow

### Default Scene Loading

```
┌──────────────────────────────────────────────────────┐
│ 1. SceneManager::LoadSceneFromDefault(SceneType)     │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓
┌──────────────────────────────────────────────────────┐
│ 2. Get ISceneDataProvider singleton                  │
│    ISceneDataProvider &provider = GetProvider();     │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓
┌──────────────────────────────────────────────────────┐
│ 3. Create DefaultSceneConfigurator                   │
│    DefaultSceneConfigurator config(type, provider);  │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓
┌──────────────────────────────────────────────────────┐
│ 4. Create SceneFactory with configurator             │
│    FlatbuffersSceneFactory factory(context, config); │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓
┌──────────────────────────────────────────────────────┐
│ 5. Factory.CreateScene() creates Scene by type       │
│    auto scene = CreateSceneByType();                  │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓
┌──────────────────────────────────────────────────────┐
│ 6. Factory calls configurator.ConfigureScene()       │
│    config.ConfigureScene(*scene, game_context);      │
│    • Configurator loads FlatBuffers internally       │
│    • Configurator creates entity configurator        │
│    • Configurator configures all Scene aspects       │
│    • NO FlatBuffers visible to factory!              │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓
┌──────────────────────────────────────────────────────┐
│ 7. SceneManager stores configured Scene              │
│    m_scenes.emplace(scene_id, std::move(scene));     │
└──────────────────────────────────────────────────────┘
```

### Saved Scene Loading

```
┌──────────────────────────────────────────────────────┐
│ 1. SceneManager::LoadSceneFromSave(slot_index)       │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓
┌──────────────────────────────────────────────────────┐
│ 2. Get ISaveDataProvider singleton                   │
│    ISaveDataProvider &provider = GetSaveProvider();  │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓
┌──────────────────────────────────────────────────────┐
│ 3. Create SavedSceneConfigurator                     │
│    SavedSceneConfigurator config(slot, provider);    │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓
┌──────────────────────────────────────────────────────┐
│ 4. Create SceneFactory with configurator             │
│    FlatbuffersSceneFactory factory(context, config); │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓
┌──────────────────────────────────────────────────────┐
│ 5. Factory.CreateScene() creates Scene by type       │
│    auto scene = CreateSceneByType();                  │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓
┌──────────────────────────────────────────────────────┐
│ 6. Factory calls configurator.ConfigureScene()       │
│    config.ConfigureScene(*scene, game_context);      │
│    • Configurator loads SaveDataFbs internally       │
│    • Configurator extracts SceneDataFbs internally   │
│    • Configurator creates entity configurator        │
│    • Configurator configures all Scene aspects       │
│    • NO FlatBuffers visible to factory!              │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓
┌──────────────────────────────────────────────────────┐
│ 7. SceneManager stores configured Scene              │
└──────────────────────────────────────────────────────┘
```

---

## Key Design Decisions

### 1. Configurators Encapsulate FlatBuffers

```cpp
class DefaultSceneConfigurator {
private:
  // FlatBuffers data is PRIVATE - not exposed
  mutable const SceneDataFbs *m_cached_scene_data{nullptr};
  
  // Internal helper to load data
  const SceneDataFbs* LoadSceneData() const {
    if (m_cached_scene_data) {
      return m_cached_scene_data;  // Return cached
    }
    // Load and cache
    FlatbuffersDataLoader loader;
    auto result = loader.LoadSceneData(m_scene_type);
    m_cached_scene_data = result.value();
    return m_cached_scene_data;
  }

public:
  // Public interface - NO FlatBuffers types!
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const GameContext &game_context) override {
    const SceneDataFbs *scene_data = LoadSceneData();
    if (!scene_data) {
      return std::unexpected(FailInfo{FailMode::NullPointer, "No scene data"});
    }
    
    // Configure scene directly from FlatBuffers
    // All FlatBuffers access happens inside configurator
    if (scene_data->scene_info()) {
      scene.m_scene_info.scene_id = scene_data->scene_info()->scene_id()->str();
      scene.m_scene_info.scene_type = scene_data->scene_info()->scene_type();
    }
    
    // Configure entities using internal entity configurator
    FlatbuffersEntityConfigurator entity_config(
        game_context.event_handler, 
        *scene_data->entity_collection());
    entity_config.ConfigureEntityMemoryPool(scene.GetEntityManager().GetEntityMemoryPool());
    
    return std::monostate{};
  }
};
```

**Reason**: Complete encapsulation - factory never sees FlatBuffers types

### 2. SceneManager Owns Configurators

```cpp
std::expected<std::monostate, FailInfo>
SceneManager::LoadSceneFromDefault(SceneType scene_type) {
  ISceneDataProvider &provider = GetSceneDataProvider();
  
  // Stack-allocated, lifetime matches this function
  DefaultSceneConfigurator configurator(scene_type, provider);
  
  // Passed by reference to factory
  FlatbuffersSceneFactory factory(m_game_context, configurator);
  
  auto scene = factory.CreateScene();
  // ... configurator destroyed when function exits
}
```

**Reason**: Configurator lifetime matches Scene creation, no need to persist

### 3. Factory Simplified - No Configuration Logic

```cpp
class ISceneFactory {
protected:
  const ISceneConfigurator &m_scene_configurator;  // Reference
  const GameContext &m_game_context;
  
public:
  ISceneFactory(const GameContext &game_context,
                const ISceneConfigurator &configurator)
      : m_game_context(game_context),
        m_scene_configurator(configurator) {}
  
  std::expected<std::unique_ptr<Scene>, FailInfo> CreateScene() {
    // 1. Create Scene by type (TitleScene, CraftingScene, etc.)
    auto scene = CreateSceneByType();
    if (!scene.has_value()) {
      return std::unexpected(scene.error());
    }
    
    // 2. Let configurator do ALL the work
    auto config_result = m_scene_configurator.ConfigureScene(
        *scene.value(), m_game_context);
    if (!config_result.has_value()) {
      return std::unexpected(config_result.error());
    }
    
    return scene;
  }
};
```

**Reason**: Factory's ONLY job is to create the Scene object and delegate configuration to configurator

### 4. SavedSceneConfigurator Extracts Internally

```cpp
std::expected<std::monostate, FailInfo>
SavedSceneConfigurator::ConfigureScene(Scene &scene, const GameContext &game_context) {
  // Load SaveDataFbs internally
  auto save_result = m_save_data_provider.LoadSave(m_save_slot_index);
  if (!save_result.has_value()) {
    return std::unexpected(save_result.error());
  }
  
  // Extract SceneDataFbs internally - NOT exposed!
  const SaveDataFbs *save_data = GetSaveDataFbs(save_result.value());
  const SceneDataFbs *scene_data = save_data->scene_data();
  
  if (!scene_data) {
    return std::unexpected(FailInfo{FailMode::NullPointer, "No scene data in save"});
  }
  
  // Configure scene from extracted data
  // All happens inside configurator - factory never sees this!
  // ... (same configuration logic as DefaultSceneConfigurator)
  
  return std::monostate{};
}
```

**Reason**: Save data extraction is encapsulated - factory only sees ConfigureScene() interface

---

## Addressing "Sticking Points"

### Original Concern: "How we marry multiple data types with multiple data sources"

**Solution**: Configurators bridge the gap and encapsulate everything

```
Data Type 1: SceneDataFbs (in default files)
    ↓ loaded internally by
DefaultSceneConfigurator
    ↓ calls
ConfigureScene(Scene&) → Configures Scene directly
    ↓ NO FlatBuffers exposed!

Data Type 2: SaveDataFbs (contains SceneDataFbs)
    ↓ loaded and extracted internally by
SavedSceneConfigurator
    ↓ calls
ConfigureScene(Scene&) → Configures Scene directly (same interface!)
    ↓ NO FlatBuffers exposed!
```

Factory only sees: `configurator.ConfigureScene(scene, game_context)`

### Original Concern: "Cannot use IDataProvider if no structure to provide"

**Clarification**: Providers DO provide structures, configurators use them internally:

```cpp
// Provider provides its native format
class ISceneDataProvider {
  virtual std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType) const = 0;
};

// Configurator uses provider internally and configures Scene directly
class DefaultSceneConfigurator {
private:
  const SceneDataFbs* LoadSceneData() const {
    // Load from provider, extract FlatBuffers
    FlatbuffersDataLoader loader;
    auto result = loader.LoadSceneData(m_scene_type);
    return result.value();  // FlatBuffers data
  }

public:
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const GameContext &game_context) override {
    // Load data internally
    const SceneDataFbs *scene_data = LoadSceneData();
    // Configure Scene directly
    // Factory NEVER sees FlatBuffers!
    return std::monostate{};
  }
};
```

Providers return data, configurators use it internally, factory NEVER sees FlatBuffers.

---

## Benefits Summary

### ✅ Complete Decoupling Achieved

- Factory never sees provider types
- Factory never sees FlatBuffers types (not even in parameters!)
- Configurator encapsulates ALL data access
- Game code works purely with interfaces

### ✅ No Duplication

- No intermediate native struct
- Single configuration logic
- FlatBuffers as single source of truth

### ✅ Extensible

Easy to add new data sources and formats:

#### Single Format (Current Documentation)

When using only one format (e.g., FlatBuffers):

```cpp
class DefaultSceneConfigurator : public ISceneConfigurator {
  // FlatBuffers internally, source is "default files"
};

class SavedSceneConfigurator : public ISceneConfigurator {
  // FlatBuffers internally, source is "save files"
};
```

#### Multiple Formats (Format-Prefixed Naming)

When supporting multiple formats, prefix with format name:

```cpp
// FlatBuffers format
class FlatbuffersDefaultSceneConfigurator : public ISceneConfigurator {
private:
  ISceneDataProvider &m_provider;
  const SceneDataFbs *m_cached_data{nullptr};  // FlatBuffers-specific
  
public:
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const GameContext &game_context) override {
    // Load FlatBuffers, configure Scene
  }
};

class FlatbuffersSavedSceneConfigurator : public ISceneConfigurator {
  // Same pattern for saved games with FlatBuffers
};

// XML format
class XmlDefaultSceneConfigurator : public ISceneConfigurator {
private:
  ISceneDataProvider &m_provider;
  XmlDocument *m_cached_doc{nullptr};  // XML-specific
  
public:
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const GameContext &game_context) override {
    // Load XML, configure Scene
  }
};

class XmlSavedSceneConfigurator : public ISceneConfigurator {
  // Same pattern for saved games with XML
};

// JSON format
class JsonDefaultSceneConfigurator : public ISceneConfigurator {
  // JSON from default files
};

class JsonSavedSceneConfigurator : public ISceneConfigurator {
  // JSON from save files
};
```

**Key Point**: Factory/SceneManager only see `ISceneConfigurator&` - they're format-agnostic. The concrete class name indicates both format (FlatBuffers/XML/JSON) and source (Default/Saved).

### ✅ Testable

Mock configurators for testing:

```cpp
class MockSceneConfigurator : public ISceneConfigurator {
private:
  SceneType m_test_scene_type;
  
public:
  MockSceneConfigurator(SceneType type) : m_test_scene_type(type) {}
  
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const GameContext &game_context) override {
    // Configure with test data
    scene.m_scene_info.scene_type = m_test_scene_type;
    // ...
    return std::monostate{};
  }
  
  SceneType GetSceneType() const override {
    return m_test_scene_type;
  }
};
```

---

## Implementation Checklist

### Phase 1: Interfaces (No Breaking Changes)
- [ ] Create `src/scenes/ISceneConfigurator.h`
- [ ] Write interface documentation
- [ ] Write interface tests (with mocks)

### Phase 2: Configurators (New Components)
- [ ] Implement `DefaultSceneConfigurator` (or `FlatbuffersDefaultSceneConfigurator` for multi-format)
- [ ] Implement `SavedSceneConfigurator` (or `FlatbuffersSavedSceneConfigurator` for multi-format)
- [ ] **Note**: Use format-prefixed names (e.g., `FlatbuffersDefaultSceneConfigurator`) when supporting multiple formats
- [ ] Write configurator unit tests
- [ ] Write configurator integration tests

### Phase 3: Factory Update (Breaking Change)
- [ ] Update `ISceneFactory` constructor signature
- [ ] Update `FlatbuffersSceneFactory` implementation
- [ ] Update factory to use `m_scene_configurator.GetSceneData()`
- [ ] Update entity configurator creation
- [ ] Write factory tests with mock configurators

### Phase 4: SceneManager Update (Breaking Change)
- [ ] Add `LoadSceneFromDefault(SceneType)` method
- [ ] Add `LoadSceneFromSave(uint32_t)` method
- [ ] Update existing load methods to use new API
- [ ] Write SceneManager integration tests

### Phase 5: Cleanup
- [ ] Remove old `AddSceneFromDefault()` if not needed
- [ ] Update all callers to use new API
- [ ] Update documentation
- [ ] Run full test suite

---

## Files Affected

### New Files
```
src/scenes/ISceneConfigurator.h
src/scenes/DefaultSceneConfigurator.h
src/scenes/DefaultSceneConfigurator.cpp
src/scenes/SavedSceneConfigurator.h
src/scenes/SavedSceneConfigurator.cpp

tests/scenes/ISceneConfigurator.test.cpp
tests/scenes/DefaultSceneConfigurator.test.cpp
tests/scenes/SavedSceneConfigurator.test.cpp
tests/scenes/SceneFactory_with_configurators.integration.test.cpp
```

### Modified Files
```
src/scenes/ISceneFactory.h              (constructor signature)
src/scenes/ISceneFactory.cpp            (use configurator)
src/scenes/FlatbuffersSceneFactory.h    (constructor signature)
src/scenes/FlatbuffersSceneFactory.cpp  (use configurator)
src/scenes/SceneManager.h               (new methods)
src/scenes/SceneManager.cpp             (create configurators)

tests/scenes/ISceneFactory.test.cpp     (update tests)
tests/scenes/FlatbuffersSceneFactory.test.cpp  (update tests)
tests/scenes/SceneManager.test.cpp      (update tests)
```

---

## Example Code

### Complete Usage Example

```cpp
// In SceneManager

std::expected<std::monostate, FailInfo>
SceneManager::LoadSceneFromDefault(SceneType scene_type) {
  // 1. Get singleton provider
  ISceneDataProvider &provider = GetSceneDataProvider();
  
  // 2. Create configurator (stack-allocated)
  DefaultSceneConfigurator configurator(scene_type, provider);
  
  // 3. Create factory with configurator
  FlatbuffersSceneFactory factory(m_game_context, configurator);
  
  // 4. Create Scene
  auto scene_result = factory.CreateScene();
  if (!scene_result.has_value()) {
    return std::unexpected(scene_result.error());
  }
  
  // 5. Store Scene
  auto scene_id = scene_result.value()->GetSceneInfo().id;
  m_scenes.emplace(scene_id, std::move(scene_result.value()));
  
  // 6. Load assets
  auto asset_result = m_game_context.asset_manager.LoadSceneAssets(scene_type);
  if (!asset_result.has_value()) {
    return std::unexpected(asset_result.error());
  }
  
  return std::monostate{};
}

std::expected<std::monostate, FailInfo>
SceneManager::LoadSceneFromSave(uint32_t save_slot_index) {
  // 1. Get singleton provider
  ISaveDataProvider &provider = GetSaveDataProvider();
  
  // 2. Create configurator (stack-allocated)
  SavedSceneConfigurator configurator(save_slot_index, provider);
  
  // 3-6: Same as LoadSceneFromDefault
  FlatbuffersSceneFactory factory(m_game_context, configurator);
  auto scene_result = factory.CreateScene();
  // ... rest is identical
  
  return std::monostate{};
}
```

---

## Related Documentation

- **Full Analysis**: [SCENE_FACTORY_AND_CONFIGURATOR_ANALYSIS.md](SCENE_FACTORY_AND_CONFIGURATOR_ANALYSIS.md)
- **Quick Reference**: [SCENE_FACTORY_QUICK_REF.md](SCENE_FACTORY_QUICK_REF.md)
- **Visual Diagrams**: [SCENE_FACTORY_VISUALS.md](SCENE_FACTORY_VISUALS.md)

---

## Questions Answered

### Q: Do we need IDataProvider interfaces?

**A**: Yes! Providers abstract data sources (files, network, etc.). Configurators wrap providers.

### Q: How do we avoid intermediate structs?

**A**: Configurators encapsulate FlatBuffers internally and configure Scene directly. Factory never sees FlatBuffers.

### Q: How do we handle save data differently from default data?

**A**: Different configurators for different sources. SavedSceneConfigurator loads SaveDataFbs and extracts SceneDataFbs internally. Both call ConfigureScene(Scene&).

### Q: Where does the strategy pattern fit?

**A**: Configurators ARE the strategy. SceneFactory receives ISceneConfigurator (strategy interface) and calls ConfigureScene(Scene&).

### Q: How do we marry multiple data types with sources?

**A**: Configurators bridge the gap. Each configurator knows its provider and calls ConfigureScene(Scene&) - same interface for all sources. Factory never sees data types.

---

## Status

**Planning**: ✅ Complete  
**Documentation**: ✅ Complete  
**Implementation**: ⏸️ Not Started  
**Testing**: ⏸️ Not Started

**Next Action**: Begin Phase 1 implementation (create interfaces)

---

**Last Updated**: December 9, 2025
