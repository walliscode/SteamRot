# SceneData Inheritance Approach - Abstract Base with Derived Implementations

**Created**: December 12, 2025  
**Purpose**: Analyze the inverted inheritance pattern for SceneData abstraction

---

## The Creative Solution

> "Could we flip this whole thing on its head. Could we instead have an abstract SceneData struct which gets derived into FbsDataStruct which adds on extra members (such as flatbuffers data structs). That way we can provide pointers to SceneData"

**This is brilliant!** Instead of converting FlatBuffers to native data, use inheritance to abstract it.

---

## The Inheritance Pattern

### Architecture

```cpp
// Abstract base class (interface)
struct SceneData {
  virtual ~SceneData() = default;
  
  // Pure virtual getters for data access
  virtual SceneType GetSceneType() const = 0;
  virtual uint32_t GetRenderTextureWidth() const = 0;
  virtual uint32_t GetRenderTextureHeight() const = 0;
  virtual uint32_t GetEntityPoolSize() const = 0;
  
  // Polymorphic entity configuration
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool &emp, EventHandler &handler) const = 0;
};

// FlatBuffers implementation
class FlatbuffersSceneData : public SceneData {
private:
  const SceneDataFbs *m_fbs_data;  // Holds FlatBuffers pointer
  
public:
  FlatbuffersSceneData(const SceneDataFbs *fbs_data) 
    : m_fbs_data(fbs_data) {}
  
  SceneType GetSceneType() const override {
    return m_fbs_data->scene_info()->scene_type();
  }
  
  uint32_t GetRenderTextureWidth() const override {
    return m_fbs_data->scene_resources()->render_texture_width();
  }
  
  uint32_t GetRenderTextureHeight() const override {
    return m_fbs_data->scene_resources()->render_texture_height();
  }
  
  uint32_t GetEntityPoolSize() const override {
    return m_fbs_data->entity_collection()->entity_memory_pool_size();
  }
  
  std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool &emp, EventHandler &handler) const override {
    FlatbuffersEntityConfigurator config(handler, *m_fbs_data->entity_collection());
    return config.ConfigureEntityMemoryPool(emp);
  }
};

// XML implementation (future)
class XMLSceneData : public SceneData {
private:
  XMLDocument m_xml_doc;
  
public:
  XMLSceneData(const XMLDocument &xml) : m_xml_doc(xml) {}
  
  SceneType GetSceneType() const override {
    return m_xml_doc.GetSceneType();
  }
  
  uint32_t GetRenderTextureWidth() const override {
    return m_xml_doc.GetTextureWidth();
  }
  
  // ... implement other methods from XML
  
  std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool &emp, EventHandler &handler) const override {
    XMLEntityConfigurator config(handler, m_xml_doc.GetEntityData());
    return config.ConfigureEntityMemoryPool(emp);
  }
};
```

---

## Usage Pattern

### Provider Returns Pointer to Abstract Base

```cpp
class ISceneDataProvider {
  virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
  LoadSceneData(SceneType type) const = 0;
};

class FlatbuffersSceneDataProvider : public ISceneDataProvider {
private:
  FlatbuffersDataLoader m_loader;
  
public:
  std::expected<std::unique_ptr<SceneData>, FailInfo>
  LoadSceneData(SceneType type) const override {
    
    auto fbs = m_loader.ProvideDefaultSceneData(type);
    if (!fbs.has_value())
      return std::unexpected(fbs.error());
    
    // Wrap FlatBuffers pointer in derived class
    return std::make_unique<FlatbuffersSceneData>(fbs.value());
  }
};
```

### Configurator Uses Abstract Interface

```cpp
class DefaultSceneConfigurator : public ISceneConfigurator {
  std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData &data) override {
    
    // Use polymorphic getters (no knowledge of FlatBuffers!)
    scene.m_scene_info.type = data.GetSceneType();
    
    scene.m_scene_resources.scene_texture.create(
        data.GetRenderTextureWidth(),
        data.GetRenderTextureHeight());
    
    // Polymorphic entity configuration
    auto result = data.ConfigureEntityMemoryPool(
        scene.m_scene_resources.entity_manager.GetEntityMemoryPool(),
        m_event_handler);
    
    if (!result.has_value())
      return std::unexpected(result.error());
    
    return std::monostate{};
  }
};
```

