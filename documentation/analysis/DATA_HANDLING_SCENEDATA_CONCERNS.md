# SceneData Approach - Addressing Specific Concerns

**Created**: December 12, 2025  
**Purpose**: Answer specific questions about the SceneData intermediate struct approach

---

## User Questions

> "Say I went down the SceneData route. We are saying that anything that isn't essentially a POD gets a POD struct to represent it? And any PODs need to be copyable? Or are we saying that we configure the EntityMemoryPool twice?
> What about non-copyable resources like render textures?"

---

## Short Answers

1. **POD representation**: No, only configuration data becomes POD. Resources like `sf::RenderTexture` are NOT copied.

2. **Copyability**: Configuration data is copyable. Resources are not copied, they're configured in place.

3. **Configure twice**: No. EntityMemoryPool configured ONCE from FlatBuffers data. SceneData just holds the FlatBuffers pointer (or metadata).

4. **Non-copyable resources**: Never copied. Created empty, then configured in place (dimensions set, etc.).

---

## Detailed Analysis

### Current Scene Structure

Looking at your current code:

```cpp
// Scene.h
class Scene {
protected:
  SceneInfo m_scene_info;           // POD: uuid + SceneType
  SceneResources m_scene_resources; // Contains NON-copyable resources!
  SceneConfig m_scene_config;       // POD
  SceneState m_scene_state;         // POD
};

// SceneResources.h  
struct SceneResources {
  const GameContext &game_context;      // Reference (not owned)
  EntityManager entity_manager;         // Contains EntityMemoryPool
  LogicCollection logic_map;            // Map of unique_ptrs (non-copyable!)
  sf::RenderTexture scene_texture;      // NON-COPYABLE!
};

// EntityManager.h
class EntityManager {
private:
  EntityMemoryPool m_entity_memory_pool;  // Tuple of vectors of components
  ArchetypeManager m_archetype_manager;
};
```

**Key observation**: `SceneResources` contains non-copyable resources:
- `sf::RenderTexture` (SFML type, non-copyable)
- `LogicCollection` (map of `unique_ptr`, non-copyable)
- `EntityManager` (contains the entity memory pool)

---

## The SceneData Approach Explained

### What SceneData Would Actually Contain

**SceneData is NOT a copy of Scene!** It's configuration metadata:

```cpp
struct SceneData {
  // POD configuration data
  SceneInfo info;              // uuid, SceneType
  SceneConfig config;          // Any POD config values
  
  // Configuration for resources (NOT the resources themselves!)
  struct ResourceConfig {
    uint32_t render_texture_width{800};
    uint32_t render_texture_height{600};
  } resource_config;
  
  // For EntityMemoryPool: Either pointer OR converted data
  // Option A: Keep FlatBuffers pointer
  const EntityCollectionFbs *entity_data{nullptr};
  
  // Option B: Convert to native (if truly needed)
  struct EntityCollectionData {
    uint32_t pool_size;
    std::vector<EntityData> entities;  // Native structs
  } entity_collection;
  
  // Logic configuration (optional, mostly static)
  // Not usually needed since LogicFactory determines this
};
```

**Critical**: SceneData doesn't contain:
- ❌ `sf::RenderTexture` itself
- ❌ `EntityMemoryPool` itself
- ❌ `LogicCollection` itself

It contains:
- ✅ Configuration VALUES (dimensions, sizes, types)
- ✅ Either FlatBuffers pointers OR converted POD data
- ✅ Metadata needed to CREATE and CONFIGURE resources

---

## Configuration Flow (No Duplication!)

### With SceneData Approach

