/////////////////////////////////////////////////
/// @file
/// @brief Implemetation of the GrimoireMachinaRenderLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GrimoireMachinaRenderLogic.h"
#include "render_grimoire_machina.h"
#include <SFML/Graphics/RenderTexture.hpp>

namespace steamrot::logic {
/////////////////////////////////////////////////
GrimoireMachinaRenderLogic::GrimoireMachinaRenderLogic(
    const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void GrimoireMachinaRenderLogic::ProcessLogic() {

  // get THE grimoire machina from the AssetManager
  auto grimoire_result = m_scene_context.asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value()) {
  }
  GrimoireMachina &grimoire_machina = *grimoire_result.value();

  // call render functions
  render::grimoire_machina::DrawCraftingCanvasBorder(
      m_scene_context.scene_texture,
      grimoire_machina.m_crafting_helpers.crafting_canvas);
}

} // namespace steamrot::logic
