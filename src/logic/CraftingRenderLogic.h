/////////////////////////////////////////////////
/// @file
/// @brief Decleration of the CraftingRenderLogic class.
/////////////////////////////////////////////////

#pragma once

#include "Logic.h"
#include "SceneContext.h"

namespace steamrot {
class CraftingRenderLogic : public Logic {
private:
  /////////////////////////////////////////////////
  /// @brief Virtual function that acts as the entry point for this Logic class
  /////////////////////////////////////////////////
  void ProcessLogic() override;

public:
  /////////////////////////////////////////////////
  /// @brief Constructor for CraftingRenderLogic taking in a SceneContext
  /// object.
  ///
  /// @param scene_context The scene context for this scene
  /////////////////////////////////////////////////
  CraftingRenderLogic(const SceneContext scene_context);
};
} // namespace steamrot
