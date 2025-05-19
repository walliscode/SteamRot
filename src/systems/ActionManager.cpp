////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "ActionManager.h"
#include "log_handler.h"
#include "magic_enum/magic_enum.hpp"
#include "spdlog/common.h"
#include <SFML/Window/Keyboard.hpp>

namespace steamrot {
////////////////////////////////////////////////////////////
ActionManager::ActionManager(json &config) {}

////////////////////////////////////////////////////////////
const std::map<std::string, sf::Keyboard::Key>
ActionManager::getStringKeyBoardMap() {

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
      {"Y", sf::Keyboard::Key::Y}, {"Z", sf::Keyboard::Key::Z}};
  return string_to_key_map;
};

////////////////////////////////////////////////////////////
void ActionManager::RegisterActions(json &config) {

  // check config object, error out if incorrect
  if (!config.contains("actions")) {
    log_handler::ProcessLog(spdlog::level::level_enum::err,
                            log_handler::LogCode::kInvalidJSONKey,
                            "Action Config should contain action key");
  }

  // cycle through the json objects and check and register
  for (auto &action : config["actions"]) {

    // create new bitset for this action
    std::bitset<sf::Keyboard::KeyCount> key_bitset;

    // cycle though keys
    for (auto &key : action["keys"]) {
      // check if key is in the map, if not error out
      auto it = getStringKeyBoardMap().find(key.get<std::string>());

      if (it == getStringKeyBoardMap().end()) {
        log_handler::ProcessLog(spdlog::level::level_enum::err,
                                log_handler::LogCode::kInvalidJSONKey,
                                "key " + key.get<std::string>() +
                                    " is not a valid key enum value");
      } else {

        // set the bitset for this key
        key_bitset.set(static_cast<size_t>(it->second));
      }
    }

    // check if string action in json is a valid action in the action enum
    std::string action_name = action["name"];
    auto action_enum = magic_enum::enum_cast<actions>(action_name);

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

} // namespace steamrot
