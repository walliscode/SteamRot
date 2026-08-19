/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for collision checking of
/// GrimoireMachina elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "collision_grimoire_machina.h"
#include <SFML/System/Vector2.hpp>
#include <variant>

namespace steamrot::logic::collision::grimoire_machina {

/////////////////////////////////////////////////
void reset_socket_proximity_state(PartGraph &part_graph) {
  for (auto &[id, variant] : part_graph) {
    std::visit(
        [](auto &instance) { instance.ResetAllSocketsInteractionState(); },
        variant);
  }
}

/////////////////////////////////////////////////
void check_socket_collisions(FragmentInstance &fragment_instance,
                             JointInstance &joint_instance) {
  fragment_instance.CheckCollisionWith(joint_instance);
}

/////////////////////////////////////////////////
void check_socket_collisions(FragmentInstance &fragment_instance,
                             PartGraph &part_graph) {
  // Reset interaction/proximity state on both sides before each pass so stale
  // nearest-distance candidates do not bleed between ticks.
  fragment_instance.ResetAllSocketsInteractionState();
  reset_socket_proximity_state(part_graph);

  for (auto &[id, variant] : part_graph) {
    if (auto *joint_instance = std::get_if<JointInstance>(&variant)) {
      check_socket_collisions(fragment_instance, *joint_instance);
    }
  }
}

/////////////////////////////////////////////////
void check_socket_collisions(JointInstance &joint_instance,
                             PartGraph &part_graph) {
  // Reset interaction/proximity state on both sides before each pass so stale
  // nearest-distance candidates do not bleed between ticks.
  joint_instance.ResetAllSocketsInteractionState();
  reset_socket_proximity_state(part_graph);

  for (auto &[id, variant] : part_graph) {
    if (auto *fragment_instance = std::get_if<FragmentInstance>(&variant)) {
      check_socket_collisions(*fragment_instance, joint_instance);
    }
  }
}

/////////////////////////////////////////////////
void check_collisions_between_ghost_and_scaffold(
    MachinaFormScaffold &scaffold_form, MrGhost &mr_ghost) {

  // Guard statement for monostate.
  if (std::holds_alternative<std::monostate>(mr_ghost.m_instance)) {
    return; // no active ghost item, nothing to check
  }

  // Check for collisions between the active ghost item and the active scaffold.
  if (auto *fragment_instance =
          std::get_if<FragmentInstance>(&mr_ghost.m_instance)) {
    check_socket_collisions(*fragment_instance, scaffold_form.parts);
  } else if (auto *joint_instance =
                 std::get_if<JointInstance>(&mr_ghost.m_instance)) {
    check_socket_collisions(*joint_instance, scaffold_form.parts);
  }
}

} // namespace steamrot::logic::collision::grimoire_machina
