/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the Button struct
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
#include <string>

namespace steamrot {

/////////////////////////////////////////////////
/// @class ButtonElement
/// @brief Button UI element
///
/////////////////////////////////////////////////
struct ButtonElement : public UIElement {

  /////////////////////////////////////////////////
  /// @brief Button label, this is the text that will be displayed on the button
  /////////////////////////////////////////////////
  std::string label{"unlabelled"};

  /////////////////////////////////////////////////
  /// @brief Draws the ButtonElement on a RenderTexture
  ///
  /// @param texture Reference to the RenderTexture to draw on
  /// @param style UIStyle providing values for drawing
  /////////////////////////////////////////////////
  void DrawUIElement(sf::RenderTexture &texture,
                     const UIStyle &style) const override {

    // Draw the border and background
    draw_ui_elements::DrawBorderAndBackground(texture, position, size,
                                              style.button_style);

    // Draw the button text
    sf::Vector2f text_position{
        position.x + style.button_style.border_thickness +
            style.button_style.inner_margin.x,
        position.y + style.button_style.border_thickness +
            style.button_style.inner_margin.y};

    draw_ui_elements::DrawText(
        texture, label, text_position, size, style.button_style.font,
        style.button_style.font_size, style.button_style.text_color);
  }
};
} // namespace steamrot
