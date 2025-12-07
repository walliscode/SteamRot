# Abstract Scene Factory Pattern Analysis

**Date**: December 7, 2025  
**Context**: Response to suggestion "provide a fully configured Scene via AbstractSceneFactory with abstract entity configurators"  
**Related**: SCENE_DATA_PROVIDER_ARCHITECTURE_ANALYSIS.md, PROVIDER_VS_VIEWER_PATTERN.md

---

## The Suggestion

**"How about a slightly different approach. Rather than providing another abstraction layer we provide a fully configured Scene? so potentially an AbstractSceneFactory that is responsible for returning a unique pointer to a fully configured scene? and then we can use abstract entity configurators inside?"**

---

## Current Architecture

### Existing SceneFactory Pattern

The codebase already uses a factory pattern for scene creation:

```cpp
// src/scenes/SceneFactory.h
class SceneFactory {
public:
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateDefaultScene(const SceneType &scene_type,
                     const GameContext &game_context);
};
```

**Current workflow in `CreateDefaultScene`:**

1. **Create scene object** (TitleScene, CraftingScene, etc.)
2. **Load scene configuration** via `ISceneDataProvider`
3. **Configure render texture** with dimensions from scene data
4. **Configure entities** via `scene->ConfigureFromDefault()`
5. **Generate archetypes** from configured entities
6. **Create logic map** via `LogicFactory`
7. **Return fully configured scene**

### Current Entity Configuration

```cpp
// Scene.h
class Scene {
  std::expected<std::monostate, FailInfo>
  ConfigureFromDefault(const DataType &data_type = DataType::Flatbuffers);
};

// EntityConfigurator.h
class EntityConfigurator {
protected:
  EventHandler &m_event_handler;
public:
  EntityConfigurator(EventHandler &event_handler);
};

// FlatbuffersConfigurator.h (concrete implementation)
class FlatbuffersConfigurator : public EntityConfigurator {
  // Configures entities from FlatBuffers data
};
```

---

## Analysis of Suggested Approach

### What the Suggestion Would Look Like

```cpp
// Abstract factory interface
class ISceneFactory {
public:
  virtual ~ISceneFactory() = default;
  
  virtual std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateScene(const SceneType &scene_type,
              const GameContext &game_context) = 0;
};

// Default scene factory (loads from default data)
class DefaultSceneFactory : public ISceneFactory {
private:
  ISceneDataProvider &m_scene_data_provider;
  IEntityConfigurator &m_entity_configurator;  // Abstract!
  
public:
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateScene(const SceneType &scene_type,
              const GameContext &game_context) override;
};

// Saved scene factory (loads from save files)
class SavedSceneFactory : public ISceneFactory {
private:
  ISaveDataProvider &m_save_data_provider;
  IEntityConfigurator &m_entity_configurator;  // Abstract!
  uint32_t m_save_slot;
  
public:
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateScene(const SceneType &scene_type,
              const GameContext &game_context) override;
};

// Abstract entity configurator
class IEntityConfigurator {
public:
  virtual ~IEntityConfigurator() = default;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntities(Scene &scene) = 0;
};

// Concrete implementations
class DefaultEntityConfigurator : public IEntityConfigurator {
  // Configure from default data files
};

class SavedEntityConfigurator : public IEntityConfigurator {
  // Configure from save file data
};
```

### Usage Example

```cpp
// For new game
ISceneFactory &factory = GetDefaultSceneFactory();
auto scene = factory.CreateScene(SceneType::SceneType_TITLE, context);

// For load game
ISceneFactory &factory = GetSavedSceneFactory(save_slot);
auto scene = factory.CreateScene(SceneType::SceneType_CRAFTING, context);

// Both return fully configured Scene ready to use
```

---

## Comparison: Current vs Suggested Approach

### Current Approach (Single Factory + Data Providers)

