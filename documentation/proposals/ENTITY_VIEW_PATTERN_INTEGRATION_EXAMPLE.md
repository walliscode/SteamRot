# Entity View Pattern - Step-by-Step Integration Example

**Date**: December 5, 2025  
**Type**: Practical Integration Guide  
**Related**: [Implementation Plan](ENTITY_VIEW_PATTERN_IMPLEMENTATION.md), [Quick Reference](ENTITY_VIEW_PATTERN_QUICK_REF.md)

---

## Overview

This document provides a **complete step-by-step example** of how to integrate the view pattern into SteamRot, using the existing `IEngineDataProvider` pattern as a reference. We'll show exactly how the entity view pattern would work in the same way.

---

## Current Pattern: IEngineDataProvider (Simple Data)

Let's first understand how the **simple provider pattern** currently works for engine data, then we'll show how the **view pattern** extends this for complex nested entity data.

### Step 1: Define Native Struct (Simple Data)

**File**: `src/data_providers/IEngineDataProvider.h`

```cpp
// Native C++ struct - no FlatBuffers dependency
struct EngineCoreData {
  uint32_t window_width{800};
  uint32_t window_height{600};
  std::string window_title{"SteamRot"};
  uint32_t framerate_limit{60};
};
```

**Key Point**: For simple data (just primitives and strings), we extract everything into a native struct.

---

### Step 2: Define Provider Interface (Simple Data)

**File**: `src/data_providers/IEngineDataProvider.h`

```cpp
class IEngineDataProvider {
public:
  virtual ~IEngineDataProvider() = default;
  
  // Returns native struct (copied from data source)
  virtual std::expected<EngineCoreData, FailInfo>
  LoadEngineCoreData() const = 0;
};
```

**Key Point**: The interface returns the native struct directly. Simple and clean.

---

### Step 3: Implement FlatBuffers Provider (Simple Data)

**File**: `src/data_providers/FlatbuffersEngineDataProvider.cpp`

```cpp
std::expected<EngineCoreData, FailInfo>
FlatbuffersEngineDataProvider::LoadEngineCoreData() const {
  // Load FlatBuffers data
  auto fb_result = m_loader.ProvideEngineCoreData();
  if (!fb_result.has_value()) {
    return std::unexpected(fb_result.error());
  }
  
  const auto* fb_data = fb_result.value();
  
  // Convert FlatBuffers → Native struct (copy data)
  EngineCoreData native_data;
  native_data.window_width = fb_data->window_width();
  native_data.window_height = fb_data->window_height();
  if (fb_data->window_title()) {
    native_data.window_title = fb_data->window_title()->str();
  }
  native_data.framerate_limit = fb_data->framerate_limit();
  
  return native_data;  // Return copied struct
}
```

**Key Point**: Provider converts FlatBuffers types to native struct. Game code never sees FlatBuffers.

---

### Step 4: Use in Game Code (Simple Data)

**File**: `src/engine/Engine.cpp`

```cpp
std::expected<std::monostate, FailInfo> Engine::StartUp() {
  // Get provider (format-independent)
  IEngineDataProvider& data_provider = GetEngineDataProvider();
  
  // Load data (returns native struct)
  auto engine_core_result = data_provider.LoadEngineCoreData();
  if (!engine_core_result) {
    return std::unexpected(engine_core_result.error());
  }
  
  // Use native struct (no FlatBuffers types!)
  const EngineCoreData& data = engine_core_result.value();
  
  // Configure from native data
  auto configure_result = core::ConfigureGameCore(m_game_core, data);
  if (!configure_result) {
    return std::unexpected(configure_result.error());
  }
  
  return std::monostate{};
}
```

**Key Point**: Game code works with `EngineCoreData` (native struct) - no format dependency.

---

## Why Views for Entity Data?

**Problem**: Entity data is **deeply nested and polymorphic**:

