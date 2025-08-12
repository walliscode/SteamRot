/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CraftingRenderLogic class.
/////////////////////////////////////////////////

#include "CraftingRenderLogic.h"
#include "ArchetypeHelpers.h"
#include "ArchetypeManager.h"
#include "CGrimoireMachina.h"
#include "emp_helpers.h"
#include "fragments_generated.h"
#include "log_handler.h"

namespace steamrot {

/////////////////////////////////////////////////
CraftingRenderLogic::CraftingRenderLogic(const LogicContext logic_context)
    : Logic(logic_context) {}

/////////////////////////////////////////////////
void CraftingRenderLogic::ProcessLogic() {

  // Draw the MachinaForm
  DrawMachinaForm();
}

/////////////////////////////////////////////////
void CraftingRenderLogic::DrawMachinaForm() {

  // get required archetype ID for CGrimoireMachina
  ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CGrimoireMachina>();
  const auto it = m_logic_context.archetypes.find(archetype_id);

  // If the archetype is not found, return early
  if (it == m_logic_context.archetypes.end()) {
    return;
  }

  const Archetype archetype = it->second;
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
  CGrimoireMachina &grimoire_machina =
      emp_helpers::GetComponent<CGrimoireMachina>(
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
  m_logic_context.scene_texture.draw(
      fragment.m_overlays[ViewDirection::ViewDirection_FRONT],
      fragment.m_transform);
}
} // namespace steamrot
