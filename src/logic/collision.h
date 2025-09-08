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

/////////////////////////////////////////////////
/// @brief Checks if the mouse is over a given UI element and updates its state
/// accordingly.
///
/// @param mouse_position The current global mouse position
/// @param element UiElement to check against
/////////////////////////////////////////////////
void CheckMouseOverUIElement(const sf::Vector2i &mouse_position,
                             UIElement &element);

/////////////////////////////////////////////////
/// @brief Checks if the mouse is over a given UI element and its nested
/// children,
///
/// This functions checks an element and all children, if a child is hovered, it
/// updates the parent to false.
/// @param mouse_position The current global mouse position
/// @param element UIElement to check against
/////////////////////////////////////////////////
void CheckMouseOverNestedUIElement(const sf::Vector2i &mouse_position,
                                   UIElement &element);

} // namespace collision
} // namespace steamrot