```
EntityCollection
  └─ EntityData[]
      └─ Components
          └─ CUserInterface
              └─ UIElement (polymorphic, recursive)
                  └─ Children[] (more UIElements)
                      └─ Children[] (and more...)
```

**Issue with Simple Struct Approach**:
```cpp
// This would require copying EVERYTHING
struct EntityData {
  std::vector<ComponentData> components;  // Copy all components
};

struct ComponentData {
  std::optional<UIData> ui;  // Copy UI tree
};

struct UIData {
  std::unique_ptr<UIElementData> root;  // Copy entire UI tree!
};

struct UIElementData {
  std::vector<UIElementData> children;  // Recursive copying!
};
```

**Memory overhead**: Potentially megabytes of copied data just to configure entities.

**Solution**: Use **views** to navigate without copying.

---

## View Pattern: IEntityDataProvider (Complex Nested Data)

Now let's see how the view pattern works for complex entity data.

### Step 1: Define View Interfaces (Instead of Native Structs)

**File**: `src/entity/IEntityCollectionView.h`

```cpp
namespace steamrot {

// Forward declarations
class IEntityView;

//==============================================================================
// IEntityCollectionView - Top-level view interface
//==============================================================================

class IEntityCollectionView {
public:
  virtual ~IEntityCollectionView() = default;
  
  // Simple queries (primitives returned directly)
  virtual size_t GetEntityCount() const = 0;
  virtual size_t GetPoolSize() const = 0;
  
  // Navigation (returns view, not copied data)
  virtual std::unique_ptr<IEntityView> GetEntity(size_t index) const = 0;
  
  // Iteration helper
  virtual void ForEachEntity(
      std::function<void(const IEntityView&)> callback) const = 0;
};

} // namespace steamrot
```

**Key Point**: Unlike `EngineCoreData` (struct), `IEntityCollectionView` is an interface for **navigating** data without copying.

---

### Step 2: Define Nested View Interfaces

**File**: `src/entity/IEntityView.h`

```cpp
namespace steamrot {

// Native struct for simple entity metadata (can be copied safely)
struct EntityMetadata {
  uint32_t index;
  bool active;
};

class IEntityView {
public:
  virtual ~IEntityView() = default;
  
  // Extract simple metadata as native struct (small, copyable)
  virtual EntityMetadata GetMetadata() const = 0;
  
  // Component queries
  virtual bool HasComponent(size_t component_register_index) const = 0;
  
  template<typename TComponent>
  bool HasComponent() const {
    return HasComponent(TupleTypeIndex<TComponent, ComponentRegister>);
  }
  
  // Navigation to components (returns view, not copied data)
  virtual std::unique_ptr<IComponentView> GetComponent(
      size_t component_register_index) const = 0;
};

} // namespace steamrot
```

**Key Point**: 
- Small data → Native struct (`EntityMetadata`)
- Nested data → View interface (`IComponentView`)

---

### Step 3: Define Component View Interfaces

**File**: `src/entity/IUserInterfaceView.h`

```cpp
namespace steamrot {

// Native struct for simple UI data (small, copyable)
struct UserInterfaceSimpleData {
  std::string ui_name;
  bool start_visible;
};

class IUIElementView;  // Forward declaration

class IUserInterfaceView : public IComponentView {
public:
  // Extract simple data as native struct
  virtual UserInterfaceSimpleData GetSimpleData() const = 0;
  
  // Or access individually
  virtual std::string GetUIName() const = 0;
  virtual bool GetStartVisible() const = 0;
  
  // Navigate to nested element (view, not copied)
  virtual bool HasRootElement() const = 0;
  virtual std::unique_ptr<IUIElementView> GetRootElement() const = 0;
};

} // namespace steamrot
```

**Key Point**: Views let you navigate down (zero-copy) and extract simple data up (small copies).

---

### Step 4: Define Provider Interface

**File**: `src/entity/IEntityDataProvider.h`

