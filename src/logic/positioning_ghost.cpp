/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for positioning MrGhost.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_ghost.h"
#include "overload.h"
#include <SFML/Graphics/Transform.hpp>
#include <SFML/System/Angle.hpp>
#include <cmath>

namespace steamrot::logic::positioning::ghost {

/////////////////////////////////////////////////
void update_position(MrGhost &mr_ghost,
                     const sf::Vector2f &world_mouse_position) {

  // store the new position in the MrGhost instance
  mr_ghost.m_position = world_mouse_position;

  std::visit(
      overload{[&](FragmentInstance &instance) {
                 instance.setPosition(mr_ghost.m_position);
                 instance.setRotation(sf::degrees(mr_ghost.m_rotation_degrees));
               },
               [&](JointInstance &instance) {
                 instance.setPosition(mr_ghost.m_position);
                 instance.setRotation(sf::degrees(mr_ghost.m_rotation_degrees));
               },
               [](std::monostate &) {
                 // std::monostate: nothing to update
               }},
      mr_ghost.m_instance);
}

/////////////////////////////////////////////////
void rotate_ghost(MrGhost &mr_ghost) {
  static constexpr float k_rotation_step = 90.f;
  static constexpr float k_full_rotation = 360.f;
  mr_ghost.m_rotation_degrees =
      std::fmod(mr_ghost.m_rotation_degrees + k_rotation_step, k_full_rotation);
}

/////////////////////////////////////////////////
void process_subscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    MrGhost &mr_ghost) {

  // guard statements for skipping subscribers
  for (const auto &subscriber : subscribers) {
    if (!subscriber->m_active)
      continue;
    if (!subscriber->captured_payload.has_value())
      continue;
    if (!std::holds_alternative<InputPayload>(
            subscriber->captured_payload.value()))
      continue;

    const InputPayload &payload =
        std::get<InputPayload>(subscriber->captured_payload.value());
    if (payload.action == InputPayload::InputAction::ROTATE_GHOST)
      rotate_ghost(mr_ghost);
  }
}

} // namespace steamrot::logic::positioning::ghost
