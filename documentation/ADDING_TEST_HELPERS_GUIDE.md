# Guide for Adding Testing Helpers and Test Infrastructure

## Purpose

This guide provides step-by-step instructions for **extending the test infrastructure** in SteamRot. Use this when you need to:

- Add new test helper functions
- Create new test fixtures
- Add new matchers
- Extend the test harness capabilities
- Add new FlatBuffers test data types

## Overview of Test Infrastructure

### Test Infrastructure Components

```
tests/
├── harness/              # Test harness (data-driven testing)
│   ├── FlatbuffersTestDataLoader      # Load .test_data.bin files
│   ├── FlatbuffersTestDataProvider    # Convert FlatBuffers → C++ structs
│   ├── TestEngine                     # Game engine for testing
│   ├── SimulationRunner               # Execute simulation steps
│   └── harness_runner                 # Orchestration functions
├── matchers/             # Catch2 custom matchers
│   ├── EntityMemoryPoolMatcher        # Compare entity pools
│   ├── EventBusMatcher                # Compare event buses
│   └── ...
├── context/              # Test fixtures and helpers
│   ├── TestFixture                    # Game/scene context for tests
│   ├── TestContext                    # Context information for matchers
│   └── test_helpers                   # General helper functions
└── unit/                 # Unit tests organized by subsystem
```

### Key Concepts

1. **Test Harness** - Data-driven testing with TestEngine
2. **Matchers** - Catch2 custom matchers for comparisons
3. **Fixtures** - Pre-configured test environments
4. **Helpers** - Reusable functions for common test tasks
5. **Test Data** - FlatBuffers-based test configurations

## When to Add New Infrastructure

### Add Test Helpers When:

✅ You find yourself copying the same setup code across multiple tests
✅ Complex setup logic is repeated (create helpers to encapsulate it)
✅ A common operation needs to be performed consistently

### Add Matchers When:

✅ You need to compare complex data structures
✅ You want rich, formatted error messages
✅ Multiple tests need the same comparison logic

### Add Test Fixtures When:

✅ Multiple tests need the same game/scene setup
✅ Setup is complex and should be reused
✅ You want to ensure consistent test environments

### Extend Test Harness When:

✅ Adding new FlatBuffers test data types
✅ Adding new simulation capabilities
✅ Adding new test execution modes

## How-To Guides

### 1. Adding a New Test Helper Function

**When:** You have repeated setup or validation logic across tests.

**Steps:**

1. **Identify the common pattern** in your tests
2. **Create a helper function** in an appropriate location
3. **Write unit tests** for the helper function
4. **Update existing tests** to use the helper

**Example: Adding a helper to create populated EntityMemoryPool**

#### Step 1: Identify the pattern

```cpp
// You notice this pattern repeated:
TEST_CASE("Test A", "[unit]") {
  EntityMemoryPool pool;
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, 10);
  
  auto &ui = steamrot::entity::memory::GetComponent<CUserInterface>(0, pool);
  ui.m_active = true;
  ui.m_ui_name = "test";
  // ... more setup
}

TEST_CASE("Test B", "[unit]") {
  EntityMemoryPool pool;
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, 10);
  
  auto &ui = steamrot::entity::memory::GetComponent<CUserInterface>(0, pool);
  ui.m_active = true;
  ui.m_ui_name = "test";
  // ... same setup
}
```

#### Step 2: Create helper function

**File:** `tests/context/entity_test_helpers.h`

```cpp
#pragma once

#include "EntityMemoryPool.h"
#include "CUserInterface.h"

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Create an EntityMemoryPool with a configured UI entity
///
/// @param pool_size Size of the pool
/// @param entity_index Index of the UI entity
/// @param ui_name Name for the UI component
/// @return Configured EntityMemoryPool
/////////////////////////////////////////////////
EntityMemoryPool CreatePoolWithUIEntity(
    size_t pool_size, 
    uint32_t entity_index, 
    const std::string& ui_name);

} // namespace steamrot::tests
```

