/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the SceneState struct.
/////////////////////////////////////////////////

#pragma once

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

  // Future: additional runtime state
  // - pause state
  // - transition state
  // - performance metrics
};

} // namespace steamrot
