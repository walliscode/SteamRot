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

namespace steamrot::logic::positioning::ghost {

/////////////////////////////////////////////////
void UpdatePosition(MrGhost &mr_ghost, sf::Vector2f &world_mouse_position,
                    const sf::Vector2i &mouse_position,
                    const CameraState &camera_state,
                    const sf::RenderTexture &scene_texture) {
  world_mouse_position =
      steamrot::logic::movement::camera::MapToWorldCoords(
          camera_state, mouse_position, scene_texture);
  mr_ghost.m_position = world_mouse_position;

  static constexpr float k_corner_offset = 5.f;

  std::visit(
      [&](auto &instance) {
        using T = std::decay_t<decltype(instance)>;
        if constexpr (std::is_same_v<T, FragmentInstance>) {
          if (!instance.fragment)
            return;
          const sf::VertexArray &va =
              instance.fragment->movement_views[ViewDirection::Front];
          const sf::FloatRect bounds = va.getBounds();
          instance.transform = sf::Transform::Identity;
          instance.transform.translate(
              world_mouse_position - bounds.position - bounds.size -
              sf::Vector2f{k_corner_offset, k_corner_offset});
        } else if constexpr (std::is_same_v<T, JointInstance>) {
          if (!instance.joint)
            return;
          const sf::VertexArray &va =
              instance.joint->movement_views[ViewDirection::Front];
          const sf::FloatRect bounds = va.getBounds();
          instance.transform = sf::Transform::Identity;
          instance.transform.translate(
              world_mouse_position - bounds.position - bounds.size -
              sf::Vector2f{k_corner_offset, k_corner_offset});
        }
        // std::monostate: nothing to update
      },
      mr_ghost.m_instance);
}

} // namespace steamrot::logic::positioning::ghost
