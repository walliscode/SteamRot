# SteamRot game structure

<!--toc:start-->
- [SteamRot game structure](#steamrot-game-structure)
  - [Game Running](#game-running)
    - [RunGame](#rungame)
    - [UpdateSystems](#updatesystems)
  - [Style Guide](#style-guide)
    - [Formatting](#formatting)
      - [Spacing between functions](#spacing-between-functions)
      - [Funtionality brief](#funtionality-brief)
<!--toc:end-->

## Game Running

The Game Engine (and thus the SteamRot game) is initiated
by creating a GameEngine object and calling the [RunGame](#rungame)
method.

### RunGame

Still within the GameEngine object, the RunGame method creates
a while loop (the game loop) that will run until a simulation
limit is reached or the user quits the game.

The game loop calls a series of methods that will update the game state:

1. sUserInput: handles external user input (such as with a mouse or keyboard).
1. [UpdateSystems](#updatesystems): updates all internal logic that affect the game state.
1. sRender: draws the game state to the screen (handled by the display manager).

### UpdateSystems

The is a GameEngine method that calls the general systems for updating the game state.
This currently just calls the UpdateScenes method from the SceneManager class.
I'm predicting that this will be expanded to include other systems in the future,
but if not this could be collapsed down to just a call to the SceneManager.

The UpdateScenes function will also be responsible for the logic deciding which vector of scenes to update

1. [sMovement](#smovement): handles the movement of all entities in the scene.

## Style Guide

In general we follow the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

### Formatting

Certain readability formatting elements have been used by B Wallis.
Inspiration for this (or just directly lifting) has been take from the [SFML Repository](https://github.com/SFML/SFML/tree/master)

#### Spacing between functions

////////////////////////////////////////////////////////////

#### Funtionality brief

////////////////////////////////////////////////////////////
/// \brief Description
///
////////////////////////////////////////////////////////////

[EOF]