**File:** `tests/context/entity_test_helpers.cpp`

```cpp
#include "entity_test_helpers.h"
#include "emp_helpers.h"

namespace steamrot::tests {

EntityMemoryPool CreatePoolWithUIEntity(
    size_t pool_size, 
    uint32_t entity_index, 
    const std::string& ui_name) {
  
  EntityMemoryPool pool;
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, pool_size);
  
  auto &ui = steamrot::entity::memory::GetComponent<CUserInterface>(
      entity_index, pool);
  ui.m_active = true;
  ui.m_ui_name = ui_name;
  
  return pool;
}

} // namespace steamrot::tests
```

#### Step 3: Write unit tests for helper

**File:** `tests/context/entity_test_helpers.test.cpp`

```cpp
#include "entity_test_helpers.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CreatePoolWithUIEntity creates correct pool size", 
          "[unit][entity_test_helpers]") {
  auto pool = steamrot::tests::CreatePoolWithUIEntity(10, 0, "test");
  REQUIRE(pool.m_memory_pool.size() == 10);
}

TEST_CASE("CreatePoolWithUIEntity configures UI component", 
          "[unit][entity_test_helpers]") {
  auto pool = steamrot::tests::CreatePoolWithUIEntity(10, 0, "test_ui");
  
  auto &ui = steamrot::entity::memory::GetComponent<CUserInterface>(0, pool);
  REQUIRE(ui.m_active == true);
  REQUIRE(ui.m_ui_name == "test_ui");
}
```

#### Step 4: Update existing tests

```cpp
// Before
TEST_CASE("Test A", "[unit]") {
  EntityMemoryPool pool;
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, 10);
  auto &ui = steamrot::entity::memory::GetComponent<CUserInterface>(0, pool);
  ui.m_active = true;
  ui.m_ui_name = "test";
  // ... test logic
}

// After
TEST_CASE("Test A", "[unit]") {
  auto pool = steamrot::tests::CreatePoolWithUIEntity(10, 0, "test");
  // ... test logic (simpler!)
}
```

---

### 2. Adding a New Catch2 Matcher

**When:** You need to compare complex data structures with rich error messages.

**Steps:**

1. **Identify comparison need** - What data structure?
2. **Create matcher class** implementing Catch2 matcher interface
3. **Create matcher factory function** for convenience
4. **Write unit tests** for the matcher
5. **Use in tests**

**Example: Adding a matcher for SceneManager**

#### Step 1: Identify need

```cpp
// You want to compare SceneManager instances:
TEST_CASE("SceneManager comparison", "[unit]") {
  SceneManager actual = /* ... */;
  SceneManager expected = /* ... */;
  
  // How to compare? Need detailed output on mismatch.
}
```

#### Step 2: Create matcher class

**File:** `tests/matchers/SceneManagerMatcher.h`

```cpp
#pragma once

#include "SceneManager.h"
#include "TestContext.h"
#include <catch2/matchers/catch_matchers.hpp>
#include <string>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Catch2 matcher for comparing SceneManager instances
/////////////////////////////////////////////////
class SceneManagerMatcher : public Catch::Matchers::MatcherGenericBase {
private:
  const SceneManager& m_expected;
  TestContext m_context;
  mutable std::string m_mismatch_description;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor
  ///
  /// @param expected Expected SceneManager
  /// @param context Test context for error messages
  /////////////////////////////////////////////////
  SceneManagerMatcher(const SceneManager& expected, 
                      const TestContext& context = {});

  /////////////////////////////////////////////////
  /// @brief Perform the match
  /////////////////////////////////////////////////
  bool match(const SceneManager& actual) const;

  /////////////////////////////////////////////////
  /// @brief Describe the matcher
  /////////////////////////////////////////////////
  std::string describe() const override;
};

/////////////////////////////////////////////////
/// @brief Factory function for SceneManagerMatcher
/////////////////////////////////////////////////
inline SceneManagerMatcher EqualsSceneManager(
    const SceneManager& expected, 
    const TestContext& context = {}) {
  return SceneManagerMatcher(expected, context);
}

} // namespace steamrot::tests
```

