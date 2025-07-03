/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CraftingRenderLogic class.
/////////////////////////////////////////////////

#include "CraftingRenderLogic.h"
#include "ArchetypeHelpers.h"
#include "ArchetypeManager.h"
#include "CGrimoireMachina.h"
#include "EntityHelpers.h"
#include "log_handler.h"

namespace steamrot {

/////////////////////////////////////////////////
CraftingRenderLogic::CraftingRenderLogic(const LogicContext logic_context)
    : BaseLogic(logic_context) {}

/////////////////////////////////////////////////
void CraftingRenderLogic::ProcessLogic() {

  // Draw the MachinaForm
  DrawMachinaForm();
}

/////////////////////////////////////////////////
void CraftingRenderLogic::DrawMachinaForm() {

  // Get the Archetype for CGrimoireMachina
  Archetype archetype =
      m_logic_context
          .archetypes[GenerateArchetypeIDfromTypes<CGrimoireMachina>()];

  // if archetype is empty, return early
  if (archetype.empty()) {
    return;
  }

  // Check if the archetype is greater than 1 and log an error if so
  if (archetype.size() > 1) {
    log_handler::ProcessLog(
        spdlog::level::level_enum::err, log_handler::LogCode::kNoCode,
        "CraftingRenderLogic: More than one CGrimoireMachina found in the "
        "archetype, expected only one.");
    return;
  }

  // Get the first entity in the archetype and pull out the CGrimoireMachina
  // component
  CGrimoireMachina &grimoire_machina = GetComponent<CGrimoireMachina>(
      archetype[0], m_logic_context.scene_entities);

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
  m_logic_context.scene_texture.draw(joint.m_render_overlay, joint.m_transform);
}

/////////////////////////////////////////////////
void CraftingRenderLogic::RenderFragment(Fragment &fragment) {

  // should be as simple as applying the transform to the render overlay
  m_logic_context.scene_texture.draw(fragment.m_render_overlay,
                                     fragment.m_transform);
}
} // namespace steamrot
