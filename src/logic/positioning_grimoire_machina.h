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
/// @brief Calculates an even spread of socket local positions for a Joint based
/// on its SocketConfig.
///
/// This uses the rotation_arc_min and rotation_arc_max to determine the upper
/// and lower bounds of the arc. This arc distance is divided evenly to
/// calculate the positions of the sockets.
///
/// @param config SocketConfig to calculate from
/// @param origin Local origin of the Joint, used as the center point for
/// calculating socket
/// @param local_positions All the local positions of the sockets will be
/// written into this vector.
/////////////////////////////////////////////////
void compute_socket_local_positions_even_spread(
    const SocketConfig &config, sf::Vector2f &origin,
    std::vector<sf::Vector2f> &local_positions);

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