#### Step 3: Implement matcher

**File:** `tests/matchers/SceneManagerMatcher.cpp`

```cpp
#include "SceneManagerMatcher.h"
#include "console_output.h"
#include <format>

namespace steamrot::tests {

SceneManagerMatcher::SceneManagerMatcher(
    const SceneManager& expected, 
    const TestContext& context)
    : m_expected(expected), m_context(context) {}

bool SceneManagerMatcher::match(const SceneManager& actual) const {
  // Compare current scene
  if (actual.GetCurrentSceneType() != m_expected.GetCurrentSceneType()) {
    m_mismatch_description = std::format(
        "Current scene mismatch: actual = {}, expected = {}",
        static_cast<int>(actual.GetCurrentSceneType()),
        static_cast<int>(m_expected.GetCurrentSceneType()));
    return false;
  }
  
  // Compare scene count
  if (actual.GetSceneCount() != m_expected.GetSceneCount()) {
    m_mismatch_description = std::format(
        "Scene count mismatch: actual = {}, expected = {}",
        actual.GetSceneCount(),
        m_expected.GetSceneCount());
    return false;
  }
  
  // All checks passed
  return true;
}

std::string SceneManagerMatcher::describe() const {
  std::string description;
  
  // Add test context if available
  if (!m_context.test_name.empty()) {
    description += console::IsColorEnabled() 
        ? console::Color::CYAN 
        : "";
    description += std::format("[Test: {}]\n", m_context.test_name);
  }
  
  if (!m_context.description.empty()) {
    description += std::format("[Description: {}]\n", m_context.description);
  }
  
  // Add mismatch details
  description += console::IsColorEnabled() 
      ? console::Color::RED 
      : "";
  description += "✗ ";
  description += m_mismatch_description;
  description += console::IsColorEnabled() 
      ? console::Color::RESET 
      : "";
  
  return description;
}

} // namespace steamrot::tests
```

#### Step 4: Write unit tests

**File:** `tests/matchers/SceneManagerMatcher.test.cpp`

```cpp
#include "SceneManagerMatcher.h"
#include "SceneManager.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("SceneManagerMatcher matches equal SceneManagers", 
          "[unit][SceneManagerMatcher]") {
  steamrot::SceneManager sm1;
  steamrot::SceneManager sm2;
  
  // Both have same default state
  REQUIRE_THAT(sm1, steamrot::tests::EqualsSceneManager(sm2));
}

TEST_CASE("SceneManagerMatcher detects scene type mismatch", 
          "[unit][SceneManagerMatcher]") {
  steamrot::SceneManager sm1;
  steamrot::SceneManager sm2;
  
  sm1.SetCurrentScene(steamrot::SceneType::TITLE);
  sm2.SetCurrentScene(steamrot::SceneType::CRAFTING);
  
  REQUIRE_FALSE(
      steamrot::tests::SceneManagerMatcher(sm2).match(sm1));
}
```

#### Step 5: Use in tests

```cpp
TEST_CASE("Scene transition updates SceneManager", "[integration]") {
  // Setup
  auto pool = CreateTestPool();
  SceneManager manager;
  manager.SetCurrentScene(SceneType::TITLE);
  
  // Execute scene change logic
  // ...
  
  // Validate with matcher
  SceneManager expected;
  expected.SetCurrentScene(SceneType::CRAFTING);
  
  TestContext context;
  context.test_name = "scene_transition_test";
  
  REQUIRE_THAT(manager, EqualsSceneManager(expected, context));
}
```

---

### 3. Adding a New Test Fixture

**When:** Multiple tests need the same game/scene setup.

**Steps:**

1. **Identify common setup** across tests
2. **Create fixture class** with setup methods
3. **Write unit tests** for fixture
4. **Use in tests**

