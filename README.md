# SteamRot game structure

<!--toc:start-->
- [SteamRot game structure](#steamrot-game-structure)
  - [Style Guide](#style-guide)
    - [Formatting](#formatting)
      - [Spacing between functions](#spacing-between-functions)
      - [funtionality brief](#funtionality-brief)
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
1. UpdateSystems: updates all internal logic that affect the game state.
1. sRender: draws the game state to the screen (handled by the display manager).

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
