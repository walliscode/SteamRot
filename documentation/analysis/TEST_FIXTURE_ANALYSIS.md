# TestFixture Usage Analysis

## Overview

This document analyzes all tests that use `TestFixture` and evaluates whether they can be migrated to the data-driven testing system (TestEngine + test_harness) or need a reimplementation of TestFixture.

## Summary

| Test File | Test Count | Migration Status | Recommendation |
|-----------|------------|------------------|----------------|
| Logic.test.cpp | 4 | Needs TestFixture | Reimplement TestFixture |
| LogicFactory.test.cpp | 8 | Needs TestFixture | Reimplement TestFixture |
| UIElementFactory.test.cpp | 13 | Needs TestFixture | Reimplement TestFixture |
| SceneFactory.test.cpp | 3 | Needs TestFixture | Reimplement TestFixture |
| SceneManager.test.cpp | 19 | Needs TestFixture | Reimplement TestFixture |
| TitleScene.test.cpp | 1 | Needs TestFixture | Reimplement TestFixture (visual test) |
| CraftingScene.test.cpp | 1 | Needs TestFixture | Reimplement TestFixture (visual test) |
| DisplayManager.test.cpp | 2 | Currently commented out | Needs TestFixture when enabled |
| GameEngine.test.cpp | 11 | Partial migration possible | Mixed approach |
| ArchetypeManager.test.cpp | 3 | Currently commented out | Could use data-driven when enabled |
| EntityManager.test.cpp | 1 | Currently commented out | Could use data-driven when enabled |

---

## Detailed Analysis

### 1. Logic.test.cpp

**Tests:**
1. `Logic::GetSubscribers returns empty vector initially`
2. `Logic::AddSubscriber adds a subscriber to the vector`
3. `Logic::AddSubscriber can add multiple subscribers`
4. `Logic::RunLogic can be called on Logic with subscribers`

**TestFixture Usage:**
- Creates `steamrot::tests::TestFixture test_context;`
- Uses `test_context.GetSceneContext()` to create Logic instances
- Tests subscriber management functionality

**Migration Analysis:**
- ❌ **Cannot migrate to data-driven system**
- These tests verify Logic class API behavior (subscriber management)
- The data-driven system is designed for entity state comparison, not Logic class method testing
- Tests need a SceneContext with mock dependencies

**Recommendation:** **Reimplement TestFixture**
- TestFixture should provide a mock SceneContext
- The tests are unit tests for Logic class methods, not entity state transformations

---

### 2. LogicFactory.test.cpp

**Tests:**
1. `LogicFactory constructed without errors`
2. `LogicFactory creates the correct Logic instances with a test Scene`
3. `LogicFactory creates correct Logic instances for TitleScene`
4. `LogicFactory creates correct Logic instances for CraftingScene`
5. `LogicFactory attaches subscribers to Logic instances`
6. `LogicFactory attaches subscribers from LogicData`
7. `LogicFactory creates Logic instances without LogicCollectionData`
8. `LogicFactory works for all scene types without LogicCollectionData`

**TestFixture Usage:**
- Creates `steamrot::tests::TestFixture test_context{SceneType}` with different scene types
- Uses `test_context.GetSceneContext()` to construct LogicFactory
- Validates LogicCollection creation for different scenes

**Migration Analysis:**
- ❌ **Cannot migrate to data-driven system**
- Tests verify LogicFactory correctly creates Logic class instances based on scene type
- Uses `CheckStaticLogicCollections()` to validate correct Logic types and order
- This is API/factory behavior testing, not entity state testing

**Recommendation:** **Reimplement TestFixture**
- TestFixture should provide mock GameContext/SceneContext for different scene types
- These tests validate factory patterns, not entity transformations

---

### 3. UIElementFactory.test.cpp

**Tests:**
1. `ConfigurePanelElement` - Creates FlatBuffers data and configures element
2. `CreateUIElement - Panel` - Tests factory creation with EventHandler
3. `ConfigureButtonElement` - Tests button configuration
4. `CreateUIElement - Button` - Tests button creation
5. `ConfigureDropDownListElement` - Tests dropdown list configuration
6. `CreateUIElement - DropDownList` - Tests dropdown list creation
7. `ConfigureDropDownContainerElement` - Tests container configuration
8. `CreateUIElement - DropDownContainer` - Tests container creation
9. `ConfigureDropDownItemElement` - Tests item configuration
10. `CreateUIElement - DropDownItem` - Tests item creation
11. `ConfigureDropDownButtonElement` - Tests dropdown button configuration
12. `CreateUIElement - DropDownButton` - Tests dropdown button creation
13. `CreateUIElement - Deeply Nested Panel` - Tests nested UI element creation

