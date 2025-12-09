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
// Factory gets FlatBuffers data directly from configurator
const SceneDataFbs *scene_data = m_scene_configurator.GetSceneData();

// Configure Scene directly from FlatBuffers
scene.m_scene_info.scene_id = scene_data->scene_info()->scene_id()->str();
```

#### 3. Two Data Sources

Both sources work through the same interface:

```cpp
// Default scene loading
ISceneDataProvider &provider = GetSceneDataProvider();
DefaultSceneConfigurator config(SceneType::TITLE, provider);
SceneFactory factory(game_context, config);  // Same factory!

// Saved scene loading  
ISaveDataProvider &save_provider = GetSaveDataProvider();
SavedSceneConfigurator config(0, save_provider);
SceneFactory factory(game_context, config);  // Same factory!
```

#### 4. Strategy Pattern

Configurators ARE the strategy:

```cpp
class ISceneConfigurator {  // Strategy interface
  virtual const SceneDataFbs* GetSceneData() const = 0;
};

class DefaultSceneConfigurator : public ISceneConfigurator {
  // Strategy for default data
};

class SavedSceneConfigurator : public ISceneConfigurator {
  // Strategy for save data
};
```

SceneFactory uses the strategy without knowing which concrete implementation:

```cpp
class ISceneFactory {
  const ISceneConfigurator &m_scene_configurator;  // Strategy reference
  
  void Configure() {
    const SceneDataFbs *data = m_scene_configurator.GetSceneData();
    // Works with ANY configurator!
  }
};
```

#### 5. Marry Data Types With Sources

The configurator pattern naturally handles this:

| Data Source | Provider Type | Configurator | Factory Gets |
|-------------|---------------|--------------|--------------|
| Default files | ISceneDataProvider | DefaultSceneConfigurator | SceneDataFbs* |
| Save files | ISaveDataProvider | SavedSceneConfigurator | SceneDataFbs* |
| Test data | (Mock providers) | (Test configurator) | SceneDataFbs* |
| JSON (future) | IJSONSceneProvider | JSONSceneConfigurator | SceneDataFbs* |

All sources → All configurators → Same factory interface

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
│ 5. Factory calls config.GetSceneData()               │
│    • Configurator loads from provider                │
│    • Configurator caches SceneDataFbs*               │
│    • Configurator returns SceneDataFbs*              │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓
┌──────────────────────────────────────────────────────┐
│ 6. Factory calls config.CreateEntityConfigurator()   │
│    • Configurator extracts EntityCollectionFbs       │
│    • Configurator creates FlatbuffersEntity          │
│      Configurator with entity data                   │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓
┌──────────────────────────────────────────────────────┐
│ 7. Factory creates and configures Scene              │
│    • Uses SceneDataFbs* from configurator            │
│    • Uses EntityConfigurator from configurator       │
│    • Directly populates Scene members                │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓
┌──────────────────────────────────────────────────────┐
│ 8. SceneManager stores configured Scene              │
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
│ 5. Factory calls config.GetSceneData()               │
│    • Configurator loads SaveDataFbs from provider    │
│    • Configurator extracts SceneDataFbs field        │
│    • Configurator caches SceneDataFbs*               │
│    • Configurator returns SceneDataFbs*              │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓ (REST IS IDENTICAL TO DEFAULT LOADING)
┌──────────────────────────────────────────────────────┐
│ 6. Factory calls config.CreateEntityConfigurator()   │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓
┌──────────────────────────────────────────────────────┐
│ 7. Factory creates and configures Scene              │
└─────────────────────┬────────────────────────────────┘
                      │
                      ↓
┌──────────────────────────────────────────────────────┐
│ 8. SceneManager stores configured Scene              │
└──────────────────────────────────────────────────────┘
```

---

## Key Design Decisions

### 1. Configurators Cache Data

```cpp
class DefaultSceneConfigurator {
  mutable const SceneDataFbs *m_cached_scene_data{nullptr};
  
  const SceneDataFbs* GetSceneData() const override {
    if (m_cached_scene_data) {
      return m_cached_scene_data;  // Return cached
    }
    // Load and cache
    auto result = m_scene_data_provider.LoadSceneData(m_scene_type);
    m_cached_scene_data = ExtractFlatBuffers(result);
    return m_cached_scene_data;
  }
};
```

