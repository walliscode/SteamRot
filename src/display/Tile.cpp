#define _USE_MATH_DEFINES
#include "Tile.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <iostream>

Tile::Tile(const json &tile_config, const sf::FloatRect &viewport_ratio,
           const sf::Vector2f &window_size) {

  //
  SetTileStaticProperties(tile_config);
  SetViewProperties(viewport_ratio, window_size);
  SetBorder(window_size);
};

void Tile::SetTileStaticProperties(const json &config) {

  m_active_border_colour = sf::Color(config["active_border_colour"]["r"],
                                     config["active_border_colour"]["g"],
                                     config["active_border_colour"]["b"]);

  m_inactive_border_colour = sf::Color(config["inactive_border_colour"]["r"],
                                       config["inactive_border_colour"]["g"],
                                       config["inactive_border_colour"]["b"]);

  m_margin = {config["margin"]["x"], config["margin"]["y"]};

  m_border_thickness = {config["border_thickness"]["x"],
                        config["border_thickness"]["y"]};

  m_radius_resolution = config["radius_resolution"];
}

void Tile::SetViewProperties(const sf::FloatRect &viewport_ratio,
                             const sf::Vector2f &window_size) {

  m_view.setViewport(viewport_ratio);

  sf::Vector2f size = {window_size.x * viewport_ratio.size.x,
                       window_size.y * viewport_ratio.size.y};

  sf::Vector2f center = {viewport_ratio.position.x + size.x / 2,
                         viewport_ratio.position.y + size.y / 2};

  m_view.setSize(size);
  m_view.setCenter(center);
};

// The tile border is composed up of 4 rectangle boxes and 4 radius corners
// drawn with sf::TriangleFan These are then stored in a sf::VertexArray

void Tile::SetBorder(const sf::Vector2f &window_size) {
  // get the origin point of the tile
  sf::Vector2f origin_point = {window_size.x * m_view.getViewport().position.x,
                               window_size.y * m_view.getViewport().position.y};

  std::cout << "origin_point.x: " << origin_point.x << std::endl;
  std::cout << "origin_point.y: " << origin_point.y << std::endl;

  // get the absolute size of the view using the ratio of the viewport of the
  // window
  sf::Vector2f size = {window_size.x * m_view.getViewport().size.x,
                       window_size.y * m_view.getViewport().size.y};

  // draw reactangle borders
  std::cout << "size.x: " << size.x << std::endl;
  float straight_width = size.x - (2 * m_border_thickness.x) - (2 * m_margin.x);
  std::cout << "straight_width: " << straight_width << std::endl;
  float straight_height =
      size.y - (2 * m_border_thickness.y) - (2 * m_margin.y);
  std::cout << "straight_height: " << straight_height << std::endl;
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
  float radian_angle_increment =
      (90.0f / m_radius_resolution) * (M_PI / 180.0f);

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
    std::cout << "corner_origin.x: " << corner_origin.x << std::endl;
    std::cout << "corner_origin.y: " << corner_origin.y << std::endl;
    m_border_corners[i][0].position = corner_origin;
    for (size_t j = 1; j < m_radius_resolution + 2; j++) {
      m_border_corners[i][j].position = {
          corner_origin.x + corner_radius +
              (cos(radian_angle_increment * (j - 1)) * corner_radius *
               ((i == 0 || i == 3) ? -1 : 1)),
          corner_origin.y + corner_radius +
              (sin(radian_angle_increment * (j - 1)) * corner_radius *
               ((i == 0 || i == 1) ? -1 : 1))};

      // {
      //   corner_origin.x + corner_radius +
      //       (cos(radian_angle_increment * (j - 1)) * corner_radius *
      //        ((i == 0 || i == 3) ? -1 : 1)),
      //   corner_origin.y + corner_radius +
      //       (sin(radian_angle_increment * (j - 1)) * corner_radius *
      //        ((i == 0 || i == 1) ? -1 : 1))};

      std::cout << "m_border_corners[i][j].position.x: "
                << m_border_corners[i][j].position.x << std::endl;
      std::cout << "m_border_corners[i][j].position.y: "
                << m_border_corners[i][j].position.y << std::endl;
      m_border_corners[i][j].color = m_inactive_border_colour;
    }
  }
};

void Tile::SetBorderColourActive() {
  for (auto &border_straight : m_border_straight) {
    border_straight.setFillColor(m_active_border_colour);
  }
  for (auto &border_corner : m_border_corners) {
    for (size_t i = 0; i < border_corner.getVertexCount(); i++) {
      border_corner[i].color = m_active_border_colour;
    }
  }
};

void Tile::SetBorderColourInactive() {
  for (auto &border_straight : m_border_straight) {
    border_straight.setFillColor(m_inactive_border_colour);
  }
  for (auto &border_corner : m_border_corners) {
    for (size_t i = 0; i < border_corner.getVertexCount(); i++) {
      border_corner[i].color = m_inactive_border_colour;
    }
  }
};

sf::View Tile::GetView() { return m_view; };

const std::array<sf::RectangleShape, 4> &Tile::GetBorderStraights() {
  return m_border_straight;
};

const std::array<sf::VertexArray, 4> &Tile::GetBorderCorners() {
  return m_border_corners;
};