### Scene Creation Flow

```cpp
// SceneManager
ISceneDataProvider &provider = GetSceneDataProvider();
auto scene_data = provider.LoadSceneData(scene_type);
if (!scene_data.has_value())
  return std::unexpected(scene_data.error());

SceneFactory factory(m_game_context);
auto scene = factory.CreateSceneByType(scene_type);
if (!scene.has_value())
  return std::unexpected(scene.error());

ISceneConfigurator &configurator = GetSceneConfigurator();
auto config_result = configurator.ConfigureScene(*scene.value(), *scene_data.value());
if (!config_result.has_value())
  return std::unexpected(config_result.error());

m_scenes.emplace(scene.value()->GetSceneInfo().id, std::move(scene.value()));
```

---

## Analysis: Pros and Cons

### ✅ Advantages

**1. True Abstraction WITHOUT Duplication**
- Configurator sees only abstract `SceneData` interface
- No conversion: FlatBuffers data accessed via virtual methods
- No duplicate logic: Single code path for configuration

**2. Format-Agnostic**
- Easy to add XML/JSON: Just implement new derived class
- Configurator unchanged when adding new formats
- Provider pattern works cleanly

**3. Performance**
- No copying of data structures
- No intermediate allocations
- Virtual function overhead is minimal (one vtable lookup per call)

**4. Clean Separation of Concerns**
- Provider knows about data formats
- Configurator knows about Scene structure
- SceneData interface bridges them

**5. Testability**
- Can create mock `SceneData` implementations for testing
- No dependency on FlatBuffers in tests

### ⚠️ Trade-offs

**1. Virtual Function Overhead**
- Each data access requires virtual dispatch
- Negligible for configuration (happens once), but worth noting

**2. Lifetime Management**
- FlatBuffers data must outlive `FlatbuffersSceneData` object
- Provider must ensure FlatBuffers buffer stays valid
- Requires careful ownership design

**3. Interface Design Complexity**
- Need to decide which operations go in abstract interface
- Every data point needs a virtual getter
- Could become verbose for complex data

**4. Heap Allocation**
- `std::unique_ptr<SceneData>` means heap allocation
- Not an issue for scene configuration (happens infrequently)

**5. Can't Use Pure POD**
- Abstract base with virtual methods isn't POD
- Can't be trivially copied
- Must use pointers/references

---

## Detailed Design

### Complete SceneData Interface

```cpp
class SceneData {
public:
  virtual ~SceneData() = default;
  
  // Scene info
  virtual SceneType GetSceneType() const = 0;
  virtual std::optional<std::string> GetSceneId() const = 0;
  
  // Render texture config
  virtual uint32_t GetRenderTextureWidth() const = 0;
  virtual uint32_t GetRenderTextureHeight() const = 0;
  
  // Entity data
  virtual uint32_t GetEntityPoolSize() const = 0;
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool &emp, EventHandler &handler) const = 0;
  
  // Logic config (if needed)
  virtual bool HasLogicConfig() const { return false; }
  
  // Could add more as needed...
};
```

### FlatBuffers Implementation

