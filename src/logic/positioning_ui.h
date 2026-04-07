/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for positioning UI elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "DropDownContainerElement.h"
#include "UIElement.h"
#include "UIStyle.h"
#include <SFML/System/Vector2.hpp>

namespace steamrot::logic::positioning::ui {

/////////////////////////////////////////////////
/// @brief Calculate the available size within a border and optional margin
///
/// @param element_size Total size of the element
/// @param border_thickness Thickness of the border on each side
/// @param inner_margin Additional margin inside the border (default {0, 0})
/// @return Available size after subtracting border and margin on both sides
/////////////////////////////////////////////////
sf::Vector2f CalculateAvailableSize(
    const sf::Vector2f &element_size, float border_thickness,
    const sf::Vector2f &inner_margin = sf::Vector2f());

/////////////////////////////////////////////////
/// @brief Calculate the inner start position after border and optional margin
///
/// @param element_position Top-left position of the element
/// @param border_thickness Thickness of the border on each side
/// @param inner_margin Additional margin inside the border (default {0, 0})
/// @return Start position inside the border and margin
/////////////////////////////////////////////////
sf::Vector2f CalculateStartPosition(
    const sf::Vector2f &element_position, float border_thickness,
    const sf::Vector2f &inner_margin = sf::Vector2f());

/////////////////////////////////////////////////
/// @brief Position the children of a DropDownContainerElement
///
/// Sets the size and position of the dropdown list (child 0) and dropdown
/// button (child 1) based on the container size, border thickness, and
/// drop_symbol_ratio from the style.
///
/// @param element DropDownContainerElement whose children will be positioned
/// @param style Style containing drop_down_container_style settings
/////////////////////////////////////////////////
void PositionDropDownContainerChildren(const DropDownContainerElement &element,
                                       const UIStyle &style);

/////////////////////////////////////////////////
/// @brief Position children of an element using the Vertical layout
///
/// Children share the available width and divide the available height equally,
/// with inner_margin.y spacing between them.
///
/// @param element Parent element whose children will be positioned
/// @param style Style containing panel_style border and margin values
/////////////////////////////////////////////////
void PositionVerticalLayoutChildren(const UIElement &element,
                                    const UIStyle &style);

/////////////////////////////////////////////////
/// @brief Position children of an element using the Horizontal layout
///
/// Children share the available height and divide the available width equally,
/// with inner_margin.x spacing between them.
///
/// @param element Parent element whose children will be positioned
/// @param style Style containing panel_style border and margin values
/////////////////////////////////////////////////
void PositionHorizontalLayoutChildren(const UIElement &element,
                                      const UIStyle &style);

/////////////////////////////////////////////////
/// @brief Position children of an element using the DropDown layout
///
/// Children are stacked vertically without inner margins; each child takes the
/// full available width and height, offset by their index.
///
/// @param element Parent element whose children will be positioned
/// @param style Style containing panel_style border thickness
/////////////////////////////////////////////////
void PositionDropDownLayoutChildren(const UIElement &element,
                                    const UIStyle &style);

/////////////////////////////////////////////////
/// @brief Update the size and position of child elements based on the parent
/// element's layout and style
///
/// Dispatches to PositionDropDownContainerChildren for DropDownContainerElement,
/// or to the appropriate layout-specific function otherwise.
///
/// @param element Parent element whose children will be positioned
/// @param style Style containing layout margins, border thickness, and ratios
/////////////////////////////////////////////////
void UpdateSizeAndPositionOfChildElements(const UIElement &element,
                                          const UIStyle &style);

/////////////////////////////////////////////////
/// @brief Recursively position nested UI elements
///
/// Positions the children of the given element and recurses into any active
/// child elements.
///
/// @param element Root element to position
/// @param style Style to use for layout calculations
/////////////////////////////////////////////////
void PositionNestedUIElements(const UIElement &element, const UIStyle &style);

} // namespace steamrot::logic::positioning::ui
