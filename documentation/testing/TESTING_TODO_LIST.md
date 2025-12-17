# Unit Testing To-Do List

This document provides a comprehensive, actionable to-do list for implementing unit tests across all architectural layers. Each item is specific and can be checked off as you complete it.

## How to Use This List

1. **Work from bottom to top** - Start with Layer 0, complete it, then move to Layer 1, etc.
2. **Check off items** as you complete them
3. **Run tests after each file** to ensure it works
4. **Update CMakeLists.txt** after adding each test file
5. **Reference examples** in `TESTING_EXAMPLES.md` for patterns

---

## Layer 0: Core Types & Utilities

**Directory**: `tests/unit/types/` (needs creation)

### Types - Core (`src/types/core/`)

- [ ] **FailInfo.test.cpp**
  - [ ] Test default construction
  - [ ] Test construction with FailMode and message
  - [ ] Test copy construction
  - [ ] Test field access (fail_mode, message)
  - [ ] Add to `tests/unit/types/CMakeLists.txt`

- [ ] **SceneConfig.test.cpp**
  - [ ] Test default construction
  - [ ] Test field initialization
  - [ ] Test entity_pool_size field
  - [ ] Test render_texture_width/height fields
  - [ ] Add to CMakeLists.txt

- [ ] **EngineConfig.test.cpp**
  - [ ] Test default construction
  - [ ] Test window_width/height fields
  - [ ] Test window_title field
  - [ ] Test framerate_limit field
  - [ ] Add to CMakeLists.txt

- [ ] **SceneInfo.test.cpp**
  - [ ] Test default construction
  - [ ] Test scene_type field
  - [ ] Test scene_id field (UUID)
  - [ ] Add to CMakeLists.txt

- [ ] **Joint.test.cpp**
  - [ ] Test default construction
  - [ ] Test field access
  - [ ] Test copy construction
  - [ ] Add to CMakeLists.txt

- [ ] **Fragment.test.cpp**
  - [ ] Test default construction
  - [ ] Test field access
  - [ ] Test copy construction
  - [ ] Add to CMakeLists.txt

### Types - Events (`src/types/events/`)

**Note**: Some tests exist in `tests/unit/events/`, may need to move to `tests/unit/types/events/`

- [ ] **Review existing EventPacket tests** in `tests/unit/events/`
- [ ] **Review existing Subscriber tests** in `tests/unit/events/`
- [ ] **Review existing UserInputBitset tests** in `tests/unit/events/`
- [ ] **Decide**: Keep in events/ or move to types/events/
- [ ] **If moving**: Update CMakeLists.txt references

### Types - User Interface (`src/types/user_interface/`)

**Directory**: `tests/unit/types/user_interface/` (needs creation)

- [ ] **UIElement.test.cpp**
  - [ ] Test default construction (if possible, may be abstract)
  - [ ] Test base fields (m_position, m_size, m_visible)
  - [ ] Add to `tests/unit/types/user_interface/CMakeLists.txt`

- [ ] **ButtonElement.test.cpp**
  - [ ] Test default construction
  - [ ] Test all fields (position, size, text, etc.)
  - [ ] Test copy construction
  - [ ] Add to CMakeLists.txt

- [ ] **PanelElement.test.cpp**
  - [ ] Test default construction
  - [ ] Test panel-specific fields
  - [ ] Test copy construction
  - [ ] Add to CMakeLists.txt

- [ ] **DropDownListElement.test.cpp**
  - [ ] Test default construction
  - [ ] Test dropdown fields
  - [ ] Test item management
  - [ ] Add to CMakeLists.txt

- [ ] **DropDownButtonElement.test.cpp**
  - [ ] Test default construction
  - [ ] Test button fields
  - [ ] Add to CMakeLists.txt

- [ ] **DropDownContainerElement.test.cpp**
  - [ ] Test default construction
  - [ ] Test container fields
  - [ ] Add to CMakeLists.txt

- [ ] **DropDownItemElement.test.cpp**
  - [ ] Test default construction
  - [ ] Test item fields
  - [ ] Add to CMakeLists.txt

### Types - UI Styles (`src/types/user_interface/styles/`)

**Directory**: `tests/unit/types/user_interface/styles/` (needs creation)

- [ ] **ButtonStyle.test.cpp**
  - [ ] Test default construction
  - [ ] Test style fields
  - [ ] Add to `tests/unit/types/user_interface/styles/CMakeLists.txt`

