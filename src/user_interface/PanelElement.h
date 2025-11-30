/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the PanelElement struct
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

struct PanelElement : public UIElement {

/////////////////////////////////////////////////
  /// @brief Create a deep copy of this PanelElement
  ///
  /// @return A new unique_ptr to a cloned PanelElement
/////////////////////////////////////////////////
  std::unique_ptr<UIElement> Clone() const override {
    auto cloned = std::make_unique<PanelElement>();
    CloneBaseUIElementData(*cloned);
    return cloned;
  }

/////////////////////////////////////////////////
  /// @brief Draws the PanelElement on a RenderTexture
  ///
  /// @param texture Reference to the RenderTexture to draw on
  /// @param style UIStyle providing values for drawing
/////////////////////////////////////////////////
  void DrawUIElement(sf::RenderTexture &texture,
                     const UIStyle &style) const override {
    draw_ui_elements::DrawBorderAndBackground(texture, *this,
                                              style.panel_style);
  }
};

} // namespace steamrot
