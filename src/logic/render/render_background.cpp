/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for rendering the background.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_background.h"

namespace steamrot::logic::render {

/////////////////////////////////////////////////
void draw_grid_background(const sf::Vector2i &grid_size,
                          const uint8_t &grid_thickness,
                          const sf::Color &grid_color,
                          sf::RenderTexture &texture) {
  // Get the size of the texture

  const sf::View &view = texture.getView();
  const sf::Vector2f view_center = view.getCenter();
  const sf::Vector2f view_size = view.getSize();
  // create and draw horizontal lines starting from center of the view and going
  // up
  for (float y = view_center.y; y >= view_center.y - view_size.y / 2;
       y -= grid_size.y) {
    sf::RectangleShape line(sf::Vector2f(view_size.x, grid_thickness));
    line.setPosition({view_center.x - view_size.x / 2, y});
    line.setFillColor(grid_color);
    texture.draw(line);
  }

  // create and draw horizontal lines starting from center of the view and going
  // down
  for (float y = view_center.y + grid_size.y;
       y <= view_center.y + view_size.y / 2; y += grid_size.y) {
    sf::RectangleShape line(sf::Vector2f(view_size.x, grid_thickness));
    line.setPosition({view_center.x - view_size.x / 2, y});
    line.setFillColor(grid_color);
    texture.draw(line);
  }

  // create and draw vertical lines starting from center of the view and going
  // left
  for (float x = view_center.x; x >= view_center.x - view_size.x / 2;
       x -= grid_size.x) {
    sf::RectangleShape line(sf::Vector2f(grid_thickness, view_size.y));
    line.setPosition({x, view_center.y - view_size.y / 2});
    line.setFillColor(grid_color);
    texture.draw(line);
  }

  // create and draw vertical lines starting from center of the view and going
  // right
  for (float x = view_center.x + grid_size.x;
       x <= view_center.x + view_size.x / 2; x += grid_size.x) {
    sf::RectangleShape line(sf::Vector2f(grid_thickness, view_size.y));
    line.setPosition({x, view_center.y - view_size.y / 2});
    line.setFillColor(grid_color);
    texture.draw(line);
  }
}
} // namespace steamrot::logic::render
