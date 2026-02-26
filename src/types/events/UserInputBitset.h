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
#include <algorithm>
#include <bitset>
#include <functional>
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

/////////////////////////////////////////////////
/// std::hash specialisation so UserInputBitset can be used as an
/// unordered_map key.  kTotalBits exceeds 64 so to_ulong() is unavailable;
/// bits are folded into 64-bit words and combined with boost-style mixing to
/// avoid any heap allocation.
/////////////////////////////////////////////////
template <> struct std::hash<steamrot::UserInputBitset> {
  size_t operator()(const steamrot::UserInputBitset &b) const noexcept {
    size_t seed = 0;
    // Process kTotalBits in 64-bit chunks.
    for (size_t i = 0; i < steamrot::kTotalBits; i += 64) {
      size_t word = 0;
      const size_t end = std::min(i + 64, steamrot::kTotalBits);
      for (size_t j = i; j < end; ++j) {
        if (b.test(j)) {
          word |= (size_t{1} << (j - i));
        }
      }
      // Boost-style hash_combine mixing.
      seed ^= word + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};
