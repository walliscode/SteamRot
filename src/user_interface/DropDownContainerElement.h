/////////////////////////////////////////////////
/// @file
/// @brief Declaration of DropDownContainerElement struct
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

struct DropDownContainerElement : public UIElement {
  /////////////////////////////////////////////////
  /// @brief Indicates whether the dropdown is expanded or not. This should be
  /// passed to associated children elements
  /////////////////////////////////////////////////
  bool is_expanded{false};

  /////////////////////////////////////////////////
  /// @brief Draws the DropDownContainerElement on a RenderTexture
  ///
  /// @param texture Reference to the RenderTexture to draw on
  /// @param style UIStyle providing values for drawing
  /////////////////////////////////////////////////
  void DrawUIElement(sf::RenderTexture &texture,
                     const UIStyle &style) const override {

    // Draw the border and background for the container
    draw_ui_elements::DrawBorderAndBackground(texture, position, size,
                                              style.drop_down_container_style);
  }
};

} // namespace steamrot
