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

  /////////////////////////////////////////////////
  /// @brief Draw the current CMachinaForm in the CGrimoireMachina
  /////////////////////////////////////////////////
  void DrawMachinaForm();

  /////////////////////////////////////////////////
  /// @brief Render the joints in the current holding form.
  ///
  /// @param joint Contains all the data required to render a joint.
  /////////////////////////////////////////////////
  void RenderJoint(Joint &joint);

  /////////////////////////////////////////////////
  /// @brief Render the fragments in the current holding form.
  ///
  /// @param fragment Contains all the data required to render a fragment.
  /////////////////////////////////////////////////
  void RenderFragment(Fragment &fragment);

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
