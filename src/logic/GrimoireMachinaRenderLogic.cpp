/////////////////////////////////////////////////
/// @file
/// @brief Implemetation of the GrimoireMachinaRenderLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GrimoireMachinaRenderLogic.h"
#include "MachinaFormScaffold.h"
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
    return;
  }
  GrimoireMachina &grimoire_machina = *grimoire_result.value();
  MachinaFormScaffold *scaffold = grimoire_machina.m_scaffold_form.get();

  // call render functions
  render::grimoire_machina::DrawCraftingCanvasBorder(
      m_scene_context.scene_texture,
      grimoire_machina.m_crafting_helpers.crafting_canvas);

  // if there is no active machina form, draw the no machina form box
  if (!scaffold) {
    render::grimoire_machina::DrawNoMachinaFormBox(
        m_scene_context.scene_texture,
        grimoire_machina.m_crafting_helpers.crafting_canvas);

    // scaffold is present but no joints or fragments
  } else if (scaffold && scaffold->joints.empty()) {

    // draw the growth point (a fill in for when no fragments or joins are
    // present)
    render::grimoire_machina::DrawGrowthPoint(
        m_scene_context.scene_texture,
        grimoire_machina.m_scaffold_form->growth_point);
  }
}

} // namespace steamrot::logic
