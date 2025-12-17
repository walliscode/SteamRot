# Complete Unit Testing Guide for SteamRot

This guide provides a comprehensive, layer-by-layer approach to implementing unit tests for the SteamRot game engine. Tests are organized by architectural layers, starting from the foundation and building up to complex systems.

## Table of Contents

- [Testing Philosophy](#testing-philosophy)
- [Prerequisites](#prerequisites)
- [Architecture Overview](#architecture-overview)
- [Layer 0: Core Types and Utilities](#layer-0-core-types-and-utilities)
- [Layer 1: Components](#layer-1-components)
- [Layer 2: Data Providers and Configurators](#layer-2-data-providers-and-configurators)
- [Layer 3: Managers](#layer-3-managers)
- [Layer 4: Logic Classes](#layer-4-logic-classes)
- [Layer 5: Scenes](#layer-5-scenes)
- [Layer 6: Engine](#layer-6-engine)
- [Test Infrastructure](#test-infrastructure)
- [Best Practices](#best-practices)
- [Common Patterns](#common-patterns)

## Testing Philosophy

SteamRot follows a **Test-Driven Development (TDD)** approach with these principles:

1. **Test concrete implementations only** - We test classes that have concrete implementations, not abstract interfaces
2. **Layer-by-layer testing** - Test foundation layers before higher layers that depend on them
3. **Unit tests are isolated** - Use mocks/fixtures to isolate the class under test
4. **Clear test organization** - Tests mirror the `src/` directory structure under `tests/unit/`
5. **Descriptive test names** - Test cases should clearly describe what is being tested

### What to Test

✅ **Test these:**
- Concrete classes (e.g., `AssetManager`, `EntityManager`, `UIRenderLogic`)
- Public methods and their behavior
- Edge cases and error conditions
- Default construction and initialization
- Component data structures

❌ **Don't test these:**
- Abstract interfaces (e.g., `IEntityConfigurator`, `ISceneDataProvider`)
- Private implementation details
- Generated code (FlatBuffers headers)
- Third-party libraries

## Prerequisites

Before writing tests, ensure you understand:
- **Catch2 testing framework** - Used for all unit tests
- **Test tags** - All tests must have `[unit]` tag plus class-specific tags
- **TestFixture pattern** - Standard way to set up test contexts
- **std::expected pattern** - Used throughout codebase for error handling

### Test File Structure

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for ClassName
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ClassName.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("ClassName description of behavior", "[unit][ClassName]") {
  // Arrange
  steamrot::ClassName object;
  
  // Act
  auto result = object.Method();
  
  // Assert
  REQUIRE(result.has_value());
}
```

## Architecture Overview

The SteamRot engine is organized into architectural layers with clear dependencies:

```
Layer 6: Engine (GameEngine)
         ↓
Layer 5: Scenes (TitleScene, CraftingScene)
         ↓
Layer 4: Logic (UIRenderLogic, UIActionLogic, UICollisionLogic, UIStateLogic)
         ↓
Layer 3: Managers (EntityManager, AssetManager, EventHandler, DisplayManager)
         ↓
Layer 2: Data Providers & Configurators (FlatbuffersDataLoader, FlatbuffersEntityConfigurator)
         ↓
Layer 1: Components (CMeta, CUserInterface, CGrimoireMachina)
         ↓
Layer 0: Core Types (FailInfo, SceneConfig, EventPacket, UIElement types)
```

**Key principle**: Lower layers have no dependencies on higher layers. Test from bottom to top.

## Layer 0: Core Types and Utilities

**Directory**: `src/types/`, `src/logger/`

**What to test**: Data structures, enums, utility functions with no external dependencies.

### Classes to Test

#### Types (`src/types/core/`)
- ✅ Simple structs: `FailInfo`, `Joint`, `Fragment`
- ✅ Config structs: `SceneConfig`, `EngineConfig`, `SceneInfo`
- ❌ Enums: `Layout`, `SpacingAndSizing` (no behavior to test)

#### Events (`src/types/events/`)
- ✅ `EventPacket` - Event data structure
- ✅ `Subscriber` - Event subscriber
- ✅ `UserInputBitset` - Input state management

#### UI Elements (`src/types/user_interface/`)
- ✅ `UIElement` hierarchy: `ButtonElement`, `PanelElement`, `DropDownListElement`, etc.
- ✅ Style types: `ButtonStyle`, `PanelStyle`, `DropDownButtonStyle`, etc.

#### Logger (`src/logger/`)
- ✅ `Logger` - Logging utilities

### Example: Testing a Simple Type

**File**: `tests/unit/types/EventPacket.test.cpp`

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

TEST_CASE("EventPacket default construction", "[unit][EventPacket]") {
  steamrot::EventPacket packet;
  
  // EventPacket should have default values
  REQUIRE(packet.event_type == steamrot::EventType::EventType_None);
  REQUIRE(packet.input_state.none());
}

TEST_CASE("EventPacket can be constructed with event type", "[unit][EventPacket]") {
  steamrot::EventPacket packet{
    steamrot::EventType::EventType_EVENT_USER_INPUT,
    steamrot::UserInputBitset{}
  };
  
  REQUIRE(packet.event_type == steamrot::EventType::EventType_EVENT_USER_INPUT);
}
```

**Location**: `tests/unit/types/` (new directory to create)

### Testing Patterns for Layer 0

1. **Value types**: Test construction, default values, field access
2. **No mocking needed**: These types have no dependencies
3. **Focus on data integrity**: Ensure structs hold correct data

### Current Status

| Subsystem | Classes | Tested | Location |
|-----------|---------|--------|----------|
| types/events | EventPacket, Subscriber, UserInputBitset | ✅ Partial | `tests/unit/events/` |
| types/user_interface | UIElement hierarchy | ❌ None | Not yet created |
| types/core | Config structs | ❌ None | Not yet created |
| logger | Logger | ❌ None | Not yet created |

**Next steps**: Create test files for untested types in Layer 0.

## Layer 1: Components

**Directory**: `src/components/`

**What to test**: Pure data container structs that inherit from `Component`.

### Understanding Components

Components are **pure data containers** with these characteristics:
- Inherit from `Component` base struct
- No logic or methods (except `GetComponentRegisterIndex()`)
- Must be default-constructible
- All member variables have default initialization
- Use `m_` prefix for member variables
- Use `C` prefix for class names

### Classes to Test

All concrete component implementations:
- ✅ `CMeta` - Entity metadata
- ✅ `CUserInterface` - UI component data
- ✅ `CGrimoireMachina` - Grimoire system data
- ✅ `CMachinaForm` - Machina form data
- ✅ `CUIState` - UI state data

### Component Test Pattern

Every component test should verify:
1. **Default construction** - Can be constructed without errors
2. **Default values** - All members have correct default values
3. **Component register** - `GetComponentRegisterIndex()` returns valid index
4. **Active flag** - Inherits `m_active` from `Component` base

### Example: Complete Component Test

**File**: `tests/unit/components/CMeta.test.cpp`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for CMeta class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CMeta.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CMeta has correct default values", "[unit][Components][CMeta]") {
  steamrot::CMeta meta;

  // Test pre-configuration state
  REQUIRE(meta.m_active == false);
  REQUIRE(meta.m_entity_alive == false);
}

TEST_CASE("CMeta::GetComponentRegisterIndex returns valid index", "[unit][Components][CMeta]") {
  steamrot::CMeta meta;
  
  // Index should be a valid position in ComponentRegister tuple
  size_t index = meta.GetComponentRegisterIndex();
  REQUIRE(index >= 0);
  REQUIRE(index < std::tuple_size_v<steamrot::ComponentRegister>);
}

TEST_CASE("CMeta can be copy constructed", "[unit][Components][CMeta]") {
  steamrot::CMeta meta1;
  meta1.m_active = true;
  meta1.m_entity_alive = true;
  
  steamrot::CMeta meta2 = meta1;
  
  REQUIRE(meta2.m_active == true);
  REQUIRE(meta2.m_entity_alive == true);
}
```

### Testing Complex Components

For components with more fields (like `CUserInterface`):

```cpp
TEST_CASE("CUserInterface has correct default values", "[unit][Components][CUserInterface]") {
  steamrot::CUserInterface ui;

  // Base component fields
  REQUIRE(ui.m_active == false);
  
  // Component-specific fields
  REQUIRE(ui.m_ui_name.empty());
  REQUIRE(ui.m_start_visible == false);
  REQUIRE(ui.m_root_ui_element == nullptr);
}

TEST_CASE("CUserInterface can store UI element", "[unit][Components][CUserInterface]") {
  steamrot::CUserInterface ui;
  
  // Create a simple UI element
  auto panel = std::make_shared<steamrot::PanelElement>();
  ui.m_root_ui_element = panel;
  
  REQUIRE(ui.m_root_ui_element != nullptr);
}
```

### Current Status

| Component | Test File | Status | Notes |
|-----------|-----------|--------|-------|
| CMeta | `CMeta.test.cpp` | ✅ Complete | Basic test coverage |
| CUserInterface | `CUserInterface.test.cpp` | ✅ Complete | Basic test coverage |
| CGrimoireMachina | `CGrimoireMachina.test.cpp` | ✅ Complete | Basic test coverage |
| CMachinaForm | `CMachinaForm.test.cpp` | ✅ Complete | Basic test coverage |
| CUIState | `CUIState.test.cpp` | ✅ Complete | Basic test coverage |

**Status**: Layer 1 has good coverage. Consider adding edge case tests.

## Layer 2: Data Providers and Configurators

**Directories**: `src/data_providers/`, `src/configuration/`, `src/entity/`, `src/user_interface/`

**What to test**: Classes that load data from files and configure objects.

### Understanding Layer 2

This layer handles:
- **Data loading** - Reading JSON/binary files via FlatBuffers
- **Configuration** - Populating objects from loaded data
- **Validation** - Ensuring data integrity and null-safety

### Classes to Test

#### Data Providers (`src/data_providers/`)
- ✅ `FlatbuffersDataLoader` - Main data loading coordinator
- ✅ `FlatbuffersAssetDataProvider` - Asset data loading
- ✅ `FlatbuffersEngineDataProvider` - Engine config loading
- ✅ `FlatbuffersSceneDataProvider` - Scene data loading
- ✅ `FlatbuffersSceneManagerDataProvider` - Scene manager config loading
- ✅ `FlatbuffersSubscriberViewer` - Subscriber data viewing
- ✅ `FlatbuffersUIStyleDataProvider` - UI style loading

#### Configurators
- ✅ `FlatbuffersEntityConfigurator` - Entity configuration from data
- ❌ `FlatbuffersSceneConfigurator` - Scene configuration
- ❌ `FlatbuffersUIElementConfigurator` - UI element configuration

### Data Provider Test Pattern

Data providers should test:
1. **Successful loading** - Can load valid data files
2. **Error handling** - Returns expected errors for invalid data
3. **Null safety** - Handles missing/null fields gracefully
4. **Data integrity** - Loaded data matches expected values

### Example: Testing a Data Provider

**File**: `tests/unit/data_providers/FlatbuffersAssetDataProvider.test.cpp`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersAssetDataProvider
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersAssetDataProvider.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("FlatbuffersAssetDataProvider loads valid data", 
          "[unit][FlatbuffersAssetDataProvider]") {
  
  steamrot::FlatbuffersAssetDataProvider provider;
  
  auto result = provider.LoadAssetData();
  
  REQUIRE(result.has_value());
  REQUIRE(result.value() != nullptr);
}

TEST_CASE("FlatbuffersAssetDataProvider handles missing file",
          "[unit][FlatbuffersAssetDataProvider]") {
  
  steamrot::FlatbuffersAssetDataProvider provider;
  
  // Test with non-existent path
  auto result = provider.LoadAssetData("/invalid/path");
  
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().fail_mode == steamrot::FailMode::FileNotFound);
}
```

### Configurator Test Pattern

Configurators should test:
1. **Valid configuration** - Can configure objects from valid data
2. **Null checks** - Handles missing fields in FlatBuffers data
3. **Component activation** - Sets `m_active` flag correctly
4. **Field mapping** - Data correctly transferred to object fields

### Example: Testing a Configurator

**File**: `tests/unit/entity/FlatbuffersEntityConfigurator.test.cpp`

```cpp
TEST_CASE("FlatbuffersEntityConfigurator configures entity correctly",
          "[unit][FlatbuffersEntityConfigurator]") {
  
  // Arrange - Load test data
  steamrot::tests::TestFixture fixture;
  steamrot::FlatbuffersEntityConfigurator configurator;
  
  // Act - Configure entities
  auto result = configurator.ConfigureEntities(
    test_data,
    fixture.GetEntityManager().GetEntityMemoryPool()
  );
  
  // Assert
  REQUIRE(result.has_value());
  
  // Verify entity was configured
  const auto& pool = fixture.GetEntityManager().GetEntityMemoryPool();
  auto& meta = steamrot::GetComponent<steamrot::CMeta>(0, pool);
  REQUIRE(meta.m_active == true);
}

TEST_CASE("FlatbuffersEntityConfigurator handles null component data",
          "[unit][FlatbuffersEntityConfigurator]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::FlatbuffersEntityConfigurator configurator;
  
  // Test with data that has null components
  auto result = configurator.ConfigureEntities(
    null_component_data,
    fixture.GetEntityManager().GetEntityMemoryPool()
  );
  
  // Should succeed - null components just aren't activated
  REQUIRE(result.has_value());
}
```

### Current Status

| Subsystem | Classes | Tested | Location |
|-----------|---------|--------|----------|
| data_providers | 7 provider classes | ✅ Most tested | `tests/unit/data_providers/` |
| configurators | FlatbuffersEntityConfigurator | ✅ Tested | `tests/unit/entity/` |
| configurators | Scene/UI configurators | ❌ Not tested | Need to create |

**Next steps**: Add tests for Scene and UI configurators.

## Layer 3: Managers

**Directories**: `src/entity/`, `src/assets/`, `src/events/`, `src/display/`, `src/scenes/`

**What to test**: Manager classes that coordinate system behavior.

### Understanding Managers

Managers are responsible for:
- **Resource management** - Pooling, caching, lifetime management
- **Coordination** - Orchestrating between systems
- **State management** - Tracking system state

### Classes to Test

#### Entity Management (`src/entity/`)
- ✅ `EntityManager` - Entity lifecycle and memory management
- ✅ `ArchetypeManager` - Grouping entities by component signatures

#### Asset Management (`src/assets/`)
- ✅ `AssetManager` - Font and UI style loading/caching

#### Event Management (`src/events/`)
- ✅ `EventHandler` - Event bus and routing

#### Display Management (`src/display/`)
- ✅ `DisplayManager` - Window and rendering coordination
- ❌ `Tile`, `ReTile`, `Session` - Display utilities

#### Scene Management (`src/scenes/`)
- ❌ `SceneManager` - Scene lifecycle and transitions
- ❌ `SceneFactory` - Scene instantiation

### Manager Test Pattern

Managers should test:
1. **Construction** - Can be constructed without errors
2. **Resource operations** - Add, get, remove operations work
3. **State management** - Internal state is consistent
4. **Error conditions** - Invalid operations return errors
5. **Memory safety** - No leaks or dangling pointers

### Example: Testing EntityManager

**File**: `tests/unit/entity/EntityManager.test.cpp`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for EntityManager
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityManager.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("EntityManager constructs with default pool size", 
          "[unit][EntityManager]") {
  
  steamrot::EntityManager manager;
  
  const auto& pool = manager.GetEntityMemoryPool();
  REQUIRE(pool.size() > 0);
}

TEST_CASE("EntityManager can access entity components",
          "[unit][EntityManager]") {
  
  steamrot::EntityManager manager;
  const auto& pool = manager.GetEntityMemoryPool();
  
  // Get component from entity 0
  auto& meta = steamrot::GetComponent<steamrot::CMeta>(0, pool);
  
  // Should have default values
  REQUIRE(meta.m_active == false);
}

TEST_CASE("EntityManager pool size matches configuration",
          "[unit][EntityManager]") {
  
  size_t expected_size = 100;
  steamrot::EntityManager manager(expected_size);
  
  REQUIRE(manager.GetEntityMemoryPool().size() == expected_size);
}
```

### Example: Testing AssetManager

**File**: `tests/unit/assets/AssetManager.test.cpp`

```cpp
TEST_CASE("AssetManager is constructed correctly", "[unit][AssetManager]") {
  steamrot::AssetManager asset_manager;
  REQUIRE_NOTHROW(asset_manager);
}

TEST_CASE("AssetManager::LoadDefaultAssets works correctly",
          "[unit][AssetManager]") {
  steamrot::AssetManager asset_manager;

  auto load_result = asset_manager.LoadDefaultAssets();
  if (!load_result.has_value())
    FAIL(load_result.error().message);

  // Verify fonts were loaded
  std::vector<std::string> expected_fonts = {"DaddyTimeMonoNerdFont-Regular"};
  const auto& fonts = asset_manager.GetAllFonts();
  
  for (const auto& font_name : expected_fonts) {
    REQUIRE(fonts.contains(font_name));
    REQUIRE(fonts.at(font_name) != nullptr);
  }

  // Verify UI styles were loaded
  const auto& ui_styles = asset_manager.GetAllUIStyles();
  REQUIRE(!ui_styles.empty());
  REQUIRE(ui_styles.contains("default"));
}

TEST_CASE("AssetManager::GetDefaultUIStyle returns valid style",
          "[unit][AssetManager]") {
  
  steamrot::AssetManager asset_manager;
  asset_manager.LoadDefaultAssets();
  
  const auto& default_style = asset_manager.GetDefaultUIStyle();
  
  // Verify style has required data
  REQUIRE(default_style.button_style != std::nullopt);
  REQUIRE(default_style.panel_style != std::nullopt);
}
```

### Example: Testing EventHandler

**File**: `tests/unit/events/EventHandler.test.cpp`

```cpp
TEST_CASE("EventHandler constructs correctly", "[unit][EventHandler]") {
  steamrot::EventHandler handler;
  REQUIRE_NOTHROW(handler);
}

TEST_CASE("EventHandler::AddEvent adds event to queue",
          "[unit][EventHandler]") {
  
  steamrot::EventHandler handler;
  
  steamrot::EventPacket event{
    steamrot::EventType::EventType_EVENT_USER_INPUT,
    steamrot::UserInputBitset{}
  };
  
  handler.AddEvent(event);
  
  // Event should be in the queue
  REQUIRE(handler.HasEvents());
}

TEST_CASE("EventHandler::ProcessEvents distributes to subscribers",
          "[unit][EventHandler]") {
  
  steamrot::EventHandler handler;
  
  // Create subscriber
  auto subscriber = std::make_shared<steamrot::Subscriber>(
    steamrot::EventType::EventType_EVENT_USER_INPUT
  );
  
  handler.AddSubscriber(subscriber);
  
  // Add event
  steamrot::EventPacket event{
    steamrot::EventType::EventType_EVENT_USER_INPUT,
    steamrot::UserInputBitset{}
  };
  handler.AddEvent(event);
  
  // Process events
  handler.ProcessEvents();
  
  // Subscriber should have received event
  REQUIRE(subscriber->HasEvents());
  REQUIRE(subscriber->GetEvents().size() == 1);
}
```

### Current Status

| Subsystem | Classes | Tested | Location |
|-----------|---------|--------|----------|
| entity | EntityManager, ArchetypeManager | ✅ Tested | `tests/unit/entity/` |
| assets | AssetManager | ✅ Tested | `tests/unit/assets/` |
| events | EventHandler | ✅ Tested | `tests/unit/events/` |
| display | DisplayManager | ✅ Tested | `tests/unit/display/` |
| display | Tile, ReTile, Session | ❌ Not tested | Need to create |
| scenes | SceneManager, SceneFactory | ❌ Not tested | Need to create |

**Next steps**: Complete testing for display utilities and scene management.

## Layer 4: Logic Classes

**Directory**: `src/logic/`

**What to test**: Logic classes that implement game system behavior.

### Understanding Logic Classes

Logic classes:
- Inherit from `Logic` abstract base class
- Implement `ProcessLogic()` protected method
- Process entities grouped by archetype
- Are organized by type: Action, Render, Collision, Movement
- Use `SceneContext` for accessing managers and resources

### Classes to Test

Current Logic implementations:
- ✅ `UIRenderLogic` - Rendering UI elements
- ✅ `UIActionLogic` - Processing UI input actions
- ✅ `UICollisionLogic` - UI element collision detection
- ✅ `UIStateLogic` - UI state management
- ✅ `CraftingRenderLogic` - Crafting scene rendering
- ✅ `LogicFactory` - Creating logic instances per scene

### Logic Test Pattern

Logic classes should test:
1. **Construction** - Can be constructed with valid SceneContext
2. **ProcessLogic execution** - RunLogic() executes without errors
3. **Entity processing** - Correct entities are processed by archetype
4. **State changes** - Logic produces expected state changes
5. **Subscriber handling** - Subscribes to correct events

### Example: Testing a Logic Class

**File**: `tests/unit/logic/UIRenderLogic.test.cpp`

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
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UIRenderLogic constructs correctly", "[unit][UIRenderLogic]") {
  steamrot::tests::TestFixture fixture;
  
  steamrot::UIRenderLogic logic(fixture.GetSceneContext());
  
  REQUIRE_NOTHROW(logic);
}

TEST_CASE("UIRenderLogic::RunLogic executes without errors",
          "[unit][UIRenderLogic]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::UIRenderLogic logic(fixture.GetSceneContext());
  
  // Should execute without throwing
  REQUIRE_NOTHROW(logic.RunLogic());
}

TEST_CASE("UIRenderLogic renders visible UI elements",
          "[unit][UIRenderLogic]") {
  
  steamrot::tests::TestFixture fixture;
  
  // Set up entity with UI component
  auto& pool = fixture.GetEntityManager().GetEntityMemoryPool();
  auto& ui = steamrot::GetComponent<steamrot::CUserInterface>(0, pool);
  ui.m_active = true;
  ui.m_start_visible = true;
  
  // Create simple UI element
  auto panel = std::make_shared<steamrot::PanelElement>();
  panel->m_position = sf::Vector2f(100, 100);
  panel->m_size = sf::Vector2f(50, 50);
  ui.m_root_ui_element = panel;
  
  // Create logic and run
  steamrot::UIRenderLogic logic(fixture.GetSceneContext());
  REQUIRE_NOTHROW(logic.RunLogic());
  
  // UI element should have been drawn to scene texture
  // (In practice, would verify render texture contents)
}
```

### Testing Logic with Archetypes

```cpp
TEST_CASE("UIActionLogic processes only entities with UI component",
          "[unit][UIActionLogic]") {
  
  steamrot::tests::TestFixture fixture;
  
  // Create entity WITH UI component (should be processed)
  auto& pool = fixture.GetEntityManager().GetEntityMemoryPool();
  auto& ui0 = steamrot::GetComponent<steamrot::CUserInterface>(0, pool);
  ui0.m_active = true;
  
  // Create entity WITHOUT UI component (should NOT be processed)
  auto& meta1 = steamrot::GetComponent<steamrot::CMeta>(1, pool);
  meta1.m_active = true;
  // Don't activate UI component for entity 1
  
  // Update archetypes
  fixture.GetEntityManager().UpdateArchetypes();
  
  // Create logic and run
  steamrot::UIActionLogic logic(fixture.GetSceneContext());
  REQUIRE_NOTHROW(logic.RunLogic());
  
  // Verify only entity 0 was processed
  // (Would check state changes specific to UIActionLogic)
}
```

### Testing LogicFactory

**File**: `tests/unit/logic/LogicFactory.test.cpp`

```cpp
TEST_CASE("LogicFactory creates correct Logic for TitleScene",
          "[unit][LogicFactory]") {
  
  steamrot::tests::TestFixture fixture(steamrot::SceneType_TITLE);
  
  steamrot::LogicFactory factory(
    steamrot::SceneType_TITLE,
    fixture.GetSceneContext()
  );
  
  auto logic_map_result = factory.CreateLogicMap();
  REQUIRE(logic_map_result.has_value());
  
  const auto& logic_map = logic_map_result.value();
  
  // Verify expected logic types exist
  REQUIRE(logic_map.contains(steamrot::LogicType::Action));
  REQUIRE(logic_map.contains(steamrot::LogicType::Render));
  REQUIRE(logic_map.contains(steamrot::LogicType::Collision));
  
  // Verify correct number of logic instances per type
  REQUIRE(logic_map.at(steamrot::LogicType::Render).size() > 0);
}

TEST_CASE("LogicFactory creates different logic for different scenes",
          "[unit][LogicFactory]") {
  
  // Create factory for Title scene
  steamrot::tests::TestFixture fixture_title(steamrot::SceneType_TITLE);
  steamrot::LogicFactory factory_title(
    steamrot::SceneType_TITLE,
    fixture_title.GetSceneContext()
  );
  
  // Create factory for Crafting scene
  steamrot::tests::TestFixture fixture_crafting(steamrot::SceneType_CRAFTING);
  steamrot::LogicFactory factory_crafting(
    steamrot::SceneType_CRAFTING,
    fixture_crafting.GetSceneContext()
  );
  
  auto title_logic = factory_title.CreateLogicMap();
  auto crafting_logic = factory_crafting.CreateLogicMap();
  
  REQUIRE(title_logic.has_value());
  REQUIRE(crafting_logic.has_value());
  
  // Different scenes may have different logic counts
  // (Specific assertions depend on scene configurations)
}
```

### Current Status

| Logic Class | Test File | Status | Notes |
|-------------|-----------|--------|-------|
| Logic (base) | `Logic.test.cpp` | ✅ Complete | Tests base class functionality |
| UIRenderLogic | Embedded in `logic_render.test.cpp` | ✅ Complete | Comprehensive tests |
| UIActionLogic | Need to extract | ⚠️ Partial | Embedded in larger tests |
| UICollisionLogic | `collision.test.cpp` | ✅ Complete | Collision detection tests |
| UIStateLogic | Need to create | ❌ Missing | Should be tested |
| CraftingRenderLogic | Need to extract | ⚠️ Partial | Embedded in larger tests |
| LogicFactory | `LogicFactory.test.cpp` | ✅ Complete | Factory pattern tests |

**Next steps**: Create dedicated test files for each Logic class, extract from larger test files.

## Layer 5: Scenes

**Directory**: `src/scenes/`

**What to test**: Scene classes that coordinate game states.

### Understanding Scenes

Scenes:
- Inherit from `Scene` abstract base class
- Implement `sMovement()`, `sCollision()`, `sAction()`, `sRender()` methods
- Contain EntityManager, ArchetypeManager, LogicFactory
- Hold scene-specific resources and state
- Coordinate logic execution for their system

### Classes to Test

Scene implementations:
- ❌ `TitleScene` - Main menu scene
- ❌ `CraftingScene` - Crafting gameplay scene
- ❌ `SceneFactory` - Scene creation and configuration
- ❌ `SceneManager` - Scene lifecycle and transitions

### Scene Test Pattern

Scenes should test:
1. **Construction** - Can be constructed with GameContext
2. **Initialization** - Loads data and creates entities correctly
3. **System execution** - Each system method (sRender, sAction, etc.) executes
4. **Entity management** - Entities are created and configured
5. **Logic coordination** - Logic classes are created and executed
6. **State transitions** - Scene active state changes correctly

### Example: Testing a Scene

**File**: `tests/unit/scenes/TitleScene.test.cpp` (to be created)

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

TEST_CASE("TitleScene constructs correctly", "[unit][TitleScene]") {
  steamrot::tests::TestFixture fixture;
  
  steamrot::TitleScene scene(fixture.GetGameContext());
  
  REQUIRE_NOTHROW(scene);
}

TEST_CASE("TitleScene initializes with correct scene type",
          "[unit][TitleScene]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::TitleScene scene(fixture.GetGameContext());
  
  const auto& scene_info = scene.GetSceneInfo();
  REQUIRE(scene_info.scene_type == steamrot::SceneType_TITLE);
}

TEST_CASE("TitleScene has valid render texture", "[unit][TitleScene]") {
  steamrot::tests::TestFixture fixture;
  steamrot::TitleScene scene(fixture.GetGameContext());
  
  auto& render_texture = scene.GetRenderTexture();
  
  // Render texture should be created with valid size
  REQUIRE(render_texture.getSize().x > 0);
  REQUIRE(render_texture.getSize().y > 0);
}

TEST_CASE("TitleScene::sRender executes without errors",
          "[unit][TitleScene]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::TitleScene scene(fixture.GetGameContext());
  
  REQUIRE_NOTHROW(scene.sRender());
}

TEST_CASE("TitleScene::sAction executes without errors",
          "[unit][TitleScene]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::TitleScene scene(fixture.GetGameContext());
  
  REQUIRE_NOTHROW(scene.sAction());
}

TEST_CASE("TitleScene::sCollision executes without errors",
          "[unit][TitleScene]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::TitleScene scene(fixture.GetGameContext());
  
  REQUIRE_NOTHROW(scene.sCollision());
}

TEST_CASE("TitleScene::sMovement executes without errors",
          "[unit][TitleScene]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::TitleScene scene(fixture.GetGameContext());
  
  REQUIRE_NOTHROW(scene.sMovement());
}

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
```

### Testing SceneFactory

**File**: `tests/unit/scenes/SceneFactory.test.cpp` (to be created)

```cpp
TEST_CASE("SceneFactory creates TitleScene", "[unit][SceneFactory]") {
  steamrot::tests::TestFixture fixture;
  
  auto scene = steamrot::SceneFactory::CreateScene(
    steamrot::SceneType_TITLE,
    fixture.GetGameContext()
  );
  
  REQUIRE(scene != nullptr);
  
  const auto& scene_info = scene->GetSceneInfo();
  REQUIRE(scene_info.scene_type == steamrot::SceneType_TITLE);
}

TEST_CASE("SceneFactory creates CraftingScene", "[unit][SceneFactory]") {
  steamrot::tests::TestFixture fixture;
  
  auto scene = steamrot::SceneFactory::CreateScene(
    steamrot::SceneType_CRAFTING,
    fixture.GetGameContext()
  );
  
  REQUIRE(scene != nullptr);
  
  const auto& scene_info = scene->GetSceneInfo();
  REQUIRE(scene_info.scene_type == steamrot::SceneType_CRAFTING);
}

TEST_CASE("SceneFactory returns error for invalid scene type",
          "[unit][SceneFactory]") {
  
  steamrot::tests::TestFixture fixture;
  
  auto result = steamrot::SceneFactory::CreateScene(
    static_cast<steamrot::SceneType>(999),  // Invalid type
    fixture.GetGameContext()
  );
  
  REQUIRE_FALSE(result.has_value());
}
```

### Testing SceneManager

**File**: `tests/unit/scenes/SceneManager.test.cpp` (to be created)

```cpp
TEST_CASE("SceneManager constructs correctly", "[unit][SceneManager]") {
  steamrot::tests::TestFixture fixture;
  
  steamrot::SceneManager manager(fixture.GetGameContext());
  
  REQUIRE_NOTHROW(manager);
}

TEST_CASE("SceneManager can add scenes", "[unit][SceneManager]") {
  steamrot::tests::TestFixture fixture;
  steamrot::SceneManager manager(fixture.GetGameContext());
  
  auto scene = steamrot::SceneFactory::CreateScene(
    steamrot::SceneType_TITLE,
    fixture.GetGameContext()
  );
  
  manager.AddScene(std::move(scene));
  
  // Manager should have one scene
  REQUIRE(manager.GetSceneCount() == 1);
}

TEST_CASE("SceneManager::ChangeScene switches active scene",
          "[unit][SceneManager]") {
  
  steamrot::tests::TestFixture fixture;
  steamrot::SceneManager manager(fixture.GetGameContext());
  
  // Add two scenes
  auto title_scene = steamrot::SceneFactory::CreateScene(
    steamrot::SceneType_TITLE,
    fixture.GetGameContext()
  );
  auto crafting_scene = steamrot::SceneFactory::CreateScene(
    steamrot::SceneType_CRAFTING,
    fixture.GetGameContext()
  );
  
  manager.AddScene(std::move(title_scene));
  manager.AddScene(std::move(crafting_scene));
  
  // Change to crafting scene
  auto result = manager.ChangeScene(steamrot::SceneType_CRAFTING);
  REQUIRE(result.has_value());
  
  // Verify active scene changed
  const auto& active_scene = manager.GetActiveScene();
  REQUIRE(active_scene.GetSceneInfo().scene_type == steamrot::SceneType_CRAFTING);
}
```

### Current Status

| Scene Class | Test File | Status | Notes |
|-------------|-----------|--------|-------|
| TitleScene | Not created | ❌ Missing | High priority |
| CraftingScene | Not created | ❌ Missing | High priority |
| SceneFactory | Not created | ❌ Missing | Medium priority |
| SceneManager | Not created | ❌ Missing | Medium priority |

**Next steps**: Create test files for all Scene classes. This is currently the biggest gap in test coverage.

## Layer 6: Engine

**Directory**: `src/engine/`

**What to test**: Top-level engine coordination and game loop.

### Understanding Engine Layer

The engine layer:
- Coordinates all systems (scenes, display, input)
- Manages the game loop
- Handles top-level error recovery
- Provides entry point to the application

### Classes to Test

- ✅ `GameEngine` - Main game engine coordination (basic test exists)
- ❌ `Engine` (abstract) - Not tested (abstract class)

### Engine Test Pattern

Engine should test:
1. **Construction** - Can be constructed and initialized
2. **Initialization** - All subsystems initialize correctly
3. **Configuration loading** - Loads engine configuration
4. **Scene management** - Can create and switch scenes
5. **Resource initialization** - AssetManager, DisplayManager setup

### Example: Testing GameEngine

**File**: `tests/unit/engine/GameEngine.test.cpp`

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

TEST_CASE("GameEngine constructs correctly", "[unit][GameEngine]") {
  steamrot::GameEngine engine;
  REQUIRE_NOTHROW(engine);
}

TEST_CASE("GameEngine initializes subsystems", "[unit][GameEngine]") {
  steamrot::GameEngine engine;
  
  auto result = engine.Initialize();
  
  if (!result.has_value()) {
    FAIL("Engine initialization failed: " + result.error().message);
  }
  
  REQUIRE(result.has_value());
}

TEST_CASE("GameEngine loads configuration correctly",
          "[unit][GameEngine]") {
  
  steamrot::GameEngine engine;
  engine.Initialize();
  
  // Verify configuration was loaded
  const auto& config = engine.GetEngineConfig();
  
  REQUIRE(config.window_width > 0);
  REQUIRE(config.window_height > 0);
  REQUIRE(!config.window_title.empty());
}

TEST_CASE("GameEngine creates initial scene", "[unit][GameEngine]") {
  steamrot::GameEngine engine;
  engine.Initialize();
  
  // Engine should have created an initial scene
  const auto* scene_manager = engine.GetSceneManager();
  REQUIRE(scene_manager != nullptr);
  
  // Should have at least one scene
  REQUIRE(scene_manager->GetSceneCount() > 0);
}
```

### Testing Engine Error Handling

```cpp
TEST_CASE("GameEngine handles missing configuration gracefully",
          "[unit][GameEngine]") {
  
  // Create engine with invalid config path
  steamrot::GameEngine engine("/invalid/path/config.json");
  
  auto result = engine.Initialize();
  
  // Should fail gracefully with error information
  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().fail_mode == steamrot::FailMode::FileNotFound);
}

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

### Current Status

| Engine Class | Test File | Status | Notes |
|--------------|-----------|--------|-------|
| GameEngine | `GameEngine.test.cpp` | ⚠️ Basic | Exists but minimal coverage |
| Engine (abstract) | N/A | N/A | Abstract class - no test needed |

**Next steps**: Expand GameEngine tests to cover initialization, configuration, and error handling.

## Test Infrastructure

### TestFixture

**Location**: `tests/context/TestFixture.h`, `TestFixture.cpp`

The `TestFixture` class provides a standard test setup with:
- Pre-configured EntityManager
- Pre-configured AssetManager
- SceneContext for Logic testing
- GameContext for Scene testing

**Usage**:

```cpp
#include "TestFixture.h"

TEST_CASE("Example test with fixture", "[unit][Example]") {
  steamrot::tests::TestFixture fixture;
  
  // Get managers
  auto& entity_manager = fixture.GetEntityManager();
  auto& asset_manager = fixture.GetAssetManager();
  
  // Get contexts
  auto scene_context = fixture.GetSceneContext();
  auto game_context = fixture.GetGameContext();
  
  // Use in test...
}
```

**Scene-specific fixtures**:

```cpp
// Create fixture for specific scene type
steamrot::tests::TestFixture fixture(steamrot::SceneType_TITLE);
```

### Test Data Helpers

**Location**: `tests/unit/*/data/`

Each subsystem has test data helpers:
- `asset_test_helpers.h/cpp` - Asset validation helpers
- `logic_test_helpers.h/cpp` - Logic validation helpers
- `TestUIElementDataProvider.h` - UI element test data

### Test Matchers

**Location**: `tests/matchers/`

Custom Catch2 matchers for complex comparisons:
- `EntityMemoryPoolEqualsMatcher` - Compare entity pools
- `CUserInterfaceEqualsMatcher` - Compare UI components
- `EventBusEqualsMatcher` - Compare event buses
- `SubscriberEqualsMatcher` - Compare subscribers

### Test Scenarios

**Location**: `tests/context/test_scenarios.h/cpp`

Pre-built test scenarios:
- `CreateEmptyPool()` - Empty entity pool
- `CreatePoolWithNEntities(n)` - Pool with n entities
- `CreatePopulatedArchetypeManager()` - Pre-configured archetypes

## Best Practices

### 1. Test Organization

✅ **DO**:
- Mirror `src/` directory structure in `tests/unit/`
- One test file per class: `ClassName.test.cpp`
- Group related tests with descriptive TEST_CASE names
- Use consistent test tags: `[unit][ClassName]`

❌ **DON'T**:
- Mix unit tests with integration tests
- Test multiple unrelated classes in one file
- Use vague test names like "test1", "test2"

### 2. Test Independence

✅ **DO**:
- Each test should be independent
- Use TestFixture for clean setup
- Reset state between tests
- Use SECTION for related sub-tests

❌ **DON'T**:
- Rely on test execution order
- Share state between tests
- Modify global state without cleanup

### 3. Assertions

✅ **DO**:
- Use descriptive assertions: `REQUIRE(result.has_value())`
- Check error conditions explicitly
- Use `REQUIRE_NOTHROW` for operations that shouldn't throw
- Fail tests with informative messages: `FAIL("Reason")`

❌ **DON'T**:
- Use bare `REQUIRE(true)` without meaning
- Ignore return values
- Catch and swallow exceptions in tests

### 4. Test Data

✅ **DO**:
- Use test data files in `tests/data/`
- Create minimal test data for each test
- Use TestFixture for standard setups
- Document test data expectations

❌ **DON'T**:
- Hardcode large data structures in tests
- Rely on production data files
- Create overly complex test scenarios

### 5. Coverage Goals

Aim for testing:
- ✅ All public methods
- ✅ Constructor and destructor
- ✅ Error conditions and edge cases
- ✅ State transitions
- ✅ Null/invalid input handling

Don't need to test:
- ❌ Private methods (test through public API)
- ❌ Getters/setters (unless they have logic)
- ❌ Abstract interfaces (test concrete implementations)

## Common Patterns

### Pattern 1: Testing std::expected Return Values

```cpp
TEST_CASE("Function returns expected on success", "[unit][Class]") {
  auto result = function_that_returns_expected();
  
  // Check success
  REQUIRE(result.has_value());
  
  // Use value
  auto value = result.value();
  REQUIRE(value == expected_value);
}

TEST_CASE("Function returns error on failure", "[unit][Class]") {
  auto result = function_that_should_fail();
  
  // Check failure
  REQUIRE_FALSE(result.has_value());
  
  // Check error details
  REQUIRE(result.error().fail_mode == FailMode::InvalidInput);
  REQUIRE(!result.error().message.empty());
}
```

### Pattern 2: Testing with FlatBuffers Data

```cpp
TEST_CASE("Configurator handles null FlatBuffers fields", "[unit][Configurator]") {
  // Create FlatBuffers data with null fields
  flatbuffers::FlatBufferBuilder builder;
  
  auto entity_data = CreateEntityData(
    builder,
    0,           // index
    nullptr,     // null component - should be handled gracefully
    ...
  );
  
  builder.Finish(entity_data);
  
  // Test configuration
  auto result = configurator.Configure(builder.GetBufferPointer());
  
  // Should succeed even with null fields
  REQUIRE(result.has_value());
}
```

### Pattern 3: Testing Entity Processing

```cpp
TEST_CASE("Logic processes entities with correct archetype", "[unit][Logic]") {
  steamrot::tests::TestFixture fixture;
  
  // Create entity with specific components
  auto& pool = fixture.GetEntityManager().GetEntityMemoryPool();
  
  auto& comp1 = steamrot::GetComponent<Component1>(0, pool);
  comp1.m_active = true;
  
  auto& comp2 = steamrot::GetComponent<Component2>(0, pool);
  comp2.m_active = true;
  
  // Update archetypes
  fixture.GetEntityManager().UpdateArchetypes();
  
  // Create and run logic
  MyLogic logic(fixture.GetSceneContext());
  REQUIRE_NOTHROW(logic.RunLogic());
  
  // Verify expected state changes
  REQUIRE(comp1.m_some_field == expected_value);
}
```

### Pattern 4: Testing Resource Loading

```cpp
TEST_CASE("Manager loads resources successfully", "[unit][Manager]") {
  Manager manager;
  
  auto result = manager.LoadResources();
  
  REQUIRE(result.has_value());
  
  // Verify resources were loaded
  REQUIRE(manager.GetResourceCount() > 0);
  
  // Verify specific resources
  auto resource = manager.GetResource("resource_name");
  REQUIRE(resource != nullptr);
}
```

### Pattern 5: Testing Event Handling

```cpp
TEST_CASE("Handler processes events correctly", "[unit][EventHandler]") {
  EventHandler handler;
  
  // Create subscriber
  auto subscriber = std::make_shared<Subscriber>(EventType::Type1);
  handler.AddSubscriber(subscriber);
  
  // Add event
  EventPacket event{EventType::Type1, {}};
  handler.AddEvent(event);
  
  // Process
  handler.ProcessEvents();
  
  // Verify subscriber received event
  REQUIRE(subscriber->HasEvents());
  REQUIRE(subscriber->GetEvents().size() == 1);
  REQUIRE(subscriber->GetEvents()[0].event_type == EventType::Type1);
}
```

## Summary

This guide provides a comprehensive approach to implementing unit tests for the SteamRot game engine by following architectural layers:

1. **Layer 0**: Core types and utilities (foundation)
2. **Layer 1**: Components (pure data)
3. **Layer 2**: Data providers and configurators (data loading)
4. **Layer 3**: Managers (resource management)
5. **Layer 4**: Logic classes (game systems)
6. **Layer 5**: Scenes (game states)
7. **Layer 6**: Engine (top-level coordination)

### Current Coverage Summary

| Layer | Components | Coverage | Priority |
|-------|------------|----------|----------|
| Layer 0 | Types, Logger | ⚠️ Partial | Medium |
| Layer 1 | Components | ✅ Good | Low |
| Layer 2 | Data/Config | ✅ Good | Low |
| Layer 3 | Managers | ⚠️ Partial | High |
| Layer 4 | Logic | ✅ Good | Low |
| Layer 5 | Scenes | ❌ Missing | **Critical** |
| Layer 6 | Engine | ⚠️ Basic | High |

### Next Steps

**Immediate priorities**:
1. **Create Scene tests** (Layer 5) - Biggest gap
2. **Expand Manager tests** (Layer 3) - Display utilities, SceneManager
3. **Improve Engine tests** (Layer 6) - Expand coverage
4. **Fill Type tests** (Layer 0) - UI Element types, core types

**Long-term**:
- Add integration tests for cross-layer interactions
- Add performance/benchmark tests
- Implement continuous testing in CI/CD

### Resources

- **Catch2 Documentation**: https://github.com/catchorg/Catch2/tree/devel/docs
- **Test-Driven Development**: https://en.wikipedia.org/wiki/Test-driven_development
- **Google C++ Style Guide**: https://google.github.io/styleguide/cppguide.html
- **SteamRot README**: `README.md`
- **Copilot Instructions**: `.github/copilot-instructions.md`

---

**Document Version**: 1.0  
**Last Updated**: 2025-12-17  
**Maintainer**: SteamRot Development Team
