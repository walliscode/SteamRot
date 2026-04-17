/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for positioning grimoire machina
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_grimoire_machina.h"

namespace steamrot::logic::positioning::grimoire_machina {

/////////////////////////////////////////////////
void position_first_part_of_machina_form_scaffold(PartMap &parts) {
  // check that parts is not empty, if not return early
  if (parts.empty()) {
    return;
  }

  // if part 0 does not exist, return early
  auto it = parts.find(0);
  if (it == parts.end()) {
    return;
  }

  // get variant
  if (FragmentInstance *fragment_instance =
          std::get_if<FragmentInstance>(&it->second)) {

    // if no sockets, return early
    if (fragment_instance->socket_states.empty())
      return;

    // if views is empty, return early
    if (fragment_instance->fragment.movement_views.empty())
      return;
  }
}
} // namespace steamrot::logic::positioning::grimoire_machina
