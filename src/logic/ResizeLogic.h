/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the ResizeLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Logic.h"
#include "LogicType.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
/// @class ResizeLogic
/// @brief Handles window resize events for a Scene.
///
/// ResizeLogic subscribes to SYSTEM RESIZE events emitted by the
/// SFMLEventConverter and updates the scene render texture dimensions
/// to match the new window size. All specific resize operations are
/// delegated to free functions in resize_logic.h for easy testing.
/////////////////////////////////////////////////
class ResizeLogic : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Process resize events and update scene resources accordingly.
  /////////////////////////////////////////////////
  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for ResizeLogic.
  ///
  /// Registers a subscriber for SYSTEM RESIZE events during construction.
  ///
  /// @param scene_context SceneContext object containing the context for the
  /// logic.
  /////////////////////////////////////////////////
  ResizeLogic(const SceneContext scene_context);

  /////////////////////////////////////////////////
  /// @brief Returns the LogicType of this Logic class.
  ///
  /// @return LogicType::Resize
  /////////////////////////////////////////////////
  LogicType GetLogicType() const override { return LogicType::Resize; }
};

} // namespace steamrot::logic
