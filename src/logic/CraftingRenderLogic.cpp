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

  // get all entity indexes with CGrimoireMachina component
  std::set<size_t> entity_indexes =
      archetypes::GenerateEntityIndexesFromComponents<CGrimoireMachina>(
          m_scene_context.archetypes, true);

  // cycle through all the entity indexs in the archetype
  for (size_t entity_id : entity_indexes) {

    // get the CGrimoireMachina component
    CGrimoireMachina &grimoire_machina =
        entity::memory::GetComponent<CGrimoireMachina>(
            entity_id, m_scene_context.scene_entities);

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
  }
};

/////////////////////////////////////////////////
void CraftingRenderLogic::RenderJoint(Joint &joint) {

  // should be as simple as applying the transform to the render overlay
  m_scene_context.scene_texture.draw(joint.m_render_overlay, joint.m_transform);
}

/////////////////////////////////////////////////
void CraftingRenderLogic::RenderFragment(Fragment &fragment) {

  // should be as simple as applying the transform to the render overlay
  m_scene_context.scene_texture.draw(
      fragment.m_overlays[ViewDirection::ViewDirection_FRONT],
      fragment.m_transform);
}
} // namespace steamrot
