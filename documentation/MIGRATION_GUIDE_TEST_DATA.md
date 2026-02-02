# Migration Guide: Converting Unit Tests from test_data.json to Appropriate Approaches

## Purpose

This guide provides **concrete step-by-step instructions** for migrating unit tests that inappropriately use `test_data.json` (TestDataConfig) to more appropriate testing approaches. Following this guide will result in faster, more stable, and more maintainable tests.

## Quick Assessment

**Before migrating, ask yourself:**

1. **What am I testing?** A single component/class or multiple systems?
2. **Why am I using test_data.json?** Do I really need TestEngine simulation?
3. **What would break my test?** Changes to the thing I'm testing, or changes to unrelated game logic?

**If your test would break due to unrelated game logic changes, it needs migration.**

## Migration Scenarios

### Scenario 1: Testing Component Construction/Defaults

**❌ BEFORE: Using test_data.json**

```cpp
// File: tests/unit/components/CUserInterface.test.cpp
#include "CUserInterface.h"
#include "harness_runner.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("CUserInterface has correct defaults", "[unit][CUserInterface]") {
  // Load test data
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = configs.value()[0];
  
  // Create TestEngine and run simulation
  steamrot::tests::TestEngine engine(*config);
  auto startup_result = engine.StartUp();
  REQUIRE(startup_result.has_value());
  
  // Get component from engine
  const auto &scene_manager = engine.GetSceneManager();
  const auto &scene = scene_manager.GetCurrentScene();
  const auto &pool = scene.GetEntities();
  
  auto &ui = steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(0, pool);
  
  // Check defaults
  REQUIRE(ui.m_active == false);
  REQUIRE(ui.m_ui_name.empty());
  REQUIRE(ui.m_is_visible == false);
}
```

**✅ AFTER: Direct Instantiation**

```cpp
// File: tests/unit/components/CUserInterface.test.cpp
#include "CUserInterface.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CUserInterface has correct defaults", "[unit][CUserInterface]") {
  steamrot::CUserInterface ui;
  
  REQUIRE(ui.m_active == false);
  REQUIRE(ui.m_ui_name.empty());
  REQUIRE(ui.m_is_visible == false);
}
```

**Benefits:**
- ✅ 90% less code
- ✅ 10x faster execution
- ✅ Stable - not affected by Logic class changes
- ✅ Clear failures - only breaks if CUserInterface changes

**Migration Steps:**

1. Remove `harness_runner.h` and `load_test_data_configs` includes
2. Remove TestEngine creation and startup code
3. Remove entity retrieval from engine
4. Directly instantiate the component
5. Delete associated test_data.json file (if it was only for this test)

---

### Scenario 2: Testing Component Field Assignment

**❌ BEFORE: Using test_data.json**

```cpp
// File: tests/unit/components/CGrimoireMachina.test.cpp
TEST_CASE("CGrimoireMachina stores fragments correctly", "[unit][CGrimoireMachina]") {
  auto configs = steamrot::tests::load_test_data_configs();
  const auto *config = configs.value()[0];
  
  steamrot::tests::TestEngine engine(*config);
  engine.StartUp();
  
  // Get component from engine
  auto &grimoire = /* ... get from engine ... */;
  
  REQUIRE(grimoire.m_fragments.size() == 2);
  REQUIRE(grimoire.m_fragments[0] == "fragment_a");
}
```

**✅ AFTER: Direct Instantiation**

```cpp
// File: tests/unit/components/CGrimoireMachina.test.cpp
TEST_CASE("CGrimoireMachina stores fragments correctly", "[unit][CGrimoireMachina]") {
  steamrot::CGrimoireMachina grimoire;
  grimoire.m_fragments = {"fragment_a", "fragment_b"};
  
  REQUIRE(grimoire.m_fragments.size() == 2);
  REQUIRE(grimoire.m_fragments[0] == "fragment_a");
  REQUIRE(grimoire.m_fragments[1] == "fragment_b");
}
```

**Migration Steps:**

1. Directly instantiate the component
2. Set fields directly instead of loading from TestEngine
3. Assert on the fields
4. Remove all TestEngine and harness infrastructure

---