- [ ] **PanelStyle.test.cpp**
  - [ ] Test default construction
  - [ ] Test style fields
  - [ ] Add to CMakeLists.txt

- [ ] **DropDownButtonStyle.test.cpp**
  - [ ] Test default construction
  - [ ] Test style fields
  - [ ] Add to CMakeLists.txt

- [ ] **DropDownContainerStyle.test.cpp**
  - [ ] Test default construction
  - [ ] Test style fields
  - [ ] Add to CMakeLists.txt

- [ ] **DropDownItemStyle.test.cpp**
  - [ ] Test default construction
  - [ ] Test style fields
  - [ ] Add to CMakeLists.txt

- [ ] **DropDownListStyle.test.cpp**
  - [ ] Test default construction
  - [ ] Test style fields
  - [ ] Add to CMakeLists.txt

- [ ] **UIStyle.test.cpp**
  - [ ] Test default construction
  - [ ] Test all optional style fields
  - [ ] Test style access methods
  - [ ] Add to CMakeLists.txt

### Logger (`src/logger/`)

**Directory**: `tests/unit/logger/` (needs creation)

- [ ] **Logger.test.cpp**
  - [ ] Test logger initialization
  - [ ] Test log message writing
  - [ ] Test different log levels
  - [ ] Add to `tests/unit/logger/CMakeLists.txt`

### Layer 0 Infrastructure

- [ ] **Create** `tests/unit/types/CMakeLists.txt`
- [ ] **Create** `tests/unit/types/user_interface/CMakeLists.txt`
- [ ] **Create** `tests/unit/types/user_interface/styles/CMakeLists.txt`
- [ ] **Create** `tests/unit/logger/CMakeLists.txt`
- [ ] **Add subdirectories** to parent CMakeLists.txt

**Layer 0 Estimated Tasks**: 30 test files

---

## Layer 1: Components

**Directory**: `tests/unit/components/` (exists)

### Existing Tests (Verify and Expand)

- [ ] **CMeta.test.cpp** (exists)
  - [x] Default construction ✓
  - [x] Component register index ✓
  - [ ] Test entity_alive state changes
  - [ ] Test interaction with EntityManager

- [ ] **CUserInterface.test.cpp** (exists)
  - [x] Default construction ✓
  - [ ] Test UI element assignment
  - [ ] Test visibility state changes
  - [ ] Test null UI element handling

- [ ] **CGrimoireMachina.test.cpp** (exists)
  - [x] Default construction ✓
  - [ ] Test grimoire-specific fields
  - [ ] Test state management

- [ ] **CMachinaForm.test.cpp** (exists)
  - [x] Default construction ✓
  - [ ] Test form-specific fields
  - [ ] Test form data handling

- [ ] **CUIState.test.cpp** (exists)
  - [x] Default construction ✓
  - [ ] Test UI state transitions
  - [ ] Test state flag management

### Layer 1 Infrastructure

- [ ] **Review** existing `tests/unit/components/CMakeLists.txt`
- [ ] **Ensure** all components are included in test executable
- [ ] **Run** all component tests to verify baseline

**Layer 1 Estimated Tasks**: 5 test file expansions

---

## Layer 2: Data Providers & Configurators

**Directory**: `tests/unit/data_providers/` (exists for providers)

### Data Providers (Verify and Expand)

- [ ] **FlatbuffersDataLoader.test.cpp** (exists)
  - [ ] Verify all load methods tested
  - [ ] Test error handling
  - [ ] Test null safety

- [ ] **FlatbuffersAssetDataProvider.test.cpp** (exists)
  - [x] Basic loading ✓
  - [ ] Test missing file handling
  - [ ] Test invalid data handling

- [ ] **FlatbuffersEngineDataProvider.test.cpp** (exists)
  - [x] Basic loading ✓
  - [ ] Test configuration validation
  - [ ] Test default values

- [ ] **FlatbuffersSceneDataProvider.test.cpp** (needs creation)
  - [ ] Test scene data loading
  - [ ] Test different scene types
  - [ ] Test error conditions
  - [ ] Add to CMakeLists.txt

- [ ] **FlatbuffersSceneManagerDataProvider.test.cpp** (exists)
  - [ ] Verify coverage
  - [ ] Test manager configuration

- [ ] **FlatbuffersSubscriberViewer.test.cpp** (exists)
  - [x] Basic functionality ✓
  - [ ] Test subscriber iteration
  - [ ] Test event type filtering

