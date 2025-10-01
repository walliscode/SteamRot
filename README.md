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
  - [Style Guide](#style-guide) - [Formatting](#formatting) -
  [Spacing between functions](#spacing-between-functions) -
  [Funtionality brief](#funtionality-brief)
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
cmake --build build
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
# Build the project
cmake --build build

# Run tests
cd build
ctest
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

#### Logic classes

Logic classes are responsible for changing the game state. Each Logic class
needs to be added to a "system" such as movement or action.

Although adding new systems will be necessary, try to keep this to a minimum as
it will increase the complexity of the game engine.

#### LogicFactory

Once a derived Logic class has been created and its functions defined it needs
to be added to the LogicFactory. This will be hard baked into the LogicFactory
on a per SceneType basis, as we don't want the Logic classes and order to be
extensible/modifiable at runtime.

1. Define a new Logic class and its functions via unit tests.
2. [TDD] LogicFactory.test.cpp should be updated to include the new Logic class
   in the relevant scene. The order and number of Logic classes should be tested
   for.
3. Update the LogicFactory.cpp to include the new Logic class in relevant
   function under the relevent switch statement. d

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
