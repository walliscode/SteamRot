/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the UIStateLogic class.
/////////////////////////////////////////////////

#pragma once

#include "Logic.h"

namespace steamrot {

class UIStateLogic : public Logic {

private:
  /////////////////////////////////////////////////
  /// @brief Override method to encapsulate all logic for UIStateLogic
  /////////////////////////////////////////////////
  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for UIStateLogic.
  ///
  /// @param scene_context SceneContext object containing the context for the logic.
  /////////////////////////////////////////////////
  UIStateLogic(const SceneContext scene_context);
};

} // namespace steamrot
