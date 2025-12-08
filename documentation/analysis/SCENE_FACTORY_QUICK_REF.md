# Scene Factory Data Sourcing - Quick Reference

**Last Updated**: December 8, 2025

---

## TL;DR

**Problem**: SceneFactory needs to support two data sources (default OR save).

**Solution**: Use Strategy Pattern - create `ISceneConfigurator` interface with two implementations:
- `DefaultSceneConfigurator` (wraps ISceneDataProvider)
- `SavedSceneConfigurator` (wraps ISaveDataProvider)

**Key Benefit**: SceneFactory has NO conditionals, NO null-checks, clean separation of concerns.

---

## Architecture Pattern

```
SceneManager (Orchestrator)
  ↓ decides source
  ↓ creates appropriate configurator
  ↓
ISceneConfigurator (Strategy)
  ├── DefaultSceneConfigurator  → ISceneDataProvider
  └── SavedSceneConfigurator    → ISaveDataProvider
  ↓ passed to
  ↓
FlatbuffersSceneFactory
  ↓ uses configurator.GetSceneData()
  ↓ uses configurator.CreateEntityConfigurator()
  ↓
Scene (configured)
```

---

## Key Architectural Patterns

### 1. Strategy Pattern
**Purpose**: Encapsulate data sourcing strategies  
**Benefit**: SceneFactory doesn't know about data sources

### 2. Abstract Factory Pattern
**Purpose**: Configurator creates EntityConfigurator  
**Benefit**: Configurator controls entire data flow

### 3. Caching Pattern
**Purpose**: Configurators cache FlatBuffers data  
**Benefit**: Avoid repeated provider calls

---

## Code Patterns

### Pattern 1: Loading Default Scene

```cpp
// In SceneManager
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromDefault(const SceneType& scene_type) {
    
    ISceneDataProvider& provider = GetSceneDataProvider();
    
    auto configurator = std::make_unique<DefaultSceneConfigurator>(
        provider, scene_type
    );
    
    auto factory = FlatbuffersSceneFactory(
        m_game_context,
        std::move(configurator)
    );
    
    auto scene = factory.CreateScene();
    // ...
}
```

### Pattern 2: Loading Saved Scene

```cpp
// In SceneManager (future)
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromSave(uint32_t slot_index) {
    
    ISaveDataProvider& provider = GetSaveDataProvider();
    
    auto configurator = std::make_unique<SavedSceneConfigurator>(
        provider, slot_index
    );
    
    auto factory = FlatbuffersSceneFactory(
        m_game_context,
        std::move(configurator)
    );
    
    auto scene = factory.CreateScene();
    // ...
}
```

### Pattern 3: Using Configurator in Factory

```cpp
// In FlatbuffersSceneFactory
std::expected<std::monostate, FailInfo>
FlatbuffersSceneFactory::ConfigureSceneResources(Scene& scene) {
    
    // Get data from configurator (source-agnostic)
    auto scene_data = m_scene_configurator->GetSceneData();
    if (!scene_data.has_value()) {
        return std::unexpected(scene_data.error());
    }
    
    // Use the data
    const SceneDataFbs* data = scene_data.value();
    // ...
}
```

---

## Interface Definitions

### ISceneConfigurator

```cpp
class ISceneConfigurator {
public:
    virtual ~ISceneConfigurator() = default;
    
    virtual std::expected<const SceneDataFbs*, FailInfo> 
    GetSceneData() = 0;
    
    virtual std::expected<std::unique_ptr<IEntityConfigurator>, FailInfo>
    CreateEntityConfigurator(EventHandler& event_handler) = 0;
    
    virtual SceneType GetSceneType() const = 0;
};
```

### DefaultSceneConfigurator

```cpp
class DefaultSceneConfigurator : public ISceneConfigurator {
private:
    ISceneDataProvider& m_scene_data_provider;
    SceneType m_scene_type;
    mutable const SceneDataFbs* m_cached_scene_data{nullptr};
    mutable bool m_data_loaded{false};
    
public:
    DefaultSceneConfigurator(
        ISceneDataProvider& provider,
        SceneType scene_type);
    
    // Implement interface methods
};
```

### SavedSceneConfigurator

