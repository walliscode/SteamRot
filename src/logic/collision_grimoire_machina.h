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
namespace steamrot::logic::collision::grimoire_machina {

/////////////////////////////////////////////////
/// @brief Checks for collisions between the a single FragmentInstance's sockets
/// and all other parts in a PartMap
///
/// @param fragment_instance FragmentInstance to check against
/// @param part_map PartMap to check against
/////////////////////////////////////////////////
void check_socket_collisions(FragmentInstance &fragment_instance,
                             PartMap &part_map);

} // namespace steamrot::logic::collision::grimoire_machina
