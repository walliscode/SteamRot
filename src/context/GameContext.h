/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GameContext class.
/////////////////////////////////////////////////

#pragma once
#include "AssetManager.h"
#include "EventHandler.h"
#include "GameResources.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot {
struct GameContext {
  GameContext() = delete;

  explicit GameContext(GameResources &resources);

/////////////////////////////////////////////////
  /// @brief Reference to the GameResources this context was created from.
/////////////////////////////////////////////////
  GameResources &game_resources;

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