**Example: Adding a fixture for crafting scene tests**

#### Step 1: Identify common setup

```cpp
// You notice this repeated pattern:
TEST_CASE("Crafting test 1", "[integration]") {
  GameContext game_context;
  SceneContext scene_context;
  
  // Setup crafting scene entities
  EntityMemoryPool pool;
  // ... 50 lines of setup
  
  // Test logic
}

TEST_CASE("Crafting test 2", "[integration]") {
  GameContext game_context;
  SceneContext scene_context;
  
  // Setup crafting scene entities (same 50 lines!)
  EntityMemoryPool pool;
  // ...
  
  // Test logic
}
```

#### Step 2: Create fixture class

**File:** `tests/context/CraftingTestFixture.h`

```cpp
#pragma once

#include "GameContext.h"
#include "SceneContext.h"
#include "EntityMemoryPool.h"
#include "FailInfo.h"
#include <expected>

namespace steamrot::tests {

/////////////////////////////////////////////////
/// @brief Test fixture for crafting scene tests
///
/// Provides a pre-configured crafting scene environment
/// with typical entities and resources.
/////////////////////////////////////////////////
class CraftingTestFixture {
private:
  GameContext m_game_context;
  SceneContext m_scene_context;
  bool m_initialized{false};

public:
  /////////////////////////////////////////////////
  /// @brief Initialize the fixture with default crafting setup
  ///
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> Initialize();

  /////////////////////////////////////////////////
  /// @brief Initialize with custom pool size
  ///
  /// @param pool_size Entity pool size
  /// @return Success or failure information
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> 
  InitializeWithPoolSize(size_t pool_size);

  /////////////////////////////////////////////////
  /// @brief Get game context
  /////////////////////////////////////////////////
  GameContext& GetGameContext() { return m_game_context; }
  
  /////////////////////////////////////////////////
  /// @brief Get scene context
  /////////////////////////////////////////////////
  SceneContext& GetSceneContext() { return m_scene_context; }

  /////////////////////////////////////////////////
  /// @brief Check if initialized
  /////////////////////////////////////////////////
  bool IsInitialized() const { return m_initialized; }
};

} // namespace steamrot::tests
```

**File:** `tests/context/CraftingTestFixture.cpp`

```cpp
#include "CraftingTestFixture.h"
#include "emp_helpers.h"
#include "CUserInterface.h"
#include "CGrimoireMachina.h"

namespace steamrot::tests {

std::expected<std::monostate, FailInfo> 
CraftingTestFixture::Initialize() {
  return InitializeWithPoolSize(100);
}

std::expected<std::monostate, FailInfo> 
CraftingTestFixture::InitializeWithPoolSize(size_t pool_size) {
  if (m_initialized) {
    return std::unexpected(FailInfo{
        FailMode::InvalidOperation,
        "Fixture already initialized"
    });
  }

  // Setup game context
  m_game_context.window_size = {800, 600};
  m_game_context.framerate = 60;
  // ... more game context setup

  // Setup scene context
  steamrot::entity::memory::ResizeEntityMemoryPool(
      m_scene_context.scene_entities, pool_size);

  // Add typical crafting scene entities
  // Entity 0: Crafting UI
  auto &ui = steamrot::entity::memory::GetComponent<CUserInterface>(
      0, m_scene_context.scene_entities);
  ui.m_active = true;
  ui.m_ui_name = "crafting_panel";
  ui.m_is_visible = true;

  // Entity 1: Grimoire
  auto &grimoire = steamrot::entity::memory::GetComponent<CGrimoireMachina>(
      1, m_scene_context.scene_entities);
  grimoire.m_active = true;

  m_initialized = true;
  return std::monostate{};
}

} // namespace steamrot::tests
```

#### Step 3: Write unit tests

**File:** `tests/context/CraftingTestFixture.test.cpp`

