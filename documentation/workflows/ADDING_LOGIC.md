# Adding Logic Classes

[← Back to Documentation](../README.md) | [Workflows Overview](../README.md#workflows)

Logic classes are responsible for changing the game state. Each Logic class represents a specific type of game system behavior (collision, rendering, actions, etc.).

**Related Documentation:**
- [Logic System Architecture](../architecture/LOGIC_SYSTEM.md) - System design
- [Testing Overview](../testing/TESTING_OVERVIEW.md) - Test-driven development
- [Examples](../examples/) - ExampleLogic code samples

---

### Adding Logic

Logic classes are responsible for changing the game state. Each Logic class
represents a specific type of game system behavior (collision, rendering,
actions, etc.). Logic classes are organized by **LogicType** and multiple Logic
instances can exist for each type within a scene.

**Important**: Follow a Test-Driven Development (TDD) approach when creating new
Logic classes. Write tests first to define expected behavior, then implement the
Logic class to pass those tests.

**See Also**: Complete example files with all patterns demonstrated can be found
in `documentation/examples/` (ExampleLogic.h, ExampleLogic.cpp, and
ExampleLogic.test.cpp).

#### Step 1: Define the Logic Class (Header)

Create the header file in `src/logic/`:

**File: `src/logic/NewLogic.h`**

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the NewLogic class.
/////////////////////////////////////////////////

#pragma once

#include "Logic.h"

namespace steamrot {

class NewLogic : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Override method to encapsulate all logic for NewLogic
  /////////////////////////////////////////////////
  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for NewLogic.
  ///
  /// @param logic_context LogicContext object containing the context for the
  /// logic.
  /////////////////////////////////////////////////
  NewLogic(const LogicContext logic_context);
};

} // namespace steamrot
```

**Key Requirements:**
- Inherit from `Logic` abstract class
- Override `ProcessLogic()` private method
- Constructor takes `const LogicContext` parameter
- Use visual dividers (`/////////////////////////////////////////////////`)
- Add Doxygen documentation

#### Step 2: Implement the Logic Class

Create the implementation file in `src/logic/`:

**File: `src/logic/NewLogic.cpp`**

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the NewLogic class.
/////////////////////////////////////////////////

#include "NewLogic.h"
#include "ArchetypeHelpers.h"
#include "ArchetypeManager.h"
#include "CYourComponent.h"
#include "emp_helpers.h"

namespace steamrot {

/////////////////////////////////////////////////
NewLogic::NewLogic(const LogicContext logic_context)
    : Logic(logic_context) {}

/////////////////////////////////////////////////
void NewLogic::ProcessLogic() {

  // Generate archetype ID for the components you need
  ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CYourComponent>();

  // Find the archetype in the context
  const auto it = m_logic_context.archetypes.find(archetype_id);
  
  // If archetype doesn't exist, skip processing
  if (it != m_logic_context.archetypes.end()) {

    // Get the archetype from the map
    const Archetype &archetype = it->second;

    // Process each entity in the archetype
    for (size_t entity_id : archetype) {

      // Get the component for this entity
      CYourComponent &component = emp_helpers::GetComponent<CYourComponent>(
          entity_id, m_logic_context.scene_entities);

      // Perform your logic here
      // Access other context members as needed:
      // - m_logic_context.scene_texture (for rendering)
      // - m_logic_context.game_window (for window info)
      // - m_logic_context.asset_manager (for assets)
      // - m_logic_context.event_handler (for events)
      // - m_logic_context.mouse_position (for input)
    }
  }
}

} // namespace steamrot
```

**Implementation Pattern:**
1. Call base class constructor with `LogicContext`
2. In `ProcessLogic()`, generate archetype IDs for needed components
3. Check if archetype exists in context
4. Iterate through entities in archetype
5. Get components and perform logic operations
6. Use `m_logic_context` members to access game state

#### Step 3: Write Unit Tests (TDD Approach)

Create or update test file in `tests/logic/`:

**File: `tests/logic/NewLogic.test.cpp`**

```cpp
/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for NewLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "NewLogic.h"
#include "TestFixture.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("NewLogic::NewLogic Constructor", "[unit][NewLogic]") {
  // Arrange - Create test fixture
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  
  // Act & Assert - Instantiate NewLogic
  steamrot::NewLogic new_logic(fixture.GetSceneContext());
  SUCCEED("NewLogic instantiated successfully");
}