- [ ] **FlatbuffersUIStyleDataProvider.test.cpp** (needs creation)
  - [ ] Test UI style loading
  - [ ] Test style validation
  - [ ] Test missing style handling
  - [ ] Add to CMakeLists.txt

### Configurators

**Directory**: `tests/unit/entity/` (for entity configurator)

- [ ] **FlatbuffersEntityConfigurator.test.cpp** (exists)
  - [x] Basic configuration ✓
  - [ ] Test all component types
  - [ ] Test null component handling
  - [ ] Test invalid data handling

**Directory**: `tests/unit/scenes/` (needs creation for scene configurator)

- [ ] **FlatbuffersSceneConfigurator.test.cpp** (needs creation)
  - [ ] Test scene configuration from data
  - [ ] Test entity creation
  - [ ] Test logic setup
  - [ ] Test error handling
  - [ ] Create `tests/unit/scenes/CMakeLists.txt`
  - [ ] Add to parent CMakeLists.txt

**Directory**: `tests/unit/user_interface/` (exists, add configurator)

- [ ] **FlatbuffersUIElementConfigurator.test.cpp** (needs creation)
  - [ ] Test UI element configuration
  - [ ] Test all UI element types
  - [ ] Test style application
  - [ ] Test hierarchy building
  - [ ] Add to existing CMakeLists.txt

### Layer 2 Infrastructure

- [ ] **Review** `tests/unit/data_providers/CMakeLists.txt`
- [ ] **Update** to include new test files
- [ ] **Create** `tests/unit/scenes/CMakeLists.txt` (if not exists)

**Layer 2 Estimated Tasks**: 8 test files (3 new, 5 expansions)

---

## Layer 3: Managers

### Entity Management

**Directory**: `tests/unit/entity/` (exists)

- [ ] **EntityManager.test.cpp** (exists)
  - [x] Construction ✓
  - [x] Pool access ✓
  - [ ] Test entity creation
  - [ ] Test entity destruction
  - [ ] Test component activation
  - [ ] Test archetype updates
  - [ ] Test edge cases (pool full, invalid IDs)

- [ ] **ArchetypeManager.test.cpp** (exists)
  - [x] Basic functionality ✓
  - [ ] Test archetype generation
  - [ ] Test entity-archetype mapping
  - [ ] Test archetype updates on component changes
  - [ ] Test archetype iteration

### Asset Management

**Directory**: `tests/unit/assets/` (exists)

- [ ] **AssetManager.test.cpp** (exists)
  - [x] Construction ✓
  - [x] LoadDefaultAssets ✓
  - [ ] Test font loading individually
  - [ ] Test UI style loading individually
  - [ ] Test scene-specific asset loading
  - [ ] Test asset retrieval
  - [ ] Test missing asset handling

### Event Management

**Directory**: `tests/unit/events/` (exists)

- [ ] **EventHandler.test.cpp** (exists)
  - [x] Basic functionality ✓
  - [ ] Test event queuing
  - [ ] Test event distribution
  - [ ] Test multiple subscribers
  - [ ] Test event priorities
  - [ ] Test error conditions

### Display Management

**Directory**: `tests/unit/display/` (exists)

- [ ] **DisplayManager.test.cpp** (exists)
  - [x] Basic functionality ✓
  - [ ] Test window creation
  - [ ] Test render target management
  - [ ] Test display updates

- [ ] **Tile.test.cpp** (needs creation)
  - [ ] Test tile construction
  - [ ] Test tile properties
  - [ ] Test tile rendering
  - [ ] Add to `tests/unit/display/CMakeLists.txt`

- [ ] **ReTile.test.cpp** (needs creation)
  - [ ] Test retile construction
  - [ ] Test retile operations
  - [ ] Test tile updates
  - [ ] Add to CMakeLists.txt

- [ ] **Session.test.cpp** (needs creation)
  - [ ] Test session creation
  - [ ] Test session management
  - [ ] Test session state
  - [ ] Add to CMakeLists.txt

### Scene Management

**Directory**: `tests/unit/scenes/` (needs creation)

- [ ] **SceneManager.test.cpp** (needs creation)
  - [ ] Test manager construction
  - [ ] Test scene addition
  - [ ] Test scene removal
  - [ ] Test scene switching
  - [ ] Test active scene tracking
  - [ ] Test scene lifecycle
  - [ ] Create CMakeLists.txt if not exists

- [ ] **SceneFactory.test.cpp** (needs creation)
  - [ ] Test scene creation for each type
  - [ ] Test TitleScene creation
  - [ ] Test CraftingScene creation
  - [ ] Test invalid scene type handling
  - [ ] Test scene configuration
  - [ ] Add to CMakeLists.txt

