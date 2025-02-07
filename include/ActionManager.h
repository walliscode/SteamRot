#pragma once
#include "global_constants.h"
#include <SFML/Graphics.hpp>
#include <bitset>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

struct Action {
  Action(std::string name, std::bitset<SteamRot::kUserInputCount> action_bitset,
         bool repeatable)
      : m_name(name), m_action_bitset(action_bitset),
        m_repeatable(repeatable) {};

  std::string m_name;
  std::bitset<SteamRot::kUserInputCount> m_action_bitset;
  // can an action be repeated if the key is held down
  bool m_repeatable{false};
  // latch the action so that it can only be triggered once, until the key is
  // released
  bool m_latch{false};

  // should the action be carried out (latch dependent)
  bool m_active{false};
};

class ActionManager {

public:
  ActionManager(std::string container_name);

  // return a vector of Actions, readied to be used. Any exclustio logic should
  // happen in GenerateActions
  std::vector<std::shared_ptr<Action>>
  GenerateActions(std::bitset<SteamRot::kUserInputCount> action_generator);

private:
  //
  std::string m_container_name;
  // list of actions, generated once at run time
  std::vector<Action> m_action_list;

  static std::map<std::string, sf::Keyboard::Key> m_key_map;
  static std::map<std::string, sf::Mouse::Button> m_mouse_map;

  void RegisterActions(std::string container_name);
};