```cpp
#include "CraftingTestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CraftingTestFixture initializes successfully", 
          "[unit][CraftingTestFixture]") {
  steamrot::tests::CraftingTestFixture fixture;
  
  auto result = fixture.Initialize();
  REQUIRE(result.has_value());
  REQUIRE(fixture.IsInitialized());
}

TEST_CASE("CraftingTestFixture sets up crafting entities", 
          "[unit][CraftingTestFixture]") {
  steamrot::tests::CraftingTestFixture fixture;
  fixture.Initialize();
  
  auto &pool = fixture.GetSceneContext().scene_entities;
  REQUIRE(pool.m_memory_pool.size() == 100);
  
  auto &ui = steamrot::entity::memory::GetComponent<CUserInterface>(0, pool);
  REQUIRE(ui.m_active == true);
  REQUIRE(ui.m_ui_name == "crafting_panel");
}

TEST_CASE("CraftingTestFixture prevents double initialization", 
          "[unit][CraftingTestFixture]") {
  steamrot::tests::CraftingTestFixture fixture;
  fixture.Initialize();
  
  auto result = fixture.Initialize();
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::InvalidOperation);
}
```

#### Step 4: Use in tests

```cpp
// Before: 50+ lines of setup
TEST_CASE("Crafting test 1", "[integration]") {
  GameContext game_context;
  SceneContext scene_context;
  EntityMemoryPool pool;
  // ... 50 lines of setup
  
  // Test logic
}

// After: Simple and clean
TEST_CASE("Crafting test 1", "[integration]") {
  steamrot::tests::CraftingTestFixture fixture;
  auto result = fixture.Initialize();
  REQUIRE(result.has_value());
  
  // Test logic - fixture provides everything needed
  auto &scene_context = fixture.GetSceneContext();
  // ...
}
```

---

### 4. Extending Test Harness - Adding New FlatBuffers Test Data Type

**When:** You need to test new game data types with the test harness.

**Steps:**

1. **Create FlatBuffers schema** for new data type
2. **Add to TestDataConfig schema**
3. **Add conversion functions** in FlatbuffersTestDataProvider
4. **Add to TestData C++ struct**
5. **Write unit tests** for conversion
6. **Use in test data JSON**

**Example: Adding UI style test data**

#### Step 1: Create FlatBuffers schema

**File:** `src/flatbuffers_headers/test_data/ui_style_test_data.fbs`

```fbs
namespace steamrot;

table UIStyleTestData {
  style_name: string;
  font_size: uint32;
  color: ColorData;
}
```

#### Step 2: Add to TestDataConfig schema

**File:** `src/flatbuffers_headers/test_data/test_data.fbs`

```fbs
include "ui_style_test_data.fbs";

table TestDataFbs {
  meta_data: TestMetadataFbs (required);
  simulation_data: SimulationDataFbs;
  num_ticks: uint32;
  starting_engine_snapshot: EngineSnapshotFbs;
  expected_engine_snapshots: [TickSnapshotPairFbs];
  
  // ADD NEW FIELD
  ui_style_data: UIStyleTestData;
}
```

#### Step 3: Add conversion function

**File:** `tests/harness/FlatbuffersTestDataProvider.h`

```cpp
class FlatbuffersTestDataProvider : public ITestDataProvider {
  // ... existing methods ...

  /////////////////////////////////////////////////
  /// @brief Configure UIStyle from FlatBuffers data
  ///
  /// @param ui_style UIStyle instance to configure
  /// @param fbs_ui_style FlatBuffers UIStyleTestData
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ConfigureUIStyle(
      UIStyle& ui_style,
      const UIStyleTestData* fbs_ui_style) const;
};
```

**File:** `tests/harness/FlatbuffersTestDataProvider.cpp`

