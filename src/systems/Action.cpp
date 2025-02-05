#include "Action.h"
#include "general_util.h"
#include <SFML/Window/Keyboard.hpp>
#include <cstddef>
#include <iostream>

Action::Action(const json &action_setup, std::string container_name)
    : m_container_name(container_name) {

  // initilise the static maps
  MapStringToKeys();
  MapStringToMouse();

  // pull in json config for user defied actions
  RegisterActions(action_setup, container_name);
}

// insert all keys for the sf keyboard into the map
void Action::MapStringToKeys() {
  m_key_map.insert({"A", sf::Keyboard::Key::A});
  m_key_map.insert({"B", sf::Keyboard::Key::B});
  m_key_map.insert({"C", sf::Keyboard::Key::C});
  m_key_map.insert({"D", sf::Keyboard::Key::D});
  m_key_map.insert({"E", sf::Keyboard::Key::E});
};

// instert all mouse buttons for the sf mouse into the map
void Action::MapStringToMouse() {
  m_mouse_map.insert({"LeftClick", sf::Mouse::Button::Left});
  m_mouse_map.insert({"RightClick", sf::Mouse::Button::Right});
  m_mouse_map.insert({"MiddleClick", sf::Mouse::Button::Middle});
};

void Action::RegisterActions(const json &action_setup,
                             std::string container_name) {
  // check json file exists and load in
  std::string file_path =
      std::string(RESOURCES_DIR) + "/actions_" + container_name + ".json";

  if (!utils::fileExists(file_path)) {
    std::cout << "File does not exist: " << file_path << std::endl;
    return;
  };

  std::ifstream f(file_path);
  json actions = json::parse(f);

  for (auto &action : actions) {

    // get string name that we will use to reference the action in the game
    // logic
    std::string action_name = action["name"];

    // generate a bitset that represents the action
    std::bitset<SteamRot::kUserInputCount> action_generator;
    for (auto &input : action["inputs"]) {
      if (input["type"] == "key") {
        // get the key number from the m_key_map
        // throw error if key does not exist
        auto it = m_key_map.find(input["code"]);
        if (it == m_key_map.end()) {
          // we need the program to terminate if the key does not exist
          throw std::runtime_error("Key does not exist in m_key_map");
        };
        size_t key_code = static_cast<size_t>(it->second);
        action_generator.set(key_code);

      } else if (input["type"] == "mouse") {
        // get the mouse number from the m_mouse_map
        // throw error if mouse does not exist
        auto it = m_mouse_map.find(input["code"]);
        if (it == m_mouse_map.end()) {
          // we need the program to terminate if the mouse does not exist
          throw std::runtime_error("Mouse does not exist in m_mouse_map");
        };
        // the mouse codes are added the key codes in the bitset so need
        // shifting
        size_t mouse_code = static_cast<size_t>(it->second) +
                            static_cast<size_t>(sf::Keyboard::ScancodeCount);
        action_generator.set(mouse_code);
      }
    }
  }
};

std::vector<std::string> Action::GenerateActions(
    std::bitset<SteamRot::kUserInputCount> action_generator) {
  std::vector<std::string> actions;

  std::string action = m_action_map.at(action_generator);

  actions.push_back(action);
  return actions;
}