### Scenario 3: Testing FlatBuffers → Component Configuration

**❌ BEFORE: Using test_data.json with TestEngine**

```cpp
// File: tests/unit/entity/FlatbuffersEntityConfigurator.test.cpp
TEST_CASE("Configurator sets UI component fields from FlatBuffers", 
          "[unit][configurator]") {
  auto configs = steamrot::tests::load_test_data_configs();
  const auto *config = configs.value()[0];
  
  steamrot::tests::TestEngine engine(*config);
  engine.StartUp();
  
  // Component was configured by TestEngine
  auto &ui = /* ... get from engine ... */;
  REQUIRE(ui.m_ui_name == "test_ui");
}
```

**✅ AFTER: Use entity_test_data.json (EntityCollectionFbs)**

```cpp
// File: tests/unit/entity/FlatbuffersEntityConfigurator.test.cpp

// Helper function to load simple entity data
auto LoadEntityTestData() {
  steamrot::PathProvider path_provider(steamrot::EnvironmentType::Test);
  std::filesystem::path data_path = 
    path_provider.GetObjectDirectoryPath(__FILE__) / "data" / "entity_test_data.bin";
  
  auto data_buffer = steamrot::LoadBinaryFile(data_path);
  const auto *entity_collection = 
    steamrot::GetEntityCollectionFbs(data_buffer.data());
  
  return std::make_pair(std::move(data_buffer), entity_collection);
}

TEST_CASE("Configurator sets UI component fields from FlatBuffers", 
          "[unit][configurator]") {
  auto [data, entity_collection] = LoadEntityTestData();
  
  steamrot::EventHandler event_handler;
  steamrot::FlatbuffersEntityConfigurator configurator(
      event_handler, *entity_collection);
  
  steamrot::EntityMemoryPool pool;
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, 20);
  
  // Configure entities
  auto result = configurator.ConfigureEntities(pool);
  REQUIRE(result.has_value());
  
  // Validate configuration
  auto &ui = steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(1, pool);
  REQUIRE(ui.m_ui_name == "test_ui");
  REQUIRE(ui.m_is_visible == true);
}
```

**Create simple entity data file:**

```json
// File: tests/unit/entity/data/entity_test_data.json
{
  "entity_memory_pool_size": 20,
  "entities": [
    {
      "index": 1,
      "c_user_interface": {
        "ui_name": "test_ui",
        "is_visible": true,
        "root_ui_element": {
          "base_data": {
            "position": { "x": 10.0, "y": 20.0 },
            "size": { "x": 100.0, "y": 50.0 },
            "children_active": false,
            "children": [],
            "layout": "Horizontal",
            "spacing_strategy": "None"
          }
        }
      }
    }
  ]
}
```

**Migration Steps:**

1. Create `entity_test_data.json` with just `EntityCollectionFbs` schema (not full TestDataConfig)
2. Create helper function to load the binary data
3. Create configurator directly (not through TestEngine)
4. Create empty EntityMemoryPool
5. Call configurator to configure entities
6. Assert on component fields
7. Delete old test_data.json file

**Benefits:**
- ✅ Still data-driven (JSON config)
- ✅ No game simulation overhead
- ✅ Tests configurator in isolation
- ✅ Not affected by Logic class changes

---

### Scenario 4: Testing Single Function Behavior

**❌ BEFORE: Using test_data.json**

```cpp
// File: tests/unit/logic/UIStateLogic.test.cpp
TEST_CASE("UIStateLogic updates UI visibility", "[unit][UIStateLogic]") {
  auto configs = steamrot::tests::load_test_data_configs();
  const auto *config = configs.value()[0];
  
  steamrot::tests::TestEngine engine(*config);
  engine.StartUp();
  engine.RunGame();
  
  // Check that visibility was updated
  auto &ui = /* ... get from engine ... */;
  REQUIRE(ui.m_is_visible == true);
}
```

**✅ AFTER: Direct Setup and Test**

