/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for positioning MrGhost.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_ghost.h"
#include "overload.h"
#include "positioning_camera.h"
#include <SFML/Graphics/Transform.hpp>
#include <cmath>

namespace steamrot::logic::positioning::ghost {

namespace {

/////////////////////////////////////////////////
/// @brief Compute the instance transform from a current mouse position and
/// rotation.
///
/// Translates the instance so its local-space centre sits at
/// @p world_mouse_position (offset by k_corner_offset), then rotates it
/// around that same local-space centre by @p rotation_degrees.
/// Because the rotation centre is specified in the vertex-array's own
/// coordinate space (before any translation), the part spins about itself
/// and the world position of the centre remains invariant across zoom levels.
///
/// @param bounds             Bounding box of the part's vertex array.
/// @param world_mouse_pos    Target world-space cursor position.
/// @param rotation_degrees   Accumulated rotation angle in degrees.
/// @return The computed sf::Transform.
/////////////////////////////////////////////////
sf::Transform compute_instance_transform(const sf::FloatRect &bounds,
                                         const sf::Vector2f &world_mouse_pos,
                                         float rotation_degrees) {
  static constexpr float k_corner_offset = 5.f;

  // Local-space centre of the vertex array.  The rotation is applied around
  // this point so the part spins about its own centre regardless of zoom.
  const sf::Vector2f local_center = bounds.position + bounds.size / 2.f;

  // Translate so the part's centre sits at the mouse cursor (with a small
  // offset).  The translation is computed in world space using the already
  // zoom-corrected world_mouse_pos.
  const sf::Vector2f translation =
      world_mouse_pos - local_center -
      sf::Vector2f{k_corner_offset, k_corner_offset};

  sf::Transform transform = sf::Transform::Identity;
  transform.translate(translation);
  // Rotate around local_center, which is in the same coordinate space as
  // the vertex-array vertices (pre-translation local space).  SFML applies
  // the rotation before the translation, so the final world position of the
  // part's centre is always (translation + local_center), i.e. world_mouse_pos
  // minus the small offset, independent of rotation angle and zoom level.
  transform.rotate(sf::degrees(rotation_degrees), local_center);
  return transform;
}

} // anonymous namespace

/////////////////////////////////////////////////
void UpdatePosition(MrGhost &mr_ghost, sf::Vector2f &world_mouse_position,
                    const sf::Vector2i &mouse_position,
                    const CameraState &camera_state,
                    const sf::RenderTexture &scene_texture) {
  world_mouse_position =
      steamrot::logic::positioning::camera::map_to_world_coords(
          camera_state, mouse_position, scene_texture);
  mr_ghost.m_position = world_mouse_position;

  // [TODO:] needs redoing here
  // std::visit(overload{[&](FragmentInstance &instance) {
  //                       const sf::FloatRect bounds =
  //                           instance.GetPart()
  //                               .positioning_views[ViewDirection::Front]
  //                               .getBounds();
  //                       instance.SetTransform(compute_instance_transform(
  //                           bounds, world_mouse_position,
  //                           mr_ghost.m_rotation_degrees));
  //                     },
  //                     [&](JointInstance &instance) {
  //                       const sf::FloatRect bounds =
  //                           instance.GetPart()
  //                               .positioning_views[ViewDirection::Front]
  //                               .getBounds();
  //                       instance.SetTransform(compute_instance_transform(
  //                           bounds, world_mouse_position,
  //                           mr_ghost.m_rotation_degrees));
  //                     },
  //                     [](std::monostate &) {
  //                       // std::monostate: nothing to update
  //                     }},
  //            mr_ghost.m_instance);
}

/////////////////////////////////////////////////
void RotateGhost(MrGhost &mr_ghost) {
  static constexpr float k_rotation_step = 90.f;
  static constexpr float k_full_rotation = 360.f;
  mr_ghost.m_rotation_degrees =
      std::fmod(mr_ghost.m_rotation_degrees + k_rotation_step, k_full_rotation);
}

/////////////////////////////////////////////////
void ProcessRotateSubscribers(
    const std::vector<std::shared_ptr<Subscriber>> &subscribers,
    MrGhost &mr_ghost) {
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
      RotateGhost(mr_ghost);
  }
}

} // namespace steamrot::logic::positioning::ghost
