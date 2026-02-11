# Adding Logic Classes

This guide provides a comprehensive workflow for adding new Logic-derived classes to the SteamRot game engine.

## Table of Contents

- [Overview](#overview)
- [When to Create a Logic Class](#when-to-create-a-logic-class)
- [Logic System Architecture](#logic-system-architecture)
- [Step-by-Step Workflow](#step-by-step-workflow)
  - [Step 1: Create the Logic Class Files](#step-1-create-the-logic-class-files)
  - [Step 2: Register LogicType Enum](#step-2-register-logictype-enum)
  - [Step 3: Register with LogicFactory](#step-3-register-with-logicfactory)
  - [Step 4: Write Unit Tests](#step-4-write-unit-tests)
  - [Step 5: Update LogicFactory Tests](#step-5-update-logicfactory-tests)
  - [Step 6: (Optional) Add FlatBuffers Configuration](#step-6-optional-add-flatbuffers-configuration)
  - [Step 7: Build and Test](#step-7-build-and-test)
- [Complete Example](#complete-example)
- [Common Patterns](#common-patterns)
- [Best Practices](#best-practices)
- [Troubleshooting](#troubleshooting)

---

## Overview

Logic classes are the heart of the game's behavior system. They:
- Inherit from the abstract `Logic` base class
- Process entities based on their component composition (archetypes)
- Are organized by `LogicGrouping` (Collision, Render, Action, Movement)
- Execute in a specific order within their grouping
- Are created and configured by the `LogicFactory` for each scene

**Key Principle**: Components contain pure data, Logic classes contain all behavior.

---

## When to Create a Logic Class

Create a new Logic class when you need to:
- Add a new game system (e.g., AI, physics, animation)
- Implement scene-specific behavior
- Process entities with specific component combinations
- Handle input or events in a structured way
- Perform rendering operations

**Do NOT** add logic to Component classes - they must remain pure data containers.

---

## Logic System Architecture

### LogicGrouping Categories

Logic classes are organized into four groupings that execute in this order:

1. **Collision** - Spatial interactions (UI collision, physics collision)
2. **Action** - Input processing and event triggering
3. **Movement** - Position and velocity updates
4. **Render** - Drawing to the render texture

### Execution Order

Within each scene:
1. Logic classes execute in the order defined in `LogicFactory::Configure{Scene}Logics()`
2. Each Logic's `RunLogic()` → `ProcessLogic()` is called once per frame
3. Order matters - later Logic classes see changes from earlier ones

### Key Components

- **`Logic`** - Abstract base class all Logic inherits from
- **`LogicType`** - Enum identifying each Logic class type
- **`LogicGrouping`** - Category enum (Collision, Render, Action, Movement)
- **`LogicFactory`** - Factory that creates and configures Logic instances per scene
- **`SceneContext`** - Struct containing all dependencies Logic needs (entities, assets, events, etc.)

---

## Step-by-Step Workflow

### Step 1: Create the Logic Class Files

#### 1.1 Create Header File

**Location**: `src/logic/YourLogicName.h`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the YourLogicName class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Logic.h"
#include "LogicType.h"

namespace steamrot {

class YourLogicName : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Override method to encapsulate all logic for YourLogicName
  /////////////////////////////////////////////////
  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for YourLogicName.
  ///
  /// @param scene_context SceneContext object containing the context for the logic.
  /////////////////////////////////////////////////
  YourLogicName(const SceneContext scene_context);

  /////////////////////////////////////////////////
  /// @brief Returns the LogicType of this Logic class.
  ///
  /// @return Returns LogicType::YourLogicName
  /////////////////////////////////////////////////
  LogicType GetLogicType() const override {
    return LogicType::YourLogicName;
  }
};

} // namespace steamrot
```

**Note**: If your Logic is scene-specific or belongs to a subsystem, you can use a nested namespace:
```cpp
namespace steamrot::logic {
  class GrimoireMachinaRenderLogic : public Logic {
    // ...
  };
}
```

#### 1.2 Create Implementation File

**Location**: `src/logic/YourLogicName.cpp`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the YourLogicName class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "YourLogicName.h"
#include "archetypes.h"
#include "entity_memory.h"
#include "CYourComponent.h"
// Add other necessary includes

namespace steamrot {

/////////////////////////////////////////////////
YourLogicName::YourLogicName(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void YourLogicName::ProcessLogic() {

  // Generate archetype ID for the components you need
  std::set<size_t> entity_indexes =
      archetypes::GenerateEntityIndexesFromComponents<CYourComponent>(
          m_scene_context.archetypes, true);

  // Iterate through entities with the required components
  for (size_t entity_id : entity_indexes) {
    
    // Get component references
    CYourComponent &component = entity::memory::GetComponent<CYourComponent>(
        entity_id, m_scene_context.scene_entities);

    // Implement your logic here
    // Access SceneContext members as needed:
    // - m_scene_context.scene_texture (for rendering)
    // - m_scene_context.event_handler (for events)
    // - m_scene_context.asset_manager (for assets)
    // - m_scene_context.archetypes (for entity queries)
    // - etc.
  }
}

} // namespace steamrot
```

#### 1.3 Update CMakeLists.txt

**Location**: `src/logic/CMakeLists.txt`

Add your new files to the source list:

```cmake
add_library(logic
    Logic.cpp
    # ... existing files ...
    YourLogicName.cpp
    YourLogicName.h
)
```

---

### Step 2: Register LogicType Enum

#### 2.1 Add to LogicType Enum

**Location**: `src/types/core/LogicType.h`

Add your Logic type to the enum:

```cpp
enum class LogicType {
  None,
  Test,
  Dummy,
  UIRender,
  UIState,
  UIAction,
  UICollision,
  CraftingRender,
  GrimoireMachinaAction,
  GrimoireMachinaPositioning,
  GrimoireMachinaRender,
  YourLogicName,  // Add this
};
```

#### 2.2 Add to EnumNameLogicType Function

In the same file, add a case to the `EnumNameLogicType()` function:

```cpp
inline std::string EnumNameLogicType(LogicType logic_type) {
  switch (logic_type) {
  case LogicType::None:
    return "None";
  // ... existing cases ...
  case LogicType::YourLogicName:
    return "YourLogicName";
  default:
    return "INVALID_LOGIC_TYPE";
  }
}
```

---

### Step 3: Register with LogicFactory

The LogicFactory creates Logic instances for each scene. You need to update three methods.

**Location**: `src/logic/LogicFactory.cpp` and `LogicFactory.h`

#### 3.1 Add Include

At the top of `LogicFactory.cpp`:

```cpp
#include "YourLogicName.h"
```

#### 3.2 Add to CreateLogicObject Method

Add a case in the switch statement:

```cpp
std::expected<std::unique_ptr<Logic>, FailInfo>
LogicFactory::CreateLogicObject(LogicType logic_type) {
  
  std::unique_ptr<Logic> logic_ptr;
  switch (logic_type) {
  // ... existing cases ...
  case LogicType::YourLogicName:
    logic_ptr = std::make_unique<YourLogicName>(m_scene_context);
    break;
  default:
    return std::unexpected(
        FailInfo(FailMode::EnumValueNotHandled,
                 "LogicFactory::CreateLogicObject: unknown LogicType"));
  }
  return logic_ptr;
}
```

#### 3.3 Add to Scene Configuration Method

Choose the appropriate scene method (or create a new one) and add your Logic to the correct grouping:

**Example for CRAFTING scene:**

```cpp
std::expected<std::monostate, FailInfo>
LogicFactory::ConfigureCraftingLogics(LogicCollection &logic_collection) {
  ////// THE ORDER OF THE LOGIC CLASSES IS VERY IMPORTANT //////
  ////// DO NOT CHANGE UNLESS YOU KNOW WHAT YOU ARE DOING //////

  // Define the Logic types for each grouping
  static constexpr std::array collision_logic_types = {LogicType::UICollision};
  
  static constexpr std::array action_logic_types = {
      LogicType::UIAction,
      LogicType::UIState,
      LogicType::GrimoireMachinaAction,
      LogicType::YourLogicName  // Add to appropriate grouping
  };
  
  static constexpr std::array render_logic_types = {
      LogicType::UIRender,
      LogicType::GrimoireMachinaRender
  };
  
  static constexpr std::array movement_logic_types = {
      LogicType::GrimoireMachinaPositioning
  };

  // ... rest of the method (AddLogicsToCollection calls)
}
```

**Important**: The order in these arrays determines execution order! Place your Logic where it makes sense in the sequence.

---

### Step 4: Write Unit Tests

#### 4.1 Create Test File

**Location**: `tests/unit/logic/YourLogicName.test.cpp`

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for YourLogicName class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "YourLogicName.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("YourLogicName constructor", "[unit][YourLogicName]") {
  // Create test fixture with mock dependencies
  steamrot::tests::TestFixture test_fixture;
  
  // Construct Logic
  REQUIRE_NOTHROW(
      steamrot::YourLogicName(test_fixture.GetSceneContext())
  );
}

TEST_CASE("YourLogicName::ProcessLogic performs expected behavior",
          "[unit][YourLogicName]") {
  // Setup
  steamrot::tests::TestFixture test_fixture;
  auto scene_context = test_fixture.GetSceneContext();
  
  // TODO: Set up test entities and components in the fixture
  
  // Create Logic instance
  steamrot::YourLogicName logic(scene_context);
  
  // Execute
  REQUIRE_NOTHROW(logic.RunLogic());
  
  // Assert expected state changes
  // REQUIRE(...);
}

TEST_CASE("YourLogicName::GetLogicType returns correct type",
          "[unit][YourLogicName]") {
  steamrot::tests::TestFixture test_fixture;
  steamrot::YourLogicName logic(test_fixture.GetSceneContext());
  
  REQUIRE(logic.GetLogicType() == steamrot::LogicType::YourLogicName);
}
```

#### 4.2 Update Test CMakeLists.txt

**Location**: `tests/unit/logic/CMakeLists.txt`

Add your test file:

```cmake
add_executable(test_logic
    # ... existing test files ...
    YourLogicName.test.cpp
)
```

---

### Step 5: Update LogicFactory Tests

The LogicFactory has integration tests that verify the correct Logic instances are created for each scene.

**Location**: `tests/unit/logic/LogicFactory.test.cpp`

#### 5.1 Add Include

```cpp
#include "YourLogicName.h"
```

#### 5.2 Update Scene Test Case

Find or create the test case for your scene and update the assertions:

```cpp
TEST_CASE("LogicFactory::ProvideLogicCollection returns valid LogicCollection "
          "for SceneType_CRAFTING",
          "[unit][LogicFactory]") {
  
  steamrot::tests::TestFixture test_fixture;
  steamrot::LogicFactory logic_factory(test_fixture.GetSceneContext());
  
  auto result = logic_factory.ProvideLogicCollection(
      steamrot::SceneType::CRAFTING);
  
  REQUIRE(result.has_value());
  const auto &logic_collection = result.value();
  
  // Check Action grouping (example - adjust based on your grouping)
  auto action_it = logic_collection.find(steamrot::LogicGrouping::Action);
  REQUIRE(action_it != logic_collection.end());
  
  const auto &action_logics = action_it->second;
  REQUIRE(action_logics.size() == 4);  // Update count
  REQUIRE(dynamic_cast<steamrot::UIActionLogic *>(action_logics[0].get()));
  REQUIRE(dynamic_cast<steamrot::UIStateLogic *>(action_logics[1].get()));
  REQUIRE(dynamic_cast<steamrot::logic::GrimoireMachinaActionLogic *>(
      action_logics[2].get()));
  REQUIRE(dynamic_cast<steamrot::YourLogicName *>(action_logics[3].get()));
  
  // ... check other groupings ...
}
```

**Important**: 
- Update the size check to match the new number of Logic instances
- Verify the order matches what you defined in `LogicFactory::Configure{Scene}Logics()`
- Use `dynamic_cast` to verify the correct type at each position

---

### Step 6: (Optional) Add FlatBuffers Configuration

If your Logic needs runtime configuration (e.g., event subscriptions), add FlatBuffers support.

#### 6.1 Update FlatBuffers Schema

**Location**: `src/types/flatbuffers/logic/logic_config.fbs`

Add a field for your Logic config:

```fbs
table LogicConfigCollectionFbs {
  test_logic: LogicConfigFbs;
  grimoire_machina_action_logic: LogicConfigFbs;
  your_logic_name: LogicConfigFbs;  // Add this
}
```

#### 6.2 Update JSON Configuration File

**Location**: `data/scene/{scene_name}/logic_config.json`

Add configuration for your Logic:

```json
{
  "your_logic_name": {
    "subscriptions": [
      {
        "event_type": "YOUR_EVENT_TYPE",
        "response_event_type": "RESPONSE_EVENT_TYPE"
      }
    ]
  }
}
```

#### 6.3 Update FlatBuffers Provider

**Location**: `src/data_providers/FlatbuffersLogicConfigCollectionProvider.cpp`

Add code to parse your Logic's configuration:

```cpp
std::expected<LogicConfigCollection, FailInfo>
FlatbuffersLogicConfigCollectionProvider::CreateLogicConfigCollection() {
  
  LogicConfigCollection collection;
  
  // ... existing logic configs ...
  
  // Your Logic config
  if (root_table->your_logic_name()) {
    LogicConfig config;
    auto subscribers_result = ParseSubscribers(
        root_table->your_logic_name()->subscriptions());
    if (!subscribers_result) {
      return std::unexpected(subscribers_result.error());
    }
    config.m_subscribers = subscribers_result.value();
    collection[LogicType::YourLogicName] = config;
  }
  
  return collection;
}
```

**Note**: Configuration is optional. Many Logic classes don't need it and work fine without any FlatBuffers setup.

---

### Step 7: Build and Test

#### 7.1 Build the Project

```bash
# Configure
cmake --preset Debug

# Build
cmake --build --preset Debug
```

#### 7.2 Run Tests

```bash
# Run all tests
ctest --preset Debug

# Run only your Logic tests
ctest --preset Debug -R YourLogicName

# Run LogicFactory tests
ctest --preset Debug -R LogicFactory
```

#### 7.3 Verify Integration

Run the game and verify your Logic executes in the appropriate scene:
- Check console output for any errors
- Verify behavior matches expectations
- Test interaction with other Logic classes

---

## Complete Example

Here's a complete example of a simple Logic class that changes UI colors based on time.

### Header (`src/logic/TimedColorLogic.h`)

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the TimedColorLogic class.
/////////////////////////////////////////////////

#pragma once

#include "Logic.h"
#include "LogicType.h"

namespace steamrot {

class TimedColorLogic : public Logic {

private:
  void ProcessLogic() override;
  float m_elapsed_time{0.0f};

public:
  TimedColorLogic(const SceneContext scene_context);
  
  LogicType GetLogicType() const override {
    return LogicType::TimedColor;
  }
};

} // namespace steamrot
```

### Implementation (`src/logic/TimedColorLogic.cpp`)

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TimedColorLogic class.
/////////////////////////////////////////////////

#include "TimedColorLogic.h"
#include "archetypes.h"
#include "entity_memory.h"
#include "CUserInterface.h"
#include <cmath>

namespace steamrot {

/////////////////////////////////////////////////
TimedColorLogic::TimedColorLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void TimedColorLogic::ProcessLogic() {
  
  // Update elapsed time
  m_elapsed_time += m_scene_context.delta_time;
  
  // Calculate color intensity (oscillates between 128 and 255)
  float intensity = 128.0f + 127.0f * std::sin(m_elapsed_time);
  
  // Get all entities with CUserInterface component
  std::set<size_t> entity_indexes =
      archetypes::GenerateEntityIndexesFromComponents<CUserInterface>(
          m_scene_context.archetypes, true);
  
  // Update each UI element's color
  for (size_t entity_id : entity_indexes) {
    CUserInterface &ui = entity::memory::GetComponent<CUserInterface>(
        entity_id, m_scene_context.scene_entities);
    
    if (ui.m_visible && ui.m_root_element) {
      // Update color (example - adjust based on your UI element structure)
      ui.m_root_element->m_fill_color.r = static_cast<sf::Uint8>(intensity);
    }
  }
}

} // namespace steamrot
```

### Test (`tests/unit/logic/TimedColorLogic.test.cpp`)

```cpp
#include "TimedColorLogic.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TimedColorLogic constructor", "[unit][TimedColorLogic]") {
  steamrot::tests::TestFixture test_fixture;
  REQUIRE_NOTHROW(
      steamrot::TimedColorLogic(test_fixture.GetSceneContext())
  );
}

TEST_CASE("TimedColorLogic::GetLogicType", "[unit][TimedColorLogic]") {
  steamrot::tests::TestFixture test_fixture;
  steamrot::TimedColorLogic logic(test_fixture.GetSceneContext());
  REQUIRE(logic.GetLogicType() == steamrot::LogicType::TimedColor);
}
```

---

## Common Patterns

### Archetype-Based Entity Processing

Most Logic classes process entities with specific components:

```cpp
void YourLogic::ProcessLogic() {
  // Get entities with specific components
  std::set<size_t> entity_indexes =
      archetypes::GenerateEntityIndexesFromComponents<CComponent1, CComponent2>(
          m_scene_context.archetypes, true);
  
  for (size_t entity_id : entity_indexes) {
    CComponent1 &comp1 = entity::memory::GetComponent<CComponent1>(
        entity_id, m_scene_context.scene_entities);
    CComponent2 &comp2 = entity::memory::GetComponent<CComponent2>(
        entity_id, m_scene_context.scene_entities);
    
    // Process entity
  }
}
```

### Rendering Logic

Rendering Logic draws to the scene's render texture:

```cpp
void RenderLogic::ProcessLogic() {
  std::set<size_t> entity_indexes =
      archetypes::GenerateEntityIndexesFromComponents<CRenderable>(
          m_scene_context.archetypes, true);
  
  for (size_t entity_id : entity_indexes) {
    CRenderable &renderable = entity::memory::GetComponent<CRenderable>(
        entity_id, m_scene_context.scene_entities);
    
    // Draw to scene texture
    m_scene_context.scene_texture.draw(renderable.m_sprite);
  }
}
```

### Event-Driven Logic

Action Logic often creates events:

```cpp
void ActionLogic::ProcessLogic() {
  // Check conditions
  if (should_trigger_event) {
    // Create event
    EventPacket event;
    event.event_type = EventType::YOUR_EVENT;
    event.entity_id = entity_id;
    
    // Add to event handler
    m_scene_context.event_handler.AddEvent(event);
  }
}
```

### Asset Access

Access game assets through the AssetManager:

```cpp
void YourLogic::ProcessLogic() {
  auto asset_result = m_scene_context.asset_manager.GetAsset();
  if (!asset_result.has_value()) {
    // Handle error
    return;
  }
  
  Asset &asset = *asset_result.value();
  // Use asset
}
```

---

## Best Practices

### Design

1. **Single Responsibility**: Each Logic class should handle one specific concern
2. **Archetype Filtering**: Only process entities with required components
3. **Early Returns**: Skip entities that don't meet processing criteria
4. **Immutable Access**: Don't modify entities outside your archetype's components

### Code Style

1. **Constructor**: Always call base `Logic(scene_context)` constructor
2. **GetLogicType**: Override and return the correct `LogicType` enum value
3. **ProcessLogic**: Keep this private, only `RunLogic()` should be public
4. **Member Variables**: Prefix with `m_`, store state that persists between frames
5. **Documentation**: Use Doxygen-style comments with visual dividers

### Performance

1. **Minimize Archetype Queries**: Query once, iterate once per frame
2. **Cache Frequently Used Data**: Store in member variables if accessed every frame
3. **Avoid Heap Allocations**: Prefer stack allocation in hot paths
4. **Early Exit**: Skip invisible/inactive entities early

### Testing

1. **Test Construction**: Verify Logic constructs without errors
2. **Test Behavior**: Verify `ProcessLogic()` produces expected state changes
3. **Test Edge Cases**: Empty archetypes, null components, boundary conditions
4. **Test Integration**: Verify correct placement in `LogicFactory`

### Error Handling

1. **Check Optional Results**: Always check `std::expected` return values
2. **Graceful Degradation**: Continue processing other entities on single-entity errors
3. **Log Warnings**: Use appropriate logging for non-critical issues
4. **Let Exceptions Propagate**: Don't catch exceptions in Logic - let them bubble up

---

## Troubleshooting

### Logic Not Executing

**Problem**: Your Logic's `ProcessLogic()` never runs.

**Solutions**:
- ✅ Verify Logic is added to `LogicFactory::CreateLogicObject()`
- ✅ Verify Logic is added to scene's configuration method
- ✅ Check that entities exist with required components (archetype query returns results)
- ✅ Verify scene type is correct when calling `ProvideLogicCollection()`

### Wrong Execution Order

**Problem**: Your Logic runs at the wrong time relative to other Logic.

**Solutions**:
- ✅ Check order in `Configure{Scene}Logics()` array - that's the execution order
- ✅ Verify you're in the correct `LogicGrouping` (Collision, Action, Movement, Render)
- ✅ Remember groupings execute in order: Collision → Action → Movement → Render

### Test Failures: "dynamic_cast returned null"

**Problem**: `LogicFactory.test.cpp` fails with null pointer from `dynamic_cast`.

**Solutions**:
- ✅ Verify Logic is included in `LogicFactory.cpp`
- ✅ Check you're casting to the correct type (including namespace)
- ✅ Verify the index in test matches the order in `Configure{Scene}Logics()`
- ✅ Update test size assertions to match actual number of Logic instances

### Build Errors: "undefined reference"

**Problem**: Linker errors about undefined references to your Logic class.

**Solutions**:
- ✅ Add `.cpp` file to `src/logic/CMakeLists.txt`
- ✅ Rebuild from scratch: `cmake --build --preset Debug --clean-first`
- ✅ Verify constructor and `ProcessLogic()` are implemented in `.cpp` file

### Components Not Found

**Problem**: `GenerateEntityIndexesFromComponents` returns empty set.

**Solutions**:
- ✅ Verify entities exist in the scene with the components you're querying
- ✅ Check components are marked active on entities
- ✅ Ensure archetypes are updated (happens automatically, but check if manually managing)
- ✅ Add debug logging to verify entity/component state

### FlatBuffers Configuration Not Loading

**Problem**: Logic config from JSON isn't being applied.

**Solutions**:
- ✅ Verify schema is updated in `logic_config.fbs`
- ✅ Rebuild to regenerate FlatBuffers headers
- ✅ Check JSON file path matches scene directory structure
- ✅ Verify parsing logic in `FlatbuffersLogicConfigCollectionProvider`
- ✅ Check for FlatBuffers parsing errors in console output

---

## Summary Checklist

When adding a new Logic class, complete these steps:

- [ ] Create header file (`src/logic/YourLogic.h`)
- [ ] Create implementation file (`src/logic/YourLogic.cpp`)
- [ ] Update `src/logic/CMakeLists.txt`
- [ ] Add to `LogicType` enum (`src/types/core/LogicType.h`)
- [ ] Add to `EnumNameLogicType()` function
- [ ] Include in `LogicFactory.cpp`
- [ ] Add case to `CreateLogicObject()`
- [ ] Add to scene's `Configure{Scene}Logics()` method
- [ ] Create unit test file (`tests/unit/logic/YourLogic.test.cpp`)
- [ ] Update `tests/unit/logic/CMakeLists.txt`
- [ ] Update `LogicFactory.test.cpp` integration tests
- [ ] (Optional) Add FlatBuffers schema field
- [ ] (Optional) Update JSON configuration file
- [ ] (Optional) Update FlatBuffers provider
- [ ] Build project
- [ ] Run unit tests
- [ ] Run LogicFactory tests
- [ ] Test in-game behavior

---

## Additional Resources

- **Logic Base Class**: `src/logic/Logic.h` and `Logic.cpp`
- **LogicFactory**: `src/logic/LogicFactory.h` and `LogicFactory.cpp`
- **SceneContext**: `src/context/SceneContext.h`
- **Example Logic Classes**: 
  - `src/logic/UIActionLogic.cpp` - Action Logic example
  - `src/logic/UIRenderLogic.cpp` - Render Logic example
  - `src/logic/GrimoireMachinaPositioningLogic.cpp` - Movement Logic example
  - `src/logic/UICollisionLogic.cpp` - Collision Logic example
- **Architecture Documentation**: `documentation/architecture/LOGIC_SYSTEM.md`
- **Testing Guide**: `documentation/testing/TESTING_OVERVIEW.md`

---

**Questions or Issues?** Refer to existing Logic classes for examples or consult the architecture documentation.
