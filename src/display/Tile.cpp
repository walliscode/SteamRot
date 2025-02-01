#define _USE_MATH_DEFINES
#include "Tile.h"
#include <SFML/Graphics/VertexArray.hpp>

Tile::Tile() : m_active(false) {};

void Tile::SetTileProperties(const json &config) {

  m_active_border_colour = sf::Color(config["active_border_colour"]["r"],
                                     config["active_border_colour"]["g"],
                                     config["active_border_colour"]["b"]);
  m_inactive_border_colour = sf::Color(config["inactive_border_colour"]["r"],
                                       config["inactive_border_colour"]["g"],
                                       config["inactive_border_colour"]["b"]);
}

// The tile border is composed up of 4 rectangle boxes and 4 radius corners
// drawn with sf::TriangleFan These are then stored in a sf::VertexArray

void Tile::SetBorder(const sf::Vector2f &origin_point,
                     const sf::Vector2f &size) {

  // draw reactangle borders

  float straight_width = size.x - (2 * m_border_thickness.x) - (2 * m_margin.x);
  float straight_height =
      size.y - (2 * m_border_thickness.y) - (2 * m_margin.y);
  // create rectangles
  m_border_straight[0].setSize({straight_width, m_border_thickness.y});
  m_border_straight[0].setFillColor(m_inactive_border_colour);
  m_border_straight[0].setPosition(
      {origin_point.x + m_margin.x + m_border_thickness.x,
       origin_point.y + m_margin.y});
  m_border_straight[1].setSize({m_border_thickness.x, straight_height});
  m_border_straight[1].setFillColor(m_inactive_border_colour);
  m_border_straight[1].setPosition(
      {origin_point.x + m_margin.x + m_border_thickness.x + straight_width,
       origin_point.y + m_margin.y + m_border_thickness.y});
  m_border_straight[2].setSize({straight_width, m_border_thickness.y});
  m_border_straight[2].setFillColor(m_inactive_border_colour);
  m_border_straight[2].setPosition(
      {origin_point.x + m_margin.x + m_border_thickness.x,
       origin_point.y + m_margin.y + m_border_thickness.y + straight_height});
  m_border_straight[3].setSize({m_border_thickness.x, straight_height});
  m_border_straight[3].setFillColor(m_inactive_border_colour);
  m_border_straight[3].setPosition(
      {origin_point.x + m_margin.x,
       origin_point.y + m_margin.y + m_border_thickness.y});

  // draw radius corners

  float corner_radius = m_border_thickness.x;
  float radian_angle_increment = 2 * M_PI / m_radius_resolution;

  for (size_t i = 0; i < 4; i++) {
    m_border_corners[i] = sf::VertexArray(sf::PrimitiveType::TriangleFan,
                                          m_radius_resolution + 2);
    sf::Vector2f corner_origin;
    switch (i) {
    case 0:
      corner_origin = {origin_point.x + m_margin.x + m_border_thickness.x,
                       origin_point.y + m_margin.y + m_border_thickness.y};
      break;
    case 1:
      corner_origin = {origin_point.x + m_margin.x + m_border_thickness.x +
                           straight_width,
                       origin_point.y + m_margin.y + m_border_thickness.y};
      break;
    case 2:
      corner_origin = {
          origin_point.x + m_margin.x + m_border_thickness.x + straight_width,
          origin_point.y + m_margin.y + m_border_thickness.y + straight_height};
      break;
    case 3:
      corner_origin = {origin_point.x + m_margin.x + m_border_thickness.x,
                       origin_point.y + m_margin.y + m_border_thickness.y +
                           straight_height};
      break;
    }
    m_border_corners[i][0].position = corner_origin;
    for (size_t j = 1; j < m_radius_resolution + 2; j++) {
      m_border_corners[i][j].position = {
          corner_origin.x + corner_radius +
              (cos(radian_angle_increment * (j - 1)) * corner_radius *
               ((i == 0 || i == 3) ? -1 : 1)),
          corner_origin.y + corner_radius +
              (sin(radian_angle_increment * (j - 1)) * corner_radius *
               ((i == 0 || i == 1) ? -1 : 1))};
      m_border_corners[i][j].color = m_inactive_border_colour;
    }
  }
};
