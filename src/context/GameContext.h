/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GameContext class.
/////////////////////////////////////////////////

#include "AssetManager.h"
#include "EventHandler.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot {
struct GameContext {

  /////////////////////////////////////////////////
  /// @brief Reference to the game window.
  ///
  /// The game is designed to run in a single window,
  /////////////////////////////////////////////////
  sf::RenderWindow &game_window;

  /////////////////////////////////////////////////
  /// @brief Reference to bitset which globally captures all pressed events.
  ///
  /////////////////////////////////////////////////
  const EventBitset &pressed_events;

  /////////////////////////////////////////////////
  /// @brief  Reference to bitset which globally captures all released events.
  /////////////////////////////////////////////////
  const EventBitset &released_events;

  /////////////////////////////////////////////////
  /// @brief Reference to mouse position in the game window. (local).
  /////////////////////////////////////////////////
  const sf::Vector2i mouse_position;

  /////////////////////////////////////////////////
  /// @brief Loop number for the current game loop. Lives on the GameEngine
  /////////////////////////////////////////////////
  const size_t &loop_number;

  /////////////////////////////////////////////////
  /// @brief Reference to the AssetManager living on the GameEngine, there
  /// should only be one instance of this.
  /////////////////////////////////////////////////
  const AssetManager &asset_manager;

  /////////////////////////////////////////////////
  /// @brief Reference to the DataManager living on the GameEngine
  ///
  /// This is a lightweight object, but having it in the GameContext object
  /// keeps things cleaner.
  /////////////////////////////////////////////////
  const DataManager &data_manager;
};
} // namespace steamrot
