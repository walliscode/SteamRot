/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the free functions related to the rendering of the
/// grimoire
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_grimoire_machina.h"
#include "GrimoireMachina.h"
#include "SocketConfigUtils.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace steamrot::logic::render::grimoire_machina {

/////////////////////////////////////////////////
void DrawEmptyActiveMachinaForm(sf::RenderTexture &texture, MachinaForm &form) {
}

/////////////////////////////////////////////////
void DrawCraftingCanvasBorder(sf::RenderTexture &texture,
                              sf::FloatRect &crafting_canvas) {

  // create a rectangle shape for the border
  sf::RectangleShape border;
  float border_thickness = 3.f; // thickness of the border
  // off set border by thicknes so that the border remains "inside" the crafting
  // canvas
  border.setPosition({crafting_canvas.position.x + border_thickness,
                      crafting_canvas.position.y + border_thickness});
  border.setSize({crafting_canvas.size.x - 2 * border_thickness,
                  crafting_canvas.size.y - 2 * border_thickness});

  border.setFillColor(sf::Color::Transparent);
  border.setOutlineColor(sf::Color::White);
  border.setOutlineThickness(border_thickness);

  texture.draw(border);
}

/////////////////////////////////////////////////
void DrawNoMachinaFormBox(sf::RenderTexture &texture,
                          sf::FloatRect &crafting_canvas) {

  // create a rectangle shape for the box
  sf::RectangleShape box;
  // set the size of the box to a third of the crafting canvas size
  box.setSize({crafting_canvas.size.x / 3.f, crafting_canvas.size.y / 3.f});
  // center the box in the crafting canvas
  box.setPosition({crafting_canvas.position.x + crafting_canvas.size.x / 2.f -
                       box.getSize().x / 2.f,
                   crafting_canvas.position.y + crafting_canvas.size.y / 2.f -
                       box.getSize().y / 2.f});
  // set the fill color of the box to be a transparent
  box.setFillColor(sf::Color::Transparent);
  // set the outline color of the box to be red
  box.setOutlineColor(sf::Color::Red);
  // set the outline thickness of the box
  box.setOutlineThickness(3.f);

  // draw the box on the texture
  texture.draw(box);
}

void DrawSocket(sf::RenderTexture &texture, sf::Vector2f world_pos,
                const SocketState &socket_state) {
  static constexpr float k_radius = 2.f;
  static constexpr int k_point_count = 10;
  static const sf::Color k_base_color{sf::Color::White};
  static const sf::Color k_hover_color{sf::Color::Yellow};

  sf::CircleShape circle(k_radius, k_point_count);
  circle.setOrigin({k_radius, k_radius});
  circle.setPosition(world_pos);
  circle.setFillColor(socket_state.is_mouse_over ? k_hover_color
                                                 : k_base_color);
  texture.draw(circle);
}

/////////////////////////////////////////////////
void DrawFragmentInstanceSockets(sf::RenderTexture &texture,
                                 FragmentInstance &fragment_instance) {
  for (size_t i = 0; i < fragment_instance.socket_states.size(); ++i) {
    const sf::Vector2f world_pos = fragment_instance.transform.transformPoint(
        fragment_instance.fragment.sockets[i]);
    DrawSocket(texture, world_pos, fragment_instance.socket_states[i]);
  }
}

/////////////////////////////////////////////////
void DrawJointInstanceSockets(sf::RenderTexture &texture,
                              JointInstance &joint_instance) {
  for (size_t i = 0; i < joint_instance.socket_states.size(); ++i) {
    const sf::Vector2f local_pos = ComputeSocketLocalPos(
        joint_instance.joint.socket_config, i, joint_instance.current_rotation);
    const sf::Vector2f world_pos =
        joint_instance.transform.transformPoint(local_pos);
    DrawSocket(texture, world_pos, joint_instance.socket_states[i]);
  }
}

/////////////////////////////////////////////////
void DrawFragmentInstance(sf::RenderTexture &texture,
                          FragmentInstance &fragment_instance,
                          const bool draw_sockets) {
  sf::RenderStates states;
  states.transform = fragment_instance.transform;
  DrawFragmentView(texture, fragment_instance.fragment, ViewDirection::Front,
                   states);
  DrawFragmentInstanceSockets(texture, fragment_instance);
}

/////////////////////////////////////////////////
void DrawJointInstance(sf::RenderTexture &texture,
                       JointInstance &joint_instance) {
  sf::RenderStates states;
  states.transform = joint_instance.transform;
  DrawJointView(texture, joint_instance.joint, ViewDirection::Front, states);
  DrawJointInstanceSockets(texture, joint_instance);
}

/////////////////////////////////////////////////
void DrawScaffoldOrPlaceholder(sf::RenderTexture &texture,
                               GrimoireMachina &grimoire_machina) {
  MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();

  DrawCraftingCanvasBorder(texture,
                           grimoire_machina.m_crafting_helpers.crafting_canvas);

  if (!scaffold) {
    DrawNoMachinaFormBox(texture,
                         grimoire_machina.m_crafting_helpers.crafting_canvas);
    return;
  }

  // pull out the bool that indicates whether to draw sockets or not
  bool draw_sockets = scaffold->are_sockets_visible;

  // Draw all placed instances on the active scaffold.
  for (auto &joint : scaffold->joints) {
    DrawJointInstance(texture, joint);
  }
  for (auto &fragment : scaffold->fragments) {
    DrawFragmentInstance(texture, fragment, draw_sockets);
  }
}

/////////////////////////////////////////////////
void DrawView(sf::RenderTexture &texture, const Views &views,
              ViewDirection view_direction) {
  texture.draw(views[view_direction]);
}

/////////////////////////////////////////////////
void DrawFragmentView(sf::RenderTexture &texture, const Fragment &fragment,
                      ViewDirection view_direction) {
  DrawView(texture, fragment.movement_views, view_direction);
}

/////////////////////////////////////////////////
void DrawFragmentView(sf::RenderTexture &texture, const Fragment &fragment,
                      ViewDirection view_direction,
                      const sf::RenderStates &states) {
  texture.draw(fragment.movement_views[view_direction], states);
}

/////////////////////////////////////////////////
void DrawJointView(sf::RenderTexture &texture, const Joint &joint,
                   ViewDirection view_direction) {
  DrawView(texture, joint.movement_views, view_direction);
}

/////////////////////////////////////////////////
void DrawJointView(sf::RenderTexture &texture, const Joint &joint,
                   ViewDirection view_direction,
                   const sf::RenderStates &states) {
  texture.draw(joint.movement_views[view_direction], states);
}

} // namespace steamrot::logic::render::grimoire_machina
