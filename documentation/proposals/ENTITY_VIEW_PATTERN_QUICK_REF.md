# Entity View Pattern - Quick Reference

**Date**: December 5, 2025  
**Type**: Quick Reference Guide  
**Related**: [Full Implementation Plan](ENTITY_VIEW_PATTERN_IMPLEMENTATION.md)

---

## Core Concept

```
Navigate DOWN with views → Extract UP with structs
```

**View** = Zero-copy accessor to data  
**Struct** = Native C++ data (copied when needed)

---

## Pattern Overview

```
Data Source (FlatBuffers/JSON)
        ↓
  Provider wraps → Returns View Interface
        ↓
  Game Code → Uses View (format-independent)
        ↓
  Extract → Native structs or primitives
```

---

## Key Interfaces

### IEntityCollectionView
```cpp
class IEntityCollectionView {
  virtual size_t GetEntityCount() const = 0;
  virtual std::unique_ptr<IEntityView> GetEntity(size_t index) const = 0;
  virtual void ForEachEntity(callback) const = 0;
};
```

### IEntityView
```cpp
class IEntityView {
  virtual EntityMetadata GetMetadata() const = 0;  // Native struct
  virtual bool HasComponent<T>() const = 0;
  virtual std::unique_ptr<IComponentView> GetComponent<T>() const = 0;
};
```

### IUserInterfaceView
```cpp
class IUserInterfaceView : public IComponentView {
  virtual UserInterfaceSimpleData GetSimpleData() const = 0;  // Native struct
  virtual std::string GetUIName() const = 0;  // Primitive
  virtual bool HasRootElement() const = 0;
  virtual std::unique_ptr<IUIElementView> GetRootElement() const = 0;  // View
};
```

### IUIElementView
```cpp
class IUIElementView {
  virtual UIElementType GetElementType() const = 0;
  virtual UIElementBaseData GetBaseData() const = 0;  // Native struct
  virtual size_t GetChildCount() const = 0;
  virtual std::unique_ptr<IUIElementView> GetChild(size_t) const = 0;
  virtual void ForEachChild(callback) const = 0;
};
```

---

## Usage Patterns

### Pattern 1: Load and Iterate

```cpp
// Get provider (format-independent)
IEntityDataProvider& provider = GetEntityProvider();

// Load collection (returns view)
auto collection_view = provider.LoadEntityCollection(scene_type).value();

// Iterate entities
collection_view->ForEachEntity([](const IEntityView& entity) {
  // Extract metadata
  EntityMetadata meta = entity.GetMetadata();
  
  // Check for component
  if (entity.HasComponent<CUserInterface>()) {
    // Get component view
    auto ui_view = entity.GetComponent<CUserInterface>();
    
    // Extract data
    std::string name = ui_view->GetUIName();
  }
});
```

### Pattern 2: Navigate Nested Data

```cpp
// Start with entity view
auto entity_view = collection_view->GetEntity(0);

// Navigate to component
auto ui_view = entity_view->GetComponent<CUserInterface>();

// Navigate to element
auto root_element = ui_view->GetRootElement();

// Navigate to children
root_element->ForEachChild([](const IUIElementView& child) {
  UIElementBaseData data = child.GetBaseData();
  // Process child...
});
```

### Pattern 3: Extract and Configure

```cpp
// Get view
auto ui_view = entity.GetComponent<CUserInterface>();

// Extract simple data as struct
UserInterfaceSimpleData data = ui_view->GetSimpleData();

// Configure game object
ui_component.m_name = data.ui_name;
ui_component.m_start_visible = data.start_visible;

// Create nested objects from view
if (ui_view->HasRootElement()) {
  ui_component.m_root_element = CreateUIElementFromView(
      ui_view->GetRootElement());
}
```

### Pattern 4: Polymorphic Element Handling

