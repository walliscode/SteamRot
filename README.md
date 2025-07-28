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
  - [UI Elements](#ui-elements)
    - [Adding New Element types](#adding-new-element-types)
    - [Actions](#actions)
      - [Action Generation](#action-generation)
      - [Action Registration](#action-registration)
  - [Classes](#classes)
    - [DataManager](#datamanager)
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

## UI Elements

### Adding New Element types

Element definition is contained in `src/user_interface`. An UI Element is a
struct such as Panel or Button.

The general UIElement struct contains information common to all element structs
and stores one of the possible structs (Panel, Button e.t.c.) as a std::variant.

To create a new struct, add it as a header file in the `src/user_interface` and
then add it to the ElementType typedef in UIElement.h file.

We then need to create the flatbuffers data equivalent. In
`src/flatbuffers_headers/user_interface.fbs` add the new element as
`table NewElementData{}` and add that to the `union UIElementDataUnion`

We need to interface the two in the UIElementFactory. Provide a specific
function for just that Element and then add it to the switch statement.

The element should contain information about the state of the UI Element. For
things like drop down choices we store in the UIElementDataPackage.

This all leads to storing the data of the UIElement in the CUserInteface
component. We then need to know how to draw the new element. This is a
combination of a style struct and logic.

In the UIRenderLogic.h create a NewElementStyle struct which inherits from the
Style struct. Again, create a relevant flatbuffers data equivalent under
themes.fbs

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

### DataManager

The DataManager class will live in the GameEngine class. Its current
responsibilities include:

1. Passing data objects to the GameEngine class.
2. Receiving data objects from the GameEngine class.
3. Loading and saving data objects to and from files.
4. Error checking the files/data objects at run time.

It will also contain factory classes for providing methods for handling
different types of data objects.

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
