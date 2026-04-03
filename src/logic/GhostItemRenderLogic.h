/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the GhostItemRenderLogic class.
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
/// @class GhostItemRenderLogic
/// @brief Draws a semi-transparent preview indicator for the ghost item.
///
/// When a ghost item is active (GhostItemState::m_is_active is true) this
/// Logic draws a simple placeholder rectangle at the current ghost position
/// to give the player visual feedback of the item being "dragged" across the
/// scene.  The indicator is intentionally generic so the same class can be
/// reused across different scenes.
/////////////////////////////////////////////////
class GhostItemRenderLogic : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Wrapper function for all ghost item render logic.
  /////////////////////////////////////////////////
  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for GhostItemRenderLogic.
  ///
  /// @param scene_context SceneContext instance providing all dependencies.
  /////////////////////////////////////////////////
  GhostItemRenderLogic(const SceneContext scene_context);

  /////////////////////////////////////////////////
  /// @brief Returns the LogicType of this Logic class.
  ///
  /// @return LogicType::GhostItemRender
  /////////////////////////////////////////////////
  LogicType GetLogicType() const override {
    return LogicType::GhostItemRender;
  }
};

} // namespace steamrot::logic