```cpp
// 1. Provider loads and converts CONFIGURATION
ISceneDataProvider &provider = GetSceneDataProvider();
auto scene_data_result = provider.LoadSceneData(scene_type);

// Provider does:
SceneData scene_data;
auto fbs = loader.ProvideDefaultSceneData(scene_type);

// Extract POD config values
scene_data.info.type = fbs->scene_info()->scene_type();
scene_data.resource_config.render_texture_width = 
    fbs->scene_resources()->render_texture_width();

// For entities: Keep pointer (no conversion!)
scene_data.entity_data = fbs->entity_collection();

// 2. Factory creates empty Scene with empty resources
SceneFactory factory(game_context);
auto scene = factory.CreateSceneByType(scene_type);
// Scene constructor creates empty RenderTexture, empty EntityMemoryPool, etc.

// 3. Configurator CONFIGURES resources in place (NO COPYING!)
ISceneConfigurator &configurator = GetSceneConfigurator();
configurator.ConfigureScene(*scene, scene_data);

// Inside ConfigureScene:
void ConfigureScene(Scene &scene, const SceneData &data) {
  // Configure SceneInfo (POD copy is fine)
  scene.m_scene_info = data.info;
  
  // Configure RenderTexture (NO COPY! Just set dimensions)
  scene.m_scene_resources.scene_texture.create(
      data.resource_config.render_texture_width,
      data.resource_config.render_texture_height);
  
  // Configure EntityMemoryPool (NO COPY! Configure from FlatBuffers)
  FlatbuffersEntityConfigurator entity_config(
      event_handler, 
      *data.entity_data);  // Pass FlatBuffers pointer!
  entity_config.ConfigureEntityMemoryPool(
      scene.m_scene_resources.entity_manager.GetEntityMemoryPool());
  // This happens ONCE, directly from FlatBuffers
  
  // Configure LogicMap (creates Logic objects, no copying)
  LogicFactory logic_factory(...);
  scene.m_scene_resources.logic_map = logic_factory.CreateLogicMap();
}
```

**Key Insight**: 
- `SceneData` holds FlatBuffers pointer to `EntityCollectionFbs`
- EntityMemoryPool configured DIRECTLY from that pointer
- NO conversion to intermediate native entity collection
- NO double configuration

---

## Answering Each Question

### Q1: "Anything that isn't POD gets a POD struct to represent it?"

**Answer**: No. Only CONFIGURATION DATA gets POD representation.

**What gets POD struct**:
- ✅ `SceneInfo` (uuid, type) - already POD
- ✅ Render texture dimensions (width, height) - simple ints
- ✅ Entity pool size - single int

**What does NOT get POD struct**:
- ❌ `sf::RenderTexture` - created empty, configured with dimensions
- ❌ `EntityMemoryPool` - created empty, configured from FlatBuffers
- ❌ `LogicCollection` - created by LogicFactory

**Example**:
```cpp
// SceneData doesn't contain:
sf::RenderTexture texture;  // ❌ NO!

// SceneData contains:
struct {
  uint32_t width{800};
  uint32_t height{600};
} render_texture_config;  // ✅ YES - just config values
```

---

### Q2: "Any PODs need to be copyable?"

**Answer**: Yes, POD configuration data should be copyable. But resources are NOT copied.

**Copyable** (configuration values):
- `SceneInfo` - small struct (uuid + enum)
- Render texture dimensions - two ints
- Entity pool size - one int

**Not copyable** (resources):
- `sf::RenderTexture` - never in SceneData, never copied
- `EntityMemoryPool` - never in SceneData, never copied
- `LogicCollection` - never in SceneData, never copied

**Cost analysis**:
```cpp
// Copying SceneData (if it held only config):
struct SceneData {
  SceneInfo info;           // 16 bytes (uuid) + 4 bytes (enum) = 20 bytes
  uint32_t texture_width;   // 4 bytes
  uint32_t texture_height;  // 4 bytes
  uint32_t pool_size;       // 4 bytes
  const EntityCollectionFbs *entity_data;  // 8 bytes (pointer)
  // Total: ~40 bytes
};
// Copying this is negligible!
```

---

### Q3: "Configure EntityMemoryPool twice?"

**Answer**: NO! Configured ONCE.

**How it works**:

```cpp
// Provider: NO conversion of entity data
SceneData scene_data;
scene_data.entity_data = fbs->entity_collection();  // Just store pointer!

// Configurator: Configure EntityMemoryPool ONCE from FlatBuffers
void ConfigureScene(Scene &scene, const SceneData &data) {
  // Pass FlatBuffers pointer directly to entity configurator
  FlatbuffersEntityConfigurator entity_config(
      event_handler,
      *data.entity_data);  // Use the FlatBuffers pointer
  
  // Configure ONCE
  entity_config.ConfigureEntityMemoryPool(
      scene.m_scene_resources.entity_manager.GetEntityMemoryPool());
  
  // Inside ConfigureEntityMemoryPool (existing code, unchanged!):
  // 1. Resize pool
  entity::memory::ResizeEntityMemoryPool(emp, entity_data.entity_memory_pool_size());
  // 2. Configure components from FlatBuffers
  for (const auto &entity_data : *m_entity_collection_data.entities()) {
    if (entity_data->c_user_interface()) {
      ConfigureComponent(...);  // Reads from FlatBuffers
    }
  }
}
```

