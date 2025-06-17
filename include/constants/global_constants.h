#pragma once
#include <SFML/Graphics.hpp>
#include <cstddef>
namespace steamrot {

// the total number of inputs the user can use from the keyboard and mouse.
size_t const kUserInputCount{sf::Keyboard::ScancodeCount +
                             sf::Mouse::ButtonCount};

sf::Vector2u const kWindowSize{900, 600};
} // namespace steamrot
