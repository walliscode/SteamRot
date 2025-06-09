////////////////////////////////////////////////////////////
// Preprocessor directives
////////////////////////////////////////////////////////////
#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "actions.h"
#include "actions_generated.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <bitset>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace steamrot {

class ActionManager {
private:
  ////////////////////////////////////////////////////////////
  // |Member: stores key mappings to actions for this scene
  ////////////////////////////////////////////////////////////
  std::unordered_map<
      std::bitset<sf::Keyboard::KeyCount + sf::Mouse::ButtonCount>, Actions>
      m_key_to_action_map;

  ////////////////////////////////////////////////////////////
  // |brief: returns map of string letters to sf::Keyboard enum
  ////////////////////////////////////////////////////////////
  static const std::map<std::string, sf::Keyboard::Key> &
  getStringToKeyboardMap();

  ////////////////////////////////////////////////////////////
  // |brief: returns map of string letters to sf::Mouse enum
  ////////////////////////////////////////////////////////////
  static const std::map<std::string, sf::Mouse::Button> &getStringToMouseMap();

  ////////////////////////////////////////////////////////////
  // |brief register actions for object instance from json
  ////////////////////////////////////////////////////////////
  void RegisterActions(const json &congig);

  /**
   * @brief Register actions for object instance from flatbuffers ActionsData
   *
   * @param actions_data ActionsData object containing action definitions
   */
  void RegisterActions(const ActionsData *actions_data);

public:
  ActionManager() = default;

  /**
   * @brief Constructor using flatbuffers ActionsData object
   *
   * @param actions_data Raw pointer to ActionsData object
   */
  ActionManager(const ActionsData *actions_data);
  ////////////////////////////////////////////////////////////
  // |brief Generate any possible actions for this scene as bitflag enum
  ////////////////////////////////////////////////////////////
  const Actions GenerateActions(
      std::bitset<sf::Keyboard::KeyCount + sf::Mouse::ButtonCount>);
};
} // namespace steamrot
