////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "ActionManager.h"
#include "log_handler.h"
#include "magic_enum/magic_enum.hpp"
#include "spdlog/common.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>

using namespace magic_enum::bitwise_operators;
namespace steamrot {

////////////////////////////////////////////////////////////
ActionManager::ActionManager(const ActionsData *actions_data) {
  // register actions from ActionsData object
  std::cout << "Registering actions from ActionsData object..." << std::endl;
  RegisterActions(actions_data);
  std::cout << "Actions registered from ActionsData object." << std::endl;
}
////////////////////////////////////////////////////////////
const std::map<std::string, sf::Keyboard::Key> &
ActionManager::getStringToKeyboardMap() {

  // map of string to sf::Keyboard enum

  static const std::map<std::string, sf::Keyboard::Key> string_to_key_map = {
      {"A", sf::Keyboard::Key::A}, {"B", sf::Keyboard::Key::B},
      {"C", sf::Keyboard::Key::C}, {"D", sf::Keyboard::Key::D},
      {"E", sf::Keyboard::Key::E}, {"F", sf::Keyboard::Key::F},
      {"G", sf::Keyboard::Key::G}, {"H", sf::Keyboard::Key::H},
      {"K", sf::Keyboard::Key::K}, {"L", sf::Keyboard::Key::L},
      {"M", sf::Keyboard::Key::M}, {"N", sf::Keyboard::Key::N},
      {"O", sf::Keyboard::Key::O}, {"P", sf::Keyboard::Key::P},
      {"Q", sf::Keyboard::Key::Q}, {"R", sf::Keyboard::Key::R},
      {"S", sf::Keyboard::Key::S}, {"T", sf::Keyboard::Key::T},
      {"U", sf::Keyboard::Key::U}, {"V", sf::Keyboard::Key::V},
      {"W", sf::Keyboard::Key::W}, {"X", sf::Keyboard::Key::X},
      {"Y", sf::Keyboard::Key::Y}, {"Z", sf::Keyboard::Key::Z},
  };
  return string_to_key_map;
}; // namespace steamrot
////////////////////////////////////////////////////////////
const std::map<std::string, sf::Mouse::Button> &
ActionManager::getStringToMouseMap() {

  // map of string to sf::Mouse enum

  static const std::map<std::string, sf::Mouse::Button> string_to_mouse_map = {

      {"Left", sf::Mouse::Button::Left},
      {"Right", sf::Mouse::Button::Right},
      {"Middle", sf::Mouse::Button::Middle},

  };
  return string_to_mouse_map;
}

////////////////////////////////////////////////////////////
void ActionManager::RegisterActions(const json &config) {

  // check config object, error out if incorrect
  if (!config.contains("actions")) {
    log_handler::ProcessLog(spdlog::level::level_enum::err,
                            log_handler::LogCode::kInvalidJSONKey,
                            "Action Config should contain action key");
  }

  // cycle through the json objects and check and register
  for (auto &action : config["actions"]) {

    // create new bitset for this action
    std::bitset<sf::Keyboard::KeyCount + sf::Mouse::ButtonCount> key_bitset;

    // cycle though keys
    for (auto &input : action["inputs"]) {

      // check if input is a keyboard or mouse input, if not error out
      if (input["type"] == "keyboard") {
        // check if key is in the map, if not error out
        auto it = getStringToKeyboardMap().find(input["value"]);

        if (it == getStringToKeyboardMap().end()) {
          log_handler::ProcessLog(spdlog::level::level_enum::err,
                                  log_handler::LogCode::kInvalidJSONKey,
                                  "key " + input["value"].get<std::string>() +
                                      " is not a valid key enum value");
        } else {
          // set the bit in the bitset for this key
          key_bitset.set(static_cast<size_t>(it->second));
        }

      } else if (input["type"] == "mouse") {
        // check if mouse button is in the map, if not error out
        auto it = getStringToMouseMap().find(input["value"]);
        if (it == getStringToMouseMap().end()) {
          log_handler::ProcessLog(spdlog::level::level_enum::err,
                                  log_handler::LogCode::kInvalidJSONKey,
                                  "mouse button " +
                                      input["value"].get<std::string>() +
                                      " is not a valid mouse enum value");
        } else {

          // set the bit in the bitset for this mouse button
          key_bitset.set(static_cast<size_t>(it->second) +
                         sf::Keyboard::KeyCount);
        }
      } else {
        log_handler::ProcessLog(spdlog::level::level_enum::err,
                                log_handler::LogCode::kInvalidJSONKey,
                                "input type " +
                                    input["type"].get<std::string>() +
                                    " is not a valid input type");
      }
    }

    // check if string action in json is a valid action in the action enum
    std::string action_name = action["name"];
    auto action_enum = magic_enum::enum_cast<Actions>(action_name);

    // check if action enum is valid, if not error out
    if (!action_enum.has_value()) {
      log_handler::ProcessLog(
          spdlog::level::level_enum::err, log_handler::LogCode::kInvalidJSONKey,
          "action name" + action_name + " is not a valid action enum value");
    } else {
      // register the action in the map
      m_key_to_action_map[key_bitset] = action_enum.value();
    }
  }
}

////////////////////////////////////////////////////////////
void ActionManager::RegisterActions(const ActionsData *actions_data) {
  // check if actions_data is null

  if (actions_data->actions() == nullptr) {
    log_handler::ProcessLog(spdlog::level::level_enum::info,
                            log_handler::LogCode::kNoCode,
                            "ActionsData object is null");
    std::cout << "ActionsData object is null, skipping registration"
              << std::endl;
    return;
  }
  // cycle through the actions in the ActionsData object
  for (const auto &action : *actions_data->actions()) {

    // create new bitset for this action
    std::bitset<sf::Keyboard::KeyCount + sf::Mouse::ButtonCount> key_bitset;

    // cycle throught the actions parts to compose the full action
    for (const auto &part : *action->parts()) {

      // check if input is a keyboard or mouse input
      if (part->type() == InputType::InputType_Keyboard) {

        // get the sf::Keyboard key from the getStringToKeyboardMap
        auto it =
            getStringToKeyboardMap().find(part->input_as_Key()->name()->str());

        // check if key is in the map, if not log as warning but move on
        if (it == getStringToKeyboardMap().end()) {
          log_handler::ProcessLog(
              spdlog::level::level_enum::warn, log_handler::LogCode::kNoCode,
              "key " + part->input_as_Key()->name()->str() +
                  " is not a valid key enum value, skipping");
        } else {
          // set the bit in the bitset for this key
          key_bitset.set(static_cast<size_t>(it->second));
        }
      } else if (part->type() == InputType::InputType_Mouse) {

        // get the sf::Mouse button from the getStringToMouseMap
        auto it = getStringToMouseMap().find(
            part->input_as_MouseButton()->name()->str());

        // check if mouse button is in the map, if not log as warning but move
        // on
        if (it == getStringToMouseMap().end()) {
          log_handler::ProcessLog(
              spdlog::level::level_enum::warn, log_handler::LogCode::kNoCode,
              "mouse button " + part->input_as_MouseButton()->name()->str() +
                  " is not a valid mouse enum value, skipping");
        } else {
          // set the bit in the bitset for this mouse button
          key_bitset.set(static_cast<size_t>(it->second) +
                         sf::Keyboard::KeyCount);
        }
      }
    }
    // once all parts are processed, add the action to the map
    // check if string action in json is a valid action in the action enum
    std::string action_name = action->name()->str();
    auto action_enum = magic_enum::enum_cast<Actions>(action_name);

    // check if action enum is valid, if not error out
    if (!action_enum.has_value()) {
      log_handler::ProcessLog(
          spdlog::level::level_enum::err, log_handler::LogCode::kInvalidJSONKey,
          "action name" + action_name + " is not a valid action enum value");
    } else {
      // register the action in the map
      m_key_to_action_map[key_bitset] = action_enum.value();
    }
  }
}
////////////////////////////////////////////////////////////
const Actions ActionManager::GenerateActions(
    std::bitset<sf::Keyboard::KeyCount + sf::Mouse::ButtonCount> key_bitset) {

  // create an actions bitset
  Actions actions;

  // cycle through the key to action map and check to see if any of the keys
  // match
  for (auto &[key, value] : m_key_to_action_map) {
    if ((key_bitset & key) == key) {

      // add to current actions
      actions |= value;
    }
  }
  return actions;
}
} // namespace steamrot
