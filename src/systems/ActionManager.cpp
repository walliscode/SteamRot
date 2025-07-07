////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "ActionManager.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

namespace steamrot {

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

} // namespace steamrot
