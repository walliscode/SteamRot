/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for collision checking of
/// GrimoireMachina elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "MachinaFormScaffold.h"
#include <SFML/Graphics/Transform.hpp>
namespace steamrot::logic::collision::grimoire_machina {

/////////////////////////////////////////////////
/// @brief Check for collisions between two sockets, updating their SocketData
///
/// @param socket_data SocketData for the first socket.
/// @param socket_transform Transform of the owning PartInstance for the first
/// socket
/// @param other_socket_data SocketData for the second socket.
/// @param other_socket_transform Transform of the owning PartInstance for the
/// second socket
/////////////////////////////////////////////////
void check_socket_collisions(SocketData &socket_data,
                             const sf::Transform &socket_transform,
                             SocketData &other_socket_data,
                             const sf::Transform &other_socket_transform);
/////////////////////////////////////////////////
/// @brief Checks for collisions between a single FragmentInstance's sockets and
/// a single JointInstance's sockets.
///
/// @param fragment_instance FragmentInstance to check against
/// @param joint_instance JointInstance to check against
/////////////////////////////////////////////////
void check_socket_collisions(FragmentInstance &fragment_instance,
                             JointInstance &joint_instance);

/////////////////////////////////////////////////
/// @brief Checks for collisions between  a single FragmentInstance's sockets
/// and all other parts in a PartMap
///
/// @param fragment_instance FragmentInstance to check against
/// @param part_map PartMap to check against
/////////////////////////////////////////////////
void check_socket_collisions(FragmentInstance &fragment_instance,
                             PartMap &part_map);

} // namespace steamrot::logic::collision::grimoire_machina