```
SceneFactory (concrete)
    ↓
ISceneDataProvider (interface)
    ├── FlatbuffersSceneDataProvider (default data)
    └── (future) SavedSceneDataProvider (saved data - via extractor)
    ↓
Scene::ConfigureFromDefault()
    ↓
EntityConfigurator (base class)
    └── FlatbuffersConfigurator (concrete)
    ↓
Fully configured Scene
```

**Characteristics**:
- ✅ Single factory class
- ✅ Data source abstraction via providers
- ✅ Clear data loading layer
- ⚠️ Scene knows how to configure itself
- ⚠️ Default vs saved handled by different data providers

---

### Suggested Approach (Abstract Factory + Abstract Configurators)

```
ISceneFactory (interface)
    ├── DefaultSceneFactory (default scenes)
    └── SavedSceneFactory (saved scenes)
    ↓
IEntityConfigurator (interface)
    ├── DefaultEntityConfigurator (default data)
    └── SavedEntityConfigurator (saved data)
    ↓
Fully configured Scene
```

**Characteristics**:
- ✅ Factory abstraction (default vs saved)
- ✅ Configurator abstraction (default vs saved)
- ✅ Scene doesn't configure itself
- ✅ Clear separation of default vs saved workflows
- ⚠️ More abstraction layers
- ⚠️ Still need data providers underneath

---

## Detailed Analysis

### Advantages of Abstract Factory Approach

#### 1. Clear Workflow Separation

```cpp
// Current: One factory, branches inside
SceneFactory factory;
auto scene = factory.CreateDefaultScene(type, context);
// Internally decides to use ISceneDataProvider

// Suggested: Two factories, explicit from start
DefaultSceneFactory default_factory;
auto scene1 = default_factory.CreateScene(type, context);

SavedSceneFactory saved_factory(save_slot);
auto scene2 = saved_factory.CreateScene(type, context);
```

**Benefit**: Caller explicitly chooses default vs saved workflow

#### 2. Entity Configuration Abstraction

```cpp
// Current: Scene configures itself
scene->ConfigureFromDefault();  // Scene knows about its data source

// Suggested: Factory configures scene
IEntityConfigurator &configurator = GetConfigurator();
configurator.ConfigureEntities(scene);  // External configuration
```

**Benefit**: Scene is passive, doesn't need to know about data sources

#### 3. Easier Testing

```cpp
// Suggested approach allows easy mocking
class MockSceneFactory : public ISceneFactory {
  // Return pre-configured test scenes
};

class MockEntityConfigurator : public IEntityConfigurator {
  // Configure with test data
};
```

**Benefit**: Can inject test factories and configurators

#### 4. Symmetry Between Default and Saved

```cpp
// Both workflows look identical from caller perspective
auto default_scene = default_factory.CreateScene(type, context);
auto saved_scene = saved_factory.CreateScene(type, context);

// vs current asymmetry
auto default_scene = factory.CreateDefaultScene(type, context);
auto saved_scene = factory.CreateFromSave(save, type, context);  // future
```

**Benefit**: Consistent interface regardless of data source

---

### Disadvantages of Abstract Factory Approach

#### 1. More Abstraction Layers

```
Current layers:
  SceneFactory → ISceneDataProvider → Scene

Suggested layers:
  ISceneFactory → IEntityConfigurator → Scene
      ↓               ↓
  Concrete        Concrete
  Factory         Configurator
      ↓               ↓
  Still need    Still need
  Providers     Providers
```

**Issue**: Adds abstraction without removing existing layers

#### 2. Data Providers Still Needed

```cpp
// Abstract factory still needs data providers internally
class DefaultSceneFactory : public ISceneFactory {
private:
  ISceneDataProvider &m_data_provider;  // Still need this!
  IAssetDataProvider &m_asset_provider;  // Still need this!
  IEntityConfigurator &m_configurator;   // New abstraction
};
```

**Issue**: Doesn't eliminate provider pattern, just wraps it

