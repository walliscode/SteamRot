////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include "user_input_generated.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <unordered_map>

namespace steamrot {

class ActionManager {
private:
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

public:
  /**
   * @brief Constructor using flatbuffers ActionsData object
   *
   * @param actions_data Raw pointer to ActionsData object
   */
  ActionManager() = default;
};
} // namespace steamrot
