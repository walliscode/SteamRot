# SceneData True Abstraction - No FlatBuffers Dependencies

**Created**: December 12, 2025  
**Purpose**: Address the requirement that SceneData must be truly agnostic (no FlatBuffers types)

---

## The Critical Point

> "But the SceneData can't contain FlatBuffers data right? It's got to be agnostic"

**Absolutely correct!** If SceneData is to provide true abstraction, it CANNOT contain FlatBuffers-specific types like `const EntityCollectionFbs*`.

The previous analysis showed `const EntityCollectionFbs*` in SceneData - **that was wrong** for the abstraction goal.

---

## True Abstraction: SceneData Must Be Pure Native C++

### What SceneData CAN Contain

```cpp
struct SceneData {
  // Pure native C++ types only!
  
  // POD configuration
  SceneInfo info;                    // uuid + SceneType enum
  uint32_t render_texture_width;
  uint32_t render_texture_height;
  
  // Native entity data (converted from FlatBuffers)
  struct EntityCollection {
    uint32_t pool_size;
    std::vector<EntityData> entities;
  } entity_collection;
  
  // Where EntityData is also native:
  struct EntityData {
    uint32_t index;
    std::optional<CUserInterfaceData> ui_component;
    std::optional<CGrimoireMachinaData> grimoire_component;
    // etc.
  };
};
```

### What SceneData CANNOT Contain

```cpp
struct SceneData {
  // ❌ NO FLATBUFFERS TYPES!
  const EntityCollectionFbs *entity_data;  // ❌ Wrong!
  const SceneDataFbs *scene_fbs;           // ❌ Wrong!
  
  // ❌ NO SFML RESOURCES!
  sf::RenderTexture texture;                // ❌ Wrong (non-copyable)
  
  // ❌ NO RAW ENTITY POOL!
  EntityMemoryPool pool;                    // ❌ Wrong (huge, non-copyable)
};
```

---

## The True Cost of Abstraction

### What Needs To Happen

**Provider must convert FlatBuffers → Native C++**:

```cpp
class FlatbuffersDefaultSceneDataProvider : public ISceneDataProvider {
  std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType type) const override {
    
    // 1. Load FlatBuffers
    auto fbs = m_loader.ProvideDefaultSceneData(type);
    if (!fbs.has_value())
      return std::unexpected(fbs.error());
    
    // 2. Convert to native SceneData
    SceneData scene_data;
    
    // Simple POD copies
    scene_data.info.type = fbs.value()->scene_info()->scene_type();
    scene_data.render_texture_width = 
        fbs.value()->scene_resources()->render_texture_width();
    
    // 3. CONVERT ENTITY DATA (this is the "heavy" part)
    auto entity_fbs = fbs.value()->entity_collection();
    scene_data.entity_collection.pool_size = 
        entity_fbs->entity_memory_pool_size();
    
    // Convert each entity
    for (const auto* entity_fbs : *entity_fbs->entities()) {
      EntityData entity_data;
      entity_data.index = entity_fbs->index();
      
      // Convert UI component if present
      if (entity_fbs->c_user_interface()) {
        CUserInterfaceData ui_data;
        ui_data.name = entity_fbs->c_user_interface()->ui_name()->str();
        ui_data.visible = entity_fbs->c_user_interface()->is_visible();
        // Convert UIElement tree...
        entity_data.ui_component = ui_data;
      }
      
      // Convert other components...
      scene_data.entity_collection.entities.push_back(entity_data);
    }
    
    return scene_data;
  }
};
```

### Then Configurator Converts Native → Scene