```cpp
std::expected<std::monostate, FailInfo>
FlatbuffersTestDataProvider::ConfigureUIStyle(
    UIStyle& ui_style,
    const UIStyleTestData* fbs_ui_style) const {

  // Null check
  if (!fbs_ui_style) {
    return std::unexpected(FailInfo{
        FailMode::FlatbuffersDataNotFound,
        "UIStyleTestData is null"
    });
  }

  // Configure fields
  if (fbs_ui_style->style_name()) {
    ui_style.name = fbs_ui_style->style_name()->str();
  }

  ui_style.font_size = fbs_ui_style->font_size();

  if (fbs_ui_style->color()) {
    ui_style.color.r = fbs_ui_style->color()->r();
    ui_style.color.g = fbs_ui_style->color()->g();
    ui_style.color.b = fbs_ui_style->color()->b();
    ui_style.color.a = fbs_ui_style->color()->a();
  }

  return std::monostate{};
}

// Update CreateTestData to use new field
std::expected<TestData, FailInfo>
FlatbuffersTestDataProvider::CreateTestData(
    const TestDataFbs* fbs_test_data) const {
  
  // ... existing code ...

  // ADD: Configure UIStyle if present
  if (fbs_test_data->ui_style_data()) {
    auto result = ConfigureUIStyle(
        test_data.ui_style, 
        fbs_test_data->ui_style_data());
    
    if (!result.has_value()) {
      return std::unexpected(result.error());
    }
  }

  return test_data;
}
```

#### Step 4: Add to TestData struct

**File:** `src/types/test_structs/TestData.h`

```cpp
namespace steamrot {

struct TestData {
  TestMetaData meta_data{};
  SimulationData simulation_data{};
  uint32_t number_of_ticks{1};
  EngineSnapshot starting_engine_snapshot{};
  std::map<size_t, EngineSnapshot> expected_engine_snapshots{};
  
  // ADD NEW FIELD
  UIStyle ui_style{};
};

} // namespace steamrot
```

#### Step 5: Write unit tests

**File:** `tests/unit/harness/FlatbuffersTestDataProvider.test.cpp`

```cpp
TEST_CASE("FlatbuffersTestDataProvider::ConfigureUIStyle handles null input",
          "[unit][FlatbuffersTestDataProvider]") {
  std::filesystem::path obj_dir_path(__FILE__);
  steamrot::EventHandler event_handler;
  FlatbuffersTestDataProvider provider(obj_dir_path, event_handler);
  
  UIStyle ui_style;
  auto result = provider.ConfigureUIStyle(ui_style, nullptr);
  
  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
}

TEST_CASE("FlatbuffersTestDataProvider::ConfigureUIStyle configures correctly",
          "[unit][FlatbuffersTestDataProvider]") {
  // Create FlatBuffers data
  flatbuffers::FlatBufferBuilder builder;
  
  auto color = steamrot::CreateColorData(builder, 255, 128, 64, 255);
  auto name = builder.CreateString("test_style");
  auto fbs_style = steamrot::CreateUIStyleTestData(
      builder, name, 16, color);
  
  builder.Finish(fbs_style);
  const UIStyleTestData* fbs_ptr = 
      steamrot::GetUIStyleTestData(builder.GetBufferPointer());

  // Configure
  std::filesystem::path obj_dir_path(__FILE__);
  steamrot::EventHandler event_handler;
  FlatbuffersTestDataProvider provider(obj_dir_path, event_handler);
  
  UIStyle ui_style;
  auto result = provider.ConfigureUIStyle(ui_style, fbs_ptr);
  
  REQUIRE(result.has_value());
  REQUIRE(ui_style.name == "test_style");
  REQUIRE(ui_style.font_size == 16);
  REQUIRE(ui_style.color.r == 255);
}
```

#### Step 6: Use in test data JSON

**File:** `tests/integration/ui/data/ui_style_test.test_data.json`