**No duplication**:
1. FlatBuffers data stays as FlatBuffers
2. Pointer stored in SceneData
3. EntityConfigurator reads directly from FlatBuffers pointer
4. Components configured ONCE into EntityMemoryPool

---

### Q4: "What about non-copyable resources like render textures?"

**Answer**: Never copied. Created empty, then configured in place.

**Pattern for all non-copyable resources**:

```cpp
// 1. Scene constructor creates EMPTY resources
Scene::Scene(const GameContext &game_context)
  : m_scene_resources(game_context) {
  // m_scene_resources.scene_texture is default-constructed (empty)
  // m_scene_resources.entity_manager is default-constructed (empty pool)
  // m_scene_resources.logic_map is empty map
}

// 2. Configurator CONFIGURES resources in place (no copying!)
void ConfigureScene(Scene &scene, const SceneData &data) {
  
  // RenderTexture: Just call create() with dimensions
  scene.m_scene_resources.scene_texture.create(
      data.resource_config.texture_width,
      data.resource_config.texture_height);
  // This configures the EXISTING texture, doesn't copy anything
  
  // EntityMemoryPool: Configure from FlatBuffers
  FlatbuffersEntityConfigurator config(handler, *data.entity_data);
  config.ConfigureEntityMemoryPool(
      scene.m_scene_resources.entity_manager.GetEntityMemoryPool());
  // This configures the EXISTING pool, doesn't copy anything
  
  // LogicCollection: Create and move
  LogicFactory factory(...);
  scene.m_scene_resources.logic_map = factory.CreateLogicMap();
  // Move assignment, not copy
}
```

**SFML RenderTexture specifics**:
```cpp
// sf::RenderTexture is non-copyable
class RenderTexture {
  RenderTexture(const RenderTexture&) = delete;  // No copy
  RenderTexture& operator=(const RenderTexture&) = delete;
  
  RenderTexture(RenderTexture&&) noexcept;  // Move OK
  RenderTexture& operator=(RenderTexture&&) noexcept;
  
  bool create(unsigned int width, unsigned int height);  // Configure in place
};

// We use create(), not copy!
```

---

## The Actual SceneData (Minimal Version)

Given these constraints, here's what SceneData would actually look like:

```cpp
struct SceneData {
  // POD configuration (copyable, small)
  SceneInfo info;
  SceneConfig config;
  
  // Render texture config (not the texture itself!)
  uint32_t render_texture_width{800};
  uint32_t render_texture_height{600};
  
  // Entity data: JUST THE POINTER (no conversion!)
  const EntityCollectionFbs *entity_collection{nullptr};
  
  // Logic config (optional, usually not needed)
  // LogicFactory determines this from SceneType
};
```

**Size**: ~50 bytes total (not counting the data pointed to by entity_collection).

---

## Comparison: With vs Without SceneData

### Without SceneData (Current/Alternative)

```cpp
// SceneManager
auto fbs = loader.ProvideDefaultSceneData(scene_type);
auto scene = factory.CreateSceneByType(scene_type);
configurator.ConfigureScene(*scene, fbs);  // Pass FlatBuffers pointer

// Configurator
void ConfigureScene(Scene &scene, const SceneDataFbs *fbs) {
  scene.m_scene_info.type = fbs->scene_info()->scene_type();
  scene.m_scene_resources.scene_texture.create(
      fbs->scene_resources()->texture_width(),
      fbs->scene_resources()->texture_height());
  
  FlatbuffersEntityConfigurator config(handler, *fbs->entity_collection());
  config.ConfigureEntityMemoryPool(scene.GetEntityMemoryPool());
}
```

### With SceneData (Provider Pattern)

```cpp
// SceneManager
ISceneDataProvider &provider = GetSceneDataProvider();
auto scene_data = provider.LoadSceneData(scene_type);
auto scene = factory.CreateSceneByType(scene_type);
configurator.ConfigureScene(*scene, scene_data.value());

// Provider (FlatBuffers implementation)
SceneData LoadSceneData(SceneType type) {
  auto fbs = loader.ProvideDefaultSceneData(type);
  
  SceneData data;
  data.info.type = fbs->scene_info()->scene_type();
  data.render_texture_width = fbs->scene_resources()->texture_width();
  data.entity_collection = fbs->entity_collection();  // Just pointer!
  return data;
}

// Configurator (generic, works with any provider)
void ConfigureScene(Scene &scene, const SceneData &data) {
  scene.m_scene_info = data.info;
  scene.m_scene_resources.scene_texture.create(
      data.render_texture_width,
      data.render_texture_height);
  
  FlatbuffersEntityConfigurator config(handler, *data.entity_collection);
  config.ConfigureEntityMemoryPool(scene.GetEntityMemoryPool());
}
```