**Reason**: Avoid repeated provider calls during Scene configuration

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

### 3. Factory References Configurator (Doesn't Own)

```cpp
class ISceneFactory {
protected:
  const ISceneConfigurator &m_scene_configurator;  // Reference, not ownership
  
public:
  ISceneFactory(const GameContext &game_context,
                const ISceneConfigurator &configurator)
      : m_game_context(game_context),
        m_scene_configurator(configurator) {}
};
```

**Reason**: Factory only needs configurator during CreateScene(), doesn't outlive it

### 4. SavedSceneConfigurator Extracts SceneDataFbs

```cpp
const SceneDataFbs* SavedSceneConfigurator::GetSceneData() const {
  if (m_cached_scene_data) {
    return m_cached_scene_data;
  }
  
  // Load SaveDataFbs
  auto save_result = m_save_data_provider.LoadSave(m_save_slot_index);
  if (!save_result.has_value()) {
    return nullptr;
  }
  
  // Extract scene_data field
  const SaveDataFbs *save_data = GetSaveDataFbs(save_result.value());
  m_cached_scene_data = save_data->scene_data();
  
  return m_cached_scene_data;
}
```

**Reason**: Factory only needs SceneDataFbs, doesn't care about SaveDataFbs wrapper

---

## Addressing "Sticking Points"

### Original Concern: "How we marry multiple data types with multiple data sources"

**Solution**: Configurators bridge the gap

```
Data Type 1: SceneDataFbs (in default files)
    ↓ accessed by
DefaultSceneConfigurator
    ↓ provides
SceneDataFbs* → SceneFactory

Data Type 2: SaveDataFbs (contains SceneDataFbs)
    ↓ accessed by
SavedSceneConfigurator
    ↓ extracts and provides
SceneDataFbs* → SceneFactory (same interface!)
```

### Original Concern: "Cannot use IDataProvider if no structure to provide"

**Clarification**: Providers DO provide structures, but configurators translate them:

```cpp
// Provider provides its native format
class ISceneDataProvider {
  virtual std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType) const = 0;
};

// Configurator extracts FlatBuffers from provider result
class DefaultSceneConfigurator {
  const SceneDataFbs* GetSceneData() const {
    auto result = m_provider.LoadSceneData(m_scene_type);
    return ExtractFlatBuffers(result);  // Get FlatBuffers from result
  }
};
```

The provider returns what it loads (including FlatBuffers data), the configurator extracts what the factory needs.

---

## Benefits Summary

### ✅ Decoupling Achieved

- Factory never sees provider types
- Factory never stores FlatBuffers pointers
- Game code works with interfaces

### ✅ No Duplication

- No intermediate native struct
- Single configuration logic
- FlatBuffers as single source of truth

### ✅ Extensible

Easy to add new data sources:

```cpp
class NetworkSceneConfigurator : public ISceneConfigurator {
  // Network provider internally
  // Same GetSceneData() interface
};

class LuaSceneConfigurator : public ISceneConfigurator {
  // Lua provider internally
  // Same GetSceneData() interface
};
```

### ✅ Testable

Mock configurators for testing:

```cpp
class MockSceneConfigurator : public ISceneConfigurator {
  const SceneDataFbs* GetSceneData() const override {
    return m_test_scene_data;
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
- [ ] Implement `DefaultSceneConfigurator`
- [ ] Implement `SavedSceneConfigurator`
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

**A**: Configurators return FlatBuffers pointers directly. Factory configures from FlatBuffers.

### Q: How do we handle save data differently from default data?

**A**: Different configurators for different sources. SavedSceneConfigurator extracts SceneDataFbs from SaveDataFbs.

### Q: Where does the strategy pattern fit?

**A**: Configurators ARE the strategy. SceneFactory receives ISceneConfigurator (strategy interface).

### Q: How do we marry multiple data types with sources?

**A**: Configurators bridge the gap. Each configurator knows its provider, returns unified SceneDataFbs* to factory.

---

## Status

**Planning**: ✅ Complete  
**Documentation**: ✅ Complete  
**Implementation**: ⏸️ Not Started  
**Testing**: ⏸️ Not Started

**Next Action**: Begin Phase 1 implementation (create interfaces)

---

**Last Updated**: December 9, 2025
