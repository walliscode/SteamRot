/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GrimoireMachinaPositioningLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GrimoireMachinaPositioningLogic.h"
#include "CUserInterface.h"
#include "entity_memory.h"
#include "positioning_grimoire_machina.h"
#include <SFML/Graphics/Rect.hpp>

namespace steamrot::logic {
/////////////////////////////////////////////////
GrimoireMachinaPositioningLogic::GrimoireMachinaPositioningLogic(
    const SceneContext &scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void GrimoireMachinaPositioningLogic::ProcessLogic() {

  // --- Asset section ---
  auto grimoire_result = m_scene_context.asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value()) {
    return;
  }
  GrimoireMachina &grimoire_machina = *grimoire_result.value();

  // --- Canvas size and position calculation ---
  sf::Vector2f render_texture_size(m_scene_context.scene_texture.getSize());
  grimoire_machina.m_crafting_helpers.crafting_canvas =
      positioning::grimoire_machina::CalculateCraftingCanvasSizeAndPosition(
          sf::FloatRect{{0.0f, 0.0f}, render_texture_size},
          entity::memory::GetComponentVector<CUserInterface>(
              m_scene_context.scene_entities));

  if (grimoire_machina.m_scaffold_form) {
    positioning::grimoire_machina::PositionGrowthPoint(
        grimoire_machina.m_scaffold_form->growth_point,
        grimoire_machina.m_crafting_helpers.crafting_canvas);
  }
}
} // namespace steamrot::logic
