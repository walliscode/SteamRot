/////////////////////////////////////////////////
/// @file
/// @brief declaration of free functions for rendering the background of the
/// scene
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include <SFML/Graphics.hpp>

namespace steamrot::logic::render {

void draw_grid_background(const sf::Vector2i &grid_size,
                          const uint8_t &grid_thickness,
                          const sf::Color &grid_color,
                          sf::RenderTexture &texture);
}