```json
{
  "meta_data": {
    "test_name": "ui_style_test",
    "test_description": "Test UI style configuration",
    "tags": ["integration", "ui"],
    "will_pass": true
  },
  "num_ticks": 1,
  "ui_style_data": {
    "style_name": "crafting_button",
    "font_size": 14,
    "color": { "r": 255, "g": 255, "b": 255, "a": 255 }
  },
  "starting_engine_snapshot": {
    "scene_collection_data": { ... }
  }
}
```

---

## Best Practices for Test Infrastructure

### 1. Test Your Test Code ✅

**Always write unit tests for test infrastructure.**

```cpp
// Good - Helper function has unit tests
TEST_CASE("CreateTestPool creates correct size", "[unit][helpers]") {
  auto pool = steamrot::tests::CreateTestPool(10);
  REQUIRE(pool.m_memory_pool.size() == 10);
}

// Then use it confidently in other tests
TEST_CASE("My feature test", "[integration]") {
  auto pool = steamrot::tests::CreateTestPool(10);
  // ...
}
```

**Why:** If your test infrastructure has bugs, all tests using it are unreliable.

### 2. Keep Helpers Focused ✅

**Each helper should do ONE thing well.**

```cpp
// Good - Focused helpers
EntityMemoryPool CreateEmptyPool(size_t size);
EntityMemoryPool CreatePoolWithUIEntities(size_t count);
EntityMemoryPool CreatePoolWithGrimoireEntities(size_t count);

// Bad - One helper does everything
EntityMemoryPool CreatePool(size_t size, bool add_ui, bool add_grimoire, 
                            int ui_count, int grimoire_count, ...);
```

### 3. Use Consistent Naming ✅

**Follow naming conventions for discoverability.**

| Type | Naming Convention | Example |
|------|------------------|---------|
| Helper Function | `Create<What>`, `Setup<What>` | `CreateTestPool`, `SetupCraftingScene` |
| Matcher | `Equals<Type>` | `EqualsEntityMemoryPool`, `EqualsEventBus` |
| Fixture | `<Context>TestFixture` | `CraftingTestFixture`, `UITestFixture` |
| Test File | `<Module>.test.cpp` | `helpers.test.cpp`, `CraftingFixture.test.cpp` |

### 4. Document Your Additions ✅

**Add Doxygen comments to all public test infrastructure.**

```cpp
/////////////////////////////////////////////////
/// @brief Create an EntityMemoryPool with UI entities
///
/// Creates a pool with the specified number of UI entities,
/// each configured with default visible state and unique names.
///
/// @param count Number of UI entities to create
/// @return Configured EntityMemoryPool
///
/// @example
/// auto pool = CreatePoolWithUIEntities(5);
/// // Pool contains 5 UI entities at indices 0-4
/////////////////////////////////////////////////
EntityMemoryPool CreatePoolWithUIEntities(size_t count);
```

### 5. Handle Errors Gracefully ✅

**Use std::expected for operations that can fail.**

```cpp
// Good - Returns std::expected
std::expected<TestFixture, FailInfo> CreateCraftingFixture() {
  TestFixture fixture;
  
  auto init_result = fixture.Initialize();
  if (!init_result.has_value()) {
    return std::unexpected(init_result.error());
  }
  
  return fixture;
}

// Bad - Throws or uses assert
TestFixture CreateCraftingFixture() {
  TestFixture fixture;
  fixture.Initialize(); // What if this fails?
  return fixture;
}
```

### 6. Make Infrastructure Discoverable ✅

**Organize by purpose, document in README files.**

```
tests/
├── context/
│   ├── README.md              # Documents all fixtures and helpers
│   ├── TestFixture.h
│   ├── CraftingTestFixture.h
│   └── test_helpers.h
├── matchers/
│   ├── README.md              # Documents all matchers
│   ├── EntityMemoryPoolMatcher.h
│   └── EventBusMatcher.h
└── harness/
    └── README.md              # Documents test harness API
```

## Checklist for Adding Test Infrastructure

When adding new test infrastructure, verify:

