/**
 * @file
 * @brief Asbtract base class for Template Logic classes.
 *
 */
#pragma once

#include "ArchetypeManager.h"
#include "AssetManager.h"
#include "EventHandler.h"
#include "containers.h"
#include "themes_generated.h"
#include <SFML/Graphics/RenderTexture.hpp>
namespace steamrot {
using EntityIndicies = std::vector<size_t>;

/**
* @class LogicContext
 * @brief Provides all the specific Scene data required for logic processing.

 */
struct LogicContext {
  /**
   * @brief Struct memmber that contains a reference to the EntityMemoryPool for
   * this Scene.
   */
  components::containers::EntityMemoryPool &scene_entities;

  /**
   * @brief Struct member that contains a reference to the archetypes in the
   * Scene.
   */
  const std::unordered_map<ArchetypeID, Archetype> &archetypes;

  /**
   * @brief Struct member that contains a reference to Scenes RenderTexture.
   */
  sf::RenderTexture &scene_texture;

  /////////////////////////////////////////////////
  /// @brief Reference to the game window.
  /////////////////////////////////////////////////
  sf::RenderWindow &game_window;

  /**
   * @brief Stuct member that contains UI thematic configuration.
   */
  const themes::UIObjects *ui_config{nullptr};

  /////////////////////////////////////////////////
  /// @brief Reference to the AssetManager for the game.
  /////////////////////////////////////////////////
  const AssetManager &asset_manager;

  /////////////////////////////////////////////////
  /// @brief Reference to the EventHandler for the game.
  ///
  /// This contains the global event bus and is used to adapt other events.
  /////////////////////////////////////////////////
  EventHandler &event_handler;
};

/**
 * @class BaseLogic
 * @brief Allows for polymorphic behaviour of template logic classes.
 *
 */
class BaseLogic {
protected:
  /**
   * @brief Collates all logic for the derived logic classes.
   *
   * @param entities Data structure containing all component data.
   * @param entity_indicies Entity mask for required entities.
   */
  virtual void ProcessLogic() = 0;

  /**
   * @brief How often the logic should be updated.
   */
  size_t m_update_frequency{1};

  /**
   * @brief Current cycle count for the logic. increased by RunLogic()
   */
  size_t m_cycle_count{1};

  /**
   * @brief Member variable that contains the context of the scene the logic
   * resides in.
   */
  LogicContext m_logic_context;

public:
  /**
   * @brief Default constructor
   */
  BaseLogic(const LogicContext logic_context);

  /**
   * @brief Default destructor
   */
  virtual ~BaseLogic() = default;
  /**
   * @brief Guard function that to only update on the required frequency.
   *
   * @param entities Data structure containing all component data.
   * @param entity_indicies Entity mask for required entities.
   */
  void RunLogic();
};
} // namespace steamrot
