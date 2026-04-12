/////////////////////////////////////////////////
/// @file
/// @brief Implementation of GrimoireMachinaActionLogic class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GrimoireMachinaActionLogic.h"
#include "CUserInterface.h"
#include "EventPayload.h"
#include "EventType.h"
#include "action_grimoire_machina.h"
#include "archetypes.h"
#include "collision_mouse.h"
#include "entity_memory.h"

namespace steamrot::logic {
/////////////////////////////////////////////////
GrimoireMachinaActionLogic::GrimoireMachinaActionLogic(
    const SceneContext scene_context)
    : Logic(scene_context) {

  // Register a subscriber for USER_INPUT SELECT events so that pick-and-place
  // clicks on the crafting canvas can be detected each tick.
  m_place_subscriber = std::make_shared<Subscriber>();
  m_place_subscriber->event_type = EventType::USER_INPUT;
  m_place_subscriber->filter_payload =
      InputPayload{InputPayload::InputAction::SELECT};
  m_scene_context.event_handler.RegisterSubscriber(m_place_subscriber);
}

/////////////////////////////////////////////////
void GrimoireMachinaActionLogic::ProcessLogic() {

  // --- Asset section ---
  auto grimoire_result = m_scene_context.asset_manager.GetGrimoireMachina();
  if (!grimoire_result.has_value()) {
    return;
  }
  GrimoireMachina &grimoire_machina = *grimoire_result.value();

  // --- Event section: LogicPayload subscribers (scaffold init/clear) ---
  for (auto &subscriber : m_subscribers) {
    if (!subscriber->m_active)
      continue;
    action::grimoire_machina::ProcessSubscriber(*subscriber, grimoire_machina);
  }

  // --- Placement section: USER_INPUT SELECT ---
  if (m_place_subscriber->m_active) {

    // Guard 1: a ghost item must be selected (not monostate).
    bool can_place =
        !std::holds_alternative<std::monostate>(m_scene_context.mr_ghost.m_selection);

    // Guard 2: the click must not land on any visible UI element.
    if (can_place) {
      const std::vector<size_t> ui_ids =
          archetypes::GetEntitiesSortedByPriority<CUserInterface>(
              m_scene_context.archetypes, m_scene_context.scene_entities,
              /*ascending=*/false);
      for (size_t id : ui_ids) {
        const CUserInterface &ui =
            entity::memory::GetComponent<CUserInterface>(
                id, m_scene_context.scene_entities);
        if (ui.m_visible &&
            collision::mouse::AnyMouseOver(*ui.m_root_element)) {
          can_place = false;
          break;
        }
      }
    }

    // Guard 3: the active scaffold must exist.
    if (can_place && !grimoire_machina.m_scaffold_form)
      can_place = false;

    // Guard 4: the click must be inside the crafting canvas.
    if (can_place) {
      can_place = collision::mouse::IsMouseOverBounds(
          m_scene_context.mouse_position,
          grimoire_machina.m_crafting_helpers.crafting_canvas);
    }

    if (can_place) {
      // Convert screen-space mouse position to world-space coordinates so
      // the placed instance is correctly positioned regardless of zoom level.
      const sf::Vector2f world_pos =
          m_scene_context.scene_texture.mapPixelToCoords(
              m_scene_context.mouse_position,
              m_scene_context.camera_state.GetWorldView(
                  m_scene_context.scene_texture));

      // [TODO:] handle the result and report failure if it fails.
      auto place_result = action::grimoire_machina::PlaceGhostOnScaffold(
          grimoire_machina, m_scene_context.mr_ghost, world_pos);
    }
  }

}
} // namespace steamrot::logic