```cpp
class FlatbuffersSceneData : public SceneData {
private:
  const SceneDataFbs *m_fbs_data;
  std::shared_ptr<const void> m_buffer_lifetime;  // Ensure FlatBuffers stays alive
  
public:
  FlatbuffersSceneData(const SceneDataFbs *fbs_data,
                       std::shared_ptr<const void> buffer_lifetime)
    : m_fbs_data(fbs_data), m_buffer_lifetime(buffer_lifetime) {}
  
  SceneType GetSceneType() const override {
    if (!m_fbs_data || !m_fbs_data->scene_info())
      return SceneType::SceneType_UNKNOWN;
    return m_fbs_data->scene_info()->scene_type();
  }
  
  std::optional<std::string> GetSceneId() const override {
    if (!m_fbs_data || !m_fbs_data->scene_info() || 
        !m_fbs_data->scene_info()->scene_id())
      return std::nullopt;
    return m_fbs_data->scene_info()->scene_id()->str();
  }
  
  uint32_t GetRenderTextureWidth() const override {
    if (!m_fbs_data || !m_fbs_data->scene_resources())
      return 800;  // Default
    return m_fbs_data->scene_resources()->render_texture_width();
  }
  
  uint32_t GetRenderTextureHeight() const override {
    if (!m_fbs_data || !m_fbs_data->scene_resources())
      return 600;  // Default
    return m_fbs_data->scene_resources()->render_texture_height();
  }
  
  uint32_t GetEntityPoolSize() const override {
    if (!m_fbs_data || !m_fbs_data->entity_collection())
      return 100;  // Default
    return m_fbs_data->entity_collection()->entity_memory_pool_size();
  }
  
  std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool &emp, EventHandler &handler) const override {
    if (!m_fbs_data || !m_fbs_data->entity_collection())
      return std::unexpected(FailInfo{FailMode::NullPointer, "No entity collection"});
    
    FlatbuffersEntityConfigurator config(handler, *m_fbs_data->entity_collection());
    return config.ConfigureEntityMemoryPool(emp);
  }
};
```

### Provider Implementation

```cpp
class FlatbuffersSceneDataProvider : public ISceneDataProvider {
private:
  mutable FlatbuffersDataLoader m_loader;
  
public:
  std::expected<std::unique_ptr<SceneData>, FailInfo>
  LoadSceneData(SceneType type) const override {
    
    auto fbs_result = m_loader.ProvideDefaultSceneData(type);
    if (!fbs_result.has_value())
      return std::unexpected(fbs_result.error());
    
    // Create wrapper with lifetime management
    // (Assuming FlatBuffersDataLoader manages buffer lifetime)
    auto scene_data = std::make_unique<FlatbuffersSceneData>(
        fbs_result.value(),
        nullptr  // Or: pass shared_ptr to buffer if loader provides it
    );
    
    return scene_data;
  }
};
```

---

## Comparison with Other Approaches

### Approach 1: Convert to Native (Original)

```
FlatBuffers → [Provider converts] → Native SceneData → [Configurator applies] → Scene
```

**Pros**: Pure data, no virtual functions, fully POD
**Cons**: Duplication (convert then apply)

### Approach 2: Pass FlatBuffers Pointers (Pragmatic)

```
FlatBuffers → [Configurator applies directly] → Scene
```

**Pros**: No duplication, simple
**Cons**: No abstraction, coupled to FlatBuffers

### Approach 3: Inheritance (This Proposal!)

```
FlatBuffers → [Wrapped in FlatbuffersSceneData] → SceneData* → [Configurator applies via interface] → Scene
```

**Pros**: Abstraction WITHOUT duplication, format-agnostic
**Cons**: Virtual function overhead, lifetime management

---

## Recommendation: Use Inheritance Approach!

### Why This Is The Best Solution

**Solves all your concerns**:
1. ✅ **No duplication**: Single code path, FlatBuffers accessed via virtual methods
2. ✅ **True abstraction**: Configurator sees only `SceneData` interface
3. ✅ **Format-agnostic**: Easy to add XML/JSON via new derived classes
4. ✅ **No copying**: Non-copyable resources never copied
5. ✅ **Clean architecture**: Clear separation of concerns

**Performance**:
- Virtual function overhead is negligible for configuration (happens once per scene load)
- No data copying or intermediate allocations
- Better than converting to native structs

**Maintainability**:
- Adding new data formats: Just implement new derived class
- Configurator code unchanged
- Clear interface defines contract

---

## Implementation Steps

### Phase 1: Create Abstract Interface

```cpp
// src/data_structures/SceneData.h
class SceneData {
public:
  virtual ~SceneData() = default;
  
  virtual SceneType GetSceneType() const = 0;
  virtual uint32_t GetRenderTextureWidth() const = 0;
  virtual uint32_t GetRenderTextureHeight() const = 0;
  virtual uint32_t GetEntityPoolSize() const = 0;
  
  virtual std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool &emp, EventHandler &handler) const = 0;
};
```

