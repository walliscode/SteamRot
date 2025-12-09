# Response to Scene Factory Data Sourcing Question

**Date**: December 8, 2025  
**Question**: How should SceneFactory receive data from ISceneDataProvider OR ISaveDataProvider?

---

## Your Question

> "So, one of my thoughts was to feed either the ISceneDataProvider or ISaveDataProvider to SceneFactory on construction and the let it use the fact that one is null and the other not to decide how to populate its data? Any other thoughts?"

---

## Short Answer

**Don't use the null-check approach.** Instead, use the **Strategy Pattern** with an `ISceneConfigurator` interface.

### Why Not Null-Checks?

Your suggested approach would work, but has significant drawbacks:
- ❌ Requires null-check conditionals throughout SceneFactory
- ❌ Violates Single Responsibility Principle
- ❌ Complex error handling (both null? both set?)
- ❌ Makes testing harder (need to test null combinations)
- ❌ Not extensible (adding third source = more conditionals)
- ❌ SceneFactory knows too much about data sources

### Better Approach: Strategy Pattern

Create an abstraction layer between SceneFactory and data sources:

```cpp
// New interface
class ISceneConfigurator {
public:
    virtual std::expected<const SceneDataFbs*, FailInfo> GetSceneData() = 0;
    virtual std::expected<std::unique_ptr<IEntityConfigurator>, FailInfo>
        CreateEntityConfigurator(EventHandler& event_handler) = 0;
    virtual SceneType GetSceneType() const = 0;
// New interface - Dual-Format Design
class ISceneConfigurator {
public:
    // PRIMARY: Returns native C++ struct (format-agnostic)
    virtual std::expected<SceneData, FailInfo> GetSceneData() = 0;
    
    // SECONDARY: Returns FlatBuffers pointer (for EntityConfigurator)
    virtual std::expected<const SceneDataFbs*, FailInfo> GetSceneDataFbs() = 0;
    
    virtual std::expected<std::unique_ptr<IEntityConfigurator>, FailInfo>
        CreateEntityConfigurator(EventHandler& event_handler) = 0;
    virtual SceneType GetSceneType() const = 0;
};

// Implementation for default scenes
class DefaultSceneConfigurator : public ISceneConfigurator {
private:
    ISceneDataProvider& m_scene_data_provider;
    SceneType m_scene_type;
    
    // Cache both formats
    mutable std::optional<SceneData> m_cached_native_data;
    mutable const SceneDataFbs* m_cached_fbs_data{nullptr};
    // Implements interface methods
};

// Implementation for saved scenes
class SavedSceneConfigurator : public ISceneConfigurator {
private:
    ISaveDataProvider& m_save_data_provider;
    uint32_t m_slot_index;
    
    // Cache both formats
    mutable std::optional<SceneData> m_cached_native_data;
    mutable const SceneDataFbs* m_cached_fbs_data{nullptr};
    // Implements interface methods
};

// Updated SceneFactory - Uses native format!
class FlatbuffersSceneFactory : public ISceneFactory {
private:
    std::unique_ptr<ISceneConfigurator> m_scene_configurator;
public:
    FlatbuffersSceneFactory(
        const GameContext& game_context,
        std::unique_ptr<ISceneConfigurator> configurator);
    
    // Factory uses GetSceneData() → native SceneData struct
    std::expected<std::monostate, FailInfo>
    ConfigureSceneResources(Scene& scene) {
        auto scene_data = m_scene_configurator->GetSceneData();  // Native!
        const SceneData& data = scene_data.value();
        
        // Configure using native struct (format-agnostic)
        scene.GetRenderTexture().create(
            data.render_texture_width,
            data.render_texture_height
        );
        // ...
    }
};
```

**Key Point**: The interface provides **both** formats, but the primary interface (`GetSceneData()`) returns the native C++ struct, keeping the abstract interface format-agnostic. The FlatBuffers method (`GetSceneDataFbs()`) is only for EntityConfigurator compatibility.

---

## Usage Examples

