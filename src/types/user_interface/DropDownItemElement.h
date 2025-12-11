/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the DropDownItemElement struct
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

namespace steamrot {

struct DropDownItemElement : public UIElement {
/////////////////////////////////////////////////
  /// @brief The name of the item
/////////////////////////////////////////////////
  std::string label{"item..."};

/////////////////////////////////////////////////
  /// @brief The value of the item, used for selection
/////////////////////////////////////////////////
  std::string value{"value..."};

/////////////////////////////////////////////////
  /// @brief Create a deep copy of this DropDownItemElement
  ///
  /// @return A new unique_ptr to a cloned DropDownItemElement
/////////////////////////////////////////////////
  std::unique_ptr<UIElement> Clone() const override {
    auto cloned = std::make_unique<DropDownItemElement>();
    CloneBaseUIElementData(*cloned);
    cloned->label = label;
    cloned->value = value;
    return cloned;
  }

/////////////////////////////////////////////////
  /// @brief Draws the DropDownItemElement on a RenderTexture
  ///
  /// @param texture Reference to the RenderTexture to draw on
  /// @param style UIStyle providing values for drawing
/////////////////////////////////////////////////
  void DrawUIElement(sf::RenderTexture &texture,
                     const UIStyle &style) const override {
    logic::render::DrawBorderAndBackground(texture, *this,
                                              style.drop_down_item_style);
  }
};

} // namespace steamrot
