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
/// @brief Populates the socket local positions of a JointInstance using the
/// even-spread algorithm.
///
/// Calls @c compute_socket_local_positions_even_spread internally and writes
/// the resulting positions back into @c instance.sockets[id].local_position.
/// This must be called after construction to give the sockets their correct
/// positions.
///
/// @param instance JointInstance whose sockets should be positioned.
/////////////////////////////////////////////////
void initialize_joint_socket_positions(JointInstance &instance);

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
/// @brief Identifies the PartInstance with a socket index of 0 and positions at
/// 0,0 of the canvas
/////////////////////////////////////////////////
void position_first_part_of_machina_form_scaffold(PartGraph &parts);

/////////////////////////////////////////////////
/// @brief Wrapper function to completely position the parts of the
/// MachinaFormScaffold.
///
/// @param parts The PartGraph of the MachinaFormScaffold to position
/////////////////////////////////////////////////
void position_machina_form_scaffold(PartGraph &parts);

} // namespace steamrot::logic::positioning::grimoire_machina
