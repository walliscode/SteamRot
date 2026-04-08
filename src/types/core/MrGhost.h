/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the MrGhost struct and its selection tag types.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <string>
#include <variant>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct FragmentTag
/// @brief Tag identifying a selected Fragment type by key.
/////////////////////////////////////////////////
struct FragmentTag {
  std::string key{};
};

/////////////////////////////////////////////////
/// @brief Variant representing the currently selected item in MrGhost.
///
/// std::monostate indicates no active selection. Additional tag types
/// can be appended to this variant as new selectable categories are
/// introduced without changing any existing code.
/////////////////////////////////////////////////
using GhostSelection = std::variant<std::monostate, FragmentTag>;

/////////////////////////////////////////////////
/// @struct MrGhost
/// @brief Per-scene POD state object tracking the current ghost selection.
///
/// MrGhost holds the type and identity of the item most recently selected
/// from a menu, dropdown, or blueprint list. It does not reference live
/// instances — only the category and key of the selection.
///
/// A single instance lives in SceneResources for each Scene. Logic classes
/// access it via the MrGhost& reference in SceneContext.
/////////////////////////////////////////////////
struct MrGhost {
  /////////////////////////////////////////////////
  /// @brief The currently selected item, or std::monostate if nothing is
  /// selected.
  /////////////////////////////////////////////////
  GhostSelection m_selection{std::monostate{}};
};

} // namespace steamrot
