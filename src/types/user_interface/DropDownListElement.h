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
#include "user_interface_generated.h"

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
  std::string unexpanded_label{"unexpanded items..."};

  /////////////////////////////////////////////////
  /// @brief The name you see when the items are expanded.
  /////////////////////////////////////////////////
  std::string expanded_label{"expanded items..."};

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
};
} // namespace steamrot