```cpp
class SavedSceneConfigurator : public ISceneConfigurator {
private:
    ISaveDataProvider& m_save_data_provider;
    uint32_t m_slot_index;
    mutable const SceneDataFbs* m_cached_scene_data{nullptr};
    mutable bool m_data_loaded{false};
    
public:
    SavedSceneConfigurator(
        ISaveDataProvider& provider,
        uint32_t slot_index);
    
    // Implement interface methods
};
```

### Updated FlatbuffersSceneFactory

```cpp
class FlatbuffersSceneFactory : public ISceneFactory {
private:
    std::unique_ptr<ISceneConfigurator> m_scene_configurator;
    
public:
    FlatbuffersSceneFactory(
        const GameContext& game_context,
        std::unique_ptr<ISceneConfigurator> configurator);
};
```

---

## Decision Flow: Where Does Configuration Happen?

### Who Creates Configurator?
**Answer**: SceneManager

**Why**: SceneManager knows whether loading default or save.

### Who Creates EntityConfigurator?
**Answer**: ISceneConfigurator (via CreateEntityConfigurator)

**Why**: Configurator knows data source details.

### Who Uses Scene Data?
**Answer**: FlatbuffersSceneFactory

**Why**: Factory's job is to configure Scene from data.

### Who Decides Scene Type?
**Answer**: SceneManager (for default), SaveData (for saved)

**Why**: Different flows need different information sources.

---

## File Organization

### New Files to Create

```
src/scenes/
├── ISceneConfigurator.h             (interface)
├── ISceneConfigurator.cpp           (if needed)
├── DefaultSceneConfigurator.h       (implementation)
├── DefaultSceneConfigurator.cpp     (implementation)
├── SavedSceneConfigurator.h         (implementation)
└── SavedSceneConfigurator.cpp       (implementation)

tests/unit/scenes/
├── DefaultSceneConfigurator.test.cpp
└── SavedSceneConfigurator.test.cpp

tests/integration/scene_loading/
├── default_scene_loading.test.cpp
└── saved_scene_loading.test.cpp
```

### Files to Modify

```
src/scenes/
├── FlatbuffersSceneFactory.h        (constructor change)
├── FlatbuffersSceneFactory.cpp      (use configurator)
└── SceneManager.cpp                 (create configurators)
```

---

## Benefits Checklist

- ✅ No null-check conditionals in SceneFactory
- ✅ Single Responsibility Principle (each class has one job)
- ✅ Open/Closed Principle (extend with new configurators)
- ✅ Easy to test (mock configurators)
- ✅ Direct FlatBuffers access (no copying)
- ✅ Clean separation of concerns
- ✅ Type-safe (compile-time guarantees)
- ✅ Extensible (new sources = new configurator)

---

## Common Mistakes to Avoid

### ❌ DON'T: Null-Check Providers in Factory

```cpp
// BAD - Factory knows about providers
FlatbuffersSceneFactory(
    GameContext& context,
    ISceneDataProvider* default_provider,
    ISaveDataProvider* save_provider)
{
    if (default_provider) {
        // ...
    } else if (save_provider) {
        // ...
    }
}
```

### ✅ DO: Use Configurator

```cpp
// GOOD - Factory agnostic to data source
FlatbuffersSceneFactory(
    GameContext& context,
    std::unique_ptr<ISceneConfigurator> configurator)
{
    auto data = configurator->GetSceneData();
    // ...
}
```

### ❌ DON'T: Make Scene Configure Itself from Providers

```cpp
// BAD - Scene should not know about data loading
class Scene {
    void ConfigureFromDefault(ISceneDataProvider& provider);
    void ConfigureFromSave(ISaveDataProvider& provider);
};
```

### ✅ DO: Use Factory to Configure Scene

```cpp
// GOOD - Factory configures Scene
class ISceneFactory {
    std::expected<std::monostate, FailInfo>
    ConfigureSceneResources(Scene& scene);
};
```

### ❌ DON'T: Create Overloaded Virtual Methods

```cpp
// BAD - Overloads don't support polymorphism
class IEntityConfigurator {
    virtual void Configure(const EntityCollectionFbs& data) = 0;
    virtual void Configure(const SavedEntityData& data) = 0;
};
```

### ✅ DO: Use Named Virtual Methods or Variant

```cpp
// GOOD - Named methods or variant
class IEntityConfigurator {
    virtual void ConfigureFromDefault(const EntityCollectionFbs& data) = 0;
    virtual void ConfigureFromSave(const EntityCollectionFbs& data) = 0;
};
```

