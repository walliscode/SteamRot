/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the MrGhost struct.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "MachinaFormScaffold.h"
#include <SFML/System/Vector2.hpp>
#include <variant>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Variant representing the currently active ghost instance.
///
/// std::monostate indicates no active selection. FragmentInstance and
/// JointInstance carry a pointer to the underlying Part definition together
/// with the transform updated each tick by positioning logic.
/////////////////////////////////////////////////
using GhostInstance =
    std::variant<std::monostate, FragmentInstance, JointInstance>;

/////////////////////////////////////////////////
/// @struct MrGhost
/// @brief Per-scene state object tracking the current ghost item.
///
/// MrGhost holds the active ghost instance (Fragment or Joint) together with
/// the raw world-space cursor position. The instance's transform is kept
/// up-to-date by GhostPositioningLogic each tick.
///
/// A single instance lives in SceneResources for each Scene. Logic classes
/// access it via the MrGhost& reference in SceneContext.
/////////////////////////////////////////////////
struct MrGhost {
  /////////////////////////////////////////////////
  /// @brief The currently active ghost instance, or std::monostate if nothing
  /// is selected.
  /////////////////////////////////////////////////
  GhostInstance m_instance{std::monostate{}};

  /////////////////////////////////////////////////
  /// @brief The current world-space position of the ghost, updated each tick
  /// to track the mouse cursor position.
  /////////////////////////////////////////////////
  sf::Vector2f m_position{};
};

} // namespace steamrot
