# Scene Factory and Configurator Architecture - Quick Reference

**Date**: December 8, 2025  
**Related**: SCENE_FACTORY_AND_CONFIGURATOR_ANALYSIS.md

---

## TL;DR Recommendations

✅ **Single path through SceneFactory** - No branching for default vs saved  
✅ **SceneManager orchestrates** data source selection and provides unified SceneData  
✅ **Self-contained configurators** - Each manages its own data loading  
✅ **IEntityConfigurator** with virtual methods (not overloads)  
✅ **Remove Scene::Configure()** - configuration happens externally  
✅ **Data-agnostic Scene** - No intermediate struct copying

---

## Key Architectural Patterns

### 1. SceneManager Data Sourcing

```cpp
class SceneManager {
  // TWO ENTRY POINTS for scene creation
  
  // For new games and scene transitions
  std::expected<uuids::uuid, FailInfo>
  LoadSceneFromDefault(const SceneType &scene_type);
  
  // For loading saved games
  std::expected<uuids::uuid, FailInfo>
  LoadSceneFromSave(uint32_t slot_index);
};
```

**Flow:**
```
LoadSceneFromDefault:
  ISceneDataProvider → SceneData → SceneFactory::CreateScene() → Scene

LoadSceneFromSave:
  ISaveDataProvider → SaveData → SceneDataExtractor → SceneData → SceneFactory::CreateScene() → Scene
```

**Key insight:** Both paths converge to the same SceneData → SceneFactory::CreateScene() method.

### 2. Simplified SceneFactory (Single Path)

```cpp
// No abstract configurator hierarchy needed
// SceneFactory has ONE CreateScene() method that works the same for all
class SceneFactory {
  SceneFactory() = default;
  
  // Single method that works the same for default and saved
  std::expected<std::unique_ptr<Scene>, FailInfo>
  CreateScene(const SceneData &scene_data, const GameContext &game_context);
};

// Implementation
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateScene(const SceneData &scene_data,
                         const GameContext &game_context) {
  // 1. Create Scene instance
  auto scene = CreateSceneInstance(scene_data.scene_type, ...);
  
  // 2. Configure render texture
  scene->m_scene_resources.scene_texture = RenderTexture(scene_data.width, scene_data.height);
  
  // 3. Self-contained configurators handle their own data loading
  auto entity_config = std::make_unique<FlatbuffersEntityConfigurator>(event_handler);
  entity_config->Configure(scene->entity_manager, scene_data.scene_type);
  
  // 4. Generate archetypes
  scene->entity_manager.GenerateAllArchetypes();
  
  // 5. Self-contained logic factory
  LogicFactory logic_factory(scene_data.scene_type, scene->GetSceneContext());
  scene->SetLogicMap(logic_factory.CreateLogicMap());
  
  return scene;
}
```

**Benefits:**
- ✅ Single path - no branching
- ✅ No configurator selection logic
- ✅ Simple and direct

### 3. Self-Contained EntityConfigurator

```cpp
// OLD (current): Overloaded methods - NOT polymorphic
class FlatbuffersConfigurator : public EntityConfigurator {
  ConfigureComponent(Component &component);  // Overload
  ConfigureComponent(const UserInterfaceData*, CUserInterface&);  // Overload
};

// NEW: Virtual methods - Polymorphic and self-contained
class IEntityConfigurator {
  virtual Configure(EntityManager&, SceneType) = 0;
};

class FlatbuffersEntityConfigurator : public IEntityConfigurator {
private:
  FlatbuffersDataLoader m_data_loader;  // Self-contained data loading
  
public:
  // Configurator handles its own data loading internally
  Configure(EntityManager &entity_manager, SceneType scene_type) override {
    // Load data internally - no external data passing needed
    const SceneDataData *scene_data =
        m_data_loader.ProvideDefaultSceneData(scene_type).value();
    
    // Configure entities
    // ...
  }
};
```

**Key principle:** Configurators are self-contained and manage their own data loading.

---

## Implementation Phases

### Phase 1: Abstract EntityConfigurator ⭐ START HERE

**Goal**: Make EntityConfigurator properly abstract and self-contained

**Changes**:
- `EntityConfigurator.h` → `IEntityConfigurator.h` (interface)
- `FlatbuffersConfigurator` → `FlatbuffersEntityConfigurator`
- Add virtual `Configure()` method
- Add `m_data_loader` member for self-contained data loading
- Keep existing overloaded methods as private helpers

