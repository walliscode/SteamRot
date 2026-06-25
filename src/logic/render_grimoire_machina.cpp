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
#include "MachinaFormScaffold.h"
#include "positioning_grimoire_machina.h"
#include "render_text.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot::logic::render::grimoire_machina {

/////////////////////////////////////////////////
void render_machina_form(sf::RenderTexture &texture,
                         GrimoireMachina &grimoire_machina,
                         const sf::Font &font) {
  MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();

  if (!scaffold) {
    draw_no_machina_form_indicator(texture);
    return;
  }

  const bool draw_sockets = scaffold->are_sockets_visible;

  // draw the PartGraph
  for (auto &[id, part] : scaffold->parts) {

    if (auto *joint = std::get_if<JointInstance>(&part))
      draw_joint_instance(texture, *joint, draw_sockets);

    else if (auto *fragment = std::get_if<FragmentInstance>(&part)) {
      draw_fragment_instance(texture, *fragment, draw_sockets);
    }
  }

  /// Strucutural Analysis status box drawing ///
  // generate a sf::FloatRect to be drawn
  const sf::FloatRect status_box =
      positioning::grimoire_machina::calculate_outer_box(scaffold->parts);

  // pass the status box to the pick_and_draw_status_box function
  pick_and_draw_status_box(scaffold->structural_analysis_state, status_box,
                           font, texture);
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
  draw_view(texture, fragment_instance.fragment->positioning_views,
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
  draw_view(texture, joint_instance.joint->positioning_views,
            ViewDirection::Front, states);
  if (draw_sockets)
    draw_joint_instance_sockets(texture, joint_instance);
}

/////////////////////////////////////////////////
void draw_fragment_instance_sockets(sf::RenderTexture &texture,
                                    FragmentInstance &fragment_instance) {
  for (auto &[socket_id, socket] : fragment_instance.sockets) {
    const sf::Vector2f world_pos =
        fragment_instance.transform.transformPoint(socket.local_position);
    draw_socket(texture, world_pos, socket);
  }
}

/////////////////////////////////////////////////
void draw_joint_instance_sockets(sf::RenderTexture &texture,
                                 JointInstance &joint_instance) {
  for (auto &[socket_id, socket] : joint_instance.sockets) {
    // transform the socket's local position to get its world position, then
    // draw
    const sf::Vector2f world_pos =
        joint_instance.transform.transformPoint(socket.local_position);
    draw_socket(texture, world_pos, socket);
  }
}

/////////////////////////////////////////////////
void draw_socket(sf::RenderTexture &texture, sf::Vector2f world_pos,
                 const SocketData &socket_data) {
  static constexpr float k_outer_radius = 2.f;
  static constexpr float k_inner_radius = 1.f;
  static constexpr int k_point_count = 10;

  // Outer white circle drawn for every visible socket state.
  sf::CircleShape outer(k_outer_radius, k_point_count);
  outer.setOrigin({k_outer_radius, k_outer_radius});
  outer.setPosition(world_pos);

  if (socket_data.is_ready_to_connect) {
    // Ready to connect: white outer + green inner circle.
    outer.setFillColor(sf::Color::White);
    texture.draw(outer);

    sf::CircleShape inner(k_inner_radius, k_point_count);
    inner.setOrigin({k_inner_radius, k_inner_radius});
    inner.setPosition(world_pos);
    inner.setFillColor(sf::Color::Green);
    texture.draw(inner);
  } else if (socket_data.is_another_socket_near) {
    // Near but not ready: white outer + blue inner circle whose brightness
    // scales with proximity (proximity_scale 0 = dim, 255 = full blue).
    outer.setFillColor(sf::Color::White);
    texture.draw(outer);

    const uint8_t brightness = socket_data.proximity_scale.value_or(0);
    sf::CircleShape inner(k_inner_radius, k_point_count);
    inner.setOrigin({k_inner_radius, k_inner_radius});
    inner.setPosition(world_pos);
    inner.setFillColor(sf::Color{0, 0, brightness});
    texture.draw(inner);
  } else {
    // Default: white outer, or blue outer when the mouse hovers.
    outer.setFillColor(socket_data.is_mouse_over ? sf::Color::Blue
                                                 : sf::Color::White);
    texture.draw(outer);
  }
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

/////////////////////////////////////////////////
void draw_status_box(sf::FloatRect box, sf::Color color,
                     const std::string &text, const sf::Font &font,
                     sf::RenderTexture &texture) {

  // set the thickness for the border
  static const float thickness{3.f};

  // create box to draw and populate variables
  sf::RectangleShape border_box{box.size};
  border_box.setPosition(box.position);
  border_box.setOutlineThickness(-thickness);
  border_box.setOutlineColor(color);
  border_box.setFillColor(sf::Color::Transparent);

  // draw to texture
  texture.draw(border_box);

  // pull out some unit of width from the box to use as increments
  const float unit_width = box.size.x * 0.05f;
  const float unit_height = box.size.y * 0.08f;

  // create a 6 point convex shape to draw the trapezium shape above the box
  sf::ConvexShape trapezium;
  trapezium.setPointCount(4);
  trapezium.setPoint(0, {box.position});
  trapezium.setPoint(
      1, {box.position.x + unit_width, box.position.y - unit_height});
  trapezium.setPoint(
      2, {box.position.x + (unit_width * 6), box.position.y - unit_height});
  trapezium.setPoint(3, {box.position.x + (unit_width * 7), box.position.y});
  trapezium.setFillColor(color);

  // draw to texture
  texture.draw(trapezium);

  // create text to draw and populate variables
  sf::Text text_to_draw{font, text};

  text_to_draw.setFillColor(sf::Color::White);
  text_to_draw.setPosition(
      {box.position.x + unit_width, box.position.y - (unit_height * 0.8f)});

  // resize the text to fit within the box square part of the trapezium
  sf::Vector2f text_box_size{unit_width * 5, unit_height};

  fit_text_to_box(text_to_draw, text_box_size, 0);
  // draw to texture
  texture.draw(text_to_draw);
}

/////////////////////////////////////////////////
void pick_and_draw_status_box(const StructuralAnalysisState state,
                              const sf::FloatRect box, const sf::Font &font,
                              sf::RenderTexture &texture) {
  // switch on the state and draw the appropriate box
  switch (state) {

  case StructuralAnalysisState::NotRun:
    // draw a grey box with "Analysis Not Run" text

    draw_status_box(box, {255, 255, 255, 50}, "No Analy.", font, texture);
    break;

  case StructuralAnalysisState::NothingFound:
    // draw a red box with "No Archetypes Found" text
    draw_status_box(box, {168, 50, 50, 255}, "No Archetypes Found", font,
                    texture);
    break;
  }
}
} // namespace steamrot::logic::render::grimoire_machina
