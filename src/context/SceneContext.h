/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SceneContext struct.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "CameraState.h"
#include "DataAccessFactory.h"
#include "EngineResources.h"
#include "EntityManager.h"
#include "EventHandler.h"
#include "MrGhost.h"
#include "SceneState.h"
#include "containers.h"
#include <SFML/Graphics.hpp>
#include <unordered_map>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Lightweight reference container for scene and game core objects.
///
/// SceneContext provides convenient access to scene-level and game-level
/// core objects, as well as entity/archetype data from EntityManager.
/// It is designed to be passed around the codebase and is cheap to copy
/// (contains only references).
///
/// SceneContext replaces the previous LogicContext struct and moves from
/// src/logic/ to src/scenes/ to better reflect its broader usage.
/////////////////////////////////////////////////
struct SceneContext {
  /////////////////////////////////////////////////
  /// @brief Constructor taking scene_texture, engine_resources, and
  /// EntityManager.
  ///
  /// @param scene_texture Reference to scene render texture
  /// @param engine_resources Reference to EngineResources (contains window,
  /// assets, etc.)
  /// @param entity_manager Reference to EntityManager (contains
  /// entities/archetypes)
  /////////////////////////////////////////////////
  SceneContext(sf::RenderTexture &scene_texture, SceneState &scene_state,
               EngineResources &engine_resources, EntityManager &entity_manager,
               DataAccessFactory &data_access_factory, MrGhost &mr_ghost,
               CameraState &camera_state, sf::Vector2f &world_mouse_position);

  /////////////////////////////////////////////////
  /// @brief Reference to the EntityMemoryPool for the Scene.
  /////////////////////////////////////////////////
  EntityMemoryPool &scene_entities;

  /////////////////////////////////////////////////
  /// @brief Reference to the available archetypes in the Scene.
  /////////////////////////////////////////////////
  const std::unordered_map<ArchetypeID, Archetype> &archetypes;

  /////////////////////////////////////////////////
  /// @brief Reference to the RenderTexture for the Scene.
  /////////////////////////////////////////////////
  sf::RenderTexture &scene_texture;

  /////////////////////////////////////////////////
  /// @brief Reference to the SceneState for the Scene.
  /////////////////////////////////////////////////
  SceneState &scene_state;

  /////////////////////////////////////////////////
  /// @brief Reference to the game window.
  /////////////////////////////////////////////////
  sf::RenderWindow &game_window;

  /////////////////////////////////////////////////
  /// @brief Reference to the AssetManager for the game.
  /////////////////////////////////////////////////
  AssetManager &asset_manager;

  /////////////////////////////////////////////////
  /// @brief Reference to the EventHandler for the game.
  ///
  /// This contains the global event bus and is used to adapt other events.
  /////////////////////////////////////////////////
  EventHandler &event_handler;

  /////////////////////////////////////////////////
  /// @brief Reference to mouse position in the game window (local).
  /////////////////////////////////////////////////
  sf::Vector2i &mouse_position;

  /////////////////////////////////////////////////
  /// @brief Reference to the DataAccessFactory for the game.
  /////////////////////////////////////////////////
  DataAccessFactory &data_access_factory;

  /////////////////////////////////////////////////
  /// @brief Reference to the MrGhost state for the scene.
  ///
  /// Provides all Logic classes with read/write access to the current
  /// ghost selection (type + key of the item selected from menus,
  /// dropdowns, blueprint lists, etc.).
  /////////////////////////////////////////////////
  MrGhost &mr_ghost;

  /////////////////////////////////////////////////
  /// @brief Reference to the camera/view state for the scene.
  ///
  /// Provides all Logic classes with read/write access to the current
  /// zoom level and camera position. Owned by SceneState.
  /////////////////////////////////////////////////
  CameraState &camera_state;

  /////////////////////////////////////////////////
  /// @brief World-space mouse position for the current tick.
  ///
  /// Computed once per tick by GhostPositioningLogic via
  /// movement::camera::MapToWorldCoords.  World-space systems (ghost rendering,
  /// grimoire collision, piece placement) read this value; screen-space
  /// systems (UI collision) use mouse_position instead.
  ///
  /// Coordinate contract:
  ///   mouse_position       (sf::Vector2i) — screen pixels — UI collision only
  ///   world_mouse_position (sf::Vector2f) — world space  — everything else
  /////////////////////////////////////////////////
  sf::Vector2f &world_mouse_position;
};

} // namespace steamrot
