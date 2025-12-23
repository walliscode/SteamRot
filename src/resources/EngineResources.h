/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the EngineResources struct.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "AssetManager.h"
#include "EventHandler.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot {

// Forward declaration
class DataAccessFactory;

/////////////////////////////////////////////////
/// @brief Engine resources struct containing global core objects.
///
/// EngineResources owns all engine-level global resources (window, event
/// handler, asset manager, etc.). These resources have a lifetime matching
/// the Engine instance.
///
/// All members are concrete objects (no references/pointers to other
/// core members). This struct should be constructed with a DataAccessFactory
/// reference which is passed to AssetManager.
/////////////////////////////////////////////////
struct EngineResources {
  EngineResources() = delete;
  
  /////////////////////////////////////////////////
  /// @brief Constructor that initializes AssetManager with DataAccessFactory
  ///
  /// @param data_access_factory Reference to DataAccessFactory for AssetManager
  /////////////////////////////////////////////////
  explicit EngineResources(DataAccessFactory &data_access_factory);
  
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
  size_t loop_number{1};

  /////////////////////////////////////////////////
  /// @brief Asset manager for the game.
  ///
  /// There should only be one instance of this.
  /////////////////////////////////////////////////
  AssetManager asset_manager;
};

} // namespace steamrot
