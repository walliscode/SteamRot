#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Tile {

public:
  Tile(const json &tile_config, const sf::FloatRect &viewport_ratio,
       const sf::Vector2f &window_size);

  void SetTileStaticProperties(const json &config);
  void SetViewProperties(const sf::FloatRect &viewport_ratio,
                         const sf::Vector2f &window_size);
  void SetBorder(const sf::Vector2f &window_size);
  void SetBorderColourActive();
  void SetBorderColourInactive();

  sf::View GetView();
  const std::array<sf::RectangleShape, 4> &GetBorderStraights();
  const std::array<sf::VertexArray, 4> &GetBorderCorners();

private:
  // pass view to sf::RenderWindow::setView() to change the view
  sf::View m_view;
  // use view port to draw the tile
  sf::FloatRect m_viewport;

  std::array<sf::RectangleShape, 4> m_border_straight;
  std::array<sf::VertexArray, 4> m_border_corners;
  sf::Color m_active_border_colour;
  sf::Color m_inactive_border_colour;
  sf::Vector2f m_margin;
  sf::Vector2f m_border_thickness;
  size_t m_radius_resolution;
};
