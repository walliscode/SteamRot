/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GameContext class.
/////////////////////////////////////////////////

#pragma once
#include "AssetManager.h"
#include "EventHandler.h"
#include "GameCore.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot {
struct GameContext {
  GameContext() = delete;

  GameContext(GameCore &game_core);

  /////////////////////////////////////////////////
  /// @brief Reference to the GameCore this context was created from.
  /////////////////////////////////////////////////
  GameCore &game_core;

  /////////////////////////////////////////////////
  /// @brief Reference to the game window.
  ///
  /// The game is designed to run in a single window,
  /////////////////////////////////////////////////
  sf::RenderWindow &game_window;

  /////////////////////////////////////////////////
  /// @brief Reference to the global event handler which contains the event bus.
  /////////////////////////////////////////////////
  EventHandler &event_handler;

  /////////////////////////////////////////////////
  /// @brief Reference to global mouse position
  /////////////////////////////////////////////////
  sf::Vector2i &mouse_position;

  /////////////////////////////////////////////////
  /// @brief Loop number for the current game loop. Lives on the GameEngine
  /////////////////////////////////////////////////
  size_t &loop_number;

  /////////////////////////////////////////////////
  /// @brief Reference to the AssetManager living on the GameEngine, there
  /// should only be one instance of this.
  /////////////////////////////////////////////////
  AssetManager &asset_manager;
};
} // namespace steamrot
