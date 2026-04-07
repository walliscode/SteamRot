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

namespace steamrot::logic::positioning::ui {

/////////////////////////////////////////////////
/// @brief Update the size and position of child elements based on the parent
/// element's layout and style
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
