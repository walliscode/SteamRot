#pragma once
#include <SFML/Graphics.hpp>
namespace SteamRot {

// the total number of inputs the user can use from the keyboard and mouse.
size_t const kUserInputCount{sf::Keyboard::ScancodeCount +
                             sf::Mouse::ButtonCount};

sf::Vector2u const kWindowSize{1280, 800};
} // namespace SteamRot