```cpp
namespace steamrot {

class IEntityDataProvider {
public:
  virtual ~IEntityDataProvider() = default;
  
  // Returns VIEW (not copied data)
  virtual std::expected<std::unique_ptr<IEntityCollectionView>, FailInfo>
  LoadEntityCollection(SceneType scene_type) const = 0;
};

} // namespace steamrot
```

**Key Point**: Provider returns a **view** to the data, not copied data.

---

### Step 5: Implement FlatBuffers Provider (View Implementation)

**File**: `src/entity/FlatbuffersEntityDataProvider.h`

```cpp
namespace steamrot {

class FlatbuffersEntityDataProvider : public IEntityDataProvider {
private:
  FlatbuffersDataLoader m_loader;
  
public:
  std::expected<std::unique_ptr<IEntityCollectionView>, FailInfo>
  LoadEntityCollection(SceneType scene_type) const override;
};

} // namespace steamrot
```

**File**: `src/entity/FlatbuffersEntityDataProvider.cpp`

```cpp
std::expected<std::unique_ptr<IEntityCollectionView>, FailInfo>
FlatbuffersEntityDataProvider::LoadEntityCollection(SceneType scene_type) const {
  // Load FlatBuffers data
  auto fb_result = m_loader.ProvideDefaultSceneData(scene_type);
  if (!fb_result.has_value()) {
    return std::unexpected(fb_result.error());
  }
  
  const auto* fb_scene_data = fb_result.value();
  const auto* fb_entity_collection = fb_scene_data->entity_collection();
  
  if (!fb_entity_collection) {
    return std::unexpected(FailInfo{FailMode::NullData, "No entity collection"});
  }
  
  // Create shared buffer ownership (keeps FlatBuffers data alive)
  auto buffer = std::make_shared<std::vector<uint8_t>>(/* loaded data */);
  
  // Wrap in view (zero-copy, just wraps pointer)
  return std::make_unique<FlatbuffersEntityCollectionView>(
      buffer, fb_entity_collection);
}
```

**Key Point**: Provider wraps FlatBuffers data in a view. No copying!

---

### Step 6: Implement Concrete View Classes

**File**: `src/entity/FlatbuffersEntityCollectionView.h`

```cpp
namespace steamrot {

class FlatbuffersEntityCollectionView : public IEntityCollectionView {
private:
  // Shared ownership of buffer (keeps data alive)
  std::shared_ptr<std::vector<uint8_t>> m_buffer;
  
  // Pointer to FlatBuffers data (within buffer)
  const EntityCollection* m_fb_collection;
  
public:
  FlatbuffersEntityCollectionView(
      std::shared_ptr<std::vector<uint8_t>> buffer,
      const EntityCollection* fb_collection)
      : m_buffer(buffer), m_fb_collection(fb_collection) {}
  
  size_t GetEntityCount() const override {
    if (!m_fb_collection || !m_fb_collection->entities())
      return 0;
    return m_fb_collection->entities()->size();
  }
  
  size_t GetPoolSize() const override {
    return m_fb_collection ? m_fb_collection->entity_memory_pool_size() : 0;
  }
  
  std::unique_ptr<IEntityView> GetEntity(size_t index) const override {
    if (!m_fb_collection || !m_fb_collection->entities())
      return nullptr;
    
    if (index >= m_fb_collection->entities()->size())
      return nullptr;
    
    const auto* fb_entity = m_fb_collection->entities()->Get(index);
    
    // Create view wrapping this entity (still zero-copy)
    return std::make_unique<FlatbuffersEntityView>(m_buffer, fb_entity);
  }
  
  void ForEachEntity(
      std::function<void(const IEntityView&)> callback) const override {
    for (size_t i = 0; i < GetEntityCount(); ++i) {
      auto entity_view = GetEntity(i);
      if (entity_view) {
        callback(*entity_view);
      }
    }
  }
};

} // namespace steamrot
```

**Key Point**: View wraps FlatBuffers pointer, shares buffer ownership. All navigation is zero-copy.

