////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "EventHandler.h"
#include "actions_generated.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

namespace steamrot {

class ActionManager {
private:
  ////////////////////////////////////////////////////////////
  // |Member: stores key mappings to actions for this scene
  ////////////////////////////////////////////////////////////
  std::unordered_map<EventBitset, ActionNames> m_scene_event_to_action_map;

  ////////////////////////////////////////////////////////////
  // |brief: returns map of string letters to sf::Keyboard enum
  ////////////////////////////////////////////////////////////
  static const std::unordered_map<KeyboardInput, sf::Keyboard::Key> &
  getFlatbuffersToSFMLKeyboardMap();

  ////////////////////////////////////////////////////////////
  // |brief: returns map of string letters to sf::Mouse enum
  ////////////////////////////////////////////////////////////
  static const std::unordered_map<MouseInput, sf::Mouse::Button> &
  getStringToMouseMap();

  /**
   * @brief Register actions for object instance from flatbuffers ActionsData
   *
   * @param actions_data ActionsData object containing action definitions
   */
  void RegisterActions(const ActionsData *actions_data);

  /////////////////////////////////////////////////
  /// @brief used to describe action generated at the Scene level
  ///
  /// These are actions that don't need some other conditional component at the
  /// Logic level. e.g. they are processed instantly.
  /////////////////////////////////////////////////
  ActionNames m_scene_level_actions{0};

public:
  /**
   * @brief Constructor using flatbuffers ActionsData object
   *
   * @param actions_data Raw pointer to ActionsData object
   */
  ActionManager(const ActionsData *actions_data);

  /////////////////////////////////////////////////
  /// @brief Modify the Scene level actions based on the input event
  ///
  /// @param input_event Bitset of user input events
  /////////////////////////////////////////////////
  void ProcessSceneLevelActions(const EventBitset &input_event);

  /////////////////////////////////////////////////
  /// @brief Reset the Scene level and Logic level actions to their default
  /////////////////////////////////////////////////
  void ClearActions();

  /////////////////////////////////////////////////
  /// @brief Return the Scene level action bit flag
  ///
  /// @return ActionNames
  /////////////////////////////////////////////////
  const ActionNames GetSceneLevelAction() const;
};
} // namespace steamrot