**Impact**: Low risk, internal refactor only

### Phase 2: Simplify SceneFactory

**Goal**: Remove Scene::Configure() and make SceneFactory single-path

**Changes**:
- Remove `Scene::ConfigureFromDefault()` method
- SceneFactory has single `CreateScene()` method
- No branching for default vs saved
- Directly calls self-contained configurators
- Scene becomes pure structural container

**Impact**: Medium risk, changes Scene and SceneFactory APIs

### Phase 3: SceneManager Data Sourcing

**Goal**: Handle default vs saved data sources with unified SceneData

**Changes**:
- Add `LoadSceneFromDefault()` method
- Add `LoadSceneFromSave()` method
- Both provide same SceneData interface to SceneFactory
- Refactor existing methods to use new methods

**Impact**: Medium risk, changes SceneManager behavior

### Phase 4: Future Enhancements

- SceneDataExtractor (when SaveData has scene_states)
- Additional EntityConfigurators (Json, Xml, Lua)

---

## Code Patterns

### Pattern 1: Loading Default Scene

```cpp
// In SceneManager
std::expected<uuids::uuid, FailInfo>
SceneManager::LoadSceneFromDefault(const SceneType &scene_type) {
  
  // 1. Get SceneData from provider
  auto scene_data = m_scene_data_provider->LoadSceneData(scene_type).value();
  
  // 2. Create configurators
  auto entity_config = std::make_unique<FlatbuffersEntityConfigurator>(
      m_game_context.event_handler);
  auto scene_config = std::make_unique<DefaultSceneConfigurator>(
      std::move(entity_config));
  
  // 3. Create factory and scene
  SceneFactory factory(std::move(scene_config));
  auto scene = factory.CreateScene(scene_data, m_game_context).value();
  
  // 4. Add to map
  auto scene_id = scene->GetSceneInfo().id;
  m_scenes.emplace(scene_id, std::move(scene));
  
  return scene_id;
}
```

### Pattern 2: Loading Saved Scene

```cpp
// In SceneManager
std::expected<uuids::uuid, FailInfo>
SceneManager::LoadSceneFromSave(uint32_t slot_index) {
  
  // 1. Load SaveData
  auto save_data = m_save_data_provider->LoadSave(slot_index).value();
  
  // 2. Extract SceneData
  SceneDataExtractor extractor;
  auto scene_data = extractor.ExtractSceneData(save_data).value();
  
  // 3. Create configurators (saved version)
  auto entity_config = std::make_unique<FlatbuffersEntityConfigurator>(
      m_game_context.event_handler);
  auto scene_config = std::make_unique<SavedSceneConfigurator>(
      std::move(entity_config));
  
  // 4. Create factory and scene
  SceneFactory factory(std::move(scene_config));
  auto scene = factory.CreateScene(scene_data, m_game_context).value();
  
  // 5. Add to map
  auto scene_id = scene->GetSceneInfo().id;
  m_scenes.emplace(scene_id, std::move(scene));
  
  return scene_id;
}
```

### Pattern 3: SceneFactory with Configurator

```cpp
// In SceneFactory
std::expected<std::unique_ptr<Scene>, FailInfo>
SceneFactory::CreateScene(const SceneData &scene_data,
                         const GameContext &game_context) {
  
  // 1. Create polymorphic Scene
  std::unique_ptr<Scene> scene = CreateSceneInstance(scene_data.scene_type, ...);
  
  // 2. Use configurator strategy
  m_configurator->ConfigureRenderTexture(*scene, scene_data);
  m_configurator->ConfigureEntities(*scene, scene_data);
  
  // 3. Generate archetypes
  scene->m_scene_resources.entity_manager.GenerateAllArchetypes();
  
  // 4. Configure logic
  m_configurator->ConfigureLogic(*scene, scene_data);
  
  return scene;
}
```

### Pattern 4: Implementing IEntityConfigurator

```cpp
class FlatbuffersEntityConfigurator : public IEntityConfigurator {
private:
  // Keep old overloaded methods as private helpers
  std::expected<std::monostate, FailInfo>
  ConfigureCUserInterface(const UserInterfaceData*, CUserInterface&);
  
public:
  // New virtual interface method
  std::expected<std::monostate, FailInfo>
  ConfigureFromDefault(EntityManager &entity_manager,
                       SceneType scene_type) override {
    
    // Load FlatBuffers data
    const SceneDataData *scene_data = LoadSceneData(scene_type);
    const EntityCollection *entities = scene_data->entity_collection();
    
    // Configure each entity using private helpers
    for (size_t i = 0; i < entities->entities()->size(); ++i) {
      const EntityData *entity_data = entities->entities()->Get(i);
      
      if (entity_data->c_user_interface()) {
        ConfigureCUserInterface(
            entity_data->c_user_interface(),
            GetComponent<CUserInterface>(i, entity_manager));
      }
      // ... other components
    }
    
    return std::monostate{};
  }
};
```