#### 3. Increased Complexity for Current Needs

**Current scene data is simple:**
- 4 fields in SceneData (~64 bytes)
- Single ConfigureFromDefault() call
- Already working well

**Abstract factory adds:**
- 2 new interfaces (ISceneFactory, IEntityConfigurator)
- 4+ new concrete classes (factories and configurators for each source)
- Dependency injection setup
- More indirection to trace through

**Issue**: High complexity cost for current simple needs

#### 4. Configurator Abstraction Complexity

```cpp
// Current: Configurators are already concrete classes
FlatbuffersConfigurator configurator(event_handler);
configurator.ConfigureEntitiesFromDefaultData(
    scene_type, entity_memory_pool);

// Suggested: Need interface + multiple implementations
class IEntityConfigurator {
  virtual ConfigureEntities(Scene &scene) = 0;
};

class DefaultEntityConfigurator : public IEntityConfigurator {
  // Wraps FlatbuffersConfigurator
};

class SavedEntityConfigurator : public IEntityConfigurator {
  // Wraps FlatbuffersConfigurator + SaveData extraction
};
```

**Issue**: Adds interface layer on top of existing configurators

---

## When Abstract Factory Would Be Beneficial

### Scenario 1: Multiple Data Sources

If the game needs to load scenes from many sources:

```cpp
ISceneFactory implementations:
- DefaultSceneFactory (local files)
- SavedSceneFactory (save files)
- NetworkSceneFactory (multiplayer server)
- ModdedSceneFactory (mod files)
- DebugSceneFactory (procedural test scenes)
- BenchmarkSceneFactory (performance test scenes)
```

**Current status**: Only 2 sources (default + saved)  
**Verdict**: Not yet justified

---

### Scenario 2: Complex Configuration Pipeline

If scene configuration involves many steps across multiple systems:

```cpp
class ISceneFactory {
  virtual std::unique_ptr<Scene> CreateScene() = 0;
  
protected:
  // Many configuration steps
  virtual void ConfigureEntities() = 0;
  virtual void ConfigureAI() = 0;
  virtual void ConfigurePhysics() = 0;
  virtual void ConfigureNetworking() = 0;
  virtual void ConfigureGraphics() = 0;
  // etc...
};
```

**Current status**: Simple single-step configuration  
**Verdict**: Not yet justified

---

### Scenario 3: Runtime Factory Selection

If factory choice is determined at runtime based on complex logic:

```cpp
ISceneFactory &factory = SelectFactory(
    user_preferences,
    game_mode,
    network_state,
    save_availability,
    mod_status
);

auto scene = factory.CreateScene(type, context);
```

**Current status**: Simple if/else (new game or load game)  
**Verdict**: Not yet justified

---

## Hybrid Recommendation

### Keep Current Architecture, Add Methods

Instead of abstract factory, extend existing `SceneFactory`:

```cpp
class SceneFactory {
public:
  // Existing (for default/new game)
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateDefaultScene(const SceneType &scene_type,
                     const GameContext &game_context);
  
  // Add new method (for saved game) - FUTURE
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateSceneFromSave(const SaveData &save_data,
                      const SceneType &scene_type,
                      const GameContext &game_context);
};
```

**Implementation of new method:**

```cpp
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateSceneFromSave(
    const SaveData &save_data,
    const SceneType &scene_type,
    const GameContext &game_context) {
  
  // Create scene object (same as default)
  auto scene = CreateSceneInstance(scene_type, game_context);
  
  // Extract scene data from save
  SceneDataExtractor extractor;
  auto scene_data = extractor.ExtractSceneData(save_data, scene_type);
  
  // Configure render texture
  ConfigureRenderTexture(scene.get(), scene_data);
  
  // Configure entities from save data
  ConfigureEntitiesFromSave(scene.get(), save_data);
  
  // Generate archetypes and logic (same as default)
  FinalizeScene(scene.get());
  
  return scene;
}
```

