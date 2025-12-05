# Save/Load Data Workflow with View Pattern

**Date**: December 5, 2025  
**Context**: Response to save/load workflow questions with nested scene data  
**Related**: CONFIGURATION_VS_DATA_STRUCTS_ANALYSIS.md

---

## The Question

How do we handle save/load workflows where we have nested data structures:
- `EngineData` contains `SceneManagerData`
- `SceneManagerData` contains multiple `SceneData` items
- Each `SceneData` contains `EntityCollection` (EntityMemoryPool configuration)

Without:
1. Passing intermediate structs with nested FlatBuffers data inside
2. Creating wasteful copies of complex nested structures
3. Coupling FlatBuffers types into game code

---

## Current FlatBuffers Schema Structure

```fbs
// engine_data.fbs
table EngineDataFbs {
  subscriptions: [SubscriberData];
  scene_manager_data: SceneManagerData;    // Nested!
  global_event_bus_data: EventBusData;
  waiting_room_event_bus_data: EventBusData;
  engine_core: EngineCoreDataFbs;
}

// scene_manager_data.fbs
table SceneManagerData {
  subscriptions: [SubscriberData];
  scene_data: [SceneDataData];             // Array of scenes!
}

// scene_data.fbs
table SceneDataData {
  entity_collection: EntityCollection;     // EMP configuration!
  assets: AssetCollection;
  scene_core: SceneCoreDataFbs;
  scene_id: string;
  scene_type: SceneType;
  logic_collection_data: LogicCollectionData;
}

// entities.fbs
table EntityCollection {
  entities: [EntityData];                  // Deep nesting!
}
```

**The Challenge**: This creates a 4-level deep hierarchy:
```
EngineData
  └─ SceneManagerData
      └─ [SceneDataData]
          └─ EntityCollection
              └─ [EntityData]
                  └─ Components (with UIElement trees, etc.)
```

---

## The Problem: Three Bad Approaches

### Bad Approach 1: Pass FlatBuffers Types Down

```cpp
// PROBLEM: FlatBuffers types leak into game code!
void LoadGame(SaveData& save) {
  const EngineDataFbs* engine_fb = LoadEngineDataFlatBuffers();
  const SceneManagerData* scene_mgr_fb = engine_fb->scene_manager_data();
  
  for (const auto* scene_fb : *scene_mgr_fb->scene_data()) {
    const EntityCollection* entities_fb = scene_fb->entity_collection();
    ConfigureScene(scene_fb);  // FlatBuffers type passed!
  }
}

void ConfigureScene(const SceneDataData* scene_fb) {
  // Game code now depends on FlatBuffers!
  const EntityCollection* entities_fb = scene_fb->entity_collection();
  // ...
}
```

**Problems**:
- ❌ FlatBuffers dependency in game code
- ❌ Can't swap to JSON/XML without changing everything
- ❌ Hard to test (need FlatBuffers files)

---

### Bad Approach 2: Create Nested Intermediate Structs

```cpp
// PROBLEM: Massive memory overhead from copying!
struct SaveGameData {
  EngineData engine;
  SceneManagerData scene_manager;
};

struct SceneManagerData {
  std::vector<SubscriberData> subscriptions;
  std::vector<SceneData> scenes;  // Copy all scenes!
};

struct SceneData {
  EntityMemoryPoolData entities;   // Copy entire EMP!
  AssetData assets;
  SceneCoreData core;
  std::string scene_id;
  SceneType scene_type;
};

struct EntityMemoryPoolData {
  std::vector<EntityData> entities;  // Copy all entities!
};

struct EntityData {
  uint32_t index;
  std::optional<CUserInterfaceData> ui;  // Copy UI trees!
  std::optional<CGrimoireMachinaData> grimoire;
  // ... all components copied
};

// Loading means copying EVERYTHING
SaveGameData LoadGame() {
  SaveGameData data;
  // Copy: EngineData → SceneManager → Scenes → Entities → Components → UIElements
  // Potentially megabytes of data copied!
  return data;
}
```

