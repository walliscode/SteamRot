#pragma once
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Tile {

public:
  Tile();

  void SetTileProperties(const json &config);
  void SetActive(bool active);
  void SetBorder(const sf::Vector2f &origin_point, const sf::Vector2f &size);
  void SetBorderColourActive();
  void SetBorderColourInactive();

  sf::View GetView();
  const std::array<sf::RectangleShape, 4> &GetBorderStraights();
  const std::array<sf::VertexArray, 4> &GetBorderCorners();

private:
  // pass view to sf::RenderWindow::setView() to change the view
  sf::View m_view;

  std::array<sf::RectangleShape, 4> m_border_straight;
  std::array<sf::VertexArray, 4> m_border_corners;
  sf::Color m_active_border_colour;
  sf::Color m_inactive_border_colour;
  sf::Vector2f m_margin;
  sf::Vector2f m_border_thickness;
  size_t m_radius_resolution;
  bool m_active;
};
