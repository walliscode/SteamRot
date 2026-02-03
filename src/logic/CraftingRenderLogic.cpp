/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CraftingRenderLogic class.
/////////////////////////////////////////////////

#include "CraftingRenderLogic.h"
#include "CGrimoireMachina.h"
#include "archetypes.h"
#include "entity_memory.h"
#include "fragments_generated.h"

namespace steamrot {

/////////////////////////////////////////////////
CraftingRenderLogic::CraftingRenderLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void CraftingRenderLogic::ProcessLogic() {

  // Draw the MachinaForm
  DrawMachinaForm();
}

/////////////////////////////////////////////////
void CraftingRenderLogic::DrawMachinaForm() {

  // Get the grimoire machina from scene context
  CGrimoireMachina &grimoire_machina = m_scene_context.grimoire_machina;

  // Check if the holding form is not null, return early if it is
  if (!grimoire_machina.m_holding_form) {
    return;
  }

  // Draw the joints in the holding form
  for (auto &joint : grimoire_machina.m_holding_form->m_joints) {
    RenderJoint(joint);
  }

  // Draw the fragments in the holding form
  for (auto &fragment : grimoire_machina.m_holding_form->m_fragments) {
    RenderFragment(fragment);
  }
};

/////////////////////////////////////////////////
void CraftingRenderLogic::RenderJoint(Joint &joint) {

  // should be as simple as applying the transform to the render overlay
  m_scene_context.scene_texture.draw(joint.render_overlay, joint.transform);
}

/////////////////////////////////////////////////
void CraftingRenderLogic::RenderFragment(Fragment &fragment) {

  // should be as simple as applying the transform to the render overlay
  m_scene_context.scene_texture.draw(
      fragment.render_overlays[ViewDirection::ViewDirection_FRONT],
      fragment.transform);
}
} // namespace steamrot