```cpp
// File: tests/unit/logic/UIStateLogic.test.cpp
#include "UIStateLogic.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UIStateLogic updates UI visibility", "[unit][UIStateLogic]") {
  // Setup minimal test environment
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  
  auto &scene_context = fixture.GetSceneContext();
  auto &pool = scene_context.scene_entities;
  
  // Create entity with UI component
  steamrot::entity::memory::ResizeEntityMemoryPool(pool, 10);
  auto &ui = steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(0, pool);
  ui.m_active = true;
  ui.m_is_visible = false;  // Start invisible
  
  // Setup archetype
  steamrot::ArchetypeID archetype_id = 
    steamrot::GenerateArchetypeIDfromTypes<steamrot::CUserInterface>();
  scene_context.archetypes[archetype_id].AddEntity(0);
  
  // Create and run Logic
  steamrot::UIStateLogic logic(fixture.GetLogicContext());
  logic.RunLogic();
  
  // Assert
  REQUIRE(ui.m_is_visible == true);
}
```

**Migration Steps:**

1. Use TestFixture for minimal setup (not full TestEngine)
2. Manually create and configure entities
3. Set up archetypes manually
4. Instantiate Logic class directly
5. Call RunLogic() directly
6. Assert on results

**Benefits:**
- ✅ Tests Logic class in isolation
- ✅ No unnecessary simulation
- ✅ Clear what's being tested
- ✅ Fast execution

---

## General Migration Process

### Step 1: Identify What's Being Tested

Look at your test and determine:

- [ ] Is it testing a single component's construction/defaults?
- [ ] Is it testing a single function's behavior?
- [ ] Is it testing data configuration (FlatBuffers → Component)?
- [ ] Is it testing interaction between 2+ systems?

### Step 2: Choose Appropriate Approach

Based on Step 1:

| What's Being Tested | Use This Approach | Migration Scenario |
|---------------------|-------------------|-------------------|
| Component construction/defaults | Direct instantiation | Scenario 1 |
| Component field assignment | Direct instantiation | Scenario 2 |
| FlatBuffers configuration | entity_test_data.json | Scenario 3 |
| Single function behavior | TestFixture + direct calls | Scenario 4 |
| Multiple systems interacting | Keep test_data.json, move to integration/ | Not a unit test! |

### Step 3: Create New Test

Follow the appropriate migration scenario above.

### Step 4: Verify New Test

```bash
# Run the new test
ctest --preset Debug -R <test_name> --verbose

# Verify it passes
```

### Step 5: Remove Old Infrastructure

1. Delete old test code using TestEngine
2. If test_data.json file is no longer used by any test, delete it:
   ```bash
   rm tests/unit/<subsystem>/data/<test_name>.test_data.json
   rm tests/unit/<subsystem>/data/<test_name>.test_data.bin
   ```
3. Update CMakeLists.txt if needed to remove data file compilation

### Step 6: Commit Changes

```bash
git add tests/unit/<subsystem>/<TestFile>.test.cpp
git commit -m "Migrate <TestFile> from test_data.json to direct instantiation"
```

## Common Patterns

### Pattern 1: Component Test Helper

Create a helper for common component setup:

```cpp
// File: tests/unit/components/component_test_helpers.h
namespace steamrot::tests {

template<typename ComponentType>
ComponentType CreateDefaultComponent() {
  ComponentType component;
  // Common setup if needed
  return component;
}

template<typename ComponentType>
ComponentType CreateActiveComponent() {
  ComponentType component;
  component.m_active = true;
  return component;
}

} // namespace steamrot::tests
```

Usage:

```cpp
TEST_CASE("Component test", "[unit]") {
  auto ui = steamrot::tests::CreateActiveComponent<CUserInterface>();
  ui.m_ui_name = "test";
  
  REQUIRE(ui.m_active == true);
  REQUIRE(ui.m_ui_name == "test");
}
```

### Pattern 2: EntityMemoryPool Test Helper

Create helpers for common entity pool setups:

```cpp
// File: tests/unit/entity/entity_test_helpers.h
namespace steamrot::tests {

EntityMemoryPool CreatePoolWithUIEntity(size_t pool_size, 
                                         uint32_t entity_index,
                                         const std::string& ui_name);

EntityMemoryPool CreatePoolWithMultipleUIEntities(size_t count);

} // namespace steamrot::tests
```

### Pattern 3: Archetype Setup Helper