### Layer 3 Infrastructure

- [ ] **Update** `tests/unit/display/CMakeLists.txt`
- [ ] **Create/Update** `tests/unit/scenes/CMakeLists.txt`

**Layer 3 Estimated Tasks**: 11 test files (5 new, 6 expansions)

---

## Layer 4: Logic Classes

**Directory**: `tests/unit/logic/` (exists)

### Existing Tests (Verify and Expand)

- [ ] **Logic.test.cpp** (exists)
  - [x] Base class functionality ✓
  - [ ] Test subscriber management
  - [ ] Test RunLogic hook

- [ ] **UIRenderLogic.test.cpp** (needs extraction/creation)
  - [ ] Extract from `logic_render.test.cpp` or create new
  - [ ] Test construction
  - [ ] Test RunLogic execution
  - [ ] Test rendering single UI element
  - [ ] Test rendering multiple UI elements
  - [ ] Test hidden element skipping
  - [ ] Test null element handling
  - [ ] Add to CMakeLists.txt if new

- [ ] **UIActionLogic.test.cpp** (needs creation)
  - [ ] Test construction
  - [ ] Test input processing
  - [ ] Test button click detection
  - [ ] Test event generation
  - [ ] Test UI state updates
  - [ ] Add to `tests/unit/logic/CMakeLists.txt`

- [ ] **UICollisionLogic.test.cpp** (exists as collision.test.cpp)
  - [x] Basic collision ✓
  - [ ] Test collision detection
  - [ ] Test boundary cases
  - [ ] Test overlapping elements

- [ ] **UIStateLogic.test.cpp** (needs creation)
  - [ ] Test construction
  - [ ] Test state management
  - [ ] Test state transitions
  - [ ] Test UI state updates
  - [ ] Add to CMakeLists.txt

- [ ] **CraftingRenderLogic.test.cpp** (needs extraction/creation)
  - [ ] Extract from existing or create new
  - [ ] Test construction
  - [ ] Test crafting scene rendering
  - [ ] Test crafting-specific elements
  - [ ] Add to CMakeLists.txt if new

- [ ] **LogicFactory.test.cpp** (exists)
  - [x] Factory pattern ✓
  - [ ] Test logic creation for all scenes
  - [ ] Test logic type mapping
  - [ ] Test error conditions

### Layer 4 Infrastructure

- [ ] **Review** `tests/unit/logic/CMakeLists.txt`
- [ ] **Add** new test files to executable
- [ ] **Verify** all Logic classes have tests

**Layer 4 Estimated Tasks**: 7 test files (3 new, 4 expansions)

---

## Layer 5: Scenes (CRITICAL PRIORITY)

**Directory**: `tests/unit/scenes/` (NEEDS CREATION)

### Scene Implementations

- [ ] **TitleScene.test.cpp** (CRITICAL - needs creation)
  - [ ] Test construction with GameContext
  - [ ] Test scene type is TITLE
  - [ ] Test render texture creation
  - [ ] Test EntityManager initialization
  - [ ] Test sRender() execution
  - [ ] Test sAction() execution
  - [ ] Test sCollision() execution
  - [ ] Test sMovement() execution
  - [ ] Test active state management
  - [ ] Test entity creation
  - [ ] Test UI element setup
  - [ ] Create `tests/unit/scenes/CMakeLists.txt`

- [ ] **CraftingScene.test.cpp** (CRITICAL - needs creation)
  - [ ] Test construction with GameContext
  - [ ] Test scene type is CRAFTING
  - [ ] Test render texture creation
  - [ ] Test EntityManager initialization
  - [ ] Test sRender() execution
  - [ ] Test sAction() execution
  - [ ] Test sCollision() execution
  - [ ] Test sMovement() execution
  - [ ] Test active state management
  - [ ] Test crafting-specific entities
  - [ ] Test crafting logic setup
  - [ ] Add to CMakeLists.txt

### Scene Management (duplicate with Layer 3, verify location)

- [ ] **SceneFactory.test.cpp** (needs creation)
  - [ ] Test CreateScene for TitleScene
  - [ ] Test CreateScene for CraftingScene
  - [ ] Test invalid scene type
  - [ ] Test scene configuration
  - [ ] Test error handling
  - [ ] Add to CMakeLists.txt