---

### Step 7: Implement Entity View

**File**: `src/entity/FlatbuffersEntityView.h`

```cpp
namespace steamrot {

class FlatbuffersEntityView : public IEntityView {
private:
  std::shared_ptr<std::vector<uint8_t>> m_buffer;
  const EntityData* m_fb_entity;
  
public:
  FlatbuffersEntityView(
      std::shared_ptr<std::vector<uint8_t>> buffer,
      const EntityData* fb_entity)
      : m_buffer(buffer), m_fb_entity(fb_entity) {}
  
  EntityMetadata GetMetadata() const override {
    EntityMetadata metadata;
    metadata.index = m_fb_entity ? m_fb_entity->index() : 0;
    metadata.active = true;
    return metadata;  // Small struct, copied
  }
  
  bool HasComponent(size_t component_register_index) const override {
    if (!m_fb_entity)
      return false;
    
    // Check if component exists
    if (component_register_index == TupleTypeIndex<CUserInterface, ComponentRegister>) {
      return m_fb_entity->c_user_interface() != nullptr;
    }
    // ... other components
    return false;
  }
  
  std::unique_ptr<IComponentView> GetComponent(
      size_t component_register_index) const override {
    if (!HasComponent(component_register_index))
      return nullptr;
    
    if (component_register_index == TupleTypeIndex<CUserInterface, ComponentRegister>) {
      const auto* ui_data = m_fb_entity->c_user_interface();
      // Wrap component in view (zero-copy)
      return std::make_unique<FlatbuffersUserInterfaceView>(m_buffer, ui_data);
    }
    // ... other components
    return nullptr;
  }
};

} // namespace steamrot
```

---

### Step 8: Use in Configurator (Format-Independent!)

**File**: `src/entity/ViewBasedEntityConfigurator.h`

```cpp
namespace steamrot {

class ViewBasedEntityConfigurator : public EntityConfigurator {
private:
  IEntityDataProvider& m_entity_provider;  // NO FlatBuffers dependency!
  
  // Configuration methods use view interfaces
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(const IUserInterfaceView& ui_view,
                     CUserInterface& ui_component);
  
public:
  ViewBasedEntityConfigurator(EventHandler& event_handler,
                              IEntityDataProvider& entity_provider);
  
  std::expected<std::monostate, FailInfo>
  ConfigureEntitiesFromDefaultData(EntityMemoryPool& entity_memory_pool,
                                   const SceneType scene_type) override;
};

} // namespace steamrot
```

**File**: `src/entity/ViewBasedEntityConfigurator.cpp`

```cpp
std::expected<std::monostate, FailInfo>
ViewBasedEntityConfigurator::ConfigureEntitiesFromDefaultData(
    EntityMemoryPool& entity_memory_pool,
    const SceneType scene_type) {
  
  // Load via provider (format-independent!)
  auto collection_result = m_entity_provider.LoadEntityCollection(scene_type);
  if (!collection_result.has_value())
    return std::unexpected(collection_result.error());
  
  auto& collection_view = *collection_result.value();
  
  // Configure from view
  collection_view.ForEachEntity([&](const IEntityView& entity_view) {
    // Extract metadata (small struct, copied)
    EntityMetadata meta = entity_view.GetMetadata();
    
    // Check for UI component
    if (entity_view.HasComponent<CUserInterface>()) {
      // Get component view (zero-copy navigation)
      auto ui_view = entity_view.GetComponent<CUserInterface>();
      
      // Get game component from pool
      auto& ui_component = emp_helpers::GetComponent<CUserInterface>(
          meta.index, entity_memory_pool);
      
      // Configure using view interface (NO FlatBuffers types!)
      ConfigureComponent(*ui_view, ui_component);
    }
  });
  
  return std::monostate{};
}

std::expected<std::monostate, FailInfo>
ViewBasedEntityConfigurator::ConfigureComponent(
    const IUserInterfaceView& ui_view,
    CUserInterface& ui_component) {
  
  // Extract simple data (small struct, copied)
  UserInterfaceSimpleData data = ui_view.GetSimpleData();
  ui_component.m_name = data.ui_name;
  ui_component.m_start_visible = data.start_visible;
  
  // Navigate to nested element (zero-copy)
  if (ui_view.HasRootElement()) {
    auto root_element_view = ui_view.GetRootElement();
    
    // Create UIElement from view
    auto element_result = CreateUIElementFromView(*root_element_view);
    if (!element_result.has_value())
      return std::unexpected(element_result.error());
    
    ui_component.m_root_element = std::move(element_result.value());
  }
  
  return std::monostate{};
}
```

