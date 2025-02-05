#pragma once
#include <SFML/Graphics.hpp>
namespace SteamRot {

// the total number of inputs the user can use from the keyboard and mouse.
size_t const kUserInputCount{sf::Keyboard::ScancodeCount +
                             sf::Mouse::ButtonCount + 1};

} // namespace SteamRot