- [ ] **SceneManager.test.cpp** (needs creation)
  - [ ] Test construction
  - [ ] Test AddScene
  - [ ] Test RemoveScene
  - [ ] Test ChangeScene
  - [ ] Test GetActiveScene
  - [ ] Test scene transitions
  - [ ] Test error conditions
  - [ ] Add to CMakeLists.txt

### Layer 5 Infrastructure

- [ ] **CREATE** `tests/unit/scenes/` directory
- [ ] **CREATE** `tests/unit/scenes/CMakeLists.txt`
- [ ] **ADD** to parent `tests/unit/CMakeLists.txt`
- [ ] **LINK** against required libraries (scenes, logic, entity, etc.)
- [ ] **CONFIGURE** Catch2 test discovery

**Layer 5 Estimated Tasks**: 4 test files (ALL NEW - CRITICAL)

---

## Layer 6: Engine

**Directory**: `tests/unit/engine/` (exists)

### Engine Implementation

- [ ] **GameEngine.test.cpp** (exists - needs major expansion)
  - [x] Basic construction ✓
  - [ ] Test Initialize() method
  - [ ] Test subsystem initialization order
  - [ ] Test AssetManager initialization
  - [ ] Test DisplayManager initialization
  - [ ] Test SceneManager initialization
  - [ ] Test configuration loading
  - [ ] Test initial scene creation
  - [ ] Test game loop setup
  - [ ] Test event handler setup
  - [ ] Test error handling on init failure
  - [ ] Test multiple Initialize() calls
  - [ ] Test cleanup on destruction
  - [ ] Test resource management

### Layer 6 Infrastructure

- [ ] **Review** `tests/unit/engine/CMakeLists.txt`
- [ ] **Verify** all dependencies linked
- [ ] **Test** integration with lower layers

**Layer 6 Estimated Tasks**: 1 test file (major expansion)

---

## Summary

### Total Estimated Tasks

| Layer | New Files | Expansions | Total Tasks | Priority |
|-------|-----------|------------|-------------|----------|
| Layer 0 | 30 | 0 | 30 | Medium |
| Layer 1 | 0 | 5 | 5 | Low |
| Layer 2 | 3 | 5 | 8 | Low |
| Layer 3 | 5 | 6 | 11 | High |
| Layer 4 | 3 | 4 | 7 | Low |
| Layer 5 | 4 | 0 | 4 | **CRITICAL** |
| Layer 6 | 0 | 1 | 1 | High |
| **Total** | **45** | **21** | **66** | |

### Recommended Work Order

1. **Phase 1 - Critical Gap** (Highest priority)
   - Layer 5: Complete all 4 scene test files
   - Estimated time: 1-2 days

2. **Phase 2 - High Priority Managers** 
   - Layer 3: Create display utility tests (Tile, ReTile, Session)
   - Layer 3: Create/expand SceneManager tests
   - Layer 6: Expand GameEngine tests
   - Estimated time: 1-2 days

3. **Phase 3 - Foundation Completion** (Medium priority)
   - Layer 0: Create all UI type tests
   - Layer 0: Create core type tests
   - Layer 0: Create logger tests
   - Estimated time: 2-3 days

4. **Phase 4 - Polish and Expansion** (Low priority)
   - Layer 1: Expand component tests
   - Layer 2: Expand configurator tests
   - Layer 4: Create missing Logic tests
   - Estimated time: 1-2 days

### Daily Work Plan Template

**Day 1**: Focus on Layer 5 (Critical)
- Morning: TitleScene.test.cpp
- Afternoon: CraftingScene.test.cpp

**Day 2**: Complete Layer 5
- Morning: SceneFactory.test.cpp
- Afternoon: SceneManager.test.cpp

**Day 3**: High Priority Layer 3
- Morning: Tile.test.cpp, ReTile.test.cpp
- Afternoon: Session.test.cpp

**Day 4**: High Priority Layer 6
- Morning/Afternoon: Expand GameEngine.test.cpp

**Day 5+**: Medium/Low Priority Layers
- Work through Layer 0, 1, 2, 4 items

### Tracking Progress

- [ ] **Create tracking branch**: `feature/complete-unit-tests`
- [ ] **Commit after each test file** with message: "Add [ClassName].test.cpp for Layer [N]"
- [ ] **Run tests after each file**: `ctest -R test_[subsystem]`
- [ ] **Update this checklist** as you complete items
- [ ] **Review coverage** after each layer completion

---

**Document Version**: 1.0  
**Last Updated**: 2025-12-17  
**Total Tasks**: 66 (45 new files, 21 expansions)