### Loading Default Scene

```cpp
// In SceneManager::AddSceneFromDefault()
ISceneDataProvider& provider = GetSceneDataProvider();

auto configurator = std::make_unique<DefaultSceneConfigurator>(
    provider,
    scene_type
);

auto factory = FlatbuffersSceneFactory(
    m_game_context,
    std::move(configurator)
);

auto scene = factory.CreateScene();
```

### Loading Saved Scene

```cpp
// In SceneManager::AddSceneFromSave() (future)
ISaveDataProvider& provider = GetSaveDataProvider();

auto configurator = std::make_unique<SavedSceneConfigurator>(
    provider,
    slot_index
);

auto factory = FlatbuffersSceneFactory(
    m_game_context,
    std::move(configurator)
);

auto scene = factory.CreateScene();
```

---

## Key Benefits

1. **No Conditionals**: SceneFactory has zero null-checks, zero if/else for sources
2. **Clean Separation**: Each class has one responsibility
3. **Type-Safe**: Impossible to have both null or both set
4. **Easy to Test**: Mock configurators
5. **Extensible**: New source = new configurator, zero changes to Factory
6. **Direct FlatBuffers Access**: No intermediate copying (as you wanted)

---

## How It Works

### The Flow

```
SceneManager (Orchestrator)
  ↓ decides: default or save?
  ↓ creates appropriate configurator
  ↓
ISceneConfigurator (Strategy)
  ├── DefaultSceneConfigurator  → wraps ISceneDataProvider
  └── SavedSceneConfigurator    → wraps ISaveDataProvider
  ↓ passed to
  ↓
FlatbuffersSceneFactory
  ↓ calls configurator.GetSceneData()
  ↓ calls configurator.CreateEntityConfigurator()
  ↓
Scene (configured)
```

### Inside Factory - No Conditionals, Format-Agnostic!

```cpp
std::expected<std::monostate, FailInfo>
FlatbuffersSceneFactory::ConfigureSceneResources(Scene& scene) {
    
    // Get NATIVE data from configurator (format-agnostic!)
    auto scene_data = m_scene_configurator->GetSceneData();
    if (!scene_data.has_value()) {
        return std::unexpected(scene_data.error());
    }
    
    // Use native SceneData struct - Factory doesn't know about FlatBuffers!
    const SceneData& data = scene_data.value();
    
    // Configure using native struct
    scene.GetRenderTexture().create(
        data.render_texture_width,
        data.render_texture_height
    );
    
    scene.GetSceneInfo().scene_id = data.scene_id;
    // ... more configuration ...
}
```

---

## Addressing Your Concerns

### "No intermediate struct - straight from FlatBuffers to Scene"

✅ **Solved with Dual-Format Interface**: 
- Configurators cache **both** native `SceneData` struct and `SceneDataFbs*` pointer
- Factory uses native struct (follows Data Loading Interface pattern)
- EntityConfigurator uses FlatBuffers pointer (maintains compatibility)
- Data loaded once, no duplication

### "Prevent too much logic for copying data"

✅ **Solved**: 
- Configurators load data once and cache both representations
- FlatBuffers data remains in buffer (zero copy)
- Native struct is small (scene metadata only, not entities)
- No performance penalty

### "Support both ISceneDataProvider and ISaveDataProvider"

✅ **Solved**: 
- Each configurator wraps one provider type
- Factory doesn't know the difference
- Clean separation of concerns

### "Baking FlatBuffers into abstract interface"

✅ **Addressed with Dual-Format Design**:
- **Primary interface**: `GetSceneData()` returns native `SceneData` struct (format-agnostic)
- **Secondary interface**: `GetSceneDataFbs()` returns `const SceneDataFbs*` (for EntityConfigurator only)
- Factory code uses native interface exclusively
- FlatBuffers method is transitional - can be removed when EntityConfigurator is refactored

