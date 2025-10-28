////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SceneContext struct.
////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "ArchetypeManager.h"
#include "AssetManager.h"
#include "EntityManager.h"
#include "EventHandler.h"
#include "GameResources.h"
#include "SceneResources.h"
#include "containers.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <unordered_map>

namespace steamrot {

////////////////////////////////////////////////////////////
/// @brief Lightweight reference container for scene and game resources.
///
/// SceneContext provides convenient access to scene-level and game-level
/// resources, as well as entity/archetype data from EntityManager.
/// It is designed to be passed around the codebase and is cheap to copy
/// (contains only references).
///
/// SceneContext replaces the previous LogicContext struct and moves from
/// src/logic/ to src/scenes/ to better reflect its broader usage.
////////////////////////////////////////////////////////////
struct SceneContext {
  ////////////////////////////////////////////////////////////
  /// @brief Constructor taking SceneResources, GameResources, and EntityManager.
  ///
  /// @param scene_res Reference to SceneResources (contains render texture)
  /// @param game_res Reference to GameResources (contains window, assets, etc.)
  /// @param entity_manager Reference to EntityManager (contains entities/archetypes)
  ////////////////////////////////////////////////////////////
  SceneContext(SceneResources &scene_res, GameResources &game_res,
               EntityManager &entity_manager);

  ////////////////////////////////////////////////////////////
  /// @brief Reference to the EntityMemoryPool for the Scene.
  ////////////////////////////////////////////////////////////
  EntityMemoryPool &scene_entities;

  ////////////////////////////////////////////////////////////
  /// @brief Reference to the available archetypes in the Scene.
  ////////////////////////////////////////////////////////////
  const std::unordered_map<ArchetypeID, Archetype> &archetypes;

  ////////////////////////////////////////////////////////////
  /// @brief Reference to the RenderTexture for the Scene.
  ////////////////////////////////////////////////////////////
  sf::RenderTexture &scene_texture;

  ////////////////////////////////////////////////////////////
  /// @brief Reference to the game window.
  ////////////////////////////////////////////////////////////
  sf::RenderWindow &game_window;

  ////////////////////////////////////////////////////////////
  /// @brief Reference to the AssetManager for the game.
  ////////////////////////////////////////////////////////////
  AssetManager &asset_manager;

  ////////////////////////////////////////////////////////////
  /// @brief Reference to the EventHandler for the game.
  ///
  /// This contains the global event bus and is used to adapt other events.
  ////////////////////////////////////////////////////////////
  EventHandler &event_handler;

  ////////////////////////////////////////////////////////////
  /// @brief Reference to mouse position in the game window (local).
  ////////////////////////////////////////////////////////////
  sf::Vector2i &mouse_position;
};

} // namespace steamrot
