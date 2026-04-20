/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for positioning MrGhost.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_ghost.h"
#include "ViewDirection.h"
#include "movement_camera.h"
#include <SFML/Graphics/Transform.hpp>
#include <cmath>

namespace steamrot::logic::positioning::ghost {

namespace {

/////////////////////////////////////////////////
/// @brief Compute the instance transform from a current mouse position and
/// rotation.
///
/// Translates the instance to align its bottom-right corner with
/// @p world_mouse_position (offset by k_corner_offset), then rotates it
/// around its own world-space centre by @p rotation_degrees.
///
/// @param bounds             Bounding box of the part's vertex array.
/// @param world_mouse_pos    Target world-space cursor position.
/// @param rotation_degrees   Accumulated rotation angle in degrees.
/// @return The computed sf::Transform.
/////////////////////////////////////////////////
sf::Transform ComputeInstanceTransform(const sf::FloatRect &bounds,
                                       const sf::Vector2f &world_mouse_pos,
                                       float rotation_degrees) {
  static constexpr float k_corner_offset = 5.f;
  const sf::Vector2f translation =
      world_mouse_pos - bounds.position - bounds.size -
      sf::Vector2f{k_corner_offset, k_corner_offset};
  const sf::Vector2f world_center =
      bounds.position + bounds.size / 2.f + translation;

  sf::Transform transform = sf::Transform::Identity;
  transform.translate(translation);
  transform.rotate(sf::degrees(rotation_degrees), world_center);
  return transform;
}

} // anonymous namespace

/////////////////////////////////////////////////
void UpdatePosition(MrGhost &mr_ghost, sf::Vector2f &world_mouse_position,
                    const sf::Vector2i &mouse_position,
                    const CameraState &camera_state,
                    const sf::RenderTexture &scene_texture) {
  world_mouse_position =
      steamrot::logic::movement::camera::MapToWorldCoords(
          camera_state, mouse_position, scene_texture);
  mr_ghost.m_position = world_mouse_position;

  std::visit(
      [&](auto &instance) {
        using T = std::decay_t<decltype(instance)>;
        if constexpr (std::is_same_v<T, FragmentInstance>) {
          if (!instance.fragment)
            return;
          const sf::FloatRect bounds =
              instance.fragment->movement_views[ViewDirection::Front]
                  .getBounds();
          instance.transform = ComputeInstanceTransform(
              bounds, world_mouse_position, mr_ghost.m_rotation_degrees);
        } else if constexpr (std::is_same_v<T, JointInstance>) {
          if (!instance.joint)
            return;
          const sf::FloatRect bounds =
              instance.joint->movement_views[ViewDirection::Front].getBounds();
          instance.transform = ComputeInstanceTransform(
              bounds, world_mouse_position, mr_ghost.m_rotation_degrees);
        }
        // std::monostate: nothing to update
      },
      mr_ghost.m_instance);
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