---

## Decision Tree

### When to Use Which Configurator?

```
Is this a new game or scene transition?
  YES → Use DefaultSceneConfigurator
        └─ Use FlatbuffersEntityConfigurator::ConfigureFromDefault()
  
  NO → Is this loading a saved game?
       YES → Use SavedSceneConfigurator
             └─ Use FlatbuffersEntityConfigurator::ConfigureFromSave()
       
       NO → Is this a test?
            YES → Use TestSceneConfigurator (future)
                  └─ Use MockEntityConfigurator::ConfigureFromTest()
```

### Where Does Configuration Happen?

```
❌ WRONG: Scene::ConfigureFromDefault()
  - Configuration is NOT Scene's responsibility
  - Scene is a structural container

✅ RIGHT: SceneFactory + ISceneConfigurator
  - Factory orchestrates creation
  - Configurator encapsulates strategy
  - Scene remains passive
```

---

## Key Data Structures

### SceneData (Flat)

```cpp
struct SceneData {
  SceneType scene_type;
  std::string scene_id;
  uint32_t render_texture_width;
  uint32_t render_texture_height;
  
  // Future: entity_states, logic_state
};
```

**Purpose**: Unified format for scene configuration  
**Source**: ISceneDataProvider (default) or SceneDataExtractor (saved)

### SaveData (Nested)

```cpp
struct SaveData {
  struct Metadata { ... } metadata;
  SceneType current_scene_type;
  
  // Future: vector<SceneState> scene_states;
};
```

**Purpose**: Persistent storage format  
**Source**: ISaveDataProvider

### SceneDataExtractor (Future)

```cpp
class SceneDataExtractor {
  std::expected<SceneData, FailInfo>
  ExtractSceneData(const SaveData &save_data, size_t scene_index = 0);
};
```

**Purpose**: Convert nested SaveData to flat SceneData  
**When**: Phase 2, when SaveData includes scene_states

---

## Interface Contracts

### ISceneConfigurator

```cpp
class ISceneConfigurator {
  // Configure render texture dimensions
  virtual ConfigureRenderTexture(Scene&, const SceneData&) = 0;
  
  // Configure entities (delegates to IEntityConfigurator)
  virtual ConfigureEntities(Scene&, const SceneData&) = 0;
  
  // Configure logic collection
  virtual ConfigureLogic(Scene&, const SceneData&) = 0;
};
```

**Implementations**:
- `DefaultSceneConfigurator` - Load from default data
- `SavedSceneConfigurator` - Restore from saved data
- `TestSceneConfigurator` (future) - Load from test data

### IEntityConfigurator

```cpp
class IEntityConfigurator {
  // Configure entities from default data (new game)
  virtual ConfigureFromDefault(EntityManager&, SceneType) = 0;
  
  // Configure entities from saved data (load game)
  virtual ConfigureFromSave(EntityManager&, const SceneData&) = 0;
  
  // Configure entities from test data (testing)
  virtual ConfigureFromTest(EntityManager&, const TestEntityConfig&) = 0;
};
```

**Implementations**:
- `FlatbuffersEntityConfigurator` - Load from FlatBuffers
- `JsonEntityConfigurator` (future) - Load from JSON
- `MockEntityConfigurator` (future) - Mock for testing

---

## Testing Strategy

### Unit Tests

```cpp
TEST_CASE("IEntityConfigurator is abstract") {
  // Can't instantiate directly
  // REQUIRE(std::is_abstract_v<IEntityConfigurator>);
}

TEST_CASE("FlatbuffersEntityConfigurator implements interface") {
  EventHandler handler;
  FlatbuffersEntityConfigurator config(handler);
  
  // Verify virtual methods work
  EntityManager manager(handler);
  auto result = config.ConfigureFromDefault(manager, SceneType_TITLE);
  REQUIRE(result.has_value());
}

TEST_CASE("DefaultSceneConfigurator uses entity configurator") {
  auto entity_config = std::make_unique<MockEntityConfigurator>();
  auto scene_config = std::make_unique<DefaultSceneConfigurator>(
      std::move(entity_config));
  
  Scene scene(...);
  SceneData data{...};
  
  auto result = scene_config->ConfigureEntities(scene, data);
  REQUIRE(result.has_value());
}
```