**Problems**:
- ❌ Massive memory overhead (copy entire game state)
- ❌ Slow (deep copying polymorphic UI hierarchies)
- ❌ Wasteful (we're just going to iterate and configure anyway)
- ❌ Parallel struct hierarchy maintenance

---

### Bad Approach 3: ID Linking

```cpp
// PROBLEM: Indirection complexity, still need to fetch data
struct SaveGameData {
  EngineDataID engine_id;
  SceneManagerDataID scene_manager_id;
};

struct SceneManagerData {
  std::vector<SceneDataID> scene_ids;
};

// Still need to resolve IDs and fetch data
SceneData GetSceneData(SceneDataID id) {
  // Where does this data come from?
  // Back to FlatBuffers or intermediate structs!
}
```

**Problems**:
- ❌ Indirection adds complexity
- ❌ Still need to store/fetch actual data
- ❌ Doesn't solve the fundamental problem

---

## The Solution: View Pattern with Hierarchical Navigation

### Key Insight: Views Navigate Down, Data Flows Up

Instead of passing data structures down the call stack, use **view interfaces** to navigate down and **native objects** that flow up.

```
Navigation:     IEngineDataView → ISceneManagerView → ISceneDataView → IEntityDataView
                     (Navigate down using view interfaces)

Configuration:  EngineConfig ← SceneManager ← Scene ← EntityMemoryPool
                     (Native objects flow up and get configured)
```

---

## Implementation: Hierarchical View Pattern

### Level 1: Engine Data View

```cpp
// Abstract interface (no FlatBuffers!)
class IEngineDataView {
public:
  virtual ~IEngineDataView() = default;
  
  // Simple data as native types
  virtual EngineCoreData GetEngineCore() const = 0;
  virtual std::vector<SubscriberConfig> GetSubscriptions() const = 0;
  
  // Complex nested data as views
  virtual ISceneManagerView GetSceneManager() const = 0;
  virtual IEventBusView GetGlobalEventBus() const = 0;
  virtual IEventBusView GetWaitingRoomEventBus() const = 0;
};

// FlatBuffers implementation (hidden in data_providers/)
class FlatbuffersEngineDataView : public IEngineDataView {
private:
  const EngineDataFbs* m_fb_data;
  
public:
  FlatbuffersEngineDataView(const EngineDataFbs* data) 
      : m_fb_data(data) {}
  
  EngineCoreData GetEngineCore() const override {
    // Convert simple data to native struct
    EngineCoreData core;
    core.window_width = m_fb_data->engine_core()->window_width();
    core.window_height = m_fb_data->engine_core()->window_height();
    // ...
    return core;
  }
  
  ISceneManagerView GetSceneManager() const override {
    // Return view for navigation - no copying!
    return FlatbuffersSceneManagerView(m_fb_data->scene_manager_data());
  }
};
```

### Level 2: Scene Manager View

```cpp
class ISceneManagerView {
public:
  virtual ~ISceneManagerView() = default;
  
  // Simple data
  virtual std::vector<SubscriberConfig> GetSubscriptions() const = 0;
  
  // Navigation to scenes
  virtual size_t GetSceneCount() const = 0;
  virtual ISceneDataView GetScene(size_t index) const = 0;
  
  // Iteration helper
  virtual void ForEachScene(
      std::function<void(const ISceneDataView&)> callback) const = 0;
};

class FlatbuffersSceneManagerView : public ISceneManagerView {
private:
  const SceneManagerData* m_fb_data;
  
public:
  size_t GetSceneCount() const override {
    return m_fb_data->scene_data() ? m_fb_data->scene_data()->size() : 0;
  }
  
  ISceneDataView GetScene(size_t index) const override {
    // Return view - no copying!
    return FlatbuffersSceneDataView(m_fb_data->scene_data()->Get(index));
  }
  
  void ForEachScene(
      std::function<void(const ISceneDataView&)> callback) const override {
    for (size_t i = 0; i < GetSceneCount(); ++i) {
      callback(GetScene(i));
    }
  }
};
```

### Level 3: Scene Data View

```cpp
class ISceneDataView {
public:
  virtual ~ISceneDataView() = default;
  
  // Simple data as native types
  virtual SceneCoreData GetSceneCore() const = 0;
  virtual std::string GetSceneID() const = 0;
  virtual SceneType GetSceneType() const = 0;
  
  // Complex nested data as views
  virtual IEntityDataView GetEntityData() const = 0;
  virtual IAssetDataView GetAssetData() const = 0;
  virtual ILogicCollectionView GetLogicData() const = 0;
};

class FlatbuffersSceneDataView : public ISceneDataView {
private:
  const SceneDataData* m_fb_data;
  
public:
  SceneCoreData GetSceneCore() const override {
    // Convert to native struct
    SceneCoreData core;
    core.entity_pool_size = m_fb_data->scene_core()->entity_pool_size();
    core.render_texture_width = m_fb_data->scene_core()->render_texture_width();
    // ...
    return core;
  }
  
  IEntityDataView GetEntityData() const override {
    // Return view for EMP configuration - no copying!
    return FlatbuffersEntityDataView(m_fb_data->entity_collection());
  }
};
```

### Level 4: Entity Data View (from previous analysis)

```cpp
class IEntityDataView {
public:
  virtual ~IEntityDataView() = default;
  
  virtual size_t GetEntityCount() const = 0;
  virtual IEntityView GetEntity(size_t index) const = 0;
  
  virtual void ForEachEntity(
      std::function<void(const IEntityView&)> callback) const = 0;
};

// Implementation in previous analysis document
```

---

## Usage: Load Game Workflow

### Step 1: Load Top-Level View

```cpp
class SaveLoadManager {
public:
  void LoadGame(uint32_t slot_index) {
    // 1. Get save data provider
    ISaveDataProvider& save_provider = GetSaveDataProvider();
    
    // 2. Load save metadata (lightweight)
    auto save_result = save_provider.LoadSave(slot_index);
    if (!save_result.has_value()) {
      // Handle error
      return;
    }
    
    SaveData save = save_result.value();
    
    // 3. Get engine data view (no copying yet!)
    IEngineDataProvider& engine_provider = GetEngineDataProvider();
    auto engine_view = engine_provider.LoadEngineDataView(save);
    
    // 4. Configure engine from view
    ConfigureEngineFromView(engine_view);
  }
};
```

### Step 2: Navigate and Configure Hierarchically

```cpp
void SaveLoadManager::ConfigureEngineFromView(const IEngineDataView& view) {
  // Configure engine core (simple data)
  EngineCoreData core = view.GetEngineCore();
  m_engine.Configure(core);
  
  // Get scene manager view (no copying!)
  auto scene_manager_view = view.GetSceneManager();
  ConfigureSceneManagerFromView(scene_manager_view);
  
  // Configure other engine components
  ConfigureEventBusFromView(view.GetGlobalEventBus());
}

void SaveLoadManager::ConfigureSceneManagerFromView(
    const ISceneManagerView& view) {
  
  // Configure scene manager itself
  auto subscriptions = view.GetSubscriptions();
  m_scene_manager.ConfigureSubscriptions(subscriptions);
  
  // Configure each scene
  view.ForEachScene([this](const ISceneDataView& scene_view) {
    ConfigureSceneFromView(scene_view);
  });
}

void SaveLoadManager::ConfigureSceneFromView(const ISceneDataView& view) {
  // Get scene core (simple data)
  SceneCoreData core = view.GetSceneCore();
  SceneType type = view.GetSceneType();
  std::string id = view.GetSceneID();
  
  // Create scene
  auto scene = m_scene_factory.CreateScene(type, core, id);
  
  // Configure entities from view (no copying!)
  auto entity_view = view.GetEntityData();
  m_entity_configurator.ConfigureEntities(scene->GetEntityPool(), entity_view);
  
  // Configure assets, logic, etc.
  auto asset_view = view.GetAssetData();
  ConfigureAssetsFromView(scene->GetAssetManager(), asset_view);
}
```

**Key Points**:
1. ✅ Views navigate down the hierarchy
2. ✅ Native structs (EngineCoreData, SceneCoreData) extracted when needed
3. ✅ Complex nested data stays as views (no copying)
4. ✅ Configuration happens at each level
5. ✅ No FlatBuffers types in game code

---

## Save Game Workflow (Reverse Direction)

### Capturing State

```cpp
SaveData SaveLoadManager::CaptureGameState() {
  SaveData save;
  
  // Capture metadata
  save.metadata.save_name = "Player Save 1";
  save.metadata.slot_index = 0;
  save.current_scene_type = m_scene_manager.GetCurrentSceneType();
  
  // NOTE: Scene state capture not yet implemented
  // Future: Capture entity pools from each scene
  // save.scene_states = CaptureSceneStates();
  
  return save;
}

// Future implementation
std::vector<SceneStateData> SaveLoadManager::CaptureSceneStates() {
  std::vector<SceneStateData> states;
  
  for (const auto& scene : m_scene_manager.GetAllScenes()) {
    SceneStateData state;
    state.scene_id = scene->GetID();
    state.scene_type = scene->GetType();
    
    // Capture entity pool state
    state.entity_state = CaptureEntityPoolState(scene->GetEntityPool());
    
    states.push_back(state);
  }
  
  return states;
}
```

### Provider Handles Serialization

```cpp
class FlatbuffersSaveDataProvider : public ISaveDataProvider {
public:
  std::expected<std::monostate, FailInfo>
  SaveGame(const SaveData& save_data) override {
    // Build FlatBuffers from native structs
    flatbuffers::FlatBufferBuilder builder;
    
    // Build metadata
    auto save_name = builder.CreateString(save_data.metadata.save_name);
    auto metadata = CreateSaveMetadata(builder,
        save_name,
        // ... other fields
    );
    
    // Build scene states (future)
    // std::vector<Offset<SceneStateData>> scene_offsets;
    // for (const auto& state : save_data.scene_states) {
    //   scene_offsets.push_back(BuildSceneState(builder, state));
    // }
    
    // Build root
    auto save = CreateSaveData(builder,
        metadata
        // , scene_states future
    );
    
    builder.Finish(save);
    
    // Write to file
    WriteBinaryFile(GetSavePath(save_data.metadata.slot_index), 
                   builder.GetBufferPointer(),
                   builder.GetSize());
    
    return std::monostate{};
  }
};
```

---

## ID Linking for Scene References

### Use Case: Scene Manager Needs Scene IDs

If SceneManager needs to track which scenes exist without loading all their data:

```cpp
class ISceneManagerView {
public:
  // Get just the scene IDs for tracking
  virtual std::vector<SceneIdentifier> GetSceneIdentifiers() const = 0;
  
  // Load specific scene data on demand
  virtual ISceneDataView GetSceneByID(const std::string& scene_id) const = 0;
  virtual ISceneDataView GetSceneByType(SceneType type) const = 0;
};

struct SceneIdentifier {
  std::string scene_id;
  SceneType scene_type;
  // Lightweight metadata only
};
```

**Usage**:

```cpp
void SceneManager::Initialize(const ISceneManagerView& view) {
  // Get scene identifiers (lightweight)
  auto scene_ids = view.GetSceneIdentifiers();
  
  // Track which scenes exist
  for (const auto& id : scene_ids) {
    m_available_scenes[id.scene_type] = id.scene_id;
  }
  
  // Load initial scene on demand
  if (m_current_scene_type != SceneType::UNKNOWN) {
    auto scene_view = view.GetSceneByType(m_current_scene_type);
    LoadScene(scene_view);
  }
}
```

---

## Memory Management Strategy

### Problem: Who Owns the FlatBuffers Data?

When using views, the underlying FlatBuffers buffer must stay alive.

**Solution: Provider Owns Buffer**

```cpp
class FlatbuffersEngineDataProvider : public IEngineDataProvider {
private:
  // Buffer stays alive as long as provider exists
  std::vector<uint8_t> m_buffer;
  const EngineDataFbs* m_root;
  
public:
  std::expected<IEngineDataView, FailInfo> LoadEngineDataView() {
    // Load buffer
    auto buffer_result = LoadBinaryFile("engine_data.bin");
    if (!buffer_result.has_value()) {
      return std::unexpected(buffer_result.error());
    }
    
    m_buffer = std::move(buffer_result.value());
    m_root = GetEngineDataFbs(m_buffer.data());
    
    // Return view that references our buffer
    return FlatbuffersEngineDataView(m_root);
  }
};
```

**Alternative: Smart Pointer Sharing**

```cpp
class SharedBufferView {
private:
  std::shared_ptr<std::vector<uint8_t>> m_buffer;
  const EngineDataFbs* m_root;
  
public:
  SharedBufferView(std::shared_ptr<std::vector<uint8_t>> buffer)
      : m_buffer(buffer),
        m_root(GetEngineDataFbs(m_buffer->data())) {}
  
  // Buffer stays alive as long as any view exists
};
```

---

## Complete Example: Load Saved Game

```cpp
class Game {
public:
  void LoadSavedGame(uint32_t slot_index) {
    // 1. Load save metadata
    ISaveDataProvider& save_provider = GetSaveDataProvider();
    auto save = save_provider.LoadSave(slot_index).value();
    
    // 2. Get appropriate data source
    IEngineDataProvider& engine_provider = GetEngineDataProvider();
    
    // For saved game, load from save file
    // For new game, load from defaults
    auto engine_view = save.version > 0 
        ? engine_provider.LoadEngineDataViewFromSave(save)
        : engine_provider.LoadEngineDataViewFromDefaults();
    
    // 3. Configure engine (navigates down hierarchy)
    ConfigureEngineFromView(engine_view);
    
    // 4. Start game
    m_engine.StartUp();
    m_engine.RunGameLoop();
  }

private:
  void ConfigureEngineFromView(const IEngineDataView& view) {
    // Simple data extracted as native structs
    EngineCoreData core = view.GetEngineCore();
    m_engine.ConfigureCore(core);
    
    // Navigate to scene manager (view, not copy)
    auto scene_mgr_view = view.GetSceneManager();
    
    // Configure scene manager
    auto subs = scene_mgr_view.GetSubscriptions();
    m_scene_manager.ConfigureSubscriptions(subs);
    
    // Configure each scene
    scene_mgr_view.ForEachScene([this](const ISceneDataView& scene_view) {
      // Extract simple data
      SceneCoreData scene_core = scene_view.GetSceneCore();
      SceneType scene_type = scene_view.GetSceneType();
      
      // Create scene
      auto scene = m_scene_factory.CreateScene(scene_type, scene_core);
      
      // Configure entities (view, not copy)
      auto entity_view = scene_view.GetEntityData();
      m_entity_configurator.ConfigureEntities(
          scene->GetEntityPool(), 
          entity_view);  // View pattern - no copying!
      
      m_scene_manager.AddScene(std::move(scene));
    });
  }
};
```

---

## Summary: Save/Load with View Pattern

### Loading Flow

```
1. ISaveDataProvider.LoadSave() 
   → SaveData (lightweight metadata)

2. IEngineDataProvider.LoadEngineDataView(save)
   → IEngineDataView (root view, no copying)

3. IEngineDataView.GetSceneManager()
   → ISceneManagerView (navigate down, no copying)

4. ISceneManagerView.ForEachScene()
   → ISceneDataView (for each scene, no copying)

5. ISceneDataView.GetEntityData()
   → IEntityDataView (EMP config, no copying)

6. EntityConfigurator.ConfigureEntities(pool, IEntityDataView)
   → Iterate and configure directly from view
```

### Key Principles

1. **Navigate Down with Views**: Use view interfaces to navigate nested hierarchy
2. **Extract Up with Native Structs**: Simple data extracted as native structs
3. **Configure In-Place**: Use views to configure directly, no intermediate copies
4. **Provider Owns Buffers**: Provider ensures FlatBuffers buffer lifetime
5. **No FlatBuffers in Game Code**: All game code uses views and native structs

### Benefits

✅ **Zero-copy navigation** through nested data  
✅ **No FlatBuffers coupling** in game code  
✅ **Memory efficient** - no wasteful intermediate structs  
✅ **Format agnostic** - can swap FlatBuffers/JSON/XML  
✅ **Testable** - mock views for testing  
✅ **Flexible** - load from saves, defaults, or network  

### Trade-offs

⚠️ **More interfaces** to define (but cleaner architecture)  
⚠️ **Buffer lifetime** management needed  
⚠️ **Initial implementation** effort (4-6 weeks)  

---

## Answers to Original Question

### Q: How to distribute/collect in save/load workflow?

**A**: Use **hierarchical view pattern**.

- **Distribute**: Navigate down with view interfaces
  - `IEngineDataView → ISceneManagerView → ISceneDataView → IEntityDataView`
  
- **Collect**: Extract up with native structs
  - Simple data (EngineCoreData, SceneCoreData) extracted as native
  - Complex data (entities, UI) configured directly from views

### Q: Do we pass intermediate structs with FlatBuffers inside?

**A**: No - pass **view interfaces** instead.

Views abstract the data source. FlatBuffers stay in provider implementations.

### Q: Do we need ID linking?

**A**: Only for **lazy loading** scenarios.

- If you need all scene data now: Use `ForEachScene()` 
- If you only need scene IDs now: Use `GetSceneIdentifiers()`
- Load specific scenes on demand: Use `GetSceneByID()`

### Q: How to use FlatBuffers data immediately?

**A**: Views provide **immediate access** without copying.

```cpp
// View provides immediate access
auto entity_view = scene_view.GetEntityData();

// Iterate and configure immediately
for (size_t i = 0; i < entity_view.GetEntityCount(); ++i) {
  auto entity = entity_view.GetEntity(i);
  // Configure directly from view - no copying!
  ConfigureEntity(pool, entity);
}
```

---

## Next Steps

1. **Review** this analysis with team
2. **Decide** if view pattern worth 4-6 week implementation
3. **If yes**: Start with Level 4 (IEntityDataView) and work up
4. **If no**: Document current approach and continue with it

The view pattern is the **clean long-term solution** for nested data distribution/collection in save/load workflows.

---

**See Also**:
- [Configuration vs Data Structs](CONFIGURATION_VS_DATA_STRUCTS_ANALYSIS.md) - View pattern details
- [Current State Analysis](CURRENT_STATE_ANALYSIS_2025.md) - Why deferred
- [Provider System](../DATA_PROVIDER_SYSTEM.md) - Current providers

---

**Analysis Complete**: December 5, 2025
