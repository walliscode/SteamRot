/////////////////////////////////////////////////
/// @file
/// @brief Delaration of collision detection functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "UIElement.h"
#include <SFML/Graphics/Rect.hpp>
namespace steamrot {
namespace collision {

/////////////////////////////////////////////////
/// @brief Checks if the mouse position is within the given bounds
///
/// @param mouse_position The current global mouse position
/// @param bounds The global bounds to check against
/// @return Bool indicating if the mouse is over the bounds
/////////////////////////////////////////////////
bool IsMouseOverBounds(const sf::Vector2i &mouse_position,
                       const sf::FloatRect &bounds);

void CheckMouseOverUIElement(const sf::Vector2i &mouse_position,
                             UIElement &element);

} // namespace collision
} // namespace steamrot