**TestFixture Usage:**
- Creates `steamrot::tests::TestFixture test_context;`
- Uses `test_context.GetGameContext().event_handler` for subscriber registration
- Validates that EventHandler registers subscribers correctly during element creation

**Migration Analysis:**
- ❌ **Cannot migrate to data-driven system**
- Tests UIElementFactory configuration and creation from FlatBuffers data
- Tests EventHandler subscriber registration as a side effect
- These are factory/configuration tests, not entity state comparisons

**Recommendation:** **Reimplement TestFixture**
- TestFixture should provide mock EventHandler that can be inspected
- Tests verify factory behavior and subscriber registration

---

### 4. SceneFactory.test.cpp

**Tests:**
1. `SceneFactory can be constructed without errors`
2. `SceneFactory can create a TitleScene from default`
3. `SceneFactory can create a CraftingScene from default`

**TestFixture Usage:**
- Creates `steamrot::tests::TestFixture test_fixture;`
- Calls `test_fixture.Initialize()` to set up resources
- Uses `test_fixture.GetGameContext()` for scene creation
- Validates that created scenes have correct type and configuration

**Migration Analysis:**
- ❌ **Cannot migrate to data-driven system**
- Tests SceneFactory creates correct Scene types
- Validates scene configuration using `CheckDefaultSceneConfiguration()`
- These are factory pattern tests, not entity state tests

**Recommendation:** **Reimplement TestFixture**
- TestFixture should initialize game resources (window, event handler, etc.)
- Tests verify factory creates correctly typed and configured scenes

---

### 5. SceneManager.test.cpp

**Tests:**
1. `SceneManager is constructed without any errors`
2. `SceneManager::RegistersSubscriber adds a subscriber`
3. `SceneManager::RegistersSubscriber fails to add duplicate subscriber`
4. `SceneManager::RegistersSubscriber fails to add null subscriber`
5. `SceneManager::ConfigureSubscribersFromData configures subscribers`
6. `SceneManager::ConfigureSubscribersFromData fails on null data`
7. `SceneManager::ConfigureSceneManagerFromData configures without errors`
8. `SceneManager's AddSceneFromDefault creates a configured TitleScene`
9. `SceneManager's AddSceneFromDefault creates a configured CraftingScene`
10. `SceneManager LoadTitleScene returns monostate`
11. `SceneManager LoadCraftingScene returns monostate`
12. `SceneManager::ProvideTextures returns empty map for no scene IDs`
13. `SceneManager::ProvideTextures returns FailInfo for invalid scene ID`
14. `SceneManager::ProvideTextures returns textures for valid scene IDs`
15. `SceneManager::ProvideAvailableSceneInfo returns available SceneInfo`
16. `SceneManager loads TitleScene when Subscriber is turned active`
17. `SceneManager loads CraftingScene when Subscriber is turned active`
18. `SceneManager::UpdateSceneManager cause scene change via subscribers`
19. `SceneManager processes Subscriber and sets it to inactive`

**TestFixture Usage:**
- Creates `steamrot::tests::TestFixture test_context;`
- Uses `test_context.GetGameContext()` for SceneManager construction
- Some tests call `test_context.Initialize()` for full resource initialization
- Tests subscriber management, scene loading, and scene change events

**Migration Analysis:**
- ⚠️ **Partially migratable** - Some tests could use data-driven approach
- Tests 16-19 involve event-driven scene changes which could potentially use the event sequence system
- However, most tests are API validation tests for SceneManager methods

**Recommendation:** **Reimplement TestFixture** (primary)
- Most tests validate SceneManager API behavior (registration, loading, etc.)
- Scene change tests (16-19) could potentially use `event_sequence` + `simulation_data` but would require significant effort
- Better to reimplement TestFixture and keep tests as-is

---

### 6. TitleScene.test.cpp