**Why This Works**:
1. **SceneFactory** remains format-agnostic (uses native structs)
2. **EntityConfigurator** gets FlatBuffers pointer it needs
3. **Future-proof**: When EntityConfigurator is refactored to use native structs, just remove `GetSceneDataFbs()`
4. **No performance cost**: Both formats cached from single data load

### "Extensible design"

✅ **Solved**: 
- Want to add network scenes? Create `NetworkSceneConfigurator`. Zero changes to Factory.
- Want JSON format? Configurator loads JSON, converts to native struct, provides to Factory.
- Factory code never changes regardless of data source or format.

---

## What About IEntityConfigurator?

**Good news**: Current design already works!

IEntityConfigurator currently receives `EntityCollectionFbs&` in constructor. This reference can come from any source (default or save), so no changes needed.

Configurator's `CreateEntityConfigurator()` method uses `GetSceneDataFbs()` internally to get the FlatBuffers pointer:

```cpp
std::expected<std::unique_ptr<IEntityConfigurator>, FailInfo>
DefaultSceneConfigurator::CreateEntityConfigurator(EventHandler& event_handler) {
    auto scene_data_fbs = GetSceneDataFbs();  // FlatBuffers pointer
    
    return std::make_unique<FlatbuffersEntityConfigurator>(
        event_handler,
        *scene_data_fbs.value()->entity_collection()
    );
}
```

If saved entities use a different FlatBuffers schema in the future:
1. Create `SavedEntityConfigurator` implementation
2. Have it adapt `SavedEntityCollectionFbs` → `EntityCollectionFbs`
3. Pass to base methods

**No changes needed to IEntityConfigurator interface at this time.**

---

## Implementation Steps

### Phase 1: Create Interfaces (2-3 days)
1. Create `src/scenes/ISceneConfigurator.h` (dual-format interface)
2. Create `src/scenes/DefaultSceneConfigurator.h/.cpp`
3. Create `src/scenes/SavedSceneConfigurator.h/.cpp` (stub for now)
4. Write unit tests

### Phase 2: Update Factory (1-2 days)
1. Modify `FlatbuffersSceneFactory.h` - new constructor
2. Modify `FlatbuffersSceneFactory.cpp` - use configurator with native `GetSceneData()`
3. Implement `ConfigureSceneConfig()` method
4. Update tests

### Phase 3: Update SceneManager (1 day)
1. Modify `AddSceneFromDefault()` to create DefaultSceneConfigurator
2. Add `AddSceneFromSave()` method (future)
3. Update tests

### Phase 4: Integration (1 day)
1. Write integration tests
2. Verify no regressions

**Total estimate**: 5-7 days

---

## Documentation Created

I've created three comprehensive documents to support this architecture:

1. **[SCENE_FACTORY_AND_CONFIGURATOR_ANALYSIS.md](SCENE_FACTORY_AND_CONFIGURATOR_ANALYSIS.md)**
   - Full technical analysis (~31KB)
   - Architecture options compared
   - Implementation details
   - Code examples

2. **[SCENE_FACTORY_QUICK_REF.md](SCENE_FACTORY_QUICK_REF.md)**
   - Quick reference (~12KB)
   - Patterns and templates
   - Common mistakes to avoid
   - Testing patterns

3. **[SCENE_FACTORY_VISUALS.md](SCENE_FACTORY_VISUALS.md)**
   - Visual diagrams (~34KB)
   - Sequence diagrams
   - Data flow diagrams
   - Pattern comparisons

---

## Recommendation

✅ **Proceed with Strategy Pattern approach**

This is a well-established pattern that solves your exact problem:
- Clean architecture
- Type-safe
- No conditionals
- Easy to test
- Extensible
- Minimal performance overhead
- Follows SOLID principles

Your null-check idea would work, but would create technical debt. The Strategy Pattern is worth the extra classes.

---

## Questions?

If you have any questions about this approach or need clarification on any aspect, feel free to ask!

The documentation I created covers:
- Why this pattern is better
- How to implement it
- What pitfalls to avoid
- How to test it
- How to extend it in the future

---

**End of Response**
