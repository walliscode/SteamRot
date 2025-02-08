#include "ActionManager.h"
#include "general_util.h"

#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <magic_enum/magic_enum_iostream.hpp>
#include <memory>

using namespace magic_enum::bitwise_operators;

bool compareFlags(EventFlags flag1, EventFlags flag2) {
  // Example: check if both flags have at least one common bit set
  return (flag1 & flag2) != static_cast<EventFlags>(0);
}

ActionManager::ActionManager(std::string container_name)
    : m_container_name(container_name) {

  // pull in json config for user defied actions
  RegisterActions(container_name);
}

// insert all keys for the sf keyboard into the map

void ActionManager::RegisterActions(std::string container_name) {
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

    // create enum to store the action flags
    EventFlags action_generator{0};

    for (auto &input : action["inputs"]) {

      if (input["type"] == "key") {
        std::string key_string = input["code"];
        auto key_check = magic_enum::enum_cast<EventFlags>(key_string);

        if (key_check.has_value()) {
          action_generator |= key_check.value();
        } else {
          // we need the program to terminate if the key does not exist
          throw std::runtime_error("Key does not exist in the EventFlags enum");
        }

      } else if (input["type"] == "mouse") {
      }
    }
    // is the action repeatable
    bool repeatable = action["repeatable"];
    // add the action to the action map
    m_action_list.push_back(Action(action_name, action_generator, repeatable));

    std::cout << "Action: " << action_name << " has been registered"
              << std::endl;
  }
};

std::vector<std::shared_ptr<Action>>
ActionManager::GenerateActions(const EventFlags &event_flags) {

  std::vector<std::shared_ptr<Action>> actionables;

  // exclusion logic
  for (auto &action : m_action_list) {

    // cast the enums to their underlying type and see if they are equal
    bool check_flags = static_cast<std::uint64_t>(action.m_action_flags) ==
                       static_cast<std::uint64_t>(event_flags);

    if (check_flags) {

      // if latch is on and action is not reatable then skip
      if (action.m_repeatable == false && action.m_latch == true) {

        continue;

      } else {
        // set the latch to true
        action.m_latch = true;
        actionables.push_back(std::make_shared<Action>(action));
      }

      // if the action is not in the bitset then turn the latch off
    } else {

      action.m_latch = false;
    }
  }
  return actionables;
};