**Key differences**:
1. With SceneData: Provider does extraction once, configurator is simpler
2. With SceneData: Configurator doesn't know about FlatBuffers structure
3. With SceneData: Easy to add XMLSceneDataProvider later

**Duplication concern**:
- Extraction logic moves from Configurator to Provider
- It's not duplicated, it's relocated
- Configurator becomes simpler and data-source agnostic

---

## The Real Trade-off

### What You Lose Without SceneData

**Cannot easily support multiple data formats**:
```cpp
// With FlatBuffers pointer in Configurator:
void ConfigureScene(Scene &scene, const SceneDataFbs *fbs);
// To add XML, need:
void ConfigureScene(Scene &scene, const XMLSceneData *xml);  // Different type!
// Interface cannot support both without variant/overloads

// With SceneData:
void ConfigureScene(Scene &scene, const SceneData &data);
// Works with FlatBuffers, XML, JSON, test data, anything!
```

### What You Gain Without SceneData

**Simpler code, less indirection**:
- No intermediate struct
- Configurator reads directly from FlatBuffers
- One less type to maintain

---

## Recommendation Update

Given your concerns, here's the pragmatic middle ground:

### Option: Minimal SceneData (Hybrid Approach)

```cpp
// Minimal SceneData: Just pointers and POD configs
struct SceneData {
  SceneType type;
  uint32_t render_texture_width;
  uint32_t render_texture_height;
  const EntityCollectionFbs *entity_collection;  // POINTER, not converted!
};

// Provider interface
class ISceneDataProvider {
  virtual std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType type) const = 0;
};

// FlatBuffers provider: Minimal conversion
class FlatbuffersSceneDataProvider : public ISceneDataProvider {
  SceneData LoadSceneData(SceneType type) const override {
    auto fbs = m_loader.ProvideDefaultSceneData(type);
    
    return SceneData{
      .type = fbs->scene_info()->scene_type(),
      .render_texture_width = fbs->scene_resources()->texture_width(),
      .render_texture_height = fbs->scene_resources()->texture_height(),
      .entity_collection = fbs->entity_collection()  // Just pointer!
    };
  }
};

// XML provider: Would load XML and convert
class XMLSceneDataProvider : public ISceneDataProvider {
  SceneData LoadSceneData(SceneType type) const override {
    auto xml = LoadXML(type);
    
    // Convert XML to same SceneData format
    // entity_collection would point to XMLEntityCollection
    // (or convert XML to FlatBuffers first)
  }
};
```

**This approach**:
- ✅ Minimal SceneData (just pointers and scalars)
- ✅ No heavy conversion logic
- ✅ EntityMemoryPool configured once from FlatBuffers
- ✅ Non-copyable resources never copied
- ✅ Provider pattern allows future XML/JSON support
- ✅ Configurator abstracted from FlatBuffers

---

## Summary

**Your concerns are valid and addressed**:

1. **POD structs**: Only configuration values become POD. Resources stay as resources.

2. **Copyability**: Only small config values copied (~50 bytes). Resources never copied.

3. **Configure twice**: NO. EntityMemoryPool configured ONCE from FlatBuffers pointer stored in SceneData.

4. **Non-copyable resources**: Never in SceneData. Created empty, configured in place.

**The SceneData approach doesn't mean**:
- ❌ Copying sf::RenderTexture
- ❌ Copying EntityMemoryPool  
- ❌ Converting all FlatBuffers to native structs
- ❌ Configuring things twice

**The SceneData approach means**:
- ✅ Small struct with config values and pointers
- ✅ Provider abstracts FlatBuffers access
- ✅ Configurator works with any data source
- ✅ Resources configured in place, once
- ✅ Minimal overhead (~50 bytes of copies)

**If you still want to avoid SceneData entirely**, the alternative (pass FlatBuffers pointers through) is valid. It just means you can't easily add XML/JSON support later without more refactoring. But if you're happy with FlatBuffers, that's a perfectly reasonable trade-off!
