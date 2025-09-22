/////////////////////////////////////////////////
/// @file
/// @brief Decleration of the UserInputBitset struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <bitset>
#include <vector>

namespace steamrot {

constexpr size_t kKeyboardBits = sf::Keyboard::KeyCount * 2;
constexpr size_t kMouseBits = sf::Mouse::ButtonCount * 2;
constexpr size_t kTotalBits = kKeyboardBits + kMouseBits;

struct UserInputBitset : public std::bitset<kTotalBits> {

  // Default constructor creates an empty bitset
  UserInputBitset() {
    // reset all bits
    this->reset();
  }

  // Constructor from vector of SFML events using getIf<>
  UserInputBitset(const std::vector<sf::Event> &events) {
    // reset all bits
    this->reset();

    // process each event
    for (const auto &event : events) {
      if (const auto *keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        setKeyPressed(keyPressed->code);

      } else if (const auto *keyReleased =
                     event.getIf<sf::Event::KeyReleased>()) {
        setKeyReleased(keyReleased->code);

      } else if (const auto *mouseButtonPressed =
                     event.getIf<sf::Event::MouseButtonPressed>()) {
        setMousePressed(mouseButtonPressed->button);

      } else if (const auto *mouseButtonReleased =
                     event.getIf<sf::Event::MouseButtonReleased>()) {
        setMouseReleased(mouseButtonReleased->button);
      }
    }
  }

  void setKeyPressed(sf::Keyboard::Key key) {
    this->set(static_cast<size_t>(key));
  }
  void setKeyReleased(sf::Keyboard::Key key) {
    this->set(static_cast<size_t>(key) + sf::Keyboard::KeyCount);
  }

  void setMousePressed(sf::Mouse::Button button) {
    this->set(kKeyboardBits + static_cast<size_t>(button));
  }
  void setMouseReleased(sf::Mouse::Button button) {
    this->set(kKeyboardBits + static_cast<size_t>(button) +
              sf::Mouse::ButtonCount);
  }

  void reset() { std::bitset<kTotalBits>::reset(); }

  bool operator==(const UserInputBitset &other) const {
    return std::bitset<kTotalBits>::operator==(other);
  }
};

} // namespace steamrot
