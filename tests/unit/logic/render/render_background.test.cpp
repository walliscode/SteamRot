/////////////////////////////////////////////////
/// @file
/// @brief unit tests for the free functions for rendering the background of the
/// scene
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_background.h"
#include "ColorEqualsMatcher.h"
#include "catch2/matchers/catch_matchers.hpp"
#include <SFML/System/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>

namespace steamrot::tests {
using namespace logic::render;

void check_grid_pixel_color(const sf::RenderTexture &texture,
                            const sf::Vector2i &grid_spacing,
                            const uint8_t &grid_thickness,
                            const sf::Color &grid_color) {
  // check the pixels in the texture, positions should account for the view and
  // the fact it is drawn from the center of the view
  const sf::View &view = texture.getView();
  const sf::Vector2f view_center = view.getCenter();
  const sf::Vector2f view_size = view.getSize();

  // check horizontal lines going up
  for (float y = view_center.y; y >= view_center.y - view_size.y / 2;
       y -= grid_spacing.y) {
    for (int i = 0; i < grid_thickness; ++i) {
      REQUIRE_THAT(
          texture.getTexture().copyToImage().getPixel(
              {static_cast<unsigned int>(view_center.x - view_size.x / 2),
               static_cast<unsigned int>(y + i)}),
          EqualsColor(grid_color));
    }
  }
  // // check horizontal lines going down
  // for (float y = view_center.y + grid_spacing.y;
  //      y <= view_center.y + view_size.y / 2; y += grid_spacing.y) {
  //   for (int i = 0; i < grid_thickness; ++i) {
  //     REQUIRE_THAT(
  //         texture.getTexture().copyToImage().getPixel(
  //             {static_cast<unsigned int>(view_center.x - view_size.x / 2),
  //              static_cast<unsigned int>(y + i)}),
  //         EqualsColor(grid_color));
  //   }
  // }

  // check vertical lines going left
  for (float x = view_center.x; x >= view_center.x - view_size.x / 2;
       x -= grid_spacing.x) {
    for (int i = 0; i < grid_thickness; ++i) {
      REQUIRE_THAT(
          texture.getTexture().copyToImage().getPixel(
              {static_cast<unsigned int>(x + i),
               static_cast<unsigned int>(view_center.y - view_size.y / 2)}),
          EqualsColor(grid_color));
    }
  }
  // // check vertical lines going right
  // for (float x = view_center.x + grid_spacing.x;
  //      x <= view_center.x + view_size.x / 2; x += grid_spacing.x) {
  //   for (int i = 0; i < grid_thickness; ++i) {
  //     REQUIRE_THAT(
  //         texture.getTexture().copyToImage().getPixel(
  //             {static_cast<unsigned int>(x + i),
  //              static_cast<unsigned int>(view_center.y - view_size.y / 2)}),
  //         EqualsColor(grid_color));
  //   }
  // }
}
TEST_CASE("draw_grid_background tests", "[render_background]") {

  SECTION("draw_grid_background draws a grid background with correct size and "
          "color, limited to the default view") {

    // set up the render texture
    sf::RenderTexture texture{{100, 100}};
    texture.clear(sf::Color::Black);

    // set up the grid parameters
    sf::Vector2i grid_spacing{10, 10};
    uint8_t grid_thickness{1};
    sf::Color grid_color{255, 255, 255, 255};

    // call the function
    draw_grid_background(grid_spacing, grid_thickness, grid_color, texture);

    // assert
    check_grid_pixel_color(texture, grid_spacing, grid_thickness, grid_color);
  }
}
} // namespace steamrot::tests
