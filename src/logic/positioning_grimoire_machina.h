/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for positioning grimoire machina
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Joint.h"
#include "MachinaFormScaffold.h"
#include <SFML/System/Vector2.hpp>

namespace steamrot::logic::positioning::grimoire_machina {

/////////////////////////////////////////////////
/// @brief Compute the local-space position of a single socket on a Joint.
///
/// Fixed anchor sockets (socket index 0 when SocketConfig::has_fixed_socket is
/// true) are placed at SocketConfig::fixed_socket_angle and are not affected by
/// @p ring_rotation. All other sockets are distributed evenly within the arc
/// [arc_min, arc_max] and then rotated by @p ring_rotation.
///
/// When the arc spans exactly 360 degrees, sockets are distributed with equal
/// spacing and no duplicate at the wrap-around point.
///
/// @param config       SocketConfig describing the Joint's socket layout.
/// @param socket_index Index of the socket to compute (0-based).
/// @param ring_rotation Current rotation of the socket ring in degrees.
/// @return Local-space position of the requested socket.
/////////////////////////////////////////////////
sf::Vector2f compute_socket_local_pos(const SocketConfig &config,
                                      size_t socket_index,
                                      float ring_rotation = 0.f);

/////////////////////////////////////////////////
/// @brief Initialise all socket_local_positions entries on a JointInstance by
/// computing each socket's position from the Joint's SocketConfig at zero ring
/// rotation.
///
/// @param joint_instance JointInstance whose socket_local_positions to fill.
/////////////////////////////////////////////////
void initialize_joint_socket_positions(JointInstance &joint_instance);

/////////////////////////////////////////////////
/// @brief Identifies the PartInstance with a socket index of 0 and positions at
/// 0,0 of the canvas
/////////////////////////////////////////////////
void position_first_part_of_machina_form_scaffold(PartMap &parts);

/////////////////////////////////////////////////
/// @brief Wrapper function to completely position the parts of the
/// MachinaFormScaffold.
///
/// @param parts The PartMap of the MachinaFormScaffold to position
/////////////////////////////////////////////////
void position_machina_form_scaffold(PartMap &parts);

} // namespace steamrot::logic::positioning::grimoire_machina
