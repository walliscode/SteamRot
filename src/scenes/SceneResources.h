/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SceneResources struct.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "EntityManager.h"
#include "GameContext.h"
#include "LogicFactory.h"
#include "MrGhost.h"
#include <SFML/Graphics/RenderTexture.hpp>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct SceneResources
/// @brief Scene-level resources struct containing long-lived objects.
///
/// SceneResources owns all scene-level resources (managers, logic systems,
/// render texture). These resources have a lifetime matching the Scene
/// instance.
///
/// All members are concrete objects (no references/pointers to other
/// scene members). This struct is default-constructible.
/////////////////////////////////////////////////
struct SceneResources {
  SceneResources(const GameContext &game_context)
      : game_context(game_context), entity_manager() {}

  /////////////////////////////////////////////////
  /// @brief GameContext object passed down from the GameEngine.
  /////////////////////////////////////////////////
  const GameContext &game_context;

  /////////////////////////////////////////////////
  /// @brief Entity Manager instance for this scene
  /////////////////////////////////////////////////
  EntityManager entity_manager;

  /////////////////////////////////////////////////
  /// @brief Map of all logic objects needed by the scene
  /////////////////////////////////////////////////
  logic::LogicCollection logic_map;

  /////////////////////////////////////////////////
  /// @brief Render texture for the scene
  ///
  /// All scene rendering is done to this texture, which is then
  /// drawn to the game window.
  /////////////////////////////////////////////////
  sf::RenderTexture scene_texture;

  /////////////////////////////////////////////////
  /// @brief MrGhost state for the scene.
  ///
  /// Tracks the currently selected item (type + key) from menus, dropdowns,
  /// blueprint lists, etc. A single instance lives here and is exposed to
  /// all Logic classes via SceneContext::mr_ghost.
  /////////////////////////////////////////////////
  MrGhost mr_ghost{};
};

} // namespace steamrot
