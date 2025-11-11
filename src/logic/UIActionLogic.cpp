/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIEventLogic class.
/////////////////////////////////////////////////
#include "UIActionLogic.h"
#include "Logic.h"
#include "archetype_helpers.h"
#include "entity_memory.h"
#include "logic_action.h"
#include <SFML/Window/Mouse.hpp>

using namespace magic_enum::bitwise_operators;
namespace steamrot {
/////////////////////////////////////////////////
UIActionLogic::UIActionLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void UIActionLogic::ProcessLogic() {

  // get all entity indexes with CUserInterface component
  std::set<size_t> entity_indexes =
      GenerateEntityIndexesFromComponents<CUserInterface>(
          m_scene_context.archetypes, true);

  // cycle through all the entity indexs in the archetype
  for (size_t entity_id : entity_indexes) {

    // get the CUserInterface component
    CUserInterface &ui_component = entity::memory::GetComponent<CUserInterface>(
        entity_id, m_scene_context.scene_entities);

    // Perform any aciton logic here, processing nested elements recursively
    logic::action::ProcessNestedUIActionsAndEvents(
        *ui_component.m_root_element, m_scene_context.event_handler,
        m_scene_context);
  }
}

} // namespace steamrot