```cpp
auto element_view = ui_view->GetRootElement();

// Check type
switch (element_view->GetElementType()) {
  case UIElementType::Panel: {
    // Downcast to specific view
    auto panel_view = dynamic_cast<const IPanelView*>(element_view.get());
    PanelSpecificData panel_data = panel_view->GetPanelData();
    // Create panel...
    break;
  }
  case UIElementType::Button: {
    auto button_view = dynamic_cast<const IButtonView*>(element_view.get());
    ButtonSpecificData button_data = button_view->GetButtonData();
    // Create button...
    break;
  }
  // ... other types
}
```

---

## Native Structs (POD Types)

### EntityMetadata
```cpp
struct EntityMetadata {
  uint32_t index;
  bool active;
};
```

### UserInterfaceSimpleData
```cpp
struct UserInterfaceSimpleData {
  std::string ui_name;
  bool start_visible;
};
```

### UIElementBaseData
```cpp
struct UIElementBaseData {
  sf::Vector2f position;
  sf::Vector2f size;
  bool children_active;
  Layout layout;
  SpacingStrategy spacing_strategy;
};
```

### PanelSpecificData
```cpp
struct PanelSpecificData {
  sf::Color background_color;
  float border_thickness;
  sf::Color border_color;
};
```

---

## Provider Pattern

### IEntityDataProvider Interface
```cpp
class IEntityDataProvider {
  virtual std::expected<std::unique_ptr<IEntityCollectionView>, FailInfo>
  LoadEntityCollection(SceneType scene_type) const = 0;
};
```

### FlatBuffers Implementation
```cpp
class FlatbuffersEntityDataProvider : public IEntityDataProvider {
private:
  FlatbuffersDataLoader m_loader;
  
public:
  std::expected<std::unique_ptr<IEntityCollectionView>, FailInfo>
  LoadEntityCollection(SceneType scene_type) const override {
    // Load FlatBuffers data
    auto fb_result = m_loader.ProvideDefaultSceneData(scene_type);
    const auto* fb_data = fb_result.value();
    
    // Create shared buffer ownership
    auto buffer = std::make_shared<std::vector<uint8_t>>(/* loaded data */);
    
    // Wrap in view
    return std::make_unique<FlatbuffersEntityCollectionView>(
        buffer, fb_data->entity_collection());
  }
};
```

### Mock Implementation (Testing)
```cpp
class MockEntityDataProvider : public IEntityDataProvider {
private:
  std::vector<MockEntityData> m_entities;
  
public:
  void AddEntity(MockEntityData entity) {
    m_entities.push_back(std::move(entity));
  }
  
  std::expected<std::unique_ptr<IEntityCollectionView>, FailInfo>
  LoadEntityCollection(SceneType scene_type) const override {
    return std::make_unique<MockEntityCollectionView>(m_entities);
  }
};
```

---

## Configurator Usage

### ViewBasedEntityConfigurator

```cpp
class ViewBasedEntityConfigurator : public EntityConfigurator {
private:
  IEntityDataProvider& m_entity_provider;  // NO FlatBuffers!
  
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(const IUserInterfaceView& ui_view,
                     CUserInterface& ui_component) {
    // Extract data from view
    ui_component.m_name = ui_view.GetUIName();
    ui_component.m_start_visible = ui_view.GetStartVisible();
    
    // Create elements from view
    if (ui_view.HasRootElement()) {
      auto root_result = CreateUIElementFromView(ui_view.GetRootElement());
      if (!root_result.has_value())
        return std::unexpected(root_result.error());
      ui_component.m_root_element = std::move(root_result.value());
    }
    
    return std::monostate{};
  }
  
public:
  std::expected<std::monostate, FailInfo>
  ConfigureEntitiesFromDefaultData(EntityMemoryPool& pool,
                                   SceneType scene_type) override {
    // Load via provider
    auto collection_result = m_entity_provider.LoadEntityCollection(scene_type);
    if (!collection_result.has_value())
      return std::unexpected(collection_result.error());
    
    auto& collection_view = *collection_result.value();
    
    // Configure from view
    return ConfigureEntitiesFromCollection(pool, collection_view);
  }
};
```

