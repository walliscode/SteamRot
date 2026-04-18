/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the free functions related to the rendering of the
/// grimoire machina.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_grimoire_machina.h"
#include "GrimoireMachina.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace steamrot::logic::render::grimoire_machina {

/////////////////////////////////////////////////
void render_machina_form(sf::RenderTexture &texture,
                         GrimoireMachina &grimoire_machina) {
  MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();

  if (!scaffold) {
    draw_no_machina_form_indicator(texture);
    return;
  }

  const bool draw_sockets = scaffold->are_sockets_visible;

  for (auto &[id, part] : scaffold->parts) {
    if (auto *joint = std::get_if<JointInstance>(&part))
      draw_joint_instance(texture, *joint, draw_sockets);
    else if (auto *fragment = std::get_if<FragmentInstance>(&part))
      draw_fragment_instance(texture, *fragment, draw_sockets);
  }
}

/////////////////////////////////////////////////
void draw_no_machina_form_indicator(sf::RenderTexture &texture) {
  static constexpr float k_half_size = 100.f;
  static constexpr float k_outline_thickness = 3.f;

  sf::RectangleShape box({k_half_size * 2.f, k_half_size * 2.f});
  box.setOrigin({k_half_size, k_half_size});
  box.setPosition({0.f, 0.f});
  box.setFillColor(sf::Color::Transparent);
  box.setOutlineColor(sf::Color::Red);
  box.setOutlineThickness(k_outline_thickness);

  texture.draw(box);
}

/////////////////////////////////////////////////
void draw_empty_active_machina_form(sf::RenderTexture &texture,
                                    MachinaForm &form) {}

/////////////////////////////////////////////////
void draw_fragment_instance(sf::RenderTexture &texture,
                            FragmentInstance &fragment_instance,
                            const bool draw_sockets) {
  sf::RenderStates states;
  states.transform = fragment_instance.transform;
  draw_view(texture, fragment_instance.fragment.movement_views,
            ViewDirection::Front, states);
  if (draw_sockets)
    draw_fragment_instance_sockets(texture, fragment_instance);
}

/////////////////////////////////////////////////
void draw_joint_instance(sf::RenderTexture &texture,
                         JointInstance &joint_instance,
                         const bool draw_sockets) {
  sf::RenderStates states;
  states.transform = joint_instance.transform;
  draw_view(texture, joint_instance.joint.movement_views, ViewDirection::Front,
            states);
  if (draw_sockets)
    draw_joint_instance_sockets(texture, joint_instance);
}

/////////////////////////////////////////////////
void draw_fragment_instance_sockets(sf::RenderTexture &texture,
                                    FragmentInstance &fragment_instance) {
  for (size_t i = 0; i < fragment_instance.socket_states.size(); ++i) {
    const sf::Vector2f world_pos = fragment_instance.transform.transformPoint(
        fragment_instance.fragment.sockets[i]);
    draw_socket(texture, world_pos, fragment_instance.socket_states[i]);
  }
}

/////////////////////////////////////////////////
void draw_joint_instance_sockets(sf::RenderTexture &texture,
                                 JointInstance &joint_instance) {
  for (size_t i = 0; i < joint_instance.socket_states.size(); ++i) {
    // transform the socket's local position to get its world position, then
    // draw
    const sf::Vector2f world_pos = joint_instance.transform.transformPoint(
        joint_instance.socket_local_positions[i]);
    draw_socket(texture, world_pos, joint_instance.socket_states[i]);
  }
}

/////////////////////////////////////////////////
void draw_socket(sf::RenderTexture &texture, sf::Vector2f world_pos,
                 const SocketState &socket_state) {
  static constexpr float k_radius = 2.f;
  static constexpr int k_point_count = 10;
  static const sf::Color k_base_color{sf::Color::White};
  static const sf::Color k_hover_color{sf::Color::Blue};

  sf::CircleShape circle(k_radius, k_point_count);
  circle.setOrigin({k_radius, k_radius});
  circle.setPosition(world_pos);
  circle.setFillColor(socket_state.is_mouse_over ? k_hover_color
                                                 : k_base_color);
  texture.draw(circle);
}

/////////////////////////////////////////////////
void draw_view(sf::RenderTexture &texture, const Views &views,
               ViewDirection view_direction) {
  texture.draw(views[view_direction]);
}

/////////////////////////////////////////////////
void draw_view(sf::RenderTexture &texture, const Views &views,
               ViewDirection view_direction, const sf::RenderStates &states) {
  texture.draw(views[view_direction], states);
}

} // namespace steamrot::logic::render::grimoire_machina
