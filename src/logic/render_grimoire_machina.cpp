/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the free functions related to the rendering of the
/// grimoire
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_grimoire_machina.h"
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

/////////////////////////////////////////////////
void Draw(sf::RenderTexture &texture, GrowthPoint &growth_point) {
  // draw the growth point on the texture
  texture.draw(growth_point.origin);
}

/////////////////////////////////////////////////
void Draw(sf::RenderTexture &texture, Socket &socket) {
  // draw the socket circle on the texture
  texture.draw(socket.circle);
}

/////////////////////////////////////////////////
void Draw(sf::RenderTexture &texture, FragmentInstance &fragment_instance) {
  for (auto &socket : fragment_instance.sockets) {
    Draw(texture, socket);
  }
}

/////////////////////////////////////////////////
void Draw(sf::RenderTexture &texture, JointInstance &joint_instance) {
  for (auto &socket : joint_instance.sockets) {
    Draw(texture, socket);
  }
}
} // namespace steamrot::logic::render::grimoire_machina
