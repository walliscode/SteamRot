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
#include "CUserInterface.h"
#include "MachinaFormScaffold.h"
#include "UIElement.h"
#include "entity_memory.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

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
/// Children are evaluated in descending priority order only when
/// element.children_active is true. When children are inactive, their stale
/// hover state is cleared and the element itself is tested against the mouse
/// bounds. If a descendant is hovered the parent's is_mouse_over is set to
/// false.
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
/// @brief Sets is_mouse_over on a SocketState based on whether the mouse is
/// within the socket's hit radius of the given world position.
///
/// The hit radius matches the visual radius used when drawing sockets so that
/// click/hover detection is pixel-accurate relative to what the user sees.
///
/// @param mouse_position Mouse position in world coordinates.
/// @param world_pos      World-space centre of the socket.
/// @param socket_state   SocketState whose is_mouse_over flag is updated.
/////////////////////////////////////////////////
void CheckMouseOver(const sf::Vector2i &mouse_position,
                    sf::Vector2f world_pos,
                    SocketState &socket_state);

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

/////////////////////////////////////////////////
/// @brief Run the two-pass mouse-collision check for all UI entities.
///
/// Pass 1: clears is_mouse_over on every entity's root element to remove stale
/// hover state from the previous tick.
/// Pass 2: iterates entities in descending priority order. The first visible
/// entity whose element tree reports a hover claims the mouse; all
/// lower-priority entities have their hover state cleared.
///
/// @param entity_indexes UI entity indices in descending priority order.
/// @param scene_entities EntityMemoryPool containing CUserInterface components.
/// @param mouse_position Current mouse cursor position in window coordinates.
/////////////////////////////////////////////////
void ProcessUIEntityCollisions(
    const std::vector<size_t> &entity_indexes,
    EntityMemoryPool &scene_entities,
    const sf::Vector2i &mouse_position);

/////////////////////////////////////////////////
/// @brief Run mouse-collision checks for all parts of an active
/// MachinaFormScaffold.
///
/// Calls CheckMouseOver for the growth point, all joints, and all fragments
/// in the scaffold, setting is_mouse_over on each part as appropriate.
///
/// @param scaffold      MachinaFormScaffold whose parts will be checked.
/// @param mouse_position Current mouse position in window coordinates.
/////////////////////////////////////////////////
void ProcessScaffoldCollisions(MachinaFormScaffold &scaffold,
                                const sf::Vector2i &mouse_position);

} // namespace steamrot::logic::collision::mouse
