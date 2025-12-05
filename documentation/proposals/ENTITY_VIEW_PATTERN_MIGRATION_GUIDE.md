# Entity View Pattern - Migration Guide

**Date**: December 5, 2025  
**Type**: Migration Guide  
**Related**: [Implementation Plan](ENTITY_VIEW_PATTERN_IMPLEMENTATION.md), [Quick Reference](ENTITY_VIEW_PATTERN_QUICK_REF.md)

---

## Table of Contents

1. [Overview](#overview)
2. [Before and After Comparison](#before-and-after-comparison)
3. [Step-by-Step Migration](#step-by-step-migration)
4. [Code Examples](#code-examples)
5. [Testing Migration](#testing-migration)
6. [Common Issues](#common-issues)

---

## Overview

### What's Changing

**From**: `FlatbuffersConfigurator` with direct FlatBuffers types  
**To**: `ViewBasedEntityConfigurator` with view interfaces

### Why Migrate

1. **Format Independence** - Not locked to FlatBuffers
2. **Better Testing** - Mock providers, no file dependencies
3. **Architectural Consistency** - Same pattern as other data systems
4. **Future-Proof** - Ready for JSON/XML/database providers

### Migration Timeline

The migration happens in phases with both configurators coexisting:

```
Phase 1-3: View implementation complete
           ↓
Phase 4:   New configurator ready
           ↓
Phase 5:   BEGIN MIGRATION ← You are here
           • Both configurators work
           • Choose which to use
           • Gradual transition
           ↓
Phase 6:   Mock provider available
           • Enhanced testing
           ↓
Future:    Old configurator removed
           • Complete migration
```

---

## Before and After Comparison

### Configuration Code

**BEFORE** (FlatBuffers-coupled):
```cpp
// src/entity/FlatbuffersConfigurator.h
class FlatbuffersConfigurator : public EntityConfigurator {
private:
  FlatbuffersDataLoader m_data_loader;  // Direct FlatBuffers usage
  
  // FlatBuffers types in interface ❌
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(const UserInterfaceData* ui_data,  // FlatBuffers type!
                     CUserInterface& ui_component);
  
public:
  FlatbuffersConfigurator(EventHandler& event_handler);
  
  std::expected<std::monostate, FailInfo>
  ConfigureEntitiesFromDefaultData(EntityMemoryPool& pool,
                                   const SceneType scene_type);
};
```

**AFTER** (View-based):
```cpp
// src/entity/ViewBasedEntityConfigurator.h
class ViewBasedEntityConfigurator : public EntityConfigurator {
private:
  IEntityDataProvider& m_entity_provider;  // Provider abstraction ✅
  
  // View interfaces only ✅
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(const IUserInterfaceView& ui_view,  // View interface!
                     CUserInterface& ui_component);
  
public:
  ViewBasedEntityConfigurator(EventHandler& event_handler,
                              IEntityDataProvider& entity_provider);
  
  std::expected<std::monostate, FailInfo>
  ConfigureEntitiesFromDefaultData(EntityMemoryPool& pool,
                                   const SceneType scene_type) override;
};
```

### Usage Code

**BEFORE**:
```cpp
// Game code directly creates FlatbuffersConfigurator
void SceneFactory::CreateScene(SceneType scene_type) {
  EventHandler& event_handler = GetEventHandler();
  
  // Directly coupled to FlatBuffers
  FlatbuffersConfigurator configurator(event_handler);
  
  EntityMemoryPool pool(100);
  configurator.ConfigureEntitiesFromDefaultData(pool, scene_type);
}
```

**AFTER**:
```cpp
// Game code uses provider abstraction
void SceneFactory::CreateScene(SceneType scene_type) {
  EventHandler& event_handler = GetEventHandler();
  
  // Provider injected (format-independent)
  IEntityDataProvider& provider = GetEntityDataProvider();
  ViewBasedEntityConfigurator configurator(event_handler, provider);
  
  EntityMemoryPool pool(100);
  configurator.ConfigureEntitiesFromDefaultData(pool, scene_type);
}
```

### Test Code

**BEFORE** (Requires FlatBuffers files):
```cpp
TEST_CASE("Configure entities", "[unit][entity]") {
  EventHandler event_handler;
  
  // Must have valid .bin file! ❌
  FlatbuffersConfigurator configurator(event_handler);
  
  EntityMemoryPool pool(10);
  auto result = configurator.ConfigureEntitiesFromDefaultData(
      pool, SceneType::TEST);
  
  REQUIRE(result.has_value());
  // Limited control over test data
}
```

**AFTER** (Mock provider, no files):
```cpp
TEST_CASE("Configure entities", "[unit][entity]") {
  EventHandler event_handler;
  
  // Create mock provider (no files!) ✅
  MockEntityDataProvider mock_provider;
  
  // Add custom test entity
  mock_provider.AddEntity(
      MockEntityData::Builder()
          .SetIndex(0)
          .AddComponent(MockUIData::Builder()
              .SetName("test_ui")
              .SetVisible(true)
              .Build())
          .Build());
  
  ViewBasedEntityConfigurator configurator(event_handler, mock_provider);
  
  EntityMemoryPool pool(10);
  auto collection_view = mock_provider.LoadEntityCollection(SceneType::TEST).value();
  auto result = configurator.ConfigureEntitiesFromCollection(pool, *collection_view);
  
  REQUIRE(result.has_value());
  // Full control over test data
}
```

---

## Step-by-Step Migration

### Step 1: Update Dependencies

**Add new includes**:
```cpp
// Remove or reduce FlatBuffers dependencies
// #include "user_interface_generated.h"  // Can remove from many files

// Add view interface includes
#include "IEntityDataProvider.h"
#include "IEntityCollectionView.h"
#include "IEntityView.h"
#include "IUserInterfaceView.h"
```

### Step 2: Update Class Members

**BEFORE**:
```cpp
class MyClass {
private:
  EventHandler& m_event_handler;
  
public:
  void ConfigureEntities(EntityMemoryPool& pool, SceneType scene_type) {
    FlatbuffersConfigurator configurator(m_event_handler);
    configurator.ConfigureEntitiesFromDefaultData(pool, scene_type);
  }
};
```

**AFTER**:
```cpp
class MyClass {
private:
  EventHandler& m_event_handler;
  IEntityDataProvider& m_entity_provider;  // Add provider dependency
  
public:
  MyClass(EventHandler& event_handler, 
          IEntityDataProvider& entity_provider)
      : m_event_handler(event_handler),
        m_entity_provider(entity_provider) {}
  
  void ConfigureEntities(EntityMemoryPool& pool, SceneType scene_type) {
    ViewBasedEntityConfigurator configurator(m_event_handler, m_entity_provider);
    configurator.ConfigureEntitiesFromDefaultData(pool, scene_type);
  }
};
```

### Step 3: Update Initialization

**BEFORE**:
```cpp
// In main.cpp or Engine.cpp
void Initialize() {
  EventHandler event_handler;
  SceneFactory scene_factory(event_handler);
  // SceneFactory creates FlatbuffersConfigurator internally
}
```

**AFTER**:
```cpp
// In main.cpp or Engine.cpp
void Initialize() {
  EventHandler event_handler;
  
  // Create provider
  FlatbuffersEntityDataProvider entity_provider;
  
  // Inject provider
  SceneFactory scene_factory(event_handler, entity_provider);
  // SceneFactory now uses ViewBasedEntityConfigurator
}
```

### Step 4: Update Method Signatures

If your code has methods that work with entity configuration:

**BEFORE**:
```cpp
// Method using FlatBuffers types
void ProcessEntityData(const EntityData* fb_entity) {  // FlatBuffers type
  if (fb_entity->c_user_interface()) {
    const auto* ui_data = fb_entity->c_user_interface();
    // Process...
  }
}
```

**AFTER**:
```cpp
// Method using view interface
void ProcessEntityData(const IEntityView& entity_view) {  // View interface
  if (entity_view.HasComponent<CUserInterface>()) {
    auto ui_view = entity_view.GetComponent<CUserInterface>();
    // Process...
  }
}
```

### Step 5: Update Component Access

**BEFORE**:
```cpp
void ConfigureUI(const UserInterfaceData* ui_data,  // FlatBuffers
                 CUserInterface& ui_component) {
  // Null checking FlatBuffers data
  if (ui_data->ui_name())
    ui_component.m_name = ui_data->ui_name()->str();
  
  ui_component.m_start_visible = ui_data->start_visible();
  
  if (ui_data->root_ui_element()) {
    // Create element from FlatBuffers
    auto element = CreateElementFromFB(ui_data->root_ui_element());
    ui_component.m_root_element = std::move(element);
  }
}
```

**AFTER**:
```cpp
void ConfigureUI(const IUserInterfaceView& ui_view,  // View interface
                 CUserInterface& ui_component) {
  // Direct access (view handles null checking internally)
  ui_component.m_name = ui_view.GetUIName();
  ui_component.m_start_visible = ui_view.GetStartVisible();
  
  if (ui_view.HasRootElement()) {
    // Create element from view
    auto element = CreateElementFromView(ui_view.GetRootElement());
    ui_component.m_root_element = std::move(element);
  }
}
```

---

## Code Examples

### Example 1: Scene Creation

**Complete migration of scene entity loading**

**BEFORE**:
```cpp
// src/scenes/SceneFactory.cpp
#include "FlatbuffersConfigurator.h"
#include "user_interface_generated.h"

Scene SceneFactory::CreateTitleScene() {
  EventHandler& event_handler = m_game_context.event_handler;
  
  // Create configurator (coupled to FlatBuffers)
  FlatbuffersConfigurator configurator(event_handler);
  
  // Create entity pool
  EntityMemoryPool entity_pool(50);
  
  // Configure from FlatBuffers
  auto result = configurator.ConfigureEntitiesFromDefaultData(
      entity_pool, SceneType::TITLE);
  
  if (!result.has_value()) {
    // Handle error
    return Scene{};
  }
  
  // Create scene with configured entities
  return Scene(std::move(entity_pool), /* other params */);
}
```

**AFTER**:
```cpp
// src/scenes/SceneFactory.cpp
#include "ViewBasedEntityConfigurator.h"
#include "IEntityDataProvider.h"

Scene SceneFactory::CreateTitleScene() {
  EventHandler& event_handler = m_game_context.event_handler;
  IEntityDataProvider& entity_provider = m_game_context.entity_provider;
  
  // Create configurator (format-independent)
  ViewBasedEntityConfigurator configurator(event_handler, entity_provider);
  
  // Create entity pool
  EntityMemoryPool entity_pool(50);
  
  // Configure via view interface
  auto result = configurator.ConfigureEntitiesFromDefaultData(
      entity_pool, SceneType::TITLE);
  
  if (!result.has_value()) {
    // Handle error
    return Scene{};
  }
  
  // Create scene with configured entities
  return Scene(std::move(entity_pool), /* other params */);
}
```

**Migration Notes**:
1. Change include from `FlatbuffersConfigurator.h` to `ViewBasedEntityConfigurator.h`
2. Remove FlatBuffers includes (e.g., `user_interface_generated.h`)
3. Add provider dependency to `SceneFactory` constructor
4. Replace `FlatbuffersConfigurator` with `ViewBasedEntityConfigurator`
5. Inject provider instead of creating configurator with just event handler

---

### Example 2: Test with Mock Data

**Complete test migration**

**BEFORE**:
```cpp
// tests/entity/FlatbuffersConfigurator.test.cpp
TEST_CASE("FlatbuffersConfigurator loads UI entities", 
          "[unit][configurator]") {
  EventHandler event_handler;
  FlatbuffersConfigurator configurator(event_handler);
  
  EntityMemoryPool pool(10);
  
  // Must use actual FlatBuffers file
  auto result = configurator.ConfigureEntitiesFromDefaultData(
      pool, SceneType::TEST);
  
  REQUIRE(result.has_value());
  
  // Verify entity 0 has UI component
  auto& ui = emp_helpers::GetComponent<CUserInterface>(0, pool);
  REQUIRE(ui.m_active == true);
  // Limited control - depends on test data file
}
```

**AFTER**:
```cpp
// tests/entity/ViewBasedEntityConfigurator.test.cpp
TEST_CASE("ViewBasedEntityConfigurator loads UI entities", 
          "[unit][configurator]") {
  EventHandler event_handler;
  
  // Create mock provider (NO files needed!)
  MockEntityDataProvider mock_provider;
  
  // Build custom test entity programmatically
  mock_provider.AddEntity(
      MockEntityData::Builder()
          .SetIndex(0)
          .SetActive(true)
          .AddComponent(
              MockUIData::Builder()
                  .SetName("test_ui")
                  .SetVisible(true)
                  .SetRootElement(
                      MockPanelData::Builder()
                          .SetPosition(10, 20)
                          .SetSize(100, 50)
                          .Build())
                  .Build())
          .Build());
  
  ViewBasedEntityConfigurator configurator(event_handler, mock_provider);
  
  EntityMemoryPool pool(10);
  auto collection_view = mock_provider.LoadEntityCollection(SceneType::TEST).value();
  auto result = configurator.ConfigureEntitiesFromCollection(pool, *collection_view);
  
  REQUIRE(result.has_value());
  
  // Verify entity 0 has UI component
  auto& ui = emp_helpers::GetComponent<CUserInterface>(0, pool);
  REQUIRE(ui.m_active == true);
  REQUIRE(ui.m_name == "test_ui");
  REQUIRE(ui.m_start_visible == true);
  REQUIRE(ui.m_root_element != nullptr);
  // Full control - programmatic test data
}
```

**Migration Notes**:
1. Create `MockEntityDataProvider` instead of using file-based provider
2. Build test entities programmatically with builder pattern
3. Full control over test data (edge cases, invalid data, etc.)
4. No dependency on external files
5. Faster test execution

---

### Example 3: UIElement Creation

**Migrating UIElement factory methods**

**BEFORE**:
```cpp
// Create UIElement from FlatBuffers data
std::expected<std::unique_ptr<UIElement>, FailInfo>
CreateUIElement(const auto* fb_element,  // FlatBuffers type
                EventHandler& event_handler) {
  if (!fb_element || !fb_element->base_data())
    return std::unexpected(FailInfo{"Null element data"});
  
  const auto* base_data = fb_element->base_data();
  
  // Determine type
  auto element_type = fb_element->element_union_type();
  
  switch (element_type) {
    case UIElementUnion::PanelData: {
      const auto* panel_data = 
          fb_element->element_union_as_PanelData();
      
      // Extract data
      sf::Vector2f position(
          base_data->position()->x(),
          base_data->position()->y());
      
      sf::Vector2f size(
          base_data->size()->x(),
          base_data->size()->y());
      
      // Create panel
      auto panel = std::make_unique<Panel>(
          position, size,
          panel_data->background_color(),
          panel_data->border_thickness());
      
      // Handle children recursively
      if (base_data->children()) {
        for (const auto* child_fb : *base_data->children()) {
          auto child = CreateUIElement(child_fb, event_handler);
          if (child.has_value()) {
            panel->AddChild(std::move(child.value()));
          }
        }
      }
      
      return panel;
    }
    // ... other types
  }
}
```

**AFTER**:
```cpp
// Create UIElement from view interface
std::expected<std::unique_ptr<UIElement>, FailInfo>
CreateUIElementFromView(const IUIElementView& element_view,  // View interface
                        EventHandler& event_handler) {
  // Determine type
  auto element_type = element_view.GetElementType();
  
  switch (element_type) {
    case UIElementType::Panel: {
      // Downcast to specific view
      const auto* panel_view = 
          dynamic_cast<const IPanelView*>(&element_view);
      
      if (!panel_view)
        return std::unexpected(FailInfo{"Invalid panel view"});
      
      // Extract base data (native struct)
      UIElementBaseData base_data = panel_view->GetBaseData();
      
      // Extract panel-specific data (native struct)
      PanelSpecificData panel_data = panel_view->GetPanelData();
      
      // Create panel
      auto panel = std::make_unique<Panel>(
          base_data.position, 
          base_data.size,
          panel_data.background_color,
          panel_data.border_thickness);
      
      // Handle children recursively (via view iteration)
      panel_view->ForEachChild(
          [&panel, &event_handler](const IUIElementView& child_view) {
            auto child = CreateUIElementFromView(child_view, event_handler);
            if (child.has_value()) {
              panel->AddChild(std::move(child.value()));
            }
          });
      
      return panel;
    }
    // ... other types
  }
}
```

**Migration Notes**:
1. Parameter changes from FlatBuffers pointer to view reference
2. No null checking needed (view handles internally)
3. Extract data as native structs instead of accessing FlatBuffers fields
4. Use view iteration (`ForEachChild`) instead of FlatBuffers vector access
5. Type checking via `GetElementType()` enum instead of union types

---

## Testing Migration

### Unit Test Migration Pattern

**Old Pattern** (File-dependent):
```cpp
TEST_CASE("Test feature X", "[unit][feature]") {
  // Setup with FlatBuffers
  FlatbuffersConfigurator configurator(event_handler);
  
  // Must have file
  configurator.ConfigureEntitiesFromDefaultData(pool, SceneType::TEST);
  
  // Test
  // ...
}
```

**New Pattern** (Mock-based):
```cpp
TEST_CASE("Test feature X", "[unit][feature]") {
  // Setup with mock
  MockEntityDataProvider mock_provider;
  mock_provider.AddEntity(/* custom data */);
  
  ViewBasedEntityConfigurator configurator(event_handler, mock_provider);
  auto collection = mock_provider.LoadEntityCollection(SceneType::TEST).value();
  configurator.ConfigureEntitiesFromCollection(pool, *collection);
  
  // Test
  // ...
}
```

### Integration Test Migration Pattern

**Old Pattern**:
```cpp
TEST_CASE("Integration test Y", "[integration][feature]") {
  // Uses real FlatBuffers files
  FlatbuffersConfigurator configurator(event_handler);
  configurator.ConfigureEntitiesFromDefaultData(pool, SceneType::TITLE);
  
  // Test full workflow
  // ...
}
```

**New Pattern** (Can still use FlatBuffers):
```cpp
TEST_CASE("Integration test Y", "[integration][feature]") {
  // Uses real FlatBuffers via provider
  FlatbuffersEntityDataProvider fb_provider;
  ViewBasedEntityConfigurator configurator(event_handler, fb_provider);
  configurator.ConfigureEntitiesFromDefaultData(pool, SceneType::TITLE);
  
  // Test full workflow
  // ...
}
```

**Note**: Integration tests can still use real FlatBuffers data through the provider interface.

---

## Common Issues

### Issue 1: Missing Provider Dependency

**Symptom**:
```
error: no matching constructor for ViewBasedEntityConfigurator
```

**Cause**: Not injecting `IEntityDataProvider`

**Solution**:
```cpp
// Add provider to constructor
MyClass(EventHandler& event_handler,
        IEntityDataProvider& entity_provider)  // Add this
    : m_event_handler(event_handler),
      m_entity_provider(entity_provider) {}

// Pass provider when creating configurator
ViewBasedEntityConfigurator configurator(
    m_event_handler, 
    m_entity_provider);  // Don't forget provider!
```

---

### Issue 2: View Lifetime Management

**Symptom**: Crash when accessing view data

**Cause**: View destroyed while still in use

**Wrong**:
```cpp
// DON'T store views long-term
std::unique_ptr<IEntityView> m_entity_view;  // ❌

void LoadData() {
  auto collection = provider.LoadEntityCollection(scene_type).value();
  m_entity_view = collection->GetEntity(0);  // Danger!
  // collection destroyed here, view becomes invalid!
}
```

**Correct**:
```cpp
// DO extract data immediately
EntityMetadata m_entity_metadata;  // ✅ Native data

void LoadData() {
  auto collection = provider.LoadEntityCollection(scene_type).value();
  auto entity_view = collection->GetEntity(0);
  m_entity_metadata = entity_view->GetMetadata();  // Extract and store
  // View can be destroyed safely now
}
```

---

### Issue 3: Forgetting to Check HasComponent

**Symptom**: Null pointer dereference

**Wrong**:
```cpp
auto ui_view = entity_view.GetComponent<CUserInterface>();
std::string name = ui_view->GetUIName();  // Crash if null!
```

**Correct**:
```cpp
if (entity_view.HasComponent<CUserInterface>()) {  // Check first
  auto ui_view = entity_view.GetComponent<CUserInterface>();
  std::string name = ui_view->GetUIName();  // Safe
}
```

---

### Issue 4: Incorrect View Type Casting

**Symptom**: Dynamic cast returns null

**Wrong**:
```cpp
auto element_view = ui_view->GetRootElement();
// Assume it's a panel
auto panel_view = static_cast<const IPanelView*>(element_view.get());  // ❌ Unsafe!
```

**Correct**:
```cpp
auto element_view = ui_view->GetRootElement();

// Check type first
if (element_view->GetElementType() == UIElementType::Panel) {
  auto panel_view = dynamic_cast<const IPanelView*>(element_view.get());
  if (panel_view) {  // Verify cast succeeded
    PanelSpecificData data = panel_view->GetPanelData();
  }
}
```

---

## Migration Checklist

### Per-File Checklist

When migrating a file:

- [ ] Update includes (remove FlatBuffers, add view interfaces)
- [ ] Update class dependencies (add `IEntityDataProvider&`)
- [ ] Replace `FlatbuffersConfigurator` with `ViewBasedEntityConfigurator`
- [ ] Update method signatures (FlatBuffers types → view interfaces)
- [ ] Update data access (FlatBuffers fields → view methods)
- [ ] Update null checking (FlatBuffers checks → view checks)
- [ ] Update tests (file-based → mock-based for unit tests)
- [ ] Verify compilation
- [ ] Run tests
- [ ] Verify behavior unchanged

### Project-Wide Checklist

For complete migration:

- [ ] Identify all uses of `FlatbuffersConfigurator`
- [ ] Migrate scene factories
- [ ] Migrate test fixtures
- [ ] Migrate entity configuration code
- [ ] Update documentation
- [ ] Verify all tests pass
- [ ] Performance testing
- [ ] Code review
- [ ] Mark `FlatbuffersConfigurator` deprecated
- [ ] Schedule removal of old configurator

---

## Rollback Procedure

If issues arise during migration:

### Step 1: Identify the Problem
- Which file/class has the issue?
- What's the error/behavior?
- Can it be fixed quickly?

### Step 2: Decide to Rollback or Fix
- **If quick fix**: Fix and continue
- **If complex issue**: Rollback to old configurator

### Step 3: Rollback Process
```cpp
// Change this:
ViewBasedEntityConfigurator configurator(event_handler, entity_provider);

// Back to this:
FlatbuffersConfigurator configurator(event_handler);

// Keep changes minimal - just revert configurator instantiation
```

### Step 4: Report Issue
- Document what went wrong
- Create issue for future fix
- Continue migration after fix

---

## Support and Help

### Where to Get Help

1. **Documentation**:
   - [Implementation Plan](ENTITY_VIEW_PATTERN_IMPLEMENTATION.md)
   - [Quick Reference](ENTITY_VIEW_PATTERN_QUICK_REF.md)
   - [View Pattern Diagrams](../analysis/VIEW_PATTERN_DIAGRAMS_AND_EXAMPLES.md)

2. **Code Examples**:
   - Look at migrated tests in `tests/entity/`
   - Check `ViewBasedEntityConfigurator` implementation
   - Review mock provider usage

3. **Common Patterns**:
   - See [Quick Reference](ENTITY_VIEW_PATTERN_QUICK_REF.md) for patterns

### Questions to Ask

- "How do I access nested data through views?"
- "How do I create test data without files?"
- "How do I handle polymorphic UIElements?"
- "What if I need to roll back?"

---

## Summary

### Key Migration Steps

1. **Add provider dependency** to classes using entity configuration
2. **Replace FlatbuffersConfigurator** with ViewBasedEntityConfigurator
3. **Update method signatures** from FlatBuffers types to view interfaces
4. **Extract data via views** instead of direct FlatBuffers access
5. **Use mock providers** in unit tests for better control

### Benefits After Migration

✅ Format independence  
✅ Better testing  
✅ Architectural consistency  
✅ Future-proof design  

### Remember

- Migration is gradual (both configurators coexist)
- Can rollback at any point
- Tests verify equivalence
- Documentation and examples available

---

**End of Migration Guide**