---

## Testing Patterns

### Unit Test: Configurator

```cpp
TEST_CASE("DefaultSceneConfigurator returns scene data", 
          "[unit][DefaultSceneConfigurator]") {
    MockSceneDataProvider provider;
    DefaultSceneConfigurator config(provider, SceneType::SceneType_TITLE);
    
    auto result = config.GetSceneData();
    REQUIRE(result.has_value());
}
```

### Unit Test: Factory with Mock Configurator

```cpp
TEST_CASE("FlatbuffersSceneFactory uses configurator", 
          "[unit][FlatbuffersSceneFactory]") {
    MockGameContext context;
    MockSceneConfigurator config;
    
    FlatbuffersSceneFactory factory(context, std::move(config));
    
    auto result = factory.CreateScene();
    REQUIRE(result.has_value());
}
```

### Integration Test: Full Flow

```cpp
TEST_CASE("SceneManager loads scene with configurator", 
          "[integration][SceneManager]") {
    GameContext context = CreateTestContext();
    SceneManager manager(context);
    
    auto result = manager.AddSceneFromDefault(SceneType::SceneType_TITLE);
    
    REQUIRE(result.has_value());
    REQUIRE(manager.GetScenes().size() == 1);
}
```

---

## Implementation Checklist

### Phase 1: Create Interfaces
- [ ] Create `ISceneConfigurator.h`
- [ ] Create `DefaultSceneConfigurator.h/.cpp`
- [ ] Create `SavedSceneConfigurator.h/.cpp` (stub)
- [ ] Write unit tests

### Phase 2: Update Factory
- [ ] Modify `FlatbuffersSceneFactory.h` (new constructor)
- [ ] Modify `FlatbuffersSceneFactory.cpp` (use configurator)
- [ ] Implement `ConfigureSceneConfig()`
- [ ] Update tests

### Phase 3: Update SceneManager
- [ ] Modify `AddSceneFromDefault()` to create configurator
- [ ] Add `AddSceneFromSave()` method (future)
- [ ] Update tests

### Phase 4: Integration
- [ ] Write integration tests
- [ ] Test default scene loading
- [ ] Test saved scene loading (when implemented)

---

## FAQ

### Q: Why not just pass both providers to SceneFactory?

**A**: Violates Single Responsibility. Factory would need conditionals everywhere. Not extensible. Hard to test.

### Q: Do we need to change IEntityConfigurator?

**A**: No! Current design already works. EntityConfigurator receives `EntityCollectionFbs&` which can come from any source.

### Q: What about performance - extra indirection?

**A**: Negligible. Configurator caches data. Factory only calls GetSceneData() a few times. Modern compilers optimize virtual calls well.

### Q: How do we add a third data source (e.g., network)?

**A**: Create `NetworkSceneConfigurator : public ISceneConfigurator`. That's it. Zero changes to Factory or SceneManager flow.

### Q: Should configurators be reusable?

**A**: No. Create fresh configurator per scene load. They're lightweight and stateful (caching).

### Q: Do we need ISceneConfigurator.cpp?

**A**: Only if there's shared implementation. Pure virtual interface can be header-only.

---

## Related Documentation

- **Full Analysis**: [SCENE_FACTORY_AND_CONFIGURATOR_ANALYSIS.md](SCENE_FACTORY_AND_CONFIGURATOR_ANALYSIS.md)
- **Visual Diagrams**: [SCENE_FACTORY_VISUALS.md](SCENE_FACTORY_VISUALS.md) (to be created)
- **Scene Data Provider**: [../proposals/DATA_LOADING_INTERFACE_README.md](../proposals/DATA_LOADING_INTERFACE_README.md)
- **Save Data Architecture**: [SCENE_DATA_PROVIDER_ARCHITECTURE_ANALYSIS.md](SCENE_DATA_PROVIDER_ARCHITECTURE_ANALYSIS.md) (from memories)

---

## Key Takeaways

1. **Strategy Pattern is the answer** - Clean, extensible, testable
2. **SceneManager orchestrates** - Decides which configurator to use
3. **Factory is data-source agnostic** - Uses configurator interface
4. **No null-checks needed** - Type-safe by design
5. **IEntityConfigurator unchanged** - Already flexible
6. **Direct FlatBuffers access** - Zero copy overhead

---

**End of Quick Reference**