**Key Point**: Configurator uses **view interfaces only**. No FlatBuffers types anywhere!

---

### Step 9: Use in Scene Factory (Game Code)

**File**: `src/scenes/SceneFactory.cpp` (BEFORE - Current)

```cpp
Scene SceneFactory::CreateTitleScene() {
  EventHandler& event_handler = m_game_context.event_handler;
  
  // Direct FlatBuffers coupling ❌
  FlatbuffersConfigurator configurator(event_handler);
  
  EntityMemoryPool entity_pool(50);
  configurator.ConfigureEntitiesFromDefaultData(entity_pool, SceneType::TITLE);
  
  return Scene(std::move(entity_pool), /* ... */);
}
```

**File**: `src/scenes/SceneFactory.cpp` (AFTER - With Views)

```cpp
Scene SceneFactory::CreateTitleScene() {
  EventHandler& event_handler = m_game_context.event_handler;
  
  // Get provider (format-independent) ✅
  IEntityDataProvider& entity_provider = GetEntityDataProvider();
  
  // Create configurator with provider
  ViewBasedEntityConfigurator configurator(event_handler, entity_provider);
  
  EntityMemoryPool entity_pool(50);
  configurator.ConfigureEntitiesFromDefaultData(entity_pool, SceneType::TITLE);
  
  return Scene(std::move(entity_pool), /* ... */);
}
```

**Key Point**: Game code uses provider interface. Can swap formats without changing scene factory!

---

## Complete Integration Flow

Let's trace a complete example end-to-end:

### Startup Flow

```
1. SceneFactory::CreateTitleScene()
   ↓
2. Get IEntityDataProvider& provider = GetEntityDataProvider();
   → Returns FlatbuffersEntityDataProvider (or JsonProvider, or MockProvider)
   ↓
3. ViewBasedEntityConfigurator configurator(event_handler, provider);
   ↓
4. configurator.ConfigureEntitiesFromDefaultData(pool, SceneType::TITLE);
   ↓
5. provider.LoadEntityCollection(SceneType::TITLE)
   → Loads FlatBuffers file
   → Returns IEntityCollectionView (wraps FlatBuffers data)
   ↓
6. collection_view.ForEachEntity([&](const IEntityView& entity_view) {
     ↓
   7. entity_view.HasComponent<CUserInterface>()
      → Checks FlatBuffers data (no copy)
      ↓
   8. entity_view.GetComponent<CUserInterface>()
      → Returns IUserInterfaceView (wraps FlatBuffers data, no copy)
      ↓
   9. ui_view.GetUIName()
      → Reads string from FlatBuffers (returns copy of string)
      ↓
   10. ui_view.GetRootElement()
       → Returns IUIElementView (wraps FlatBuffers element, no copy)
       ↓
   11. CreateUIElementFromView(element_view)
       → Navigates UIElement tree
       → Creates game UIElement objects
       → This is where copying happens (game objects created)
});
```

### Key Points in Flow

- **Steps 1-5**: Setup and provider interface
- **Steps 6-10**: Zero-copy navigation via views
- **Step 11**: Game objects created from views (copying happens here)