TEST_CASE("NewLogic::ProcessLogic performs expected logic", "[unit][NewLogic]") {
  // Arrange
  steamrot::tests::TestFixture fixture;
  fixture.Initialize();
  
  // Get context references
  auto &scene_context = fixture.GetSceneContext();
  auto &game_context = fixture.GetGameContext();
  
  // Set up test data (entities, components, etc.)
  // ...
  
  // Create Logic instance
  steamrot::NewLogic new_logic(scene_context);
  
  // Act - Run the logic
  new_logic.RunLogic();
  
  // Assert - Verify expected state changes
  // REQUIRE(...);
}
```

**Add to `tests/logic/CMakeLists.txt`:**

```cmake
add_executable(test_logic
  LogicFactory.test.cpp
  UIActionLogic.test.cpp
  # ... existing tests ...
  NewLogic.test.cpp  # Add this
)
```

**Test Patterns:**
- Test construction without errors
- Test logic execution with valid data
- Test edge cases (empty archetypes, null components, etc.)
- Test interaction with other systems (events, rendering, etc.)
- Use `TestFixture` to provide mock dependencies
- Use `CAPTURE()` macro to debug test failures

#### Step 4: Update LogicFactory Tests (TDD)

Update `tests/logic/LogicFactory.test.cpp` to expect the new Logic class:

```cpp
TEST_CASE("LogicFactory creates correct Logic instances for YourScene",
          "[unit][LogicFactory]") {

  steamrot::tests::TestFixture fixture(
      steamrot::SceneType::SceneType_YOUR_SCENE);
  fixture.Initialize();
  
  steamrot::LogicFactory logic_factory(
      steamrot::SceneType::SceneType_YOUR_SCENE,
      fixture.GetSceneContext());

  auto logic_map_result = logic_factory.CreateLogicMap();
  if (!logic_map_result.has_value()) {
    FAIL("LogicFactory failed to create logic map: " +
         logic_map_result.error().message);
  }

  steamrot::LogicCollection &logic_collection = logic_map_result.value();

  steamrot::tests::CheckStaticLogicCollections(
      logic_collection, steamrot::SceneType::SceneType_YOUR_SCENE);
}
```

#### Step 5: Update LogicFactory Implementation

Add the new Logic class to `src/logic/LogicFactory.cpp`:

**Add include at top:**
```cpp
#include "NewLogic.h"
```

**Add to appropriate Create method** (e.g., `CreateActionLogics()`, `CreateRenderLogics()`, etc.):

```cpp
std::expected<LogicVector, FailInfo> LogicFactory::CreateActionLogics() {

  LogicVector action_logics;

  switch (m_scene_type) {
  case SceneType::SceneType_TITLE: {
    action_logics.push_back(std::make_unique<UIActionLogic>(m_logic_context));
    // Add your new Logic class here if needed for TITLE scene
    break;
  }
  case SceneType::SceneType_YOUR_SCENE: {
    action_logics.push_back(std::make_unique<NewLogic>(m_logic_context));
    break;
  }
  // ... other cases ...
  default:
    return std::unexpected(FailInfo{FailMode::NonExistentEnumValue,
                                    "Unsupported scene type for action logic"});
  }
  return action_logics;
}
```

**Important**: Order matters! Logic classes are executed in the order they're added to the vector.

#### Step 6: Update Test Helper Functions

Update `tests/logic/logic_helpers.cpp` to validate the new Logic class:

**Add include:**
```cpp
#include "NewLogic.h"
```

**Update `CheckStaticLogicCollections()` function:**

```cpp
void CheckStaticLogicCollections(const steamrot::LogicCollection &collection,
                                 const steamrot::SceneType &scene_type) {
  switch (scene_type) {
  case steamrot::SceneType::SceneType_YOUR_SCENE: {
    // Check map size
    REQUIRE(collection.size() == 3);
    
    // Check LogicType keys exist
    REQUIRE(collection.find(steamrot::LogicType::Action) != collection.end());
    REQUIRE(collection.find(steamrot::LogicType::Collision) != collection.end());
    REQUIRE(collection.find(steamrot::LogicType::Render) != collection.end());
    
    // Validate Action logics
    const steamrot::LogicVector &action_logics =
        collection.at(steamrot::LogicType::Action);
    REQUIRE(action_logics.size() == 2);  // Updated count
    REQUIRE(dynamic_cast<steamrot::NewLogic *>(action_logics[0].get()));
    REQUIRE(dynamic_cast<steamrot::UIActionLogic *>(action_logics[1].get()));
    
    // Validate other logic types...
    break;
  }
  // ... other cases ...
  }
}
```

**Validation checks:**
- Verify correct number of Logic instances per LogicType
- Verify correct Logic class types using `dynamic_cast`
- Verify correct order of Logic instances
- Add a case for each SceneType that uses the Logic class

#### Step 7: Build and Test

```bash
# Configure the project (if not already done)
cmake --preset Debug

