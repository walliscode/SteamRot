/////////////////////////////////////////////////
/// @file
/// @brief Declaration of DropDownListElement struct
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
struct DropDownListElement : public UIElement {
  /////////////////////////////////////////////////
  /// @brief Indicates whether the dropdown is expanded or not.
  /////////////////////////////////////////////////
  bool is_expanded{false};

  /////////////////////////////////////////////////
  /// @brief The name you see when the items are not expanded. It should
  /// indicate role of the dropdown
  /////////////////////////////////////////////////
  std::string unexpanded_label{"items..."};

  /////////////////////////////////////////////////
  /// @brief The name you see when the items are expanded.
  /////////////////////////////////////////////////
  std::string expanded_label{"items..."};

  /////////////////////////////////////////////////
  /// @brief Draws the DropDownListElement on a RenderTexture
  ///
  /// @param texture Reference to the RenderTexture to draw on
  /// @param style UIStyle providing values for drawing
  /////////////////////////////////////////////////
  void DrawUIElement(sf::RenderTexture &texture,
                     const UIStyle &style) const override {
    draw_ui_elements::DrawBorderAndBackground(texture, position, size,
                                              style.drop_down_list_style);

    if (!is_expanded) {
      // calculate the position for the text
      sf::Vector2f text_position{
          position.x + style.drop_down_list_style.border_thickness +
              style.drop_down_list_style.inner_margin.x,
          position.y + style.drop_down_list_style.border_thickness +
              style.drop_down_list_style.inner_margin.y};

      draw_ui_elements::DrawText(texture, unexpanded_label, text_position, size,
                                 style.drop_down_list_style.font,
                                 style.drop_down_list_style.font_size,
                                 style.drop_down_list_style.text_color);
      ;
    }
  }
};
} // namespace steamrot
