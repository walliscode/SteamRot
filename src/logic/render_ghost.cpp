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
void draw_ghost_sockets(sf::RenderTexture &texture,
                        const std::vector<sf::Vector2f> &sockets,
                        const sf::RenderStates &states) {

  static constexpr float k_socket_radius = 1.5f;
  static const sf::Color k_socket_color{173, 216, 230, 200};

  sf::CircleShape circle(k_socket_radius);
  circle.setFillColor(k_socket_color);
  circle.setOrigin({k_socket_radius, k_socket_radius});

  for (const sf::Vector2f &socket_pos : sockets) {
    circle.setPosition(socket_pos);
    texture.draw(circle, states);
  }
}

} // namespace

/////////////////////////////////////////////////
void draw_ghost_item(sf::RenderTexture &texture, const MrGhost &mr_ghost,
                     GrimoireMachina &grimoire_machina) {

  static constexpr float k_corner_offset = 5.f;

  /// FragmentTag Logic ///
  if (auto *fragment_tag = std::get_if<FragmentTag>(&mr_ghost.m_selection)) {
    auto it = grimoire_machina.m_all_fragments.find(fragment_tag->key);
    if (it == grimoire_machina.m_all_fragments.end())
      return;
    const sf::FloatRect bounds =
        it->second.movement_views[ViewDirection::Front].getBounds();
    sf::RenderStates states;
    states.transform.translate(mr_ghost.m_position - bounds.position -
                               bounds.size -
                               sf::Vector2f(k_corner_offset, k_corner_offset));
    grimoire_machina::draw_view(texture, it->second.movement_views,
                                ViewDirection::Front, states);
    draw_ghost_sockets(texture, it->second.sockets, states);

    /// JointTag Logic ///
  } else if (auto *joint_tag = std::get_if<JointTag>(&mr_ghost.m_selection)) {

    auto it = grimoire_machina.m_all_joints.find(joint_tag->key);
    if (it == grimoire_machina.m_all_joints.end())
      return;

    const sf::FloatRect bounds =
        it->second.movement_views[ViewDirection::Front].getBounds();
    sf::RenderStates states;
    states.transform.translate(mr_ghost.m_position - bounds.position -
                               bounds.size -
                               sf::Vector2f(k_corner_offset, k_corner_offset));
    grimoire_machina::draw_view(texture, it->second.movement_views,
                                ViewDirection::Front, states);
  }
}

} // namespace steamrot::logic::render::ghost
