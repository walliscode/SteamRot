/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for mouse collision checking of
/// MachinaFormScaffold parts
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "MachinaFormScaffold.h"
#include "UIElement.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot::logic::collision::mouse {

/////////////////////////////////////////////////
/// @brief Checks if the mouse position is within the given bounds.
///
/// @param mouse_position The current global mouse position.
/// @param bounds The global bounds to check against.
/// @return True if the mouse is within the bounds, false otherwise.
/////////////////////////////////////////////////
bool IsMouseOverBounds(const sf::Vector2i &mouse_position,
                       const sf::FloatRect &bounds);

/////////////////////////////////////////////////
/// @brief Checks if the mouse is over a UIElement or any of its nested
/// children, and sets each element's is_mouse_over accordingly.
///
/// Children are evaluated in descending priority order. Once any element in a
/// child's subtree is hovered, lower-priority siblings are skipped entirely.
/// If a descendant is hovered the parent's is_mouse_over is set to false.
///
/// @param mouse_position The current global mouse position.
/// @param element UIElement to check against (recurses into children).
/////////////////////////////////////////////////
void CheckMouseOver(const sf::Vector2i &mouse_position, UIElement &element);

/////////////////////////////////////////////////
/// @brief Recursively clears is_mouse_over on an element and all its children.
///
/// Used to ensure that lower-priority UI entities do not retain hover state
/// when a higher-priority entity has claimed the mouse input.
///
/// @param element UIElement whose is_mouse_over is cleared (recurses into
/// children).
/////////////////////////////////////////////////
void ClearMouseOver(UIElement &element);

/////////////////////////////////////////////////
/// @brief Returns true if any element in the tree (element or any descendant)
/// has is_mouse_over set to true.
///
/// @param element Root UIElement to check recursively.
/// @return True if any element in the tree is hovered.
/////////////////////////////////////////////////
bool AnyMouseOver(const UIElement &element);

/////////////////////////////////////////////////
/// @brief Checks whether the mouse is over the growth point and sets
/// is_mouse_over accordingly.
///
/// @param mouse_position Mouse position in world coordinates.
/// @param growth_point GrowthPoint to check against.
/////////////////////////////////////////////////
void CheckMouseOver(const sf::Vector2i &mouse_position,
                    GrowthPoint &growth_point);

/////////////////////////////////////////////////
/// @brief Checks whether the mouse is over the socket and sets
/// is_mouse_over accordingly.
///
/// @param mouse_position Mouse position in world coordinates.
/// @param socket Socket to check against.
/////////////////////////////////////////////////
void CheckMouseOver(const sf::Vector2i &mouse_position, Socket &socket);

/////////////////////////////////////////////////
/// @brief Checks whether the mouse is over any socket on a FragmentInstance
/// and sets each socket's is_mouse_over accordingly.
///
/// @param mouse_position Mouse position in world coordinates.
/// @param fragment_instance FragmentInstance whose sockets are checked.
/////////////////////////////////////////////////
void CheckMouseOver(const sf::Vector2i &mouse_position,
                    FragmentInstance &fragment_instance);

/////////////////////////////////////////////////
/// @brief Checks whether the mouse is over any socket on a JointInstance
/// and sets each socket's is_mouse_over accordingly.
///
/// @param mouse_position Mouse position in world coordinates.
/// @param joint_instance JointInstance whose sockets are checked.
/////////////////////////////////////////////////
void CheckMouseOver(const sf::Vector2i &mouse_position,
                    JointInstance &joint_instance);

} // namespace steamrot::logic::collision::mouse
