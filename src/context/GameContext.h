/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GameContext class.
/////////////////////////////////////////////////

#pragma once
#include "AssetManager.h"
#include "DataAccessFactory.h"
#include "EngineResources.h"
#include "EventHandler.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot {
struct GameContext {
  GameContext() = delete;

  GameContext(EngineResources &engine_resources,
              DataAccessFactory &data_access_factory);

  /////////////////////////////////////////////////
  /// @brief Reference to the EngineResources this context was created from.
  /////////////////////////////////////////////////
  EngineResources &engine_resources;

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
  /// @brief Loop number for the current game loop. Lives on the Engine
  /////////////////////////////////////////////////
  size_t &loop_number;

  /////////////////////////////////////////////////
  /// @brief Reference to the AssetManager living on the Engine, there
  /// should only be one instance of this.
  /////////////////////////////////////////////////
  AssetManager &asset_manager;

  /////////////////////////////////////////////////
  /// @brief Reference to the DataAccessFactory for accessing data providers.
  ///
  /// This allows components to access data providers without using global
  /// functions, following the same pattern as AssetManager.
  /////////////////////////////////////////////////
  DataAccessFactory &data_access_factory;
};
} // namespace steamrot
