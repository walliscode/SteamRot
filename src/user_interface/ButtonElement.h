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
#include "logic_render.h"
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
  /// @brief Create a deep copy of this ButtonElement
  ///
  /// @return A new unique_ptr to a cloned ButtonElement
/////////////////////////////////////////////////
  std::unique_ptr<UIElement> Clone() const override {
    auto cloned = std::make_unique<ButtonElement>();
    CloneBaseUIElementData(*cloned);
    cloned->label = label;
    return cloned;
  }

/////////////////////////////////////////////////
  /// @brief Draws the ButtonElement on a RenderTexture
  ///
  /// @param texture Reference to the RenderTexture to draw on
  /// @param style UIStyle providing values for drawing
/////////////////////////////////////////////////
  void DrawUIElement(sf::RenderTexture &texture,
                     const UIStyle &style) const override {

    // Draw the border and background
    logic::render::DrawBorderAndBackground(texture, *this,
                                              style.button_style);

    // Draw the button text
    sf::Vector2f text_position{
        position.x + style.button_style.border_thickness +
            style.button_style.inner_margin.x,
        position.y + style.button_style.border_thickness +
            style.button_style.inner_margin.y};

    logic::render::DrawText(
        texture, label, text_position, size, style.button_style.font,
        style.button_style.font_size, style.button_style.text_color);
  }
};
} // namespace steamrot