**Tests:**
1. `TitleScene's call of sRender is correct` (tagged `[.visual]`)

**TestFixture Usage:**
- Creates `steamrot::tests::TestFixture test_fixture;`
- Calls `test_fixture.Initialize()` for resource setup
- Uses `test_fixture.GetGameContext()` for scene creation
- Calls `DisplayRenderTexture()` for visual verification

**Migration Analysis:**
- ❌ **Cannot migrate to data-driven system**
- Visual test requiring human verification
- Tests scene rendering output, not entity states

**Recommendation:** **Reimplement TestFixture**
- TestFixture should provide initialized GameContext with mock resources
- Visual tests are inherently not automatable

---

### 7. CraftingScene.test.cpp

**Tests:**
1. `CraftingScene's call to sRender is correct` (tagged `[.visual]`)

**TestFixture Usage:**
- Same pattern as TitleScene.test.cpp
- Visual verification test for rendering

**Migration Analysis:**
- ❌ **Cannot migrate to data-driven system**
- Same reasons as TitleScene

**Recommendation:** **Reimplement TestFixture**

---

### 8. DisplayManager.test.cpp

**Tests:**
1. `DisplayManager Initializes with SceneManager` (commented out)
2. `DisplayManager Render Cycle` (commented out)

**TestFixture Usage:**
- Currently commented out but would use TestFixture
- Would use `test_context.GetGameContext()` for window and scene manager

**Migration Analysis:**
- ❌ **Cannot migrate to data-driven system**
- Display tests require actual window/rendering infrastructure
- Tests visual/display components

**Recommendation:** **Reimplement TestFixture** (when enabled)

---

### 9. GameEngine.test.cpp

**Tests:**
1. `GameEngine initializes correctly` - No TestFixture
2. `GameEngine runs for a set number of frames` - No TestFixture
3. `GameEngine::RegisterSubscriber adds a subscriber` - No TestFixture
4. `GameEngine::RegisterSubscriber fails to add null subscriber` - No TestFixture
5. `GameEngine::ConfigureSubscribersFromData fails on null data` - No TestFixture
6. `GameEngine::ConfigureSubscribersFromData configures subscribers` - No TestFixture
7. `GameEngine::ConfigureGameEngineFromData configures without errors` - No TestFixture
8. `GameEngine::ProcessSubscribers quits game when correct Subscriber is active` - No TestFixture
9. `GameEngine::RunGameLoop processes subscribers and quits game` - No TestFixture
10. `GameEngine::ProcessSubscriptions does not quit if another subscriber type is present` - No TestFixture
11. `GameEngine::UpdateGameContext updates members of GameContext correctly` - **Uses TestFixture**

**TestFixture Usage:**
- Only test 11 uses TestFixture
- Uses `test_fixture.GetGameContext()` to get reference to GameContext
- Tests that `UpdateGameResources()` correctly updates mouse position

**Migration Analysis:**
- ❌ **Cannot migrate to data-driven system** (for test 11)
- Tests GameEngine API behavior for updating GameContext
- Tests mouse position update which requires window interaction

**Recommendation:** **Reimplement TestFixture** (for test 11 only)
- Most GameEngine tests don't need TestFixture
- Test 11 needs mock window to test mouse position updates

---

### 10. ArchetypeManager.test.cpp

**Tests:**
1. `ArchetypeManager is constructed without errors` - No TestFixture (direct EMP creation)
2. `ArchetypeManager archetype map is empty with a non configured EMP` (commented out)
3. `ArchetypeManager generates archetype IDs correctly` (commented out)

**TestFixture Usage:**
- Currently commented out tests reference TestFixture
- Would use `test_context.GetGameContext().event_handler` for EntityManager

**Migration Analysis:**
- ✅ **Could potentially use data-driven system** (when enabled)
- Tests archetype generation from configured EntityMemoryPool
- Data-driven system supports `start_entity_collection` which could configure entities
- `expected_entity_collection` comparison could validate archetype membership

**Recommendation:** **Consider data-driven approach** (when tests are enabled)
- EntityMemoryPool configuration and comparison is what the data-driven system excels at
- Would need to enhance test harness to expose ArchetypeManager state for comparison

---

### 11. EntityManager.test.cpp

**Tests:**
1. `EntityManager calls configurator with no errors` (commented out)

