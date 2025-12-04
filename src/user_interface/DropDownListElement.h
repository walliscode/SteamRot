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
#include "logic_render.h"

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
  /// @brief Function to populate dropdown data dynamically
/////////////////////////////////////////////////
  DataPopulateFunction data_populate_function{
      DataPopulateFunction::DataPopulateFunction_None};

/////////////////////////////////////////////////
  /// @brief Create a deep copy of this DropDownListElement
  ///
  /// @return A new unique_ptr to a cloned DropDownListElement
/////////////////////////////////////////////////
  std::unique_ptr<UIElement> Clone() const override {
    auto cloned = std::make_unique<DropDownListElement>();
    CloneBaseUIElementData(*cloned);
    cloned->is_expanded = is_expanded;
    cloned->unexpanded_label = unexpanded_label;
    cloned->expanded_label = expanded_label;
    cloned->data_populate_function = data_populate_function;
    return cloned;
  }

/////////////////////////////////////////////////
  /// @brief Draws the DropDownListElement on a RenderTexture
  ///
  /// @param texture Reference to the RenderTexture to draw on
  /// @param style UIStyle providing values for drawing
/////////////////////////////////////////////////
  void DrawUIElement(sf::RenderTexture &texture,
                     const UIStyle &style) const override {
    logic::render::DrawBorderAndBackground(texture, *this,
                                              style.drop_down_list_style);

    // calculate the position for the text
    sf::Vector2f text_position{
        position.x + style.drop_down_list_style.border_thickness +
            style.drop_down_list_style.inner_margin.x,
        position.y + style.drop_down_list_style.border_thickness +
            style.drop_down_list_style.inner_margin.y};

    // set the label based on whether the dropdown is expanded
    std::string label = is_expanded ? expanded_label : unexpanded_label;

    logic::render::DrawText(texture, label, text_position, size,
                               style.drop_down_list_style.font,
                               style.drop_down_list_style.font_size,
                               style.drop_down_list_style.text_color);
    ;
  }
};
} // namespace steamrot