```cpp
class DefaultSceneConfigurator : public ISceneConfigurator {
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData &data) override {
    
    // Configure POD values
    scene.m_scene_info = data.info;
    
    // Configure RenderTexture
    scene.m_scene_resources.scene_texture.create(
        data.render_texture_width,
        data.render_texture_height);
    
    // Configure EntityMemoryPool from native data
    auto &emp = scene.m_scene_resources.entity_manager.GetEntityMemoryPool();
    
    // Resize pool
    entity::memory::ResizeEntityMemoryPool(
        emp, data.entity_collection.pool_size);
    
    // Configure each entity
    for (const auto &entity_data : data.entity_collection.entities) {
      if (entity_data.ui_component) {
        auto &ui_comp = entity::memory::GetComponent<CUserInterface>(
            entity_data.index, emp);
        ui_comp.m_name = entity_data.ui_component->name;
        ui_comp.m_visible = entity_data.ui_component->visible;
        // Construct UIElement tree from native data...
      }
      // Configure other components...
    }
    
    return std::monostate{};
  }
};
```

---

## Yes, This IS Duplication

### The Reality

**You were right to be concerned.** With true abstraction:

1. **Provider converts**: FlatBuffers → Native SceneData
   - Extract scalars
   - Convert vectors
   - Build native structs

2. **Configurator converts**: Native SceneData → Scene objects
   - Apply scalars to Scene members
   - Populate EntityMemoryPool
   - Construct component trees

**This is indeed "configuring twice" in a sense:**
- Once to convert from FlatBuffers format to native format
- Once to apply native format to Scene objects

---

## The Trade-Off Is Real

### With Full Abstraction (Native SceneData)

**Pros**:
- ✅ Configurator completely agnostic to FlatBuffers
- ✅ Can add XML/JSON providers easily
- ✅ Configurator testable with mock data

**Cons**:
- ❌ Conversion logic in provider (FlatBuffers → Native)
- ❌ Configuration logic in configurator (Native → Scene)
- ❌ Duplication of entity/component handling
- ❌ Memory allocation for native SceneData
- ❌ Performance overhead (convert then apply)

### Without Abstraction (Pass FlatBuffers Pointer)

**Pros**:
- ✅ No duplication - configure directly from FlatBuffers
- ✅ No intermediate allocations
- ✅ Better performance
- ✅ Simpler code

**Cons**:
- ❌ Configurator coupled to FlatBuffers
- ❌ Cannot easily add XML/JSON
- ❌ Testing requires FlatBuffers data

---

## The Honest Assessment

### If You Need True Abstraction

**You MUST accept the duplication.** There's no way around it:

```
FlatBuffers Data → [Provider converts] → Native SceneData → [Configurator applies] → Scene
```

Both conversions are necessary:
1. **Provider**: Format-specific → Format-agnostic
2. **Configurator**: Data structs → Live objects

This is the **cost of abstraction**.

### If You Don't Need XML/JSON Support Soon

**The pragmatic approach** (pass FlatBuffers pointers) **is better**:

```
FlatBuffers Data → [Configurator applies directly] → Scene
```

Only one conversion: Data → Live objects

**When you actually need XML support**, then refactor to add abstraction. Don't pay the cost until you need the benefit (YAGNI principle).

---

## Recommendation: Pragmatic Middle Ground

### Option: Delayed Abstraction

**Current state**: Improve architecture without full abstraction

```cpp
// Phase 1: Pass FlatBuffers, but clean up data loading
class ISceneConfigurator {
  // Takes FlatBuffers pointer (not fully abstract)
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneDataFbs *data) = 0;
};

// SceneManager loads once
auto scene_data = loader.ProvideDefaultSceneData(scene_type);
auto scene = factory.CreateSceneByType(scene_type);
configurator.ConfigureScene(*scene, scene_data.value());

// Benefits:
// - Eliminates redundant loading
// - Clarifies responsibilities
// - Simple, performant
// - No duplication

// Trade-off:
// - Coupled to FlatBuffers
```

**Future state**: Add abstraction when XML/JSON needed

```cpp
// Phase 2: Add provider layer when needed
class ISceneDataProvider {
  virtual std::expected<SceneData, FailInfo>
  LoadSceneData(SceneType type) const = 0;
};

// Refactor configurator to take native SceneData
class ISceneConfigurator {
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData &data) = 0;
};

// When this is needed:
// - Add XML support requirement emerges
// - Testing needs become more complex
// - Multiple data sources actually exist
```

