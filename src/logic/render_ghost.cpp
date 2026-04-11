/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for rendering the MrGhost item.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_ghost.h"
#include "ViewDirection.h"
#include "render_grimoire_machina.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <variant>
#include <vector>

namespace steamrot::logic::render::ghost {

namespace {

/////////////////////////////////////////////////
/// @brief Draw pale blue socket indicators for the given socket positions.
///
/// Each socket is rendered as a pale blue filled circle of radius
/// k_socket_radius, positioned using the same render states as the ghost
/// shape so that sockets remain aligned with the fragment/joint geometry.
///
/// @param texture  RenderTexture to draw on.
/// @param sockets  Local socket positions from the Fragment or Joint.
/// @param states   RenderStates (transform) matching the ghost item.
/////////////////////////////////////////////////
void DrawGhostSockets(sf::RenderTexture &texture,
                      const std::vector<sf::Vector2f> &sockets,
                      const sf::RenderStates &states) {

  static constexpr float k_socket_radius = 1.5f;
  static const sf::Color k_socket_color{173, 216, 230, 200};

  for (const sf::Vector2f &socket_pos : sockets) {
    sf::CircleShape circle(k_socket_radius);
    circle.setFillColor(k_socket_color);
    circle.setOrigin({k_socket_radius, k_socket_radius});
    circle.setPosition(socket_pos);
    texture.draw(circle, states);
  }
}

} // namespace

/////////////////////////////////////////////////
void DrawGhostItem(sf::RenderTexture &texture, const MrGhost &mr_ghost,
                   GrimoireMachina &grimoire_machina) {

  static constexpr float k_corner_offset = 5.f;

  if (std::holds_alternative<std::monostate>(mr_ghost.m_selection)) {
    return; // nothing selected — draw nothing

  } else if (std::holds_alternative<FragmentTag>(mr_ghost.m_selection)) {
    const auto &tag = std::get<FragmentTag>(mr_ghost.m_selection);
    auto it = grimoire_machina.m_all_fragments.find(tag.key);
    if (it == grimoire_machina.m_all_fragments.end()) {
      return;
    }
    const sf::FloatRect bounds =
        it->second.movement_views[ViewDirection::Front].getBounds();
    sf::RenderStates states;
    states.transform.translate(mr_ghost.m_position - bounds.position -
                               bounds.size -
                               sf::Vector2f(k_corner_offset, k_corner_offset));
    grimoire_machina::DrawFragmentView(texture, it->second,
                                       ViewDirection::Front, states);
    DrawGhostSockets(texture, it->second.sockets, states);

  } else if (std::holds_alternative<JointTag>(mr_ghost.m_selection)) {
    const auto &tag = std::get<JointTag>(mr_ghost.m_selection);
    auto it = grimoire_machina.m_all_joints.find(tag.key);
    if (it == grimoire_machina.m_all_joints.end()) {
      return;
    }
    const sf::FloatRect bounds =
        it->second.movement_views[ViewDirection::Front].getBounds();
    sf::RenderStates states;
    states.transform.translate(mr_ghost.m_position - bounds.position -
                               bounds.size -
                               sf::Vector2f(k_corner_offset, k_corner_offset));
    grimoire_machina::DrawJointView(texture, it->second, ViewDirection::Front,
                                    states);
    DrawGhostSockets(texture, it->second.sockets, states);
  }
}

} // namespace steamrot::logic::render::ghost