```cpp
// File: tests/unit/entity/archetype_test_helpers.h
namespace steamrot::tests {

void SetupArchetypeWithEntity(
    steamrot::ArchetypeManager& manager,
    steamrot::ArchetypeID archetype_id,
    uint32_t entity_id);

template<typename... Components>
void SetupArchetypeForEntity(
    steamrot::ArchetypeManager& manager,
    uint32_t entity_id) {
  auto archetype_id = steamrot::GenerateArchetypeIDfromTypes<Components...>();
  manager.AddEntity(entity_id, archetype_id);
}

} // namespace steamrot::tests
```

## Checklist: Is My Unit Test Appropriate?

Use this checklist after migrating (or when writing new tests):

- [ ] **Single Responsibility**: Test checks ONE thing
- [ ] **No TestEngine**: Unless testing TestEngine itself
- [ ] **No test_data.json**: Unless it's an integration/system test
- [ ] **Fast**: Runs in milliseconds, not seconds
- [ ] **Isolated**: Not affected by unrelated Logic class changes
- [ ] **Clear Failures**: When it fails, I know exactly what broke
- [ ] **Minimal Setup**: Only creates what's needed for the test
- [ ] **No Simulation**: Doesn't run game ticks (unless testing game simulation)

## File Organization After Migration

```
tests/
├── unit/
│   ├── components/
│   │   ├── CUserInterface.test.cpp         # Direct instantiation
│   │   ├── CGrimoireMachina.test.cpp       # Direct instantiation
│   │   └── component_test_helpers.h/cpp    # Reusable helpers
│   ├── entity/
│   │   ├── FlatbuffersEntityConfigurator.test.cpp  # Uses entity_test_data.json
│   │   ├── ArchetypeManager.test.cpp                # Direct instantiation
│   │   ├── data/
│   │   │   ├── entity_test_data.json       # Simple EntityCollectionFbs
│   │   │   └── entity_test_data.bin        # Compiled binary
│   │   └── entity_test_helpers.h/cpp       # Reusable helpers
│   └── logic/
│       ├── UIStateLogic.test.cpp           # TestFixture + direct calls
│       └── logic_test_helpers.h/cpp        # Reusable helpers
├── integration/
│   └── ui/
│       ├── ui_interaction.integration.test.cpp  # Uses test_data.json
│       └── data/
│           ├── button_click.test_data.json      # Full TestDataConfig
│           └── button_click.test_data.bin
└── system/
    └── crafting/
        ├── crafting_workflow.system.test.cpp    # Uses test_data.json
        └── data/
            ├── success_path.test_data.json      # Full TestDataConfig
            └── success_path.test_data.bin
```

## Real-World Example: Complete Migration

Let's walk through a complete migration example:

### BEFORE: Inappropriate use of test_data.json

**File: tests/unit/components/CUserInterface.test.cpp**

```cpp
#include "CUserInterface.h"
#include "harness_runner.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("CUserInterface default construction", "[unit][CUserInterface]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = GENERATE_COPY(from_range(configs.value()));
  
  steamrot::tests::TestEngine engine(*config);
  auto startup_result = engine.StartUp();
  REQUIRE(startup_result.has_value());
  
  const auto &scene_manager = engine.GetSceneManager();
  const auto &scene = scene_manager.GetCurrentScene();
  const auto &pool = scene.GetEntities();
  
  auto &ui = steamrot::entity::memory::GetComponent<steamrot::CUserInterface>(0, pool);
  
  REQUIRE(ui.m_active == false);
  REQUIRE(ui.m_ui_name.empty());
  REQUIRE(ui.m_is_visible == false);
}

TEST_CASE("CUserInterface can be activated", "[unit][CUserInterface]") {
  auto configs = steamrot::tests::load_test_data_configs();
  REQUIRE(configs.has_value());
  
  const auto *config = configs.value()[0];
  
  steamrot::tests::TestEngine engine(*config);
  engine.StartUp();
  
  auto &ui = /* ... get from engine ... */;
  ui.m_active = true;
  
  REQUIRE(ui.m_active == true);
}
```

**File: tests/unit/components/data/ui_defaults.test_data.json** (40+ lines of JSON)

### AFTER: Appropriate unit test

