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
#include <SFML/System/Vector2.hpp>

namespace steamrot::logic::collision::mouse {

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
