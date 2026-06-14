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
#include <array>
#include <magic_enum/magic_enum.hpp>
#include <set>

using namespace magic_enum::bitwise_operators;
namespace steamrot::logic {
/////////////////////////////////////////////////
UIActionLogic::UIActionLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void UIActionLogic::ProcessLogic() {

  std::set<size_t> entity_index_set =
      archetypes::GenerateEntityIndexesFromComponents<CUserInterface>(
          m_scene_context.archetypes, true);
  std::vector<size_t> entity_indexes(entity_index_set.begin(),
                                     entity_index_set.end());

  static constexpr std::array k_action_pass_order{
      UIPriorityTier::Modal, UIPriorityTier::Elevated, UIPriorityTier::Normal,
      UIPriorityTier::Background};

  for (const UIPriorityTier tier : k_action_pass_order) {
    for (size_t entity_id : entity_indexes) {

      // get the CUserInterface component
      CUserInterface &ui_component = entity::memory::GetComponent<CUserInterface>(
          entity_id, m_scene_context.scene_entities);

      // skip entities outside the active pass
      if (ui_component.m_priority_tier != tier || !ui_component.m_visible) {
        continue;
      }

      // check hover state BEFORE processing actions: we need to know whether
      // this entity owned the mouse going into this frame so we can stop
      // processing lower-tier entities afterwards.
      bool entity_has_hover =
          collision::mouse::AnyMouseOver(*ui_component.m_root_element);

      // Perform any action logic here, processing nested elements recursively
      action::ui::ProcessNestedUIActionsAndEvents(
          *ui_component.m_root_element, m_scene_context.event_handler,
          m_scene_context);

      // if this entity had a hovered element, stop processing further entities
      if (entity_has_hover) {
        return;
      }
    }
  }
}

} // namespace steamrot::logic
