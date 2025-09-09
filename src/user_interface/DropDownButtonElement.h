/////////////////////////////////////////////////
/// @file
/// @brief Declaration of DropDownButtonElement struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIElement.h"
#include "draw_ui_elements.h"

namespace steamrot {
struct DropDownButtonElement : public UIElement {
  /////////////////////////////////////////////////
  /// @brief Indicates whether the associated dropdown is expanded or not
  /////////////////////////////////////////////////
  bool is_expanded{false};

  /////////////////////////////////////////////////
  /// @brief Draws the DropDownButtonElement on a RenderTexture
  ///
  /// @param texture Reference to the RenderTexture to draw on
  /// @param style UIStyle providing values for drawing
  /////////////////////////////////////////////////
  void DrawUIElement(sf::RenderTexture &texture,
                     const UIStyle &style) const override {

    draw_ui_elements::DrawBorderAndBackground(texture, *this,
                                              style.drop_down_button_style);

    // calculate the radius of the triangle using the size, border thickness,
    // and inner margin of the button
    float triangle_radius =
        (size.x - 2 * style.drop_down_button_style.border_thickness -
         2 * style.drop_down_button_style.inner_margin.x) /
        2.0f;

    // create a triangle shape for the dropdown indicator
    sf::CircleShape triangle{triangle_radius, 3};
    triangle.setFillColor(style.drop_down_button_style.triangle_color);

    // set the origin to the center of the triangle
    triangle.setOrigin(triangle.getLocalBounds().getCenter());
    if (!is_expanded) {
      // rotate the triangle 180 degrees to point downwards if the dropdown is
      // not expanded
      triangle.setRotation(sf::degrees(180.0f));
    }
    // position the triangle in the centre of the button
    sf::FloatRect button_bounds{position, size};
    triangle.setPosition(button_bounds.getCenter());

    // draw the triangle on the texture
    texture.draw(triangle);
  }
};
} // namespace steamrot
