/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIPositioningLogic.h"
#include "CUserInterface.h"
#include "archetypes.h"
#include "entity_memory.h"
#include "positioning_ui.h"

namespace steamrot::logic {

/////////////////////////////////////////////////
UIPositioningLogic::UIPositioningLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void UIPositioningLogic::ProcessLogic() {

  // Generate entity indexes for entities with CUserInterface component
  auto entity_indexes =
      archetypes::GenerateEntityIndexesFromComponents<CUserInterface>(
          m_scene_context.archetypes, true);

  if (entity_indexes.empty())
    return;

  positioning::ui::PositionAllUIEntities(
      entity_indexes, m_scene_context.scene_entities,
      m_scene_context.asset_manager.GetDefaultUIStyle());
}

} // namespace steamrot::logic
