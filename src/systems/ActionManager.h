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
  std::unordered_map<EventBitset, ActionNames> m_key_to_action_map;

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

public:
  /**
   * @brief Constructor using flatbuffers ActionsData object
   *
   * @param actions_data Raw pointer to ActionsData object
   */
  ActionManager(const ActionsData *actions_data);

  ////////////////////////////////////////////////////////////
  // |brief Generate any possible actions for this scene as bitflag enum
  ////////////////////////////////////////////////////////////
  const ActionNames GenerateActions(const EventBitset &input_event);
};
} // namespace steamrot
