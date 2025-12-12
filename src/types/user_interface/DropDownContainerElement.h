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

namespace steamrot {

struct DropDownContainerElement : public UIElement {
/////////////////////////////////////////////////
  /// @brief Indicates whether the dropdown is expanded or not. This should be
  /// passed to associated children elements
/////////////////////////////////////////////////
  bool is_expanded{false};

/////////////////////////////////////////////////
  /// @brief Create a deep copy of this DropDownContainerElement
  ///
  /// @return A new unique_ptr to a cloned DropDownContainerElement
/////////////////////////////////////////////////
  std::unique_ptr<UIElement> Clone() const override {
    auto cloned = std::make_unique<DropDownContainerElement>();
    CloneBaseUIElementData(*cloned);
    cloned->is_expanded = is_expanded;
    return cloned;
  }
};

} // namespace steamrot
