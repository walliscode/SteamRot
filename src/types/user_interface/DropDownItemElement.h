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
#include "MrGhost.h"
#include "UIElement.h"

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
  /// @brief The GhostInstance to emit when this item is clicked.
  ///
  /// Set by ProcessDropDownListElementActions when the item is created.
  /// std::monostate indicates no ghost selection is associated.
/////////////////////////////////////////////////
  GhostInstance ghost_selection_tag{std::monostate{}};

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
    cloned->ghost_selection_tag = ghost_selection_tag;
    return cloned;
  }
};

} // namespace steamrot
