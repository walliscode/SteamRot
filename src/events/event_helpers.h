#pragma once

#include "user_input_generated.h"
#include <SFML/Graphics.hpp>
#include <bitset>

using UserInputBitset =
    std::bitset<sf::Keyboard::KeyCount + sf::Keyboard::KeyCount +
                sf::Mouse::ButtonCount + sf::Mouse::ButtonCount>;
namespace steamrot {

////////////////////////////////////////////////////////////
// |brief: maps flatbuffer defined enum KeyboardInput to SFML enum
// sf::Keyboard::Key
////////////////////////////////////////////////////////////
static const std::unordered_map<KeyboardInput, sf::Keyboard::Key> &
GetFlatbuffersToSFMLKeyboardMap();

////////////////////////////////////////////////////////////
// |brief: maps flatbuffer defined enum MouseInput to SFML enum
// sf::Mouse::Button
////////////////////////////////////////////////////////////
static const std::unordered_map<MouseInput, sf::Mouse::Button> &
GetFlatbuffersToSFMLMouseMap();

/////////////////////////////////////////////////
/// @brief Take in flatbuffers data object and convert EventBitset
///
/// @return [TODO:return]
/////////////////////////////////////////////////
const UserInputBitset ConvertActionKeysToEvent(const UserInputBitsetData &data);
} // namespace steamrot
