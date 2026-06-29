/////////////////////////////////////////////////
/// @file
/// @brief Premade vertex arrays for the Fragment and Joint parts. These are
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ViewDirection.h"
#include <SFML/Graphics/PrimitiveType.hpp>

namespace steamrot::parts {

/////////////////////////////////////////////////
enum class PremadeViews {
  WhiteOblong,
  WhiteSquare

};

/////////////////////////////////////////////////
inline Views get_white_oblong_Views() {
  // set up a Views object
  Views views;

  // front view
  sf::VertexArray front(sf::PrimitiveType::Triangles, 6);
  front[0].position = {0.f, 0.f};
  front[0].color = sf::Color::White;
  front[1].position = {50.f, 0.f};
  front[1].color = sf::Color::White;
  front[2].position = {50.f, 10.f};
  front[2].color = sf::Color::White;
  front[3].position = {0.f, 0.f};
  front[3].color = sf::Color::White;
  front[4].position = {50.f, 10.f};
  front[4].color = sf::Color::White;
  front[5].position = {0.f, 10.f};
  front[5].color = sf::Color::White;

  views.insert_or_assign(ViewDirection::Front, std::move(front));

  return views;
}
/////////////////////////////////////////////////
inline Views get_white_square_Views() {
  // set up a Views object
  Views views;

  // front view
  sf::VertexArray front(sf::PrimitiveType::Triangles, 6);
  front[0].position = {0.f, 0.f};
  front[0].color = sf::Color::White;
  front[1].position = {20.f, 0.f};
  front[1].color = sf::Color::White;
  front[2].position = {20.f, 20.f};
  front[2].color = sf::Color::White;
  front[3].position = {0.f, 0.f};
  front[3].color = sf::Color::White;
  front[4].position = {20.f, 20.f};
  front[4].color = sf::Color::White;
  front[5].position = {0.f, 20.f};

  views.insert_or_assign(ViewDirection::Front, std::move(front));

  return views;
}
} // namespace steamrot::parts
