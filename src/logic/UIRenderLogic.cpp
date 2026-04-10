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

  // Generate entity indexes sorted by priority ascending so that
  // higher-priority entities are drawn last (on top)
  auto entity_indexes =
      archetypes::GetEntitiesSortedByPriority<CUserInterface>(
          m_scene_context.archetypes, m_scene_context.scene_entities,
          /*ascending=*/true);

  render::ui::DrawAllUIEntities(
      entity_indexes, m_scene_context.scene_entities,
      m_scene_context.scene_texture,
      m_scene_context.asset_manager.GetDefaultUIStyle());
}

} // namespace steamrot::logic