---

## Alternative: Minimal Native SceneData

### Hybrid Approach

If you want SOME abstraction without FULL duplication:

```cpp
// SceneData with minimal conversion
struct SceneData {
  // Simple scalars (cheap to copy)
  SceneType type;
  uint32_t render_texture_width;
  uint32_t render_texture_height;
  uint32_t entity_pool_size;
  
  // Complex data: Use std::function or std::any
  std::function<void(EntityMemoryPool&, EventHandler&)> configure_entities;
};

// FlatBuffers provider
SceneData LoadSceneData(SceneType type) const override {
  auto fbs = m_loader.ProvideDefaultSceneData(type);
  
  SceneData data;
  data.type = fbs->scene_info()->scene_type();
  data.render_texture_width = fbs->scene_resources()->render_texture_width();
  data.entity_pool_size = fbs->entity_collection()->entity_memory_pool_size();
  
  // Capture FlatBuffers data in lambda (provider-specific)
  auto entity_fbs = fbs->entity_collection();
  data.configure_entities = [entity_fbs](EntityMemoryPool& emp, EventHandler& handler) {
    FlatbuffersEntityConfigurator config(handler, *entity_fbs);
    config.ConfigureEntityMemoryPool(emp);
  };
  
  return data;
}

// XML provider
SceneData LoadSceneData(SceneType type) const override {
  auto xml = LoadXML(type);
  
  SceneData data;
  data.type = xml->GetSceneType();
  data.render_texture_width = xml->GetTextureWidth();
  
  // Capture XML data in lambda
  auto entity_xml = xml->GetEntityCollection();
  data.configure_entities = [entity_xml](EntityMemoryPool& emp, EventHandler& handler) {
    XMLEntityConfigurator config(handler, entity_xml);
    config.ConfigureEntityMemoryPool(emp);
  };
  
  return data;
}

// Configurator (calls lambda, doesn't know source!)
void ConfigureScene(Scene &scene, const SceneData &data) {
  scene.m_scene_info.type = data.type;
  scene.m_scene_resources.scene_texture.create(
      data.render_texture_width, data.render_texture_height);
  
  data.configure_entities(scene.GetEntityMemoryPool(), m_event_handler);
}
```

**This approach**:
- ✅ SceneData is format-agnostic (no FlatBuffers types)
- ✅ No duplication (lambda configures directly from source)
- ✅ Configurator abstracted from source format
- ⚠️ More complex (lambdas capture source data)
- ⚠️ Less "pure" (mixing data and behavior)

---

## Final Recommendation

### For Your Use Case

Based on your concerns about duplication, I recommend:

**Go with Approach 2 (Pass FlatBuffers Pointers)** for now:

```cpp
// Clean up current architecture without full abstraction
class ISceneConfigurator {
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneDataFbs *data) = 0;
};

// SceneManager
auto scene_data = loader.ProvideDefaultSceneData(scene_type);
auto scene = factory.CreateSceneByType(scene_type);
configurator.ConfigureScene(*scene, scene_data.value());
```

**Why**:
1. No duplication
2. Simple and performant
3. Improves current architecture (eliminates redundant loading)
4. You're not actually planning XML/JSON support right now
5. Can refactor to full abstraction later IF needed

**When to reconsider**:
- If/when you need XML/JSON/test data sources
- If testing becomes difficult without abstraction
- If FlatBuffers coupling causes real problems

---

## Summary

**Your instinct was correct**: True abstraction with native SceneData DOES cause duplication:
1. Provider converts FlatBuffers → Native
2. Configurator converts Native → Scene

**The choice is real**:
- **Abstraction**: Pay cost of duplication, gain flexibility
- **Pragmatic**: Skip abstraction, avoid duplication, defer flexibility

**Recommendation**: Pass FlatBuffers pointers for now. Add abstraction layer later IF you actually need multiple data formats.

**YAGNI** (You Aren't Gonna Need It): Don't build abstraction you don't need yet. The cost is real, and the benefit is hypothetical until you actually add XML/JSON support.
