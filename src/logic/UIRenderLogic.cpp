/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIRenderLogic.h"
#include "CUserInterface.h"
#include "archetypes.h"
#include "entity_memory.h"
#include "render_ui.h"
#include <SFML/Graphics.hpp>

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

  // Generate entity indexes sorted by priority ascending so that
  // higher-priority entities are drawn last (on top)
  auto entity_indexes =
      archetypes::GetEntitiesSortedByPriority<CUserInterface>(
          m_scene_context.archetypes, m_scene_context.scene_entities,
          /*ascending=*/true);

  if (entity_indexes.empty())
    return;

  render::ui::DrawAllUIEntities(
      entity_indexes, m_scene_context.scene_entities,
      m_scene_context.scene_texture,
      m_scene_context.asset_manager.GetAllUIStyles());
}

} // namespace steamrot::logic
