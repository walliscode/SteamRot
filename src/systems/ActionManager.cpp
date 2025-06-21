////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "ActionManager.h"
#include "EventHandler.h"
#include "actions_generated.h"
#include "log_handler.h"

#include "spdlog/common.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>

namespace steamrot {

////////////////////////////////////////////////////////////
ActionManager::ActionManager(const ActionsData *actions_data) {
  // register actions from ActionsData object
  std::cout << "Registering actions from ActionsData object..." << std::endl;
  RegisterActions(actions_data);
  std::cout << "Actions registered from ActionsData object." << std::endl;
}
////////////////////////////////////////////////////////////
const std::unordered_map<KeyboardInput, sf::Keyboard::Key> &
ActionManager::getFlatbuffersToSFMLKeyboardMap() {

  // map of string to sf::Keyboard enum

  static const std::unordered_map<KeyboardInput, sf::Keyboard::Key>
      string_to_key_map = {
          {KeyboardInput_A, sf::Keyboard::Key::A},
          {KeyboardInput_B, sf::Keyboard::Key::B},
          {KeyboardInput_C, sf::Keyboard::Key::C},
          {KeyboardInput_D, sf::Keyboard::Key::D},
          {KeyboardInput_E, sf::Keyboard::Key::E},
          {KeyboardInput_F, sf::Keyboard::Key::F},
          {KeyboardInput_G, sf::Keyboard::Key::G},
          {KeyboardInput_H, sf::Keyboard::Key::H},
          {KeyboardInput_K, sf::Keyboard::Key::K},
          {KeyboardInput_L, sf::Keyboard::Key::L},
          {KeyboardInput_M, sf::Keyboard::Key::M},
          {KeyboardInput_N, sf::Keyboard::Key::N},
          {KeyboardInput_O, sf::Keyboard::Key::O},
          {KeyboardInput_P, sf::Keyboard::Key::P},
          {KeyboardInput_Q, sf::Keyboard::Key::Q},
          {KeyboardInput_R, sf::Keyboard::Key::R},
          {KeyboardInput_S, sf::Keyboard::Key::S},
          {KeyboardInput_T, sf::Keyboard::Key::T},
          {KeyboardInput_U, sf::Keyboard::Key::U},
          {KeyboardInput_V, sf::Keyboard::Key::V},
          {KeyboardInput_W, sf::Keyboard::Key::W},
          {KeyboardInput_X, sf::Keyboard::Key::X},
          {KeyboardInput_Y, sf::Keyboard::Key::Y},
          {KeyboardInput_Z, sf::Keyboard::Key::Z},
      };
  return string_to_key_map;
}; // namespace steamrot
////////////////////////////////////////////////////////////
const std::unordered_map<MouseInput, sf::Mouse::Button> &
ActionManager::getStringToMouseMap() {

  // map of string to sf::Mouse enum

  static const std::unordered_map<MouseInput, sf::Mouse::Button>
      string_to_mouse_map = {

          {MouseInput_LEFT_CLICK, sf::Mouse::Button::Left},
          {MouseInput_RIGHT_CLICK, sf::Mouse::Button::Right},
          {MouseInput_MIDDLE_CLICK, sf::Mouse::Button::Middle},

      };
  return string_to_mouse_map;
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
    EventBitset key_bitset;

    if (action->keyboard_pressed()) {
      for (const auto &key : *action->keyboard_pressed()) {
        // needs casting back to its enum type (not sure why it has not
        // maintained its original type)
        KeyboardInput keyboard_press = static_cast<KeyboardInput>(key);
        key_bitset.set(static_cast<size_t>(keyboard_press));
      }
    }
    if (action->keyboard_released()) {
      for (const auto &key : *action->keyboard_released()) {
        // needs casting back to its enum type (not sure why it has not
        // maintained its original type)
        KeyboardInput keyboard_release = static_cast<KeyboardInput>(key);
        // make sure it has been shifted by the number of keys
        key_bitset.set(
            static_cast<size_t>(keyboard_release + sf::Keyboard::KeyCount));
      }
    }

    if (action->mouse_pressed()) {
      for (const auto &button : *action->mouse_pressed()) {
        MouseInput mouse_press = static_cast<MouseInput>(button);

        // make sure it has been shifted by the number of keys * 2
        key_bitset.set(static_cast<size_t>(mouse_press) +
                       (sf::Keyboard::KeyCount * 2));
      }
    }
    if (action->mouse_released()) {
      for (const auto &button : *action->mouse_released()) {
        MouseInput mouse_release = static_cast<MouseInput>(button);

        // make sure it has been shifted by the number of keys * 2 and the
        // number of mouse buttons
        key_bitset.set(static_cast<size_t>(mouse_release) +
                       (sf::Keyboard::KeyCount * 2) +
                       static_cast<size_t>(sf::Mouse::ButtonCount));
      }
    }

    // add to the action map
    m_scene_event_to_action_map.at(key_bitset) =
        ActionNames(action->action_name());
  }
}

////////////////////////////////////////////////////////////
void ActionManager::ProcessSceneLevelActions(const EventBitset &input_event) {
  m_scene_level_actions = m_scene_event_to_action_map[input_event];
}

/////////////////////////////////////////////////
void ActionManager::ClearActions() {
  m_scene_level_actions = static_cast<ActionNames>(0);
}
} // namespace steamrot
