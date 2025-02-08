#pragma once
#include "EventFlags.h"

#include <SFML/Graphics.hpp>

#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

struct Action {
  Action(std::string name, EventFlags action_flags, bool repeatable)
      : m_name(name), m_action_flags(action_flags), m_repeatable(repeatable) {};

  std::string m_name;
  const EventFlags m_action_flags;
  // can an action be repeated if the key is held down
  bool m_repeatable{false};
  // latch the action so that it can only be triggered once, until the key is
  // released
  bool m_latch{false};
};

class ActionManager {

public:
  ActionManager(std::string container_name);

  // return a vector of Actions, readied to be used. Any exclustio logic should
  // happen in GenerateActions
  std::vector<std::shared_ptr<Action>>
  GenerateActions(const EventFlags &event_flags);

private:
  //
  std::string m_container_name;
  // list of actions, generated once at run time
  std::vector<Action> m_action_list;

  static std::map<std::string, sf::Keyboard::Key> m_key_map;
  static std::map<std::string, sf::Mouse::Button> m_mouse_map;

  void RegisterActions(std::string container_name);
};