---

## Testing Examples

### Unit Test with Mock Provider

```cpp
TEST_CASE("Configure entity from mock view", "[unit][entity-view]") {
  // Create mock provider
  MockEntityDataProvider mock_provider;
  
  // Add test entity
  mock_provider.AddEntity(
      MockEntityData::Builder()
          .SetIndex(0)
          .AddComponent(MockUIData::Builder()
              .SetName("test_ui")
              .SetVisible(true)
              .Build())
          .Build());
  
  // Load view (NO files!)
  auto collection_view = mock_provider.LoadEntityCollection(SceneType::TEST).value();
  
  // Verify
  REQUIRE(collection_view->GetEntityCount() == 1);
  
  auto entity_view = collection_view->GetEntity(0);
  REQUIRE(entity_view->HasComponent<CUserInterface>() == true);
  
  auto ui_view = entity_view->GetComponent<CUserInterface>();
  REQUIRE(ui_view->GetUIName() == "test_ui");
}
```

### Integration Test with FlatBuffers

```cpp
TEST_CASE("Configure from FlatBuffers view", "[integration][entity-view]") {
  // Use real FlatBuffers provider
  FlatbuffersEntityDataProvider fb_provider;
  
  // Load via view interface
  auto collection_view = fb_provider.LoadEntityCollection(SceneType::TITLE).value();
  
  // Configure entity pool
  TestFixture fixture;
  ViewBasedEntityConfigurator configurator(fixture.GetEventHandler(), fb_provider);
  auto result = configurator.ConfigureEntitiesFromDefaultData(
      fixture.GetEntityMemoryPool(), SceneType::TITLE);
  
  REQUIRE(result.has_value());
  
  // Verify configuration
  // ...
}
```

---

## Memory Management

### View Lifetime

```cpp
// Provider owns the data
FlatbuffersEntityDataProvider provider;

// View references data (shared ownership)
auto collection_view = provider.LoadEntityCollection(scene_type).value();
//     ↑
//     └─ Shared pointer to buffer

// While view exists, data stays in memory
collection_view->ForEachEntity([](const IEntityView& entity) {
  // Safe to use - view keeps data alive
});

// View destroyed → buffer released (if no other references)
```

### Zero-Copy Principle

```cpp
// ❌ BAD: Copying entire hierarchy
EntityData data = LoadAllData();  // Deep copy!

// ✅ GOOD: Zero-copy view
auto view = provider.LoadView();  // Just pointers
view->ForEachEntity([](const IEntityView& entity) {
  // Direct access, no copy
});
```

---

## Implementation Checklist

### Phase 1: Foundation
- [ ] Define `IEntityCollectionView`
- [ ] Define `IEntityView`
- [ ] Define `IComponentView`
- [ ] Define `IEntityDataProvider`
- [ ] Implement `FlatbuffersEntityDataProvider`
- [ ] Implement `FlatbuffersEntityCollectionView`
- [ ] Implement `FlatbuffersEntityView`
- [ ] Write basic unit tests

### Phase 2: UIElement Views
- [ ] Define `IUIElementView` (base)
- [ ] Define derived element views (Panel, Button, etc.)
- [ ] Implement `FlatbuffersUIElementView` (factory)
- [ ] Implement derived FlatBuffers element views
- [ ] Add iteration support (`ForEachChild`)
- [ ] Write nested structure tests

### Phase 3: Remaining Components
- [ ] Define remaining component views
- [ ] Implement FlatBuffers component views
- [ ] Update `FlatbuffersEntityView` for all components
- [ ] Write component-specific tests