### Integration Tests

```cpp
TEST_CASE("SceneManager loads default scene correctly") {
  SceneManager manager(game_context);
  
  auto scene_id = manager.LoadSceneFromDefault(SceneType_TITLE);
  REQUIRE(scene_id.has_value());
  
  const auto& scenes = manager.GetScenes();
  REQUIRE(scenes.count(scene_id.value()) == 1);
  REQUIRE(scenes.at(scene_id.value())->GetSceneInfo().type == SceneType_TITLE);
}

TEST_CASE("SceneFactory creates scene with configurator") {
  auto entity_config = std::make_unique<FlatbuffersEntityConfigurator>(handler);
  auto scene_config = std::make_unique<DefaultSceneConfigurator>(
      std::move(entity_config));
  
  SceneFactory factory(std::move(scene_config));
  
  SceneData data{SceneType_TITLE, "", 800, 600};
  auto scene = factory.CreateScene(data, game_context);
  
  REQUIRE(scene.has_value());
  REQUIRE(scene.value()->GetSceneInfo().type == SceneType_TITLE);
}
```

---

## Common Pitfalls

### ❌ Pitfall 1: Using Overloaded Methods for Polymorphism

```cpp
// WRONG: Overloads are resolved at compile time
class Configurator {
  void Configure(int x);     // Overload 1
  void Configure(float x);   // Overload 2
};
// Can't swap implementations at runtime!
```

### ✅ Solution: Use Virtual Methods

```cpp
// RIGHT: Virtual methods are resolved at runtime
class IConfigurator {
  virtual void Configure() = 0;
};

class ConcreteConfigurator : public IConfigurator {
  void Configure() override { ... }
};
```

### ❌ Pitfall 2: Configuration in Scene Class

```cpp
// WRONG: Scene has configuration method
class Scene {
  void ConfigureFromDefault() { ... }
};
// Violates Single Responsibility Principle
```

### ✅ Solution: External Configuration

```cpp
// RIGHT: Configuration happens externally
class SceneFactory {
  Scene CreateScene() {
    Scene scene;
    m_configurator->ConfigureScene(scene);
    return scene;
  }
};
```

### ❌ Pitfall 3: Tight Coupling to Data Format

```cpp
// WRONG: Scene knows about FlatBuffers
class Scene {
  void Configure(const SceneDataData *flatbuffers_data) { ... }
};
```

### ✅ Solution: Data Abstraction Layer

```cpp
// RIGHT: Scene works with C++ structs
struct SceneData {
  SceneType scene_type;
  uint32_t render_texture_width;
  uint32_t render_texture_height;
};

class Scene {
  // No configuration method!
  // Configuration happens in SceneFactory via ISceneConfigurator
};
```

---

## Benefits Summary

### Why Abstract Factory?

✅ **Runtime flexibility** - Swap strategies without recompilation  
✅ **Open/Closed Principle** - Extend without modifying  
✅ **Single Responsibility** - Each configurator has one job  
✅ **Testability** - Mock configurators in tests  
✅ **Composability** - Configurators can use sub-configurators

### Why Abstract EntityConfigurator?

✅ **True polymorphism** - Virtual methods override correctly  
✅ **Runtime swappable** - Choose configurator at runtime  
✅ **Testable** - Mock IEntityConfigurator easily  
✅ **Extensible** - Add new data sources without changing interface  
✅ **Decoupled** - Game code doesn't know about FlatBuffers

### Why Remove Scene::Configure()?

✅ **Separation of concerns** - Scene is structure, not behavior  
✅ **Single Responsibility** - Configuration is SceneFactory's job  
✅ **Testability** - Easier to test Scene without configuration  
✅ **Flexibility** - Different configuration strategies don't affect Scene

---

## Related Documentation

- **Full Analysis**: `SCENE_FACTORY_AND_CONFIGURATOR_ANALYSIS.md`
- **Save/Load Workflow**: `SAVE_LOAD_WORKFLOW_ANALYSIS.md`
- **Data Provider Pattern**: `../proposals/DATA_LOADING_INTERFACE_QUICK_REF.md`
- **Engine Data Organization**: `../proposals/ENGINE_DATA_ORGANIZATION_QUICK_REF.md`

---

**End of Quick Reference**
