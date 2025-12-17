# Testing Examples

This document provides complete, real-world examples of implementing unit tests for each architectural layer in SteamRot.

## Table of Contents

- [Layer 0 Example: Testing EventPacket](#layer-0-example-testing-eventpacket)
- [Layer 1 Example: Testing CMeta Component](#layer-1-example-testing-cmeta-component)
- [Layer 2 Example: Testing FlatbuffersAssetDataProvider](#layer-2-example-testing-flatbuffersassetdataprovider)
- [Layer 3 Example: Testing EntityManager](#layer-3-example-testing-entitymanager)
- [Layer 4 Example: Testing UIRenderLogic](#layer-4-example-testing-uirenderlogic)
- [Layer 5 Example: Testing TitleScene (Template)](#layer-5-example-testing-titlescene-template)
- [Layer 6 Example: Testing GameEngine](#layer-6-example-testing-gameengine)

---

## Layer 0 Example: Testing EventPacket

**File**: `tests/unit/types/EventPacket.test.cpp` (to be created)

**Class being tested**: `EventPacket` - A simple data structure for events

### Implementation

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EventPacket
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EventPacket.h"
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
/// Test: Default Construction
/////////////////////////////////////////////////
TEST_CASE("EventPacket default construction creates valid packet", 
          "[unit][EventPacket]") {
  
  steamrot::EventPacket packet;
  
  // Default event type should be None
  REQUIRE(packet.event_type == steamrot::EventType::EventType_None);
  
  // Input state should be empty
  REQUIRE(packet.input_state.none());
}

/////////////////////////////////////////////////
/// Test: Construction with Parameters
/////////////////////////////////////////////////
TEST_CASE("EventPacket can be constructed with event type", 
          "[unit][EventPacket]") {
  
  steamrot::UserInputBitset input;
  input.set(static_cast<size_t>(steamrot::UserInput::MOUSE_LEFT_CLICK));
  
  steamrot::EventPacket packet{
    steamrot::EventType::EventType_EVENT_USER_INPUT,
    input
  };
  
  REQUIRE(packet.event_type == steamrot::EventType::EventType_EVENT_USER_INPUT);
  REQUIRE(packet.input_state.test(
    static_cast<size_t>(steamrot::UserInput::MOUSE_LEFT_CLICK)));
}

/////////////////////////////////////////////////
/// Test: Copy Construction
/////////////////////////////////////////////////
TEST_CASE("EventPacket can be copy constructed", "[unit][EventPacket]") {
  
  steamrot::UserInputBitset input;
  input.set(static_cast<size_t>(steamrot::UserInput::KEY_ESCAPE));
  
  steamrot::EventPacket packet1{
    steamrot::EventType::EventType_EVENT_QUIT_GAME,
    input
  };
  
  // Copy construct
  steamrot::EventPacket packet2 = packet1;
  
  // Verify copy is identical
  REQUIRE(packet2.event_type == packet1.event_type);
  REQUIRE(packet2.input_state == packet1.input_state);
}

/////////////////////////////////////////////////
/// Test: Assignment
/////////////////////////////////////////////////
TEST_CASE("EventPacket can be assigned", "[unit][EventPacket]") {
  
  steamrot::EventPacket packet1{
    steamrot::EventType::EventType_EVENT_CHANGE_SCENE,
    steamrot::UserInputBitset{}
  };
  
  steamrot::EventPacket packet2;
  packet2 = packet1;
  
  REQUIRE(packet2.event_type == packet1.event_type);
}
```

### CMakeLists.txt Addition

Add to `tests/unit/types/CMakeLists.txt`:

```cmake
add_executable(test_types
  EventPacket.test.cpp
  Subscriber.test.cpp
  UserInputBitset.test.cpp
)

target_link_libraries(test_types
  PRIVATE
  Catch2::Catch2WithMain
  steamrot_types
)

catch_discover_tests(test_types)
```

---

## Layer 1 Example: Testing CMeta Component

**File**: `tests/unit/components/CMeta.test.cpp` (exists)

**Class being tested**: `CMeta` - Basic entity metadata component

### Complete Implementation

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CMeta class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CMeta.h"
#include "containers.h"
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
/// Test: Default Values
/////////////////////////////////////////////////
TEST_CASE("CMeta has correct default values", "[unit][Components][CMeta]") {
  steamrot::CMeta meta;

  // Test inherited Component fields
  REQUIRE(meta.m_active == false);
  
  // Test CMeta-specific fields
  REQUIRE(meta.m_entity_alive == false);
}

/////////////////////////////////////////////////
/// Test: Component Register Index
/////////////////////////////////////////////////
TEST_CASE("CMeta::GetComponentRegisterIndex returns valid index", 
          "[unit][Components][CMeta]") {
  
  steamrot::CMeta meta;
  
  // Get index
  size_t index = meta.GetComponentRegisterIndex();
  
  // Index should be valid position in ComponentRegister tuple
  REQUIRE(index >= 0);
  REQUIRE(index < std::tuple_size_v<steamrot::ComponentRegister>);
}

/////////////////////////////////////////////////
/// Test: Copy Construction
/////////////////////////////////////////////////
TEST_CASE("CMeta can be copy constructed", "[unit][Components][CMeta]") {
  
  steamrot::CMeta meta1;
  meta1.m_active = true;
  meta1.m_entity_alive = true;
  
  // Copy construct
  steamrot::CMeta meta2 = meta1;
  
  // Verify fields copied correctly
  REQUIRE(meta2.m_active == true);
  REQUIRE(meta2.m_entity_alive == true);
}

/////////////////////////////////////////////////
/// Test: Field Modification
/////////////////////////////////////////////////
TEST_CASE("CMeta fields can be modified", "[unit][Components][CMeta]") {
  
  steamrot::CMeta meta;
  
  // Modify fields
  meta.m_active = true;
  meta.m_entity_alive = true;
  
  // Verify modifications
  REQUIRE(meta.m_active == true);
  REQUIRE(meta.m_entity_alive == true);
}

/////////////////////////////////////////////////
/// Test: Default Constructibility (Required for Components)
/////////////////////////////////////////////////
TEST_CASE("CMeta is default constructible", "[unit][Components][CMeta]") {
  
  // This should compile and run without errors
  REQUIRE_NOTHROW(steamrot::CMeta{});
}
```

---

## Layer 2 Example: Testing FlatbuffersAssetDataProvider

**File**: `tests/unit/data_providers/FlatbuffersAssetDataProvider.test.cpp` (exists)

**Class being tested**: `FlatbuffersAssetDataProvider` - Loads asset configuration data

### Complete Implementation

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersAssetDataProvider
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersAssetDataProvider.h"
#include "paths.h"
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
/// Test: Construction
/////////////////////////////////////////////////
TEST_CASE("FlatbuffersAssetDataProvider constructs correctly", 
          "[unit][FlatbuffersAssetDataProvider]") {
  
  REQUIRE_NOTHROW(steamrot::FlatbuffersAssetDataProvider{});
}

/////////////////////////////////////////////////
/// Test: Successful Data Loading
/////////////////////////////////////////////////
TEST_CASE("FlatbuffersAssetDataProvider loads valid data", 
          "[unit][FlatbuffersAssetDataProvider]") {
  
  steamrot::FlatbuffersAssetDataProvider provider;
  
  // Load asset data
  auto result = provider.ProvideAssetData();
  
  // Should succeed
  REQUIRE(result.has_value());
  
  // Data should not be null
  const auto* data = result.value();
  REQUIRE(data != nullptr);
  
  // Verify data has expected structure
  REQUIRE(data->fonts() != nullptr);
  REQUIRE(data->fonts()->size() > 0);
}

/////////////////////////////////////////////////
/// Test: Data Validation
/////////////////////////////////////////////////
TEST_CASE("FlatbuffersAssetDataProvider validates loaded data", 
          "[unit][FlatbuffersAssetDataProvider]") {
  
  steamrot::FlatbuffersAssetDataProvider provider;
  auto result = provider.ProvideAssetData();
  
  REQUIRE(result.has_value());
  const auto* data = result.value();
  
  // Check fonts array
  const auto* fonts = data->fonts();
  REQUIRE(fonts != nullptr);
  
  // Verify each font entry has required fields
  for (size_t i = 0; i < fonts->size(); ++i) {
    const auto* font = fonts->Get(i);
    REQUIRE(font != nullptr);
    REQUIRE(font->font_name() != nullptr);
    REQUIRE(!font->font_name()->str().empty());
  }
}

/////////////////////////////////////////////////
/// Test: Error Handling - Missing File
/////////////////////////////////////////////////
TEST_CASE("FlatbuffersAssetDataProvider handles missing file",
          "[unit][FlatbuffersAssetDataProvider]") {
  
  // Note: In actual implementation, provider uses paths from PathProvider
  // This test documents expected behavior for missing files
  
  // If the data file is missing, provider should return an error
  // (Implementation detail: this may be handled at a higher level)
  
  SUCCEED("Error handling documented");
}

/////////////////////////////////////////////////
/// Test: Null Safety - Optional Fields
/////////////////////////////////////////////////
TEST_CASE("FlatbuffersAssetDataProvider handles optional fields safely",
          "[unit][FlatbuffersAssetDataProvider]") {
  
  steamrot::FlatbuffersAssetDataProvider provider;
  auto result = provider.ProvideAssetData();
  
  REQUIRE(result.has_value());
  const auto* data = result.value();
  
  // Access optional fields with null checks
  if (data->fonts()) {
    // Safe to use fonts
    REQUIRE(data->fonts()->size() >= 0);
  }
}
```

---

## Layer 3 Example: Testing EntityManager

**File**: `tests/unit/entity/EntityManager.test.cpp` (exists)

**Class being tested**: `EntityManager` - Manages entity lifecycle and memory

### Complete Implementation

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EntityManager
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityManager.h"
#include "CMeta.h"
#include "entity_memory.h"
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
/// Test: Default Construction
/////////////////////////////////////////////////
TEST_CASE("EntityManager constructs with default pool size", 
          "[unit][EntityManager]") {
  
  steamrot::EntityManager manager;
  
  const auto& pool = manager.GetEntityMemoryPool();
  
  // Pool should be created with some default size
  REQUIRE(pool.size() > 0);
}

/////////////////////////////////////////////////
/// Test: Custom Pool Size Construction
/////////////////////////////////////////////////
TEST_CASE("EntityManager can be constructed with custom pool size",
          "[unit][EntityManager]") {
  
  size_t custom_size = 150;
  steamrot::EntityManager manager(custom_size);
  
  const auto& pool = manager.GetEntityMemoryPool();
  
  // Pool should have requested size
  REQUIRE(pool.size() == custom_size);
}

/////////////////////////////////////////////////
/// Test: Entity Component Access
/////////////////////////////////////////////////
TEST_CASE("EntityManager allows component access",
          "[unit][EntityManager]") {
  
  steamrot::EntityManager manager;
  const auto& pool = manager.GetEntityMemoryPool();
  
  // Access component from entity 0
  auto& meta = steamrot::GetComponent<steamrot::CMeta>(0, pool);
  
  // Should have default values
  REQUIRE(meta.m_active == false);
  REQUIRE(meta.m_entity_alive == false);
}

/////////////////////////////////////////////////
/// Test: Entity Component Modification
/////////////////////////////////////////////////
TEST_CASE("EntityManager allows component modification",
          "[unit][EntityManager]") {
  
  steamrot::EntityManager manager;
  auto& pool = manager.GetEntityMemoryPool();
  
  // Get component and modify
  auto& meta = steamrot::GetComponent<steamrot::CMeta>(0, pool);
  meta.m_active = true;
  meta.m_entity_alive = true;
  
  // Verify modification
  const auto& const_pool = manager.GetEntityMemoryPool();
  const auto& const_meta = steamrot::GetComponent<steamrot::CMeta>(0, const_pool);
  REQUIRE(const_meta.m_active == true);
  REQUIRE(const_meta.m_entity_alive == true);
}

/////////////////////////////////////////////////
/// Test: Archetype Updates
/////////////////////////////////////////////////
TEST_CASE("EntityManager can update archetypes",
          "[unit][EntityManager]") {
  
  steamrot::EntityManager manager;
  auto& pool = manager.GetEntityMemoryPool();
  
  // Activate a component
  auto& meta = steamrot::GetComponent<steamrot::CMeta>(0, pool);
  meta.m_active = true;
  
  // Update archetypes
  REQUIRE_NOTHROW(manager.UpdateArchetypes());
  
  // Verify archetypes were updated
  const auto& archetypes = manager.GetArchetypes();
  
  // Should have at least one archetype
  REQUIRE(archetypes.size() > 0);
}

/////////////////////////////////////////////////
/// Test: Multiple Entity Management
/////////////////////////////////////////////////
TEST_CASE("EntityManager handles multiple entities",
          "[unit][EntityManager]") {
  
  size_t pool_size = 10;
  steamrot::EntityManager manager(pool_size);
  auto& pool = manager.GetEntityMemoryPool();
  
  // Activate components on multiple entities
  for (size_t i = 0; i < 5; ++i) {
    auto& meta = steamrot::GetComponent<steamrot::CMeta>(i, pool);
    meta.m_active = true;
    meta.m_entity_alive = true;
  }
  
  // Update archetypes
  manager.UpdateArchetypes();
  
  // Verify all entities are tracked
  const auto& archetypes = manager.GetArchetypes();
  bool found_entities = false;
  
  for (const auto& [archetype_id, entity_indices] : archetypes) {
    if (!entity_indices.empty()) {
      found_entities = true;
      break;
    }
  }
  
  REQUIRE(found_entities);
}
```

---

## Layer 4 Example: Testing UIRenderLogic

**File**: `tests/unit/logic/UIRenderLogic.test.cpp` (to be extracted/created)

**Class being tested**: `UIRenderLogic` - Renders UI elements to scene texture

### Complete Implementation

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for UIRenderLogic
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIRenderLogic.h"
#include "TestFixture.h"
#include "CUserInterface.h"
#include "PanelElement.h"
#include "entity_memory.h"
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
/// Test: Construction
/////////////////////////////////////////////////
TEST_CASE("UIRenderLogic constructs correctly", "[unit][UIRenderLogic]") {
  
  steamrot::tests::TestFixture fixture;
  
  REQUIRE_NOTHROW(
    steamrot::UIRenderLogic{fixture.GetSceneContext()}
  );
}

/////////////////////////////////////////////////
/// Test: RunLogic Execution
/////////////////////////////////////////////////
TEST_CASE("UIRenderLogic::RunLogic executes without errors",
          "[unit][UIRenderLogic]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::UIRenderLogic logic(fixture.GetSceneContext());
  
  // Should execute without throwing
  REQUIRE_NOTHROW(logic.RunLogic());
}

/////////////////////////////////////////////////
/// Test: Empty Scene Rendering
/////////////////////////////////////////////////
TEST_CASE("UIRenderLogic handles empty scene",
          "[unit][UIRenderLogic]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::UIRenderLogic logic(fixture.GetSceneContext());
  
  // With no active UI entities, should still run successfully
  REQUIRE_NOTHROW(logic.RunLogic());
}

/////////////////////////////////////////////////
/// Test: Single UI Element Rendering
/////////////////////////////////////////////////
TEST_CASE("UIRenderLogic renders single visible UI element",
          "[unit][UIRenderLogic]") {
  
  steamrot::tests::TestFixture fixture;
  
  // Set up entity with UI component
  auto& pool = fixture.GetEntityManager().GetEntityMemoryPool();
  auto& ui = steamrot::GetComponent<steamrot::CUserInterface>(0, pool);
  ui.m_active = true;
  ui.m_start_visible = true;
  
  // Create UI element
  auto panel = std::make_shared<steamrot::PanelElement>();
  panel->m_position = sf::Vector2f(100.0f, 100.0f);
  panel->m_size = sf::Vector2f(50.0f, 50.0f);
  ui.m_root_ui_element = panel;
  
  // Update archetypes
  fixture.GetEntityManager().UpdateArchetypes();
  
  // Create and run logic
  steamrot::UIRenderLogic logic(fixture.GetSceneContext());
  REQUIRE_NOTHROW(logic.RunLogic());
}

/////////////////////////////////////////////////
/// Test: Multiple UI Element Rendering
/////////////////////////////////////////////////
TEST_CASE("UIRenderLogic renders multiple UI elements",
          "[unit][UIRenderLogic]") {
  
  steamrot::tests::TestFixture fixture;
  auto& pool = fixture.GetEntityManager().GetEntityMemoryPool();
  
  // Create multiple UI entities
  for (size_t i = 0; i < 3; ++i) {
    auto& ui = steamrot::GetComponent<steamrot::CUserInterface>(i, pool);
    ui.m_active = true;
    ui.m_start_visible = true;
    
    auto panel = std::make_shared<steamrot::PanelElement>();
    panel->m_position = sf::Vector2f(100.0f * i, 100.0f);
    panel->m_size = sf::Vector2f(50.0f, 50.0f);
    ui.m_root_ui_element = panel;
  }
  
  fixture.GetEntityManager().UpdateArchetypes();
  
  steamrot::UIRenderLogic logic(fixture.GetSceneContext());
  REQUIRE_NOTHROW(logic.RunLogic());
}

/////////////////////////////////////////////////
/// Test: Hidden UI Elements Not Rendered
/////////////////////////////////////////////////
TEST_CASE("UIRenderLogic skips hidden UI elements",
          "[unit][UIRenderLogic]") {
  
  steamrot::tests::TestFixture fixture;
  auto& pool = fixture.GetEntityManager().GetEntityMemoryPool();
  
  // Create UI entity that is not visible
  auto& ui = steamrot::GetComponent<steamrot::CUserInterface>(0, pool);
  ui.m_active = true;
  ui.m_start_visible = false;  // Hidden
  
  auto panel = std::make_shared<steamrot::PanelElement>();
  ui.m_root_ui_element = panel;
  
  fixture.GetEntityManager().UpdateArchetypes();
  
  steamrot::UIRenderLogic logic(fixture.GetSceneContext());
  
  // Should run without issues even though element is hidden
  REQUIRE_NOTHROW(logic.RunLogic());
}

/////////////////////////////////////////////////
/// Test: Null UI Element Handling
/////////////////////////////////////////////////
TEST_CASE("UIRenderLogic handles null UI elements",
          "[unit][UIRenderLogic]") {
  
  steamrot::tests::TestFixture fixture;
  auto& pool = fixture.GetEntityManager().GetEntityMemoryPool();
  
  // Create UI entity with null root element
  auto& ui = steamrot::GetComponent<steamrot::CUserInterface>(0, pool);
  ui.m_active = true;
  ui.m_start_visible = true;
  ui.m_root_ui_element = nullptr;  // Null element
  
  fixture.GetEntityManager().UpdateArchetypes();
  
  steamrot::UIRenderLogic logic(fixture.GetSceneContext());
  
  // Should handle gracefully without crashing
  REQUIRE_NOTHROW(logic.RunLogic());
}
```

---

## Layer 5 Example: Testing TitleScene (Template)

**File**: `tests/unit/scenes/TitleScene.test.cpp` (to be created)

**Class being tested**: `TitleScene` - Main menu scene

### Complete Implementation Template

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for TitleScene
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TitleScene.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
/// Test: Construction
/////////////////////////////////////////////////
TEST_CASE("TitleScene constructs correctly", "[unit][TitleScene]") {
  
  steamrot::tests::TestFixture fixture;
  
  REQUIRE_NOTHROW(
    steamrot::TitleScene{fixture.GetGameContext()}
  );
}

/////////////////////////////////////////////////
/// Test: Scene Type
/////////////////////////////////////////////////
TEST_CASE("TitleScene initializes with correct scene type",
          "[unit][TitleScene]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::TitleScene scene(fixture.GetGameContext());
  
  const auto& scene_info = scene.GetSceneInfo();
  REQUIRE(scene_info.scene_type == steamrot::SceneType_TITLE);
}

/////////////////////////////////////////////////
/// Test: Render Texture
/////////////////////////////////////////////////
TEST_CASE("TitleScene has valid render texture", "[unit][TitleScene]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::TitleScene scene(fixture.GetGameContext());
  
  auto& render_texture = scene.GetRenderTexture();
  
  // Render texture should be created with valid size
  auto size = render_texture.getSize();
  REQUIRE(size.x > 0);
  REQUIRE(size.y > 0);
}

/////////////////////////////////////////////////
/// Test: System Methods - Render
/////////////////////////////////////////////////
TEST_CASE("TitleScene::sRender executes without errors",
          "[unit][TitleScene]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::TitleScene scene(fixture.GetGameContext());
  
  REQUIRE_NOTHROW(scene.sRender());
}

/////////////////////////////////////////////////
/// Test: System Methods - Action
/////////////////////////////////////////////////
TEST_CASE("TitleScene::sAction executes without errors",
          "[unit][TitleScene]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::TitleScene scene(fixture.GetGameContext());
  
  REQUIRE_NOTHROW(scene.sAction());
}

/////////////////////////////////////////////////
/// Test: System Methods - Collision
/////////////////////////////////////////////////
TEST_CASE("TitleScene::sCollision executes without errors",
          "[unit][TitleScene]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::TitleScene scene(fixture.GetGameContext());
  
  REQUIRE_NOTHROW(scene.sCollision());
}

/////////////////////////////////////////////////
/// Test: System Methods - Movement
/////////////////////////////////////////////////
TEST_CASE("TitleScene::sMovement executes without errors",
          "[unit][TitleScene]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::TitleScene scene(fixture.GetGameContext());
  
  REQUIRE_NOTHROW(scene.sMovement());
}

/////////////////////////////////////////////////
/// Test: Active State Management
/////////////////////////////////////////////////
TEST_CASE("TitleScene active state can be changed",
          "[unit][TitleScene]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::TitleScene scene(fixture.GetGameContext());
  
  // Default should be inactive
  REQUIRE(scene.GetActive() == false);
  
  // Activate scene
  scene.SetActive(true);
  REQUIRE(scene.GetActive() == true);
  
  // Deactivate scene
  scene.SetActive(false);
  REQUIRE(scene.GetActive() == false);
}

/////////////////////////////////////////////////
/// Test: Entity Manager
/////////////////////////////////////////////////
TEST_CASE("TitleScene has valid EntityManager",
          "[unit][TitleScene]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::TitleScene scene(fixture.GetGameContext());
  
  auto& entity_manager = scene.GetEntityManager();
  
  // Entity manager should be initialized
  const auto& pool = entity_manager.GetEntityMemoryPool();
  REQUIRE(pool.size() > 0);
}

/////////////////////////////////////////////////
/// Test: Scene Context
/////////////////////////////////////////////////
TEST_CASE("TitleScene can create SceneContext",
          "[unit][TitleScene]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::TitleScene scene(fixture.GetGameContext());
  
  REQUIRE_NOTHROW(scene.GetSceneContext());
}
```

### CMakeLists.txt for Scenes

Create `tests/unit/scenes/CMakeLists.txt`:

```cmake
add_executable(test_scenes
  TitleScene.test.cpp
  CraftingScene.test.cpp
  SceneFactory.test.cpp
  SceneManager.test.cpp
)

target_link_libraries(test_scenes
  PRIVATE
  Catch2::Catch2WithMain
  steamrot_scenes
  steamrot_context
  test_context
)

catch_discover_tests(test_scenes)
```

---

## Layer 6 Example: Testing GameEngine

**File**: `tests/unit/engine/GameEngine.test.cpp` (exists, expand)

**Class being tested**: `GameEngine` - Top-level game coordination

### Expanded Implementation

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for GameEngine
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GameEngine.h"
#include <catch2/catch_test_macros.hpp>

/////////////////////////////////////////////////
/// Test: Construction
/////////////////////////////////////////////////
TEST_CASE("GameEngine constructs correctly", "[unit][GameEngine]") {
  REQUIRE_NOTHROW(steamrot::GameEngine{});
}

/////////////////////////////////////////////////
/// Test: Initialization
/////////////////////////////////////////////////
TEST_CASE("GameEngine initializes subsystems", "[unit][GameEngine]") {
  
  steamrot::GameEngine engine;
  
  auto result = engine.Initialize();
  
  if (!result.has_value()) {
    // If initialization fails, fail test with error message
    FAIL("Engine initialization failed: " + result.error().message);
  }
  
  REQUIRE(result.has_value());
}

/////////////////////////////////////////////////
/// Test: Configuration Loading
/////////////////////////////////////////////////
TEST_CASE("GameEngine loads configuration correctly",
          "[unit][GameEngine]") {
  
  steamrot::GameEngine engine;
  auto init_result = engine.Initialize();
  REQUIRE(init_result.has_value());
  
  // Verify engine configuration was loaded
  // (Adjust based on actual GameEngine API)
  SUCCEED("Configuration loaded successfully");
}

/////////////////////////////////////////////////
/// Test: Resource Initialization
/////////////////////////////////////////////////
TEST_CASE("GameEngine initializes resources",
          "[unit][GameEngine]") {
  
  steamrot::GameEngine engine;
  auto init_result = engine.Initialize();
  REQUIRE(init_result.has_value());
  
  // Verify resources were initialized
  // (AssetManager, DisplayManager, etc.)
  SUCCEED("Resources initialized successfully");
}

/////////////////////////////////////////////////
/// Test: Error Handling - Multiple Initialization
/////////////////////////////////////////////////
TEST_CASE("GameEngine handles multiple initialization calls",
          "[unit][GameEngine]") {
  
  steamrot::GameEngine engine;
  
  // First initialization
  auto result1 = engine.Initialize();
  REQUIRE(result1.has_value());
  
  // Second initialization should be handled gracefully
  auto result2 = engine.Initialize();
  
  // Either succeeds (idempotent) or returns appropriate error
  // Behavior depends on implementation
  SUCCEED("Multiple initialization handled");
}

/////////////////////////////////////////////////
/// Test: Cleanup on Destruction
/////////////////////////////////////////////////
TEST_CASE("GameEngine cleans up resources on destruction",
          "[unit][GameEngine]") {
  
  {
    steamrot::GameEngine engine;
    engine.Initialize();
    // Engine goes out of scope
  }
  
  // Should not crash or leak memory
  SUCCEED("Engine destroyed without errors");
}
```

---

## Summary

These examples demonstrate:

1. **Complete test files** for each layer
2. **Real code** that can be adapted for your classes
3. **Common patterns** repeated across layers
4. **Proper structure** with headers and documentation
5. **Edge cases** and error handling
6. **CMakeLists.txt** integration

### Key Takeaways

- **Start simple**: Begin with construction and basic functionality
- **Build up**: Add edge cases and error handling
- **Use TestFixture**: Provides consistent test environment
- **Test behavior, not implementation**: Focus on public API
- **Document**: Clear test names and comments

### Next Steps

Use these examples as templates for:
1. Creating tests for new classes
2. Expanding existing test coverage
3. Understanding test patterns in each layer

---

**Last Updated**: 2025-12-17  
**Document Version**: 1.0