### Phase 4: New Configurator
- [ ] Create `ViewBasedEntityConfigurator`
- [ ] Migrate configuration logic
- [ ] Implement UIElement creation from views
- [ ] Write integration tests

### Phase 5: Migration
- [ ] Update usage sites
- [ ] Migrate existing tests
- [ ] Deprecate old configurator
- [ ] Update documentation

### Phase 6: Mock Provider
- [ ] Implement `MockEntityDataProvider`
- [ ] Implement mock view classes
- [ ] Update test infrastructure
- [ ] Write examples

---

## Common Patterns

### Check-Navigate-Extract

```cpp
// 1. Check existence
if (entity.HasComponent<CUserInterface>()) {
  // 2. Navigate to component
  auto ui_view = entity.GetComponent<CUserInterface>();
  
  // 3. Extract data
  std::string name = ui_view->GetUIName();
}
```

### Iterate-Extract-Configure

```cpp
collection_view->ForEachEntity([&pool](const IEntityView& entity_view) {
  EntityMetadata meta = entity_view.GetMetadata();
  
  // Get game object
  auto& component = emp_helpers::GetComponent<CUserInterface>(
      meta.index, pool);
  
  // Configure from view
  if (entity_view.HasComponent<CUserInterface>()) {
    auto ui_view = entity_view.GetComponent<CUserInterface>();
    component.m_name = ui_view->GetUIName();
  }
});
```

### Recursive Navigation

```cpp
void ProcessElement(const IUIElementView& element_view) {
  // Extract data
  UIElementBaseData data = element_view.GetBaseData();
  
  // Process this element
  CreateElement(data);
  
  // Recurse to children
  element_view.ForEachChild([](const IUIElementView& child) {
    ProcessElement(child);  // Recursive call
  });
}
```

---

## Benefits Summary

✅ **Format Independence** - Swap FlatBuffers/JSON/XML/Database  
✅ **Zero-Copy** - No wasteful data copying  
✅ **Testable** - Mock providers, no file dependencies  
✅ **Consistent** - Same pattern as other data systems  
✅ **Future-Proof** - Ready for new data sources  

---

## Anti-Patterns (Avoid)

### ❌ Storing Views Long-Term

```cpp
// BAD: Don't store views as members
class MyClass {
  std::unique_ptr<IEntityView> m_entity_view;  // ❌ NO!
};

// GOOD: Extract data, discard view
class MyClass {
  EntityMetadata m_metadata;  // ✅ Native data
};
```

### ❌ Copying from Views Unnecessarily

```cpp
// BAD: Copying nested data
std::vector<EntityData> all_entities;
collection_view->ForEachEntity([&all_entities](const IEntityView& entity) {
  all_entities.push_back(CopyEverything(entity));  // ❌ Wasteful!
});

// GOOD: Configure in-place
collection_view->ForEachEntity([&pool](const IEntityView& entity) {
  ConfigureDirectly(entity, pool);  // ✅ Zero-copy!
});
```

### ❌ Leaking Format Types

```cpp
// BAD: Exposing FlatBuffers in interface
void ConfigureUI(const UserInterfaceData* fb_data);  // ❌ Coupled!

// GOOD: Using view interface
void ConfigureUI(const IUserInterfaceView& ui_view);  // ✅ Abstract!
```

---

## Key Takeaways

1. **Views are navigators**, not storage
2. **Extract simple data** as native types/structs
3. **Navigate nested data** with view interfaces
4. **Zero-copy** for performance
5. **Format-independent** for flexibility
6. **Testable** with mock providers

---

**See Also**:
- [Full Implementation Plan](ENTITY_VIEW_PATTERN_IMPLEMENTATION.md)
- [View Pattern Diagrams](../analysis/VIEW_PATTERN_DIAGRAMS_AND_EXAMPLES.md)
- [Configuration Analysis](../analysis/CONFIGURATION_VS_DATA_STRUCTS_ANALYSIS.md)

---

**End of Quick Reference**