# Build the project
cmake --build --preset Debug

# Run all tests
ctest --preset Debug

# Run specific Logic tests
ctest --preset Debug -R logic
```

#### Common Patterns and Best Practices

**1. Archetype-Based Processing:**
```cpp
// Generate archetype for multiple components
ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CComponent1, CComponent2>();

// Check if archetype exists
const auto it = m_logic_context.archetypes.find(archetype_id);
if (it != m_logic_context.archetypes.end()) {
  // Process entities...
}
```

**2. Component Access:**
```cpp
// Get component from entity
CYourComponent &component = emp_helpers::GetComponent<CYourComponent>(
    entity_id, m_logic_context.scene_entities);
```

**3. Rendering Logic Pattern:**
```cpp
void YourRenderLogic::ProcessLogic() {
  // Draw to scene texture
  m_logic_context.scene_texture.draw(drawable);
}
```

**4. Collision Logic Pattern:**
```cpp
void YourCollisionLogic::ProcessLogic() {
  // Check mouse position against element bounds
  if (element.position.x <= m_logic_context.mouse_position.x &&
      element.position.y <= m_logic_context.mouse_position.y) {
    element.is_mouse_over = true;
  }
}
```

**5. Action Logic Pattern:**
```cpp
void YourActionLogic::ProcessLogic() {
  // Process events and trigger responses
  if (should_trigger_action) {
    m_logic_context.event_handler.AddEvent(event_packet);
  }
}
```

#### LogicType Categories

Logic classes are organized by type:
- **Collision**: Handle spatial interactions (UI collision, physics collision)
- **Render**: Draw to render texture (UI rendering, entity rendering)
- **Action**: Process input and trigger events (UI actions, player actions)
- **Movement**: Update entity positions (player movement, AI movement)

**Note**: Keep systems focused. If a Logic class grows too large, consider
splitting it into multiple smaller Logic classes within the same LogicType.

#### Testing Strategy

1. **Unit Tests**: Test Logic class in isolation
   - Constructor
   - ProcessLogic() with various input states
   - Edge cases and error conditions

2. **Integration Tests**: Test Logic with LogicFactory
   - Verify Logic is created for correct scenes
   - Verify correct order in LogicVector
   - Verify correct LogicType assignment

3. **System Tests**: Test Logic with full game context
   - Use TestFixture with appropriate SceneType for realistic scenarios
   - Test interaction with other Logic classes
   - Test component state changes

#### Debugging Tips

- Use `CAPTURE()` in tests to output variable values on failure
- Check archetype exists before accessing entities
- Verify component data is properly initialized in test setup
- Run Logic tests individually during development: `./test_logic "[YourLogic]"`
- Use `std::cout` for debugging (will appear in test output)
