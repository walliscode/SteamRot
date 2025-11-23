/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the GameResources struct.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AssetManager.h"
#include "EventHandler.h"
#include "PathProvider.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Resource struct containing game-level resources.
///
/// GameResources owns all game-level resources (window, event handler,
/// asset manager, etc.). These resources have a lifetime matching the
/// GameEngine instance.
///
/// All members are concrete objects (no references/pointers to other
/// resource members). This struct is default-constructible and should
/// be configured via ConfigureGameResources free function.
/////////////////////////////////////////////////
struct GameResources {
/////////////////////////////////////////////////
  /// @brief The game window.
  ///
  /// The game is designed to run in a single window.
/////////////////////////////////////////////////
  sf::RenderWindow game_window;

/////////////////////////////////////////////////
  /// @brief Global event handler containing the event bus.
/////////////////////////////////////////////////
  EventHandler event_handler;

/////////////////////////////////////////////////
  /// @brief Current mouse position in window coordinates.
  ///
  /// This is not a live value, it must be updated each frame.
/////////////////////////////////////////////////
  sf::Vector2i mouse_position{0, 0};

/////////////////////////////////////////////////
  /// @brief Loop number for the current game loop.
/////////////////////////////////////////////////
  size_t loop_number{0};

/////////////////////////////////////////////////
  /// @brief Asset manager for the game.
  ///
  /// There should only be one instance of this.
/////////////////////////////////////////////////
  AssetManager asset_manager;

/////////////////////////////////////////////////
  /// @brief Environment type for the game.
/////////////////////////////////////////////////
  EnvironmentType env_type{EnvironmentType::None};
};

} // namespace steamrot
