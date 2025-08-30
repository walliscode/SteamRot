/////////////////////////////////////////////////
/// @file
/// @brief Implementation of helper functions for testing draw_ui_elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "draw_ui_elements_helpers.h"
#include "AssetManager.h"
#include "catch2/catch_test_macros.hpp"
#include <SFML/Graphics/Image.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot::tests {

/////////////////////////////////////////////////
UIStyle CreateTestUIStyle() {
  steamrot::AssetManager asset_manager{steamrot::EnvironmentType::Test};
  auto load_scene_assets_result =
      asset_manager.LoadSceneAssets(steamrot::SceneType::SceneType_TEST);
  if (!load_scene_assets_result) {
    FAIL(load_scene_assets_result.error().message);
  }
  UIStyle style;
  style.name = "test_style";

  // Panel style
  style.panel_style.background_color = sf::Color::Green;
  style.panel_style.border_color = sf::Color::Red;
  style.panel_style.border_thickness = 5.0f;
  style.panel_style.radius_resolution = 10;
  style.panel_style.inner_margin = {10.0f, 10.0f};
  style.panel_style.minimum_size = {20.0f, 20.0f};
  style.panel_style.maximum_size = {200.0f, 200.0f};

  // Button style
  style.button_style.background_color = sf::Color::Blue;
  style.button_style.border_color = sf::Color::Yellow;
  style.button_style.border_thickness = 3.0f;
  style.button_style.radius_resolution = 10;
  style.button_style.inner_margin = {5.0f, 5.0f};
  style.button_style.minimum_size = {10.0f, 10.0f};
  style.button_style.maximum_size = {100.0f, 100.0f};
  style.button_style.text_color = sf::Color::White;
  style.button_style.hover_color = sf::Color::Cyan;
  style.button_style.font_size = 12;
  style.button_style.font =
      asset_manager.GetFont("DaddyTimeMonoNerdFont-Regular").value();

  return style;
}
/////////////////////////////////////////////////
void TestDrawBoxWithBorder(const sf::Image &image, const Style &base_style,
                           const sf::Vector2f &position,
                           const sf::Vector2f &size) {
  // Check the border pixels
  for (int x = static_cast<int>(position.x);
       x < static_cast<int>(position.x + size.x); x++) {
    for (int y = static_cast<int>(position.y);
         y < static_cast<int>(position.y + size.y); y++) {
      bool is_border =
          x < static_cast<int>(position.x + base_style.border_thickness) ||
          x >= static_cast<int>(position.x + size.x -
                                base_style.border_thickness) ||
          y < static_cast<int>(position.y + base_style.border_thickness) ||
          y >= static_cast<int>(position.y + size.y -
                                base_style.border_thickness);

      sf::Color pixel = image.getPixel(
          {static_cast<unsigned int>(x), static_cast<unsigned int>(y)});
      // Add CAPTURE for all relevant values
      CAPTURE(x, y, is_border, pixel, base_style.border_color,
              base_style.background_color);

      if (is_border) {
        // Pixel should be border color
        REQUIRE(pixel == base_style.border_color);
      } else {
        // Pixel should be background color
        REQUIRE(pixel == base_style.background_color);
      }
    }
  }
}
/////////////////////////////////////////////////
void TestTextIsPresent(const sf::Image &image, const sf::Vector2f &position,
                       const sf::Vector2f &size, const sf::Color &text_color) {
  bool found_text_pixel = false;
  for (int x = static_cast<int>(position.x);
       x < static_cast<int>(position.x + size.x); x++) {
    for (int y = static_cast<int>(position.y);
         y < static_cast<int>(position.y + size.y); y++) {
      sf::Color pixel = image.getPixel(
          {static_cast<unsigned int>(x), static_cast<unsigned int>(y)});
      if (pixel == text_color) {
        found_text_pixel = true;
      }
    }
  }

  REQUIRE(found_text_pixel == true);
}

/////////////////////////////////////////////////
void TestDrawPanel(sf::Image &image, const PanelElement &panel,
                   const UIStyle &style) {

  // test that the correct pixels are drawn
  TestDrawBoxWithBorder(image, style.panel_style, panel.position, panel.size);
}

/////////////////////////////////////////////////
void TestDrawButton(sf::Image &image, const ButtonElement &button,
                    const UIStyle &style) {
  // test that button box with border is drawn correctly
  TestDrawBoxWithBorder(image, style.button_style, button.position,
                        button.size);

  // create inner section
  sf::Vector2f inner_position{
      (button.position.x + style.button_style.border_thickness +
       style.button_style.inner_margin.x),
      (button.position.y + style.button_style.border_thickness +
       style.button_style.inner_margin.y)};
  sf::Vector2f inner_size{
      button.size.x - 2 * style.button_style.border_thickness -
          2 * style.button_style.inner_margin.x,
      button.size.y - 2 * style.button_style.border_thickness -
          2 * style.button_style.inner_margin.y};

  TestTextIsPresent(image, inner_position, inner_size,
                    style.button_style.text_color);
}
} // namespace steamrot::tests