**Benefits**:
- ✅ Same concrete factory class
- ✅ Clear separation of default vs saved methods
- ✅ No new abstractions needed
- ✅ Explicit method names
- ✅ Easier to understand and maintain
- ✅ Can share helper methods between both

**Usage**:

```cpp
SceneFactory factory;

// New game
auto default_scene = factory.CreateDefaultScene(type, context);

// Load game
auto saved_scene = factory.CreateSceneFromSave(save_data, type, context);
```

---

## Alternative: Strategy Pattern for Configuration

If entity configuration complexity grows, use strategy pattern:

```cpp
// Strategy interface
class IEntityConfigurationStrategy {
public:
  virtual ~IEntityConfigurationStrategy() = default;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntities(EntityMemoryPool &pool,
                    const SceneType scene_type) = 0;
};

// Concrete strategies
class DefaultEntityConfiguration : public IEntityConfigurationStrategy {
  // Configure from default data
};

class SavedEntityConfiguration : public IEntityConfigurationStrategy {
  // Configure from saved data
};

// SceneFactory uses strategy
class SceneFactory {
private:
  std::unique_ptr<IEntityConfigurationStrategy> m_config_strategy;
  
public:
  void SetConfigurationStrategy(
      std::unique_ptr<IEntityConfigurationStrategy> strategy) {
    m_config_strategy = std::move(strategy);
  }
  
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateScene(const SceneType &scene_type,
              const GameContext &game_context) {
    // Create scene
    // Configure using strategy
    m_config_strategy->ConfigureEntities(scene->GetEntityPool(), scene_type);
    // Finalize
  }
};
```

**Usage**:

```cpp
SceneFactory factory;

// New game
factory.SetConfigurationStrategy(
    std::make_unique<DefaultEntityConfiguration>());
auto scene = factory.CreateScene(type, context);

// Load game
factory.SetConfigurationStrategy(
    std::make_unique<SavedEntityConfiguration>(save_data));
auto scene = factory.CreateScene(type, context);
```

**Benefits**:
- ✅ Single factory (no abstract factory needed)
- ✅ Flexible configuration swapping
- ✅ Easier to add new strategies
- ⚠️ Need to remember to set strategy before creating scene

---

## Recommendation Summary

### Current State: Keep Existing Pattern ✅

**Reasons:**
1. **Adequate for current needs** - Simple scene data, single configuration step
2. **Already working** - No issues identified
3. **Clear and maintainable** - Easy to understand workflow
4. **Low complexity** - Minimal abstraction layers

### Future: When Complexity Grows

**Option A: Extend SceneFactory** (Recommended for near-term)
```cpp
class SceneFactory {
  CreateDefaultScene();     // Existing
  CreateSceneFromSave();    // Add when needed
};
```
- Simple extension of current pattern
- No new abstractions
- Clear method names

**Option B: Strategy Pattern** (For configuration complexity)
```cpp
class SceneFactory {
  SetConfigurationStrategy();  // Swap strategies
  CreateScene();               // Single create method
};
```
- Use when entity configuration becomes complex
- Allows flexible configuration swapping
- Still single factory

**Option C: Abstract Factory** (For many data sources)
```cpp
ISceneFactory {
  CreateScene();
};
// Multiple implementations
```
- Use when 3+ distinct scene sources exist
- Justified when workflow differences are significant
- Current 2 sources don't justify this yet

---

## Decision Criteria

### Use Abstract Factory Pattern When:

- [ ] **3+ distinct scene data sources** (currently 2: default + saved)
- [ ] **Significant workflow differences** between sources (currently similar)
- [ ] **Runtime factory selection** needed based on complex logic (currently simple if/else)
- [ ] **Team consensus** that abstraction benefit > complexity cost
- [ ] **Clear use cases** that concrete factory can't handle

**Current state**: 0/5 criteria met

