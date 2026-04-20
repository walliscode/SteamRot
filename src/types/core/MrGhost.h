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
#include <string>
#include <variant>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct FragmentTag
/// @brief Lightweight tag identifying a Fragment by name.
///
/// Used in GhostSelection and GhostPayload to refer to a Fragment without
/// holding a pointer. action_ghost resolves this tag to a FragmentInstance
/// via GrimoireMachina at selection time.
/////////////////////////////////////////////////
struct FragmentTag {
  std::string key{};
};

/////////////////////////////////////////////////
/// @struct JointTag
/// @brief Lightweight tag identifying a Joint by name.
///
/// Used in GhostSelection and GhostPayload to refer to a Joint without
/// holding a pointer. action_ghost resolves this tag to a JointInstance
/// via GrimoireMachina at selection time.
/////////////////////////////////////////////////
struct JointTag {
  std::string key{};
};

/////////////////////////////////////////////////
/// @brief Variant carrying a lightweight selection tag for a ghost item.
///
/// Transported in GhostPayload and stored on DropDownItemElement.
/// std::monostate indicates no active selection or a CLEAR action.
/// Resolved to a GhostInstance by action_ghost::SelectGhostItem.
/////////////////////////////////////////////////
using GhostSelection = std::variant<std::monostate, FragmentTag, JointTag>;

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

  /////////////////////////////////////////////////
  /// @brief The accumulated global rotation of the ghost, in degrees.
  ///
  /// Incremented by 90 degrees each time the user presses the R key.
  /// Applied by GhostPositioningLogic each tick when building the instance
  /// transform.
  /////////////////////////////////////////////////
  float m_rotation_degrees{0.f};
};

} // namespace steamrot
