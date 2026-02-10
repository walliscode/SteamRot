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
#include <iostream>

namespace steamrot::logic {
/////////////////////////////////////////////////
GrimoireMachinaPositioningLogic::GrimoireMachinaPositioningLogic(
    const SceneContext &scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void GrimoireMachinaPositioningLogic::ProcessLogic() {

  // get THE grimoire machina from the AssetManager
  auto grimoire_result = m_scene_context.asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value()) {
  }
  GrimoireMachina &grimoire_machina = *grimoire_result.value();

  // call one time only logic here
  if (run_set_up_logic) {

    // pull out the render texture size as a Vector2f for use in the positioning
    // calculations
    sf::Vector2f render_texture_size(m_scene_context.scene_texture.getSize());

    // set up the positioning of the crafting canvas
    grimoire_machina.m_crafting_helpers.crafting_canvas =
        positioning::grimoire_machina::CalculateCraftingCanvasSizeAndPosition(
            sf::FloatRect{{0.0f, 0.0f}, render_texture_size},
            entity::memory::GetComponentVector<CUserInterface>(
                m_scene_context.scene_entities));

    std::cout << "Grimoire Machina Crafting Canvas Position: "
              << grimoire_machina.m_crafting_helpers.crafting_canvas.position.x
              << ", "
              << grimoire_machina.m_crafting_helpers.crafting_canvas.position.y
              << std::endl;
    std::cout << "Grimoire Machina Crafting Canvas Size: "
              << grimoire_machina.m_crafting_helpers.crafting_canvas.size.x
              << ", "
              << grimoire_machina.m_crafting_helpers.crafting_canvas.size.y
              << std::endl;
    // shut latch
    run_set_up_logic = false;
  }
}
} // namespace steamrot::logic