### Stay with Concrete Factory When:

- [x] **2 or fewer data sources**
- [x] **Similar workflows** between sources
- [x] **Simple decision logic** (new game vs load game)
- [x] **Adequate current solution**
- [x] **No identified issues**

**Current state**: 5/5 criteria met ✅

---

## Addressing the Suggestion Directly

### "Rather than providing another abstraction layer"

**Observation**: Abstract factory would actually **add** abstraction layers, not remove them.

- Current: `SceneFactory → ISceneDataProvider → Scene`
- Suggested: `ISceneFactory → IEntityConfigurator → ISceneDataProvider → Scene`

The suggestion appears to prefer fewer abstractions, but abstract factory adds more.

### "Provide a fully configured Scene"

**Current behavior**: `SceneFactory.CreateDefaultScene()` already returns fully configured scene.

```cpp
// Already doing this
auto scene = factory.CreateDefaultScene(type, context);
// Scene is fully configured and ready to use
```

Abstract factory would provide same end result with more steps.

### "Use abstract entity configurators inside"

**Current**: Entity configuration uses concrete `FlatbuffersConfigurator` class.

**Potential benefit**: Abstracting configurator could allow:
- Mock configurators for testing
- Different configurator implementations for different data formats
- Swappable configuration strategies

**However**: Current concrete configurator is adequate and simple.

---

## Conclusion

### Answer to "How about AbstractSceneFactory approach?"

**Verdict**: Not recommended for current needs.

**Reasons:**
1. **Adds complexity without solving current problems**
   - Current factory works well
   - Simple scene data doesn't need elaborate abstraction

2. **Doesn't eliminate existing abstractions**
   - Still need data providers underneath
   - Just adds more layers on top

3. **Premature abstraction**
   - Only 2 data sources (default + saved)
   - Similar workflows between them
   - Simple decision logic

4. **Alternative solutions are simpler**
   - Extend existing factory with `CreateSceneFromSave()` method
   - Use strategy pattern if configuration gets complex
   - Keep concrete factory until 3+ distinct sources exist

### Recommended Path Forward

**Phase 1: Current** (Keep as-is) ✅
- Single concrete `SceneFactory`
- Data source abstraction via providers
- Working and adequate

**Phase 2: When Save System Added**
- Add `CreateSceneFromSave()` method to existing factory
- Use `SceneDataExtractor` to flatten saved data
- No new abstractions needed

**Phase 3: If Complexity Grows**
- Consider strategy pattern for entity configuration
- Still avoid abstract factory until 3+ sources

**Phase 4: If Many Sources Emerge**
- Only then consider abstract factory pattern
- Justified when clear benefits > complexity cost

---

## Related Documentation

- **Scene Provider Analysis**: SCENE_DATA_PROVIDER_ARCHITECTURE_ANALYSIS.md
- **Provider vs Viewer**: PROVIDER_VS_VIEWER_PATTERN.md
- **Save/Load Workflow**: SAVE_LOAD_WORKFLOW_ANALYSIS.md
- **Current SceneFactory**: src/scenes/SceneFactory.h

---

## Code Example: Recommended Future Extension

```cpp
// Future addition to existing SceneFactory.h
class SceneFactory {
public:
  // Existing method
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateDefaultScene(const SceneType &scene_type,
                     const GameContext &game_context);
  
  // Add when save system implemented
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateSceneFromSave(const SaveData &save_data,
                      const SceneType &scene_type,
                      const GameContext &game_context);

private:
  // Shared helper methods
  std::unique_ptr<Scene> CreateSceneInstance(
      const SceneType &scene_type,
      const GameContext &game_context);
  
  void ConfigureRenderTexture(Scene *scene, const SceneData &data);
  void FinalizeScene(Scene *scene);
};
```

**Simple, clear, maintainable - no abstract factory needed.**

---

**Analysis Complete**: December 7, 2025
