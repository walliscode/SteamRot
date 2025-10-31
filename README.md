# SteamRot game structure

<!--toc:start-->

- [SteamRot game structure](#steamrot-game-structure)
  - [src structure](#src-structure)
    - [Data](#data)
    - [data_handlers](#datahandlers)
    - [PathProvider](#pathprovider)
  - [Error/Exception handling](#errorexception-handling)
    - [Exceptions handling](#exceptions-handling)
    - [Error handling](#error-handling)
  - [Game Running](#game-running)
    - [RunGame](#rungame)
    - [UpdateSystems](#updatesystems)
  - [Workflows](#workflows)
    - [Adding/Modifying Components](#addingmodifying-components)
      - [Creating Components](#creating-components)
      - [Creating Flatbuffers data for Components](#creating-flatbuffers-data-for-components)
      - [Configuring and testing the Component](#configuring-and-testing-the-component)
    - [UI Elements](#ui-elements)
      - [Adding New Element types](#adding-new-element-types)
    - [Adding Logic](#adding-logic)
      - [Logic classes](#logic-classes)
      - [LogicFactory](#logicfactory)
    - [Actions](#actions)
      - [Action Generation](#action-generation)
      - [Action Registration](#action-registration)
  - [Classes](#classes)
    - [Logic Class](#logic-class)
    - [LogicFactory](#logicfactory)
  - [Testing](#testing)
    - [Test Directory Structure](#test-directory-structure)
    - [Test Classification](#test-classification)
    - [Running Tests](#running-tests)
    - [Writing Tests](#writing-tests)
    - [Test-Driven Development (TDD) in SteamRot](#test-driven-development-tdd-in-steamrot)
      - [TDD Workflow](#tdd-workflow)
      - [Using Reusable Test Infrastructure](#using-reusable-test-infrastructure)
      - [TDD Examples for Common Scenarios](#tdd-examples-for-common-scenarios)
      - [TDD Best Practices for SteamRot](#tdd-best-practices-for-steamrot)
      - [Testing Checklist](#testing-checklist)
    - [user_interface](#user_interface)
      - [Testing UI Elements](#testing-ui-elements)
  - [Style Guide](#style-guide)
    - [File Naming Conventions](#file-naming-conventions)
    - [Formatting](#formatting)
      - [Spacing between functions](#spacing-between-functions)
      - [Funtionality brief](#funtionality-brief)
  <!--toc:end-->

## src structure

Structure of the project is derived and heavily influenced by the
[Pitchfork](https://github.com/vector-of-bool/pitchfork) repo and more
specifically the information laid out in the
[spec.bs](https://github.com/vector-of-bool/pitchfork/blob/develop/data/spec.bs)

### Data

All non-code data is stored here e.g. images, json files e.t.c. Subsequent
folders will be names after purpose as opposed to data type e.g prefer `scene/`
over `json/`.

### data_handlers

This will deal with interfacing with files and providing data objects to the
game

### PathProvider

All paths to data files will be provided by the PathProvider class. This is
configured using cmake to provide the absolute source directory.

The EnvironmentType is provided once per run time to dictate which data
directory to use and then cannot be configured again.

## Error/Exception handling

In general, we want the program to fail if it encounters an exception. We do not
want to propagate failures further into the code (failure to load assets
e.t.c.). However, we want to fail gracefully and give the user a good experience
upon failure, not just a program crashing.

### Exceptions handling

We will let exceptions propagate up through the stack and _not_ use try/catch
statements. Except for the one try/catch statement.

This try/catch statement will be at the top game loop level and move us out of
the general game loop and into some kind of Error screen, where we will display
the error information, actionable info and the option to quit/reload. The goal
will to keep the dependencies small so that further errors can't be generated in
the Error scene (maybe self contained assets)

### Error handling

Where we expect a certain outcome from code but a runtime dependency could cause
a bug (such as a required file being missing) we will use std::expected to
indicate intent and then provide a mechanism for handling the error.

The current method will be to have an ErrorHandler namespace, this takes in an
Error object and processes it depending on severity e.g. critical errors will
throw an exception which then gets call backed up the stack and handled like
other exceptions.

## Game Running

The Game Engine (and thus the SteamRot game) is initiated by creating a
GameEngine object and calling the [RunGame](#rungame) method.

### RunGame

Still within the GameEngine object, the RunGame method creates a while loop (the
game loop) that will run until a simulation limit is reached or the user quits
the game.

The game loop calls a series of methods that will update the game state:

1. sUserInput: handles external user input (such as with a mouse or keyboard).
1. [UpdateSystems](#updatesystems): updates all internal logic that affect the
   game state.
1. sRender: draws the game state to the screen (handled by the display manager).

### UpdateSystems

The is a GameEngine method that calls the general systems for updating the game
state. This currently just calls the UpdateScenes method from the SceneManager
class. I'm predicting that this will be expanded to include other systems in the
future, but if not this could be collapsed down to just a call to the
SceneManager.

The UpdateScenes function will also be responsible for the logic deciding which
vector of scenes to update

## Workflows

### Adding/Modifying Components

This workflow guides you through adding a new component to the game engine. 
Components are pure data containers that inherit from the `Component` struct and
are designed to hold no logic.

#### Step 1: Creating the Component Struct

1. Create header and source files in `src/components/` directory:
   - Header file: `CNewComponent.h`
   - Source file: `CNewComponent.cpp`

2. The component struct should:
   - Inherit from `Component` struct
   - Have a `C` prefix (e.g., `CNewComponent`)
   - Be default-constructible (provide default values for all member variables)
   - Contain only data members (no logic/methods except `GetComponentRegisterIndex()`)
   - Use `m_` prefix for member variables

**Example Component Header (`src/components/CNewComponent.h`):**

```cpp
////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of the CNewComponent class.
////////////////////////////////////////////////////////////

#pragma once

#include "Component.h"
#include <string>
#include <vector>

namespace steamrot {

struct CNewComponent : public Component {
  CNewComponent() = default;

  ////////////////////////////////////////////////////////////
  /// @brief Description of the data member
  ////////////////////////////////////////////////////////////
  std::string m_component_name{"default_name"};

  ////////////////////////////////////////////////////////////
  /// @brief Another data member example
  ////////////////////////////////////////////////////////////
  int m_value{0};

  ////////////////////////////////////////////////////////////
  /// @brief Get the position of the Component in the Component Register.
  ///
  /// @return index of the component in the component register
  ////////////////////////////////////////////////////////////
  size_t GetComponentRegisterIndex() const override;
};
} // namespace steamrot
```

**Example Component Source (`src/components/CNewComponent.cpp`):**

```cpp
////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CNewComponent class.
////////////////////////////////////////////////////////////

#include "CNewComponent.h"
#include "containers.h"

namespace steamrot {

////////////////////////////////////////////////////////////
size_t CNewComponent::GetComponentRegisterIndex() const {
  return TupleTypeIndex<CNewComponent, ComponentRegister>;
}

} // namespace steamrot
```

#### Step 2: Register the Component

Add the new component to the `ComponentRegister` tuple in `src/components/containers.h`:

```cpp
typedef std::tuple<CMeta, CUserInterface, CMachinaForm, CGrimoireMachina, CNewComponent>
    ComponentRegister;
```

**Important:** The order matters for indexing, so add new components at the end.

Also add the component header to the includes section:

```cpp
#include "CNewComponent.h"
```

#### Step 3: Create FlatBuffers Schema

1. Create a new `.fbs` file in `src/flatbuffers_headers/` directory:
   - File name: `new_component.fbs`
   - Table name: `NewComponentData` (add `Data` suffix)

**Example Schema (`src/flatbuffers_headers/new_component.fbs`):**

```fbs
namespace steamrot;

table NewComponentData {
  component_name: string;
  value: int;
}
```

2. Add the schema to `src/flatbuffers_headers/generate_flatbuffers_headers.cmake`:

```cmake
set(schema_files
    ${CMAKE_CURRENT_SOURCE_DIR}/ui_style.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/scenes.fbs
    # ... existing schemas ...
    ${CMAKE_CURRENT_SOURCE_DIR}/new_component.fbs  # Add this line
)
```

3. Include the schema in `src/flatbuffers_headers/entities.fbs`:

```fbs
include "user_interface.fbs";
include "grimoire_machina.fbs";
include "new_component.fbs";  // Add this line
namespace steamrot;

table EntityData{
  index: uint32;
  c_user_interface: UserInterfaceData;
  c_grimoire_machina: GrimoireMachinaData;
  c_new_component: NewComponentData;  // Add this line
}
```

4. Rebuild the project to generate FlatBuffers headers:

```bash
cmake --build --preset Debug
```

This generates `new_component_generated.h` automatically.

#### Step 4: Create ConfigureComponent Method (TDD Approach)

1. **Write Tests First**: Create or update test file in `tests/entity/FlatbuffersConfigurator.test.cpp`

2. Add the overloaded `ConfigureComponent` method to `FlatbuffersConfigurator`:

**In `src/entity/FlatbuffersConfigurator.h`:**

Add the include:
```cpp
#include "CNewComponent.h"
#include "new_component_generated.h"
```

Add the method declaration:
```cpp
private:
  ////////////////////////////////////////////////////////////
  /// @brief Overloaded method for configuring CNewComponent component
  ///
  /// @param component_data Flatbuffers table data for NewComponent
  /// @param component CNewComponent instance to be configured
  ////////////////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureComponent(const NewComponentData *component_data,
                     CNewComponent &component);
```

**In `src/entity/FlatbuffersConfigurator.cpp`:**

Add the implementation:
```cpp
////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersConfigurator::ConfigureComponent(
    const NewComponentData *component_data,
    CNewComponent &component) {

  // Configure the underlying Component type first
  auto configure_result =
      ConfigureComponent(static_cast<Component &>(component));

  if (!configure_result.has_value())
    return std::unexpected(configure_result.error());

  // Configure component-specific data with null checks
  // FlatBuffers data is susceptible to segfaults, so validate everything
  if (component_data->component_name())
    component.m_component_name = component_data->component_name()->str();

  // For primitive types, direct assignment is safe
  component.m_value = component_data->value();

  return std::monostate{};
}
```

#### Step 5: Add Configuration Call in Main Configure Method

In `FlatbuffersConfigurator::ConfigureEntitiesFromDefaultData`, add the component
configuration block:

```cpp
// CNewComponent component configuration
if (entity_data->c_new_component()) {
  auto configure_result = ConfigureComponent(
      entity_data->c_new_component(),
      emp_helpers::GetComponent<CNewComponent>(i, entity_memory_pool));

  if (!configure_result.has_value())
    return std::unexpected(configure_result.error());
}
```

#### Step 6: Add Component Tests

If testing the component itself (not just configuration), create a test file:

`tests/components/CNewComponent.test.cpp`:

```cpp
#include "CNewComponent.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CNewComponent is default constructible", "[CNewComponent]") {
  steamrot::CNewComponent component;
  REQUIRE(component.m_active == false);
  REQUIRE(component.m_component_name == "default_name");
  REQUIRE(component.m_value == 0);
}
```

Add the test to `tests/components/CMakeLists.txt`:

```cmake
add_executable(test_components
  CGrimoireMachina.test.cpp
  CNewComponent.test.cpp  # Add this line
)
```

#### Step 7: Build and Test

```bash
# Configure the project (if not already done)
cmake --preset Debug

# Build the project
cmake --build --preset Debug

# Run tests
ctest --preset Debug
```

#### Important Notes

- **Null Checks**: Always wrap FlatBuffers field access in `if` statements to
  avoid segfaults. FlatBuffers schema doesn't allow all data types to be required.
- **Base Configuration**: Always call the base `ConfigureComponent(Component&)`
  method first to configure the underlying Component data (e.g., `m_active`).
- **Default Construction**: All components must be default-constructible for the
  memory pool to work correctly.
- **TDD**: Write tests before implementing configuration logic to ensure correctness.
- **Member Prefix**: Use `m_` prefix for all member variables.
- **Component Prefix**: Use `C` prefix for all component class names.

### UI Elements

#### Adding New Element types

Each element is derived from UIElement contained in `src/user_interface/`,
UIElement contains a virtual destructor to allow for polymorphism

The UIElement and derived types are designed to be pure data containers with no
methods.

The UIElement contains data common to all UI elements such as position, size,
visibility e.t.c.

Once a new UIElement type has been created, a style and drawing method will need
to be created for it.

Creating tests for this is covered under Testing

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
#include "TestContext.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("NewLogic::NewLogic Constructor", "[NewLogic]") {
  // Arrange - Create test context
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  
  // Act & Assert - Instantiate NewLogic
  steamrot::NewLogic new_logic(
      test_context.GetLogicContextForTestScene());
  SUCCEED("NewLogic instantiated successfully");
}

TEST_CASE("NewLogic::ProcessLogic performs expected logic", "[NewLogic]") {
  // Arrange
  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context;
  
  // Get context references
  auto &game_context = test_context.GetGameContext();
  auto logic_context = test_context.GetLogicContextForTestScene();
  
  // Set up test data (entities, components, etc.)
  // ...
  
  // Create Logic instance
  steamrot::NewLogic new_logic(logic_context);
  
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
- Use `TestContext` to provide mock dependencies
- Use `CAPTURE()` macro to debug test failures

#### Step 4: Update LogicFactory Tests (TDD)

Update `tests/logic/LogicFactory.test.cpp` to expect the new Logic class:

```cpp
TEST_CASE("LogicFactory creates correct Logic instances for YourScene",
          "[LogicFactory]") {

  steamrot::PathProvider path_provider{steamrot::EnvironmentType::Test};
  steamrot::tests::TestContext test_context{
      steamrot::SceneType::SceneType_YOUR_SCENE};
  
  steamrot::LogicFactory logic_factory(
      steamrot::SceneType::SceneType_YOUR_SCENE,
      test_context.GetLogicContextForYourScene());

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

#### Step 7: Add TestContext Support (if new scene)

If adding a new scene type, update `tests/context/TestContext.h` and `.cpp`:

**In TestContext.h:**
```cpp
private:
  std::unique_ptr<LogicContext> logic_context_for_your_scene{nullptr};
  void ConfigureLogicContextForYourScene();

public:
  const steamrot::LogicContext &GetLogicContextForYourScene() const;
```

**In TestContext.cpp:**
```cpp
void TestContext::ConfigureLogicContextForYourScene() {
  logic_context_for_your_scene = std::make_unique<LogicContext>(
      LogicContext{entity_memory_pool, archetype_manager.GetArchetypes(),
                   render_texture, game_context_ptr->window,
                   game_context_ptr->asset_manager,
                   game_context_ptr->event_handler,
                   game_context_ptr->mouse_position});
}

const LogicContext &TestContext::GetLogicContextForYourScene() const {
  return *logic_context_for_your_scene;
}
```

#### Step 8: Build and Test

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
   - Use TestContext for realistic scenarios
   - Test interaction with other Logic classes
   - Test component state changes

#### Debugging Tips

- Use `CAPTURE()` in tests to output variable values on failure
- Check archetype exists before accessing entities
- Verify component data is properly initialized in test setup
- Run Logic tests individually during development: `./test_logic "[YourLogic]"`
- Use `std::cout` for debugging (will appear in test output)

### Actions

This will attempt to describe the intent behind how actions are set up and how
an operator can add actions to the game.

#### Action Generation

Currently, actions are designed around capturing user input. Though I imagine
this will be expanded to act on global events e.t.c. The events are handled at
the beginning of the game loop (keyboard input and mouse input), this generates
a bit flag that. The bit flag will be available globally or passed to any Action
Managers that need it.

ActionManager instances will be placed in key locations such as Scene instance
and the DisplayManager. They will compare this global bit flag against their own
internal map of actions and generate another bit flag, this time of actions
(that have been defined in an enum). This will be made available to the Scene
Logic to allow for flow control.

These bit flags will be reset each tick of the game loop, so that they are only
valid for the current tick.

#### Action Registration

There is an enum called Actions which the ActionManager has access to. All
actions will be registered in this enum, the predicition being that their will
not be massives on actions that a user can take.

An operator then creates a json which maps keys to actions. The string
representation of the keys and mouse are checked by internal static maps for
correctness. The internal logic of the ActionManager creates bitset id for each
action that maps one or more key/mouse bindings to the action (as a bitflag).

So the steps an operator would take to add an action are:

- check the Action enum to see if the action exists, if not add it to the enum
  and recompile the code.
- create a json segment of the scene JSON that maps the key/mouse to the action.
  The json file should be in the format of:

```json
{
  "actions": [
    {
      "name": "action_name",
      "inputs": [{ "type": "keyboard/mouse", "value": "key_name/mouse_name" }]
    }
  ]
}
```

## Classes

### Logic Class

The Logic class will be an abstract class. The derived classes will be
responsible for changing the game state, such as movement logic, physics logic,
e.t.c.

An inherited member function (PerformLogic()) will take in a unique pointer to
the EntityMemoryPool as well as a container of indices to the entities that need
to be updated. The logic class will then be responsible for updating the game
state of the entities in the container.

It will not be responsible for determining which archetype to use.

Logic objects will provided by the [LogicFactory](#logicfactory) abstract class.

### LogicFactory

The LogicFactory abstract class will be responsible for creating Logic objects.
It will have a pure virtual function (CreateLogic()) that will return a unique
pointer to a Logic object. So a MovementLogicFactory will create a MovementLogic
object depending on the parameters passed to it.

## Testing

SteamRot uses Catch2 for testing with a clear test taxonomy to distinguish between unit, integration, and system tests.

### Test Directory Structure

```
tests/
├── unit/                    # Unit tests organized by subsystem
│   ├── assets/
│   ├── components/
│   ├── data_handlers/
│   ├── display/
│   ├── entity/
│   ├── events/
│   ├── logic/
│   ├── scenes/
│   ├── systems/
│   └── user_interface/
├── integration/             # Integration tests (2+ components)
│   └── scene_change/
├── system/                  # End-to-end tests (future)
├── perf/                    # Performance tests (future)
│   └── benchmarks/
├── data/                    # Test data files
└── context/                 # Test utilities (TestContext, helpers)
```

**Note**: All unit tests have been migrated to the `unit/` directory and organized by subsystem. Integration tests are in `integration/` directory. System tests and performance tests directories are ready for future use.

### Test Classification

All tests are tagged with Catch2 tags to enable filtering by test type:

- **`[unit]`** - Unit tests: Test a single class/function in isolation with mocked dependencies
- **`[integration]`** - Integration tests: Test interactions between multiple components
- **`[system]`** - System tests: End-to-end tests through the entire system (future)
- **`[perf]`** - Performance tests: Benchmarks for critical code paths (future)
- **`[.visual]`** - Visual confirmation tests: Tests requiring user input or visual verification (hidden tag - excluded by default)

### Running Tests

**Build and run all tests:**
```bash
# Configure with preset
cmake --preset Debug

# Build
cmake --build --preset Debug

# Run all tests
ctest --preset Debug
```

**Run specific test types:**
```bash
# Run only unit tests
ctest --preset Debug -L unit

# Run only integration tests
ctest --preset Debug -L integration

# Run tests from a specific subsystem
ctest --preset Debug -R logic
```

**Run tests with verbose output:**
```bash
ctest --preset Debug --output-on-failure
```

**Visual confirmation tests:**

Some tests require user input or visual confirmation (they open SFML windows and wait for interaction). These tests are tagged with `[.visual]` (hidden tag), which means they are **automatically excluded** from default test runs.

```bash
# Run all tests (visual tests excluded by default due to hidden tag)
ctest --preset Debug

# Run ONLY visual tests (explicit request for manual verification)
ctest --preset Debug --test-args "[.visual]"
# or with test executable:
./build/Debug/tests/unit/logic/test_logic "[.visual]"

# Run all hidden tests
./build/Debug/tests/unit/logic/test_logic "[.]"
```

### Writing Tests

All tests should include appropriate Catch2 tags:

```cpp
TEST_CASE("ClassName constructor", "[unit][ClassName]") {
  // Unit test implementation
}

TEST_CASE("Feature workflow", "[integration][feature_name]") {
  // Integration test implementation
}

TEST_CASE("Visual confirmation test", "[unit][ClassName][.visual]") {
  // Test requiring user input or visual confirmation
  // These tests open windows and wait for user interaction
  // Hidden tag [.visual] means this test is excluded by default
}
```

### Test-Driven Development (TDD) in SteamRot

SteamRot follows a Test-Driven Development approach for all new features and components. This section documents how to implement TDD effectively in this repository.

#### TDD Workflow

The standard TDD workflow is:

1. **Write the test first** - Define expected behavior before implementing
2. **Run the test** - Verify it fails (red)
3. **Implement minimal code** - Make the test pass (green)
4. **Run all tests** - Ensure no regressions
5. **Refactor** - Improve code while keeping tests green
6. **Repeat** - Continue with next feature

#### Using Reusable Test Infrastructure

SteamRot provides reusable test infrastructure to reduce boilerplate and improve test quality:

##### TestScenarios Class

Provides pre-configured entity and component setups:

```cpp
#include "TestScenarios.h"

TEST_CASE("Logic processes entities correctly", "[unit][MyLogic]") {
  // Create 10 entities with UI components
  auto pool = steamrot::tests::TestScenarios::CreatePoolWithMultipleUIEntities(10);
  
  // Create populated archetype manager
  auto manager = steamrot::tests::TestScenarios::CreatePopulatedArchetypeManager(pool);
  
  // Test your logic...
}
```

**Available methods:**
- `CreateEmptyPool()` - Empty entity pool
- `CreatePoolWithNEntities(n)` - Pool with n default entities
- `CreatePoolWithArchetype(archetype_id, n)` - Pool with entities matching archetype
- `CreatePoolWithSingleUIEntity()` - One UI entity
- `CreatePoolWithMultipleUIEntities(n)` - n UI entities
- `CreatePoolWithSingleGrimoireEntity()` - One grimoire entity
- `CreatePopulatedArchetypeManager(pool)` - Configured archetype manager
- `ActivateComponent<T>(entity_id, pool)` - Activate specific component
- `DeactivateComponent<T>(entity_id, pool)` - Deactivate specific component

##### Test Assertions

Domain-specific assertions for clearer test intent:

```cpp
#include "test_assertions.h"

TEST_CASE("Component configuration", "[unit][MyComponent]") {
  MyComponent component;
  component.m_active = true;
  
  // Use domain-specific assertions
  steamrot::tests::AssertComponentActive(component, true, "MyTest");
  steamrot::tests::AssertArchetypeExists(archetype_id, manager, "MyTest");
  steamrot::tests::AssertEntityInArchetype(0, archetype_id, manager, "MyTest");
  steamrot::tests::AssertArchetypeEntityCount(archetype_id, 5, manager, "MyTest");
}
```

**Available assertions:**
- `AssertComponentActive(component, expected, test_name)` - Verify activation state
- `AssertEntityInArchetype(entity_id, archetype_id, manager, test_name)` - Verify membership
- `AssertArchetypeExists(archetype_id, manager, test_name)` - Verify archetype presence
- `AssertArchetypeEntityCount(archetype_id, count, manager, test_name)` - Verify entity count
- `AssertEntityPoolValid(pool, test_name)` - Verify pool consistency

##### ComponentTestMixin

Template for testing Component contract compliance:

```cpp
#include "component_test_mixin.h"

TEST_CASE("MyComponent follows Component contract", "[unit][MyComponent]") {
  // Run all standard component tests
  steamrot::tests::ComponentTestMixin<MyComponent>::RunAllTests();
  
  // Or run individual tests
  steamrot::tests::ComponentTestMixin<MyComponent>::TestDefaultConstruction();
  steamrot::tests::ComponentTestMixin<MyComponent>::TestComponentRegisterIndex();
  steamrot::tests::ComponentTestMixin<MyComponent>::TestActivation();
  steamrot::tests::ComponentTestMixin<MyComponent>::TestCopyConstruction();
  steamrot::tests::ComponentTestMixin<MyComponent>::TestCopyAssignment();
}
```

This ensures all components:
- Are default-constructible
- Return correct register index
- Can be activated/deactivated
- Are copyable (if needed)

##### LogicTestBase

Base class for Logic tests to reduce boilerplate:

```cpp
#include "logic_test_base.h"

class MyLogicTest : public steamrot::tests::LogicTestBase<MyLogic> {
protected:
  void TestProcessWithSingleEntity() override {
    SetUp();
    
    // Create entity with required components
    auto& game_context = test_context->GetGameContext();
    // ... setup entities ...
    
    logic = CreateLogic();
    REQUIRE_NOTHROW(logic->RunLogic());
    
    // Verify expected behavior
    // REQUIRE(...);
  }
};

TEST_CASE_METHOD(MyLogicTest, "MyLogic construction", "[unit][MyLogic]") {
  TestConstruction();
}

TEST_CASE_METHOD(MyLogicTest, "MyLogic with entities", "[unit][MyLogic]") {
  TestProcessWithSingleEntity();
  TestProcessWithMultipleEntities(5);
}
```

**Provided test methods:**
- `TestConstruction()` - Verify Logic instantiates without errors
- `TestProcessWithEmptyArchetype()` - Verify graceful handling of no entities
- `TestProcessWithSingleEntity()` - Override to test single entity processing
- `TestProcessWithMultipleEntities(n)` - Override to test batch processing
- `RunStandardTests()` - Run all standard tests at once

#### TDD Examples for Common Scenarios

##### Example 1: Adding a New Component (TDD)

**Step 1: Write the test first**

```cpp
// tests/components/CNewComponent.test.cpp
#include "CNewComponent.h"
#include "component_test_mixin.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CNewComponent follows Component contract", "[unit][CNewComponent]") {
  steamrot::tests::ComponentTestMixin<steamrot::CNewComponent>::RunAllTests();
}

TEST_CASE("CNewComponent has correct default values", "[unit][CNewComponent]") {
  steamrot::CNewComponent component;
  REQUIRE(component.m_data_field == "default");
  REQUIRE(component.m_value == 0);
}
```

**Step 2: Run the test (it will fail - component doesn't exist yet)**

```bash
cmake --build --preset Debug
# Compilation will fail - CNewComponent doesn't exist
```

**Step 3: Implement minimal Component**

```cpp
// src/components/CNewComponent.h
#pragma once
#include "Component.h"
#include <string>

namespace steamrot {
struct CNewComponent : public Component {
  CNewComponent() = default;
  std::string m_data_field{"default"};
  int m_value{0};
  size_t GetComponentRegisterIndex() const override;
};
}
```

**Step 4: Run tests - verify they pass**

```bash
cmake --build --preset Debug
ctest --preset Debug -R CNewComponent
```

**Step 5: Add FlatBuffers schema and configurator (with tests)**

Continue the TDD cycle for configuration...

##### Example 2: Adding a New Logic Class (TDD)

**Step 1: Write the test first**

```cpp
// tests/logic/NewLogic.test.cpp
#include "NewLogic.h"
#include "logic_test_base.h"
#include "TestScenarios.h"
#include <catch2/catch_test_macros.hpp>

class NewLogicTest : public steamrot::tests::LogicTestBase<steamrot::NewLogic> {
protected:
  void TestProcessWithSingleEntity() override {
    SetUp();
    
    // Create entity with required components
    // Use TestScenarios for setup
    
    logic = CreateLogic();
    REQUIRE_NOTHROW(logic->RunLogic());
    
    // Verify expected state changes
  }
};

TEST_CASE_METHOD(NewLogicTest, "NewLogic construction", "[unit][NewLogic]") {
  TestConstruction();
}

TEST_CASE_METHOD(NewLogicTest, "NewLogic processes entities", "[unit][NewLogic]") {
  TestProcessWithSingleEntity();
}
```

**Step 2: Run test (will fail - Logic doesn't exist)**

**Step 3: Implement minimal Logic class**

```cpp
// src/logic/NewLogic.h & .cpp
// Implement ProcessLogic() with minimal behavior to make test pass
```

**Step 4: Run tests - verify they pass**

**Step 5: Add to LogicFactory (with tests)**

Update LogicFactory tests first, then implementation.

#### TDD Best Practices for SteamRot

1. **Always write tests first** - Define behavior before implementation
2. **Use reusable infrastructure** - Leverage TestScenarios, assertions, mixins
3. **Test one thing at a time** - Each TEST_CASE should verify a single behavior
4. **Use descriptive names** - Test names should describe what is being tested
5. **Tag appropriately** - Use `[unit]`, `[integration]`, or `[system]` tags
6. **Keep tests fast** - Unit tests should run in milliseconds
7. **Avoid test interdependence** - Each test should be independent
8. **Test edge cases** - Include tests for boundary conditions and error cases
9. **Refactor tests too** - Keep test code clean and maintainable
10. **Run tests frequently** - After every change, run relevant tests

#### Testing Checklist

Before committing code, ensure:

- [ ] All new code has tests written first (TDD)
- [ ] Tests use reusable infrastructure where applicable
- [ ] Tests are tagged with `[unit]`, `[integration]`, or `[system]`
- [ ] All tests pass: `ctest --preset Debug`
- [ ] Test names clearly describe what is being tested
- [ ] Tests are independent and can run in any order
- [ ] Edge cases and error conditions are tested
- [ ] Test code is clean and well-organized

#### Data-Driven Testing with Test Harness

SteamRot has a comprehensive test harness system that enables data-driven testing using JSON configuration files. This approach reduces test code boilerplate and makes tests easier to maintain.

**Test Harness Adoption Progress**:
- ✅ **Phase 2.1 Complete**: Entity pool comparison tests migrated to data-driven approach
- 🔄 **In Progress**: Additional phases documented in `documentation/TEST_HARNESS_ADOPTION_PLAN.md`

**Basic Usage**:

```cpp
#include "test_data_harness.h"
#include <catch2/generators/catch_generators_range.hpp>

TEST_CASE("Data-driven test example", "[unit][MyComponent]") {
  // Load test data from adjacent data/ directory
  auto configs_result = steamrot::tests::load_test_data_configs();
  REQUIRE(configs_result.has_value());

  // Use Catch2 generator for parameterized testing
  const auto *config = GENERATE_COPY(from_range(configs_result.value()));

  SECTION(config->metadata()->test_name()->c_str()) {
    // Run fixture test - creates fixture, executes simulations, compares results
    auto result = steamrot::tests::run_fixture_test(config);
    REQUIRE(result.has_value());
  }
}
```

**Test Data File Structure** (`tests/unit/entity/data/my_test.test_data.json`):

```json
{
  "metadata": {
    "test_name": "my_test",
    "description": "What this test verifies",
    "tags": ["unit", "entity"],
    "expected_to_pass": true,
    "version": 1
  },
  "start_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": [/* initial entity configuration */]
  },
  "expected_entity_collection": {
    "entity_memory_pool_size": 5,
    "entities": [/* expected entity state after test */]
  }
}
```

**Key Benefits**:
- Test data is externalized to JSON files (easier to read and maintain)
- Automatic compilation of JSON to binary during build
- Parameterized testing via Catch2 generators
- Detailed error messages with test metadata
- Reusable test infrastructure across subsystems

**Documentation**:
- Complete adoption plan: `documentation/TEST_HARNESS_ADOPTION_PLAN.md`
- Test data configuration guide: `documentation/TEST_DATA_CONFIGURATION.md`
- Migration patterns and examples included in adoption plan

### user_interface

#### Testing UI Elements

The TestUIElementDataProvider.h file defines a class with static methods that
provide some specific methods. (This could be updated in the future to provide
randomly generated data so we can provide a wider range of test cases).

So any new UIElement type will need to have a static method added to this class
to provide data.

The ui_element_factory_helpers.h defines a series of functions that will compare
the flatbuffers data to the produced UIElement (from the UIElementFactory
configure).

Any new UIElement type will need to have a new test function added to this file.

The ui_element_factory_helpers.cpp file also contains a full template
specialization for each flatbuffers data type to UIElement type. Any new
UIElement type will need to have a new template specialization added to this
file. These templates are to allow for testing a nested strcture of UIElements.

## Style Guide

In general we follow the
[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

### File Naming Conventions

The project uses a cohesive naming system to distinguish between classes, free functions, and test utilities. See:

- **[FILE_NAMING_CONVENTIONS.md](documentation/FILE_NAMING_CONVENTIONS.md)** - Complete naming system documentation
- **[NAMING_QUICK_REFERENCE.md](documentation/NAMING_QUICK_REFERENCE.md)** - Quick reference guide

**Quick Summary**:
- **Classes**: `ClassName.h/cpp` (PascalCase) → `steamrot::ClassName`
- **Free Functions**: `subsystem_category.h/cpp` (snake_case) → `steamrot::subsystem::category`
- **Template Utils**: `SubsystemUtils.h` (PascalCase, header-only) → `steamrot::subsystem`
- **Test Helpers**: `subsystem_test_helpers.h/cpp` (snake_case) → `steamrot::tests::subsystem`
- **Test Files**: `ModuleName.test.cpp` (matches module being tested)

### Formatting

Certain readability formatting elements have been used by B Wallis. Inspiration
for this (or just directly lifting) has been take from the
[SFML Repository](https://github.com/SFML/SFML/tree/master)

#### Spacing between functions

////////////////////////////////////////////////////////////

#### Funtionality brief

//////////////////////////////////////////////////////////// /// |brief ///
////////////////////////////////////////////////////////////

[EOF]
