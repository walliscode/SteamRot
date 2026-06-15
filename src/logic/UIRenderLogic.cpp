/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIRenderLogic.h"
#include "CUserInterface.h"
#include "UIPriorityTier.h"
#include "archetypes.h"
#include "render_ui.h"
#include <SFML/Graphics.hpp>
#include <array>
#include <set>

namespace steamrot::logic {

/////////////////////////////////////////////////
UIRenderLogic::UIRenderLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void UIRenderLogic::ProcessLogic() {

  // Reset to the default (unzoomed) view so UI elements are always drawn
  // at their fixed screen-space positions, unaffected by world zoom.
  m_scene_context.scene_texture.setView(
      m_scene_context.scene_texture.getDefaultView());

  // Get all UI entities once; draw order is controlled by fixed tier passes.
  std::set<size_t> entity_index_set =
      archetypes::GenerateEntityIndexesFromComponents<CUserInterface>(
          m_scene_context.archetypes, true);

  std::vector<size_t> entity_indexes(entity_index_set.begin(),
                                     entity_index_set.end());

  if (entity_indexes.empty())
    return;

  // to prevent skipping over the elevated UIElements in a normal
  // CUserInterface, we need cycle through the tiers here first before
  // proceeeding to the next CUserInterface
  static constexpr std::array k_render_pass_order{
      UIPriorityTier::Normal, UIPriorityTier::Elevated, UIPriorityTier::Modal};

  for (const UIPriorityTier tier : k_render_pass_order) {
    render::ui::DrawAllUIEntitiesInTier(
        entity_indexes, m_scene_context.scene_entities,
        m_scene_context.scene_texture,
        m_scene_context.asset_manager.GetAllUIStyles(), tier);
  }
}

} // namespace steamrot::logic