**TestFixture Usage:**
- Currently commented out but references TestFixture
- Would use `test_context.GetGameContext().event_handler` for EntityManager construction

**Migration Analysis:**
- ✅ **Could potentially use data-driven system** (when enabled)
- Tests EntityMemoryPool configuration from default data
- Data-driven system supports entity configuration and comparison

**Recommendation:** **Consider data-driven approach** (when tests are enabled)
- Entity configuration and EMP comparison is a core use case for the data-driven system

---

## What TestFixture Should Provide

Based on the analysis, a reimplemented `TestFixture` should provide:

### Core Functionality

1. **Mock GameContext**
   - Mock SFML window (or headless alternative)
   - EventHandler with working event bus
   - AssetManager (can be empty or with test assets)
   - References to loop_number, mouse_position

2. **Mock SceneContext**
   - EntityMemoryPool (configurable size)
   - ArchetypeManager
   - RenderTexture (SFML)
   - GameContext reference
   - GameResources reference

3. **Scene Type Support**
   - Constructor accepting SceneType for scene-specific configuration
   - `GetGameContext()` method
   - `GetSceneContext()` method
   - `Initialize()` method for full resource setup

### Suggested Interface

```cpp
namespace steamrot::tests {

class TestFixture {
public:
  // Default constructor for TEST scene type
  TestFixture();
  
  // Constructor with specific scene type
  explicit TestFixture(SceneType scene_type);
  
  // Initialize all resources (window, assets, etc.)
  void Initialize();
  
  // Access GameContext for scene construction
  GameContext& GetGameContext();
  
  // Access SceneContext for Logic construction
  SceneContext& GetSceneContext();
  
  // Get the current scene type
  SceneType GetSceneType() const;

private:
  SceneType m_scene_type;
  std::unique_ptr<sf::RenderWindow> m_window;
  std::unique_ptr<EventHandler> m_event_handler;
  std::unique_ptr<AssetManager> m_asset_manager;
  std::unique_ptr<EntityMemoryPool> m_entity_pool;
  std::unique_ptr<ArchetypeManager> m_archetype_manager;
  // ... other mock resources
};

} // namespace steamrot::tests
```

---

## Tests That Could Use Data-Driven System

While most current tests need TestFixture reimplementation, the following test scenarios could potentially benefit from the data-driven system:

### Scenario 1: Entity State Transformation Tests
- Tests that configure entities, run logic, and compare entity states
- Use `start_entity_collection`, `simulation_data`, `expected_entity_collection`

### Scenario 2: Event-Driven Behavior Tests  
- Tests that trigger events and validate resulting entity states
- Use `event_sequence` with tick-based execution

### Scenario 3: Input-Driven Behavior Tests
- Tests that simulate user input and validate entity responses
- Use `input_sequence` with tick-based execution

### Currently Commented Tests That Could Migrate
- `ArchetypeManager` tests (when enabled) - entity configuration + archetype validation
- `EntityManager` tests (when enabled) - entity configuration + EMP comparison

---

## Conclusion

### Tests Requiring TestFixture Reimplementation (High Priority)

1. **Logic.test.cpp** - Needs SceneContext for Logic instantiation
2. **LogicFactory.test.cpp** - Needs SceneContext for LogicFactory
3. **UIElementFactory.test.cpp** - Needs EventHandler for subscriber testing
4. **SceneFactory.test.cpp** - Needs GameContext for scene creation
5. **SceneManager.test.cpp** - Needs GameContext for SceneManager
6. **TitleScene.test.cpp** - Needs GameContext for visual tests
7. **CraftingScene.test.cpp** - Needs GameContext for visual tests
8. **GameEngine.test.cpp** (test 11) - Needs GameContext with window

### Tests That Could Use Data-Driven System (When Enabled)

1. **ArchetypeManager.test.cpp** - Entity configuration and archetype validation
2. **EntityManager.test.cpp** - Entity configuration and EMP comparison

### Recommended Approach

1. **Reimplement TestFixture** with the interface described above
2. **Keep existing tests as-is** - They test API behavior, not entity state transformations
3. **Consider data-driven for future tests** that involve:
   - Entity state configuration and comparison
   - Tick-based simulation execution
   - Event/input sequence testing
