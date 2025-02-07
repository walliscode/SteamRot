#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Tile {

public:
  Tile(const json &tile_config, const sf::FloatRect &viewport_ratio,
       sf::RenderWindow &window);

  // Set the static properties of the tile, values taken from json file
  void SetTileStaticProperties(const json &config);

  // Function dealing with Views (not Viewports)
  void SetViewProperties();
  sf::View &GetView();

  // Functions dealing with the Viewport
  void SetViewPort(const sf::FloatRect &viewport);

  // Functions dealing with the border UI
  void SetBorder();
  void SetBorderColourActive();
  void SetBorderColourInactive();
  ;
  const std::array<sf::RectangleShape, 4> &GetBorderStraights();
  const std::array<sf::VertexArray, 4> &GetBorderCorners();

private:
  sf::RenderWindow &m_window;
  sf::View m_view;

  std::array<sf::RectangleShape, 4> m_border_straight;
  std::array<sf::VertexArray, 4> m_border_corners;
  sf::Color m_active_border_colour;
  sf::Color m_inactive_border_colour;
  sf::Vector2f m_margin;
  sf::Vector2f m_border_thickness;
  size_t m_radius_resolution;
};