**File: tests/unit/components/CUserInterface.test.cpp**

```cpp
#include "CUserInterface.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CUserInterface default construction", "[unit][CUserInterface]") {
  steamrot::CUserInterface ui;
  
  REQUIRE(ui.m_active == false);
  REQUIRE(ui.m_ui_name.empty());
  REQUIRE(ui.m_is_visible == false);
}

TEST_CASE("CUserInterface can be activated", "[unit][CUserInterface]") {
  steamrot::CUserInterface ui;
  ui.m_active = true;
  
  REQUIRE(ui.m_active == true);
}

TEST_CASE("CUserInterface stores UI name", "[unit][CUserInterface]") {
  steamrot::CUserInterface ui;
  ui.m_ui_name = "test_panel";
  
  REQUIRE(ui.m_ui_name == "test_panel");
}
```

**Delete: tests/unit/components/data/ui_defaults.test_data.json**

**Results:**
- Before: 30+ lines of test code, 40+ lines of JSON, slow execution
- After: 15 lines of test code, no JSON, instant execution
- **80% reduction in code, 10x faster, more stable**

## FAQ

### Q: When should I still use test_data.json?

**A:** Only for integration/system/regression tests:
- Testing 2+ systems interacting
- Testing complete user workflows
- Testing bug reproductions that require simulation

### Q: Can I use entity_test_data.json for unit tests?

**A:** Yes, but only for testing FlatBuffers configuration:
- Testing FlatbuffersEntityConfigurator
- Testing data loading and parsing
- **NOT** for testing component behavior

### Q: What if I need a complex entity setup?

**A:** Create a helper function:

```cpp
// tests/unit/entity/entity_test_helpers.h
EntityMemoryPool CreatePoolWithComplexSetup() {
  EntityMemoryPool pool;
  // ... setup logic ...
  return pool;
}

// Then use it in tests
TEST_CASE("My test", "[unit]") {
  auto pool = CreatePoolWithComplexSetup();
  // ... test logic ...
}
```

### Q: My test needs game resources (AssetManager, EventHandler). What do I do?

**A:** Use TestFixture for minimal setup:

```cpp
#include "TestFixture.h"

TEST_CASE("My test", "[unit]") {
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  
  // Access game resources
  auto &event_handler = fixture.GetGameResources().event_handler;
  auto &asset_manager = fixture.GetGameResources().asset_manager;
  
  // Test logic
}
```

### Q: Can I run both old and new tests during migration?

**A:** Yes! Keep both until migration is complete:
1. Create new test with `_v2` suffix
2. Verify new test passes
3. Delete old test
4. Rename `_v2` test to original name

### Q: How do I know if migration was successful?

**A:** Check these indicators:
- [ ] Test runs faster (milliseconds vs seconds)
- [ ] Test has fewer dependencies (no harness includes)
- [ ] Test code is shorter and clearer
- [ ] Test still validates the same behavior
- [ ] Test doesn't break when unrelated Logic classes change

## Summary

**Key Principles:**

1. ✅ **Unit tests should be simple** - Direct instantiation, minimal setup
2. ✅ **test_data.json is for integration/system tests** - Multiple systems, simulations
3. ✅ **entity_test_data.json is for configuration tests** - FlatBuffers loading only
4. ✅ **Use helpers for common setup** - Reduce duplication
5. ✅ **Test one thing** - Single responsibility per test

**Migration Benefits:**

- ✅ 10x faster execution
- ✅ 80% less code
- ✅ More stable (not affected by unrelated changes)
- ✅ Clearer failures
- ✅ Easier to maintain

**Next Steps:**

1. Review your unit tests
2. Identify any using test_data.json
3. Apply appropriate migration scenario
4. Delete unused test_data.json files
5. Enjoy faster, more stable tests!

## See Also

- [TESTING_BEST_PRACTICES.md](TESTING_BEST_PRACTICES.md) - When to use each approach
- [ADDING_TEST_HELPERS_GUIDE.md](ADDING_TEST_HELPERS_GUIDE.md) - Creating reusable helpers
- [TEST_HARNESS_ANALYSIS.md](TEST_HARNESS_ANALYSIS.md) - Architecture details
