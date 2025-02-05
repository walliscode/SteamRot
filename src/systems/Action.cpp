#include "Action.h"
#include "general_util.h"
#include <cstddef>
#include <iostream>

Action::Action(std::string container_name) : m_container_name(container_name) {

  // pull in json config for user defied actions
  RegisterActions(container_name);
}

// insert all keys for the sf keyboard into the map

std::map<std::string, sf::Keyboard::Key> Action::m_key_map = {
    {"A", sf::Keyboard::Key::A},
    {"B", sf::Keyboard::Key::B},
    {"C", sf::Keyboard::Key::C},
    {"D", sf::Keyboard::Key::D},
    {"E", sf::Keyboard::Key::E},
    {"F", sf::Keyboard::Key::F},
    {"G", sf::Keyboard::Key::G},
    {"H", sf::Keyboard::Key::H},
    {"I", sf::Keyboard::Key::I},
    {"J", sf::Keyboard::Key::J},
    {"K", sf::Keyboard::Key::K},
    {"L", sf::Keyboard::Key::L},
    {"M", sf::Keyboard::Key::M},
    {"N", sf::Keyboard::Key::N},
    {"O", sf::Keyboard::Key::O},
    {"P", sf::Keyboard::Key::P},
    {"Q", sf::Keyboard::Key::Q},
    {"R", sf::Keyboard::Key::R},
    {"S", sf::Keyboard::Key::S},
    {"T", sf::Keyboard::Key::T},
    {"U", sf::Keyboard::Key::U},
    {"V", sf::Keyboard::Key::V},
    {"W", sf::Keyboard::Key::W},
    {"X", sf::Keyboard::Key::X},
    {"Y", sf::Keyboard::Key::Y},
    {"Z", sf::Keyboard::Key::Z},
    {"Num0", sf::Keyboard::Key::Num0},
    {"Num1", sf::Keyboard::Key::Num1},
    {"Num2", sf::Keyboard::Key::Num2},
    {"Num3", sf::Keyboard::Key::Num3},
    {"Num4", sf::Keyboard::Key::Num4},
    {"Num5", sf::Keyboard::Key::Num5},
    {"Num6", sf::Keyboard::Key::Num6},
    {"Num7", sf::Keyboard::Key::Num7},
    {"Num8", sf::Keyboard::Key::Num8},
    {"Num9", sf::Keyboard::Key::Num9},
    {"Escape", sf::Keyboard::Key::Escape},
    {"LControl", sf::Keyboard::Key::LControl},
    {"LShift", sf::Keyboard::Key::LShift},
    {"LAlt", sf::Keyboard::Key::LAlt},
};

std::map<std::string, sf::Mouse::Button> Action::m_mouse_map = {
    {"LeftClick", sf::Mouse::Button::Left},
    {"RightClick", sf::Mouse::Button::Right},
    {"MiddleClick", sf::Mouse::Button::Middle},
};

void Action::RegisterActions(std::string container_name) {
  // check json file exists and load in
  std::string file_path =
      std::string(RESOURCES_DIR) + "/jsons/actions_" + container_name + ".json";

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
    std::cout << "Registering action: " << action_name << std::endl;
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
    std::cout << "Action: " << action_name << " has been registered"
              << std::endl;
  }
};

std::vector<std::string> Action::GenerateActions(
    std::bitset<SteamRot::kUserInputCount> action_generator) {
  std::vector<std::string> actions;

  std::string action = m_action_map.at(action_generator);

  actions.push_back(action);
  return actions;
}
