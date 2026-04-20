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
/// @brief Reset proximity state on every socket in the PartMap.
///
/// Clears @c is_another_socket_near, @c is_ready_to_connect, and
/// @c distance_to_nearest_socket on all sockets of every part in @p part_map.
/// Must be called at the start of each full collision pass so that stale state
/// from the previous tick does not bleed into the new results.
///
/// @param part_map PartMap whose socket proximity state should be cleared.
/////////////////////////////////////////////////
void reset_socket_proximity_state(PartMap &part_map);

/////////////////////////////////////////////////
/// @brief Check for a collision between two individual sockets, writing state
///        only when the measured distance is strictly smaller than the best
///        candidate already recorded on each socket.
///
/// Both sockets are updated symmetrically. If the distance between the two
/// world-space socket positions is within the connection threshold,
/// @c is_ready_to_connect is set; if it is within the proximity threshold but
/// outside the connection threshold, @c is_another_socket_near is set.
/// In either case the state is written only when this pair is closer than the
/// current @c distance_to_nearest_socket on the respective socket, ensuring
/// that the most-proximal candidate always wins across multiple pairs.
///
/// @param socket_data             SocketData for the first socket.
/// @param socket_transform        World-space transform of the first socket's
///                                owning PartInstance.
/// @param other_socket_data       SocketData for the second socket.
/// @param other_socket_transform  World-space transform of the second socket's
///                                owning PartInstance.
/////////////////////////////////////////////////
void check_socket_collisions(SocketData &socket_data,
                             const sf::Transform &socket_transform,
                             SocketData &other_socket_data,
                             const sf::Transform &other_socket_transform);

/////////////////////////////////////////////////
/// @brief Check for collisions between all sockets of a FragmentInstance and
///        all sockets of a JointInstance.
///
/// @param fragment_instance FragmentInstance to check against.
/// @param joint_instance    JointInstance to check against.
/////////////////////////////////////////////////
void check_socket_collisions(FragmentInstance &fragment_instance,
                             JointInstance &joint_instance);

/////////////////////////////////////////////////
/// @brief Check for collisions between a FragmentInstance's sockets and every
///        JointInstance in the PartMap, keeping only the most-proximal
///        candidate per socket.
///
/// Resets all proximity state on both the @p fragment_instance sockets and all
/// PartMap sockets before iterating, so that each call produces a clean result
/// independent of any prior state. Only Fragment–Joint pairs are evaluated;
/// Fragment–Fragment collisions are not checked here.
///
/// @param fragment_instance FragmentInstance whose sockets are tested.
/// @param part_map          PartMap containing the candidate JointInstances.
/////////////////////////////////////////////////
void check_socket_collisions(FragmentInstance &fragment_instance,
                             PartMap &part_map);

} // namespace steamrot::logic::collision::grimoire_machina
