/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIEventLogic class.
/////////////////////////////////////////////////
#include "UIActionLogic.h"
#include "CUserInterface.h"
#include "Logic.h"
#include "archetypes.h"
#include "collision_mouse.h"
#include "entity_memory.h"
#include "action_ui.h"
#include <SFML/Window/Mouse.hpp>
#include <magic_enum/magic_enum.hpp>

using namespace magic_enum::bitwise_operators;
namespace steamrot::logic {
/////////////////////////////////////////////////
UIActionLogic::UIActionLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void UIActionLogic::ProcessLogic() {

  // get entity indexes sorted by priority, highest first, so that the
  // highest-priority entity processes actions before lower-priority ones.
  // collision::UICollisionLogic already clears is_mouse_over on lower-priority
  // entities, so this early-exit is a defensive measure only.
  std::vector<size_t> entity_indexes =
      archetypes::GetEntitiesSortedByPriority<CUserInterface>(
          m_scene_context.archetypes, m_scene_context.scene_entities,
          /*ascending=*/false);

  for (size_t entity_id : entity_indexes) {

    // get the CUserInterface component
    CUserInterface &ui_component = entity::memory::GetComponent<CUserInterface>(
        entity_id, m_scene_context.scene_entities);

    // skip if not visible
    if (!ui_component.m_visible) {
      continue;
    }

    // check hover state BEFORE processing actions: we need to know whether this
    // entity owned the mouse going into this frame so we can stop processing
    // lower-priority entities afterwards. Actions cannot retroactively change
    // another entity's hover state, and UICollisionLogic has already cleared
    // is_mouse_over on all lower-priority entities.
    bool entity_has_hover =
        collision::mouse::AnyMouseOver(*ui_component.m_root_element);

    // Perform any action logic here, processing nested elements recursively
    action::ui::ProcessNestedUIActionsAndEvents(
        *ui_component.m_root_element, m_scene_context.event_handler,
        m_scene_context);

    // if this entity had a hovered element, stop processing further entities
    if (entity_has_hover) {
      break;
    }
  }
}

} // namespace steamrot::logic