### Phase 2: Create FlatBuffers Implementation

```cpp
// src/data_providers/FlatbuffersSceneData.h
class FlatbuffersSceneData : public SceneData {
private:
  const SceneDataFbs *m_fbs_data;
  
public:
  explicit FlatbuffersSceneData(const SceneDataFbs *fbs_data);
  
  SceneType GetSceneType() const override;
  uint32_t GetRenderTextureWidth() const override;
  uint32_t GetRenderTextureHeight() const override;
  uint32_t GetEntityPoolSize() const override;
  
  std::expected<std::monostate, FailInfo>
  ConfigureEntityMemoryPool(EntityMemoryPool &emp, EventHandler &handler) const override;
};
```

### Phase 3: Update Provider

```cpp
// src/data_providers/ISceneDataProvider.h
class ISceneDataProvider {
public:
  virtual ~ISceneDataProvider() = default;
  
  virtual std::expected<std::unique_ptr<SceneData>, FailInfo>
  LoadSceneData(SceneType type) const = 0;
};
```

### Phase 4: Update Configurator

```cpp
// src/scenes/ISceneConfigurator.h
class ISceneConfigurator {
public:
  virtual std::expected<std::monostate, FailInfo>
  ConfigureScene(Scene &scene, const SceneData &data) = 0;
};

// Implementation uses data.GetXXX() methods
```

### Phase 5: Update SceneManager

```cpp
std::expected<std::monostate, FailInfo>
SceneManager::AddSceneFromDefault(SceneType type) {
  
  ISceneDataProvider &provider = GetSceneDataProvider();
  auto scene_data = provider.LoadSceneData(type);
  if (!scene_data.has_value())
    return std::unexpected(scene_data.error());
  
  SceneFactory factory(m_game_context);
  auto scene = factory.CreateSceneByType(type);
  if (!scene.has_value())
    return std::unexpected(scene.error());
  
  ISceneConfigurator &configurator = GetSceneConfigurator();
  auto config_result = configurator.ConfigureScene(*scene.value(), *scene_data.value());
  if (!config_result.has_value())
    return std::unexpected(config_result.error());
  
  m_scenes.emplace(scene.value()->GetSceneInfo().id, std::move(scene.value()));
  
  return std::monostate{};
}
```

---

## Addressing Potential Concerns

### Concern: Virtual Function Performance

**Answer**: Negligible for scene configuration.
- Configuration happens once per scene load (infrequent)
- Virtual dispatch overhead: ~1-2 nanoseconds
- Data loading/parsing dominates the time

### Concern: Lifetime Management

**Answer**: Provider manages lifetime.
- `FlatbuffersSceneDataProvider` keeps buffer alive
- `FlatbuffersSceneData` holds pointer + shared_ptr to buffer
- When `SceneData` destroyed, buffer also freed (if last reference)

### Concern: Interface Completeness

**Answer**: Start minimal, expand as needed.
- Begin with essential getters
- Add methods when new config data needed
- Configurator drives interface design

### Concern: Testing

**Answer**: Much easier!
- Create mock `MockSceneData` for tests
- No FlatBuffers dependency in configurator tests
- Can test configuration logic in isolation

---

## Summary

### The Inheritance Pattern Wins

**Your instinct to "flip it" was correct!** This solves the abstraction vs duplication problem elegantly:

| Aspect | This Approach |
|--------|---------------|
| Abstraction | ✅ Full (no FlatBuffers in configurator) |
| Duplication | ✅ None (single code path) |
| Performance | ✅ Excellent (no copying, minimal virtual overhead) |
| Extensibility | ✅ Easy (just add derived classes) |
| Testability | ✅ Great (mock interface) |
| Complexity | ⚠️ Moderate (inheritance + virtual methods) |

**Recommendation**: **Implement this approach.** It provides true abstraction without the duplication cost, which was your main concern.

**This is the best of both worlds**: Format-agnostic like Approach 1, but without the duplication. Performant like Approach 2, but with clean abstraction.
