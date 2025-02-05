#pragma once
#include "global_constants.h"
#include <SFML/Graphics.hpp>
#include <bitset>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Action {

public:
  Action(std::string container_name);
  std::vector<std::string>
  GenerateActions(std::bitset<SteamRot::kUserInputCount> action_generator);

private:
  std::string m_container_name;
  // bitset that combines global events to produce object specific actions
  std::unordered_map<std::bitset<SteamRot::kUserInputCount>, std::string>
      m_action_map;

  static std::map<std::string, sf::Keyboard::Key> m_key_map;
  static std::map<std::string, sf::Mouse::Button> m_mouse_map;

  void RegisterActions(std::string container_name);
};
