/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SceneState struct.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CameraState.h"

namespace steamrot {

/////////////////////////////////////////////////
/// @struct SceneState
/// @brief Scene runtime state.
///
/// SceneState contains runtime operational data that changes during
/// scene execution. This includes execution control flags and
/// performance tracking. Unlike SceneConfig, this state is mutable
/// and changes frequently during runtime.
/////////////////////////////////////////////////
struct SceneState {
  SceneState() = default;

  /////////////////////////////////////////////////
  /// @brief Flag indicating if the scene is active
  ///
  /// Active scenes update logic and render to their texture.
  /////////////////////////////////////////////////
  bool active{true};

  /////////////////////////////////////////////////
  /// @brief State flag indicating whether the mouse is currently over any UI
  /// layer in the scene.
  ///
  /// This flag is set by UICollisionLogic during mouse collision checks and can
  /// be used by other logic to conditionally disable non-UI interactions when
  /// the mouse is over the UI.
  /////////////////////////////////////////////////
  bool is_mouse_over_ui_layer{false};

  /////////////////////////////////////////////////
  /// @brief Camera/view state for the scene.
  ///
  /// Holds the current zoom level and world-space camera position applied to
  /// world-space rendering. Owned here as part of scene runtime state and
  /// exposed to all Logic classes via SceneContext::camera_state.
  /////////////////////////////////////////////////
  CameraState camera_state{};
};

} // namespace steamrot