- [ ] **Purpose is clear** - One focused responsibility
- [ ] **Unit tests written** - Test infrastructure is tested
- [ ] **Error handling** - Uses std::expected, handles edge cases
- [ ] **Documentation** - Doxygen comments, examples
- [ ] **Naming consistent** - Follows conventions
- [ ] **Location appropriate** - In correct directory (context/, matchers/, harness/)
- [ ] **Reusable** - Can be used by multiple tests
- [ ] **Stable** - Not affected by unrelated game logic changes

## Common Patterns

### Pattern: Builder for Complex Test Data

```cpp
class TestPoolBuilder {
private:
  EntityMemoryPool m_pool;
  size_t m_size{10};

public:
  TestPoolBuilder& WithSize(size_t size) {
    m_size = size;
    return *this;
  }

  TestPoolBuilder& WithUIEntity(uint32_t index, const std::string& name) {
    auto &ui = emp_helpers::GetComponent<CUserInterface>(index, m_pool);
    ui.m_active = true;
    ui.m_ui_name = name;
    return *this;
  }

  EntityMemoryPool Build() {
    entity::memory::ResizeEntityMemoryPool(m_pool, m_size);
    return m_pool;
  }
};

// Usage
auto pool = TestPoolBuilder()
    .WithSize(20)
    .WithUIEntity(0, "button1")
    .WithUIEntity(1, "button2")
    .Build();
```

### Pattern: Fixture with Tear-Down

```cpp
class ResourceTestFixture {
private:
  std::filesystem::path m_temp_dir;
  bool m_initialized{false};

public:
  std::expected<std::monostate, FailInfo> Initialize() {
    m_temp_dir = std::filesystem::temp_directory_path() / "test_resources";
    std::filesystem::create_directories(m_temp_dir);
    m_initialized = true;
    return std::monostate{};
  }

  ~ResourceTestFixture() {
    if (m_initialized && std::filesystem::exists(m_temp_dir)) {
      std::filesystem::remove_all(m_temp_dir);
    }
  }

  const std::filesystem::path& GetTempDir() const { return m_temp_dir; }
};
```

### Pattern: Parameterized Helper

```cpp
template<typename ComponentType>
EntityMemoryPool CreatePoolWithComponent(
    size_t pool_size,
    uint32_t entity_index,
    std::function<void(ComponentType&)> configure) {
  
  EntityMemoryPool pool;
  entity::memory::ResizeEntityMemoryPool(pool, pool_size);
  
  auto &component = emp_helpers::GetComponent<ComponentType>(entity_index, pool);
  component.m_active = true;
  configure(component);
  
  return pool;
}

// Usage
auto pool = CreatePoolWithComponent<CUserInterface>(
    10, 0,
    [](CUserInterface& ui) {
      ui.m_ui_name = "test";
      ui.m_is_visible = true;
    });
```

## Resources

- `TEST_HARNESS_ANALYSIS.md` - Architecture analysis
- `TESTING_BEST_PRACTICES.md` - When to use each approach
- `tests/harness/README.md` - Test harness API reference
- `tests/matchers/README.md` - Available matchers
- `tests/context/README.md` - Available fixtures and helpers

## Summary

### Key Takeaways

1. ✅ **Test your test code** - Infrastructure needs unit tests
2. ✅ **Keep helpers focused** - One responsibility per helper
3. ✅ **Use consistent naming** - Easier to discover and use
4. ✅ **Document thoroughly** - Doxygen + examples
5. ✅ **Handle errors** - Use std::expected
6. ✅ **Organize well** - Appropriate directories, README files

### Quick Start

Want to add new test infrastructure?

1. **Identify the need** - What's repeated or complex?
2. **Choose the right type** - Helper? Matcher? Fixture? Harness extension?
3. **Follow the guide** - See relevant section above
4. **Write unit tests** - Test your test code
5. **Document it** - Doxygen comments + examples
6. **Use the checklist** - Verify completeness

**Remember:** Good test infrastructure makes writing tests easier, faster, and more reliable. Invest time in building it well!