**Memory Usage**:
- FlatBuffers buffer: 1 MB (loaded once)
- Views: ~100 bytes (just pointers)
- Game objects: 500 KB (what you actually need)
- **Total**: ~1.5 MB

**Without Views** (copying to intermediate structs):
- FlatBuffers buffer: 1 MB
- Intermediate structs: 1 MB (wasteful copy!)
- Game objects: 500 KB
- **Total**: ~2.5 MB

---

## Comparison Table: Simple vs View Pattern

| Aspect | Simple Provider (Engine) | View Provider (Entity) |
|--------|-------------------------|------------------------|
| **Data Type** | `EngineCoreData` struct | `IEntityCollectionView` interface |
| **Return** | Copy of struct | View to data |
| **Memory** | Small copy acceptable | Large, avoid copying |
| **Navigation** | Not needed (flat) | Essential (nested) |
| **Example** | Window width, title | Entity → Component → UIElement → Children |
| **Use Case** | Simple configuration | Complex hierarchies |

---

## Mock Provider Example (Testing)

Here's how you'd create a mock provider for testing:

**File**: `tests/entity/MockEntityDataProvider.h`

```cpp
class MockEntityDataProvider : public IEntityDataProvider {
private:
  std::vector<MockEntityData> m_entities;
  
public:
  // Programmatic test data creation (no files!)
  void AddEntity(MockEntityData entity) {
    m_entities.push_back(std::move(entity));
  }
  
  std::expected<std::unique_ptr<IEntityCollectionView>, FailInfo>
  LoadEntityCollection(SceneType scene_type) const override {
    // Return mock view wrapping in-memory data
    return std::make_unique<MockEntityCollectionView>(m_entities);
  }
};
```

**Usage in Test**:

```cpp
TEST_CASE("Configure entity with mock provider", "[unit][entity]") {
  EventHandler event_handler;
  
  // Create mock provider (no files!)
  MockEntityDataProvider mock_provider;
  
  // Add test entity programmatically
  mock_provider.AddEntity(
      MockEntityData::Builder()
          .SetIndex(0)
          .AddComponent(MockUIData::Builder()
              .SetName("test_ui")
              .SetVisible(true)
              .Build())
          .Build());
  
  // Use same configurator with mock provider!
  ViewBasedEntityConfigurator configurator(event_handler, mock_provider);
  
  EntityMemoryPool pool(10);
  auto collection = mock_provider.LoadEntityCollection(SceneType::TEST).value();
  configurator.ConfigureEntitiesFromCollection(pool, *collection);
  
  // Verify
  auto& ui = emp_helpers::GetComponent<CUserInterface>(0, pool);
  REQUIRE(ui.m_name == "test_ui");
}
```

---

## Summary: Integration Checklist

To integrate the view pattern:

- [ ] **Step 1**: Define view interfaces (`IEntityCollectionView`, `IEntityView`, `IComponentView`)
- [ ] **Step 2**: Define provider interface (`IEntityDataProvider`)
- [ ] **Step 3**: Implement FlatBuffers view classes (`FlatbuffersEntityCollectionView`, etc.)
- [ ] **Step 4**: Implement FlatBuffers provider (`FlatbuffersEntityDataProvider`)
- [ ] **Step 5**: Create view-based configurator (`ViewBasedEntityConfigurator`)
- [ ] **Step 6**: Update scene factory to use provider
- [ ] **Step 7**: Create mock provider for testing
- [ ] **Step 8**: Migrate tests to use mock provider

---

## Key Takeaways

1. **Simple data** (like `EngineCoreData`) → Use native structs (copy is fine)
2. **Complex nested data** (like entities) → Use views (avoid copying)
3. **Views navigate**, native structs **extract**
4. **Provider interface** hides format (FlatBuffers/JSON/Mock)
5. **Configurator** uses views only (format-independent)
6. **Game code** uses provider interface (no format knowledge)

---

**The pattern scales**: Once you understand it for entities, you can apply it anywhere you have complex nested data!

---

**End of Integration Example**
