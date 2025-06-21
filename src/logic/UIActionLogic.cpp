/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIEventLogic class.
/////////////////////////////////////////////////
#include "UIActionLogic.h"
#include "EntityHelpers.h"
#include <SFML/Window/Mouse.hpp>
#include <magic_enum/magic_enum.hpp>

using namespace magic_enum::bitwise_operators;
namespace steamrot {
/////////////////////////////////////////////////
UIActionLogic::UIActionLogic(const LogicContext logic_context)
    : Logic<CUserInterface>(logic_context) {}

/////////////////////////////////////////////////
void UIActionLogic::ProcessLogic() {

  // cycle through all the Archetype IDs  associated with this logic class
  for (const ArchetypeID &archetype_id : m_archetype_IDs) {

    // if it is not in the archetyps map, then skip
    if (!m_logic_context.archetypes.contains(archetype_id)) {
      continue;
    } else {
      // get the archetype from the map
      Archetype &archetype = m_logic_context.archetypes[archetype_id];

      // cycle through all the entity indexs in the archetype
      for (size_t entity_id : archetype) {

        // get the CUserInterface component
        CUserInterface &ui_component = GetComponent<CUserInterface>(
            entity_id, m_logic_context.scene_entities);

        ProcessMouseEvents(ui_component);
      }
    }
  }
}

/////////////////////////////////////////////////
void UIActionLogic::ProcessMouseEvents(CUserInterface &ui_component) {
  RecursiveProcessMouseEvents(ui_component.m_root_element);
}

/////////////////////////////////////////////////
void UIActionLogic::RecursiveProcessMouseEvents(UIElement &element) {

  // Process mouse events for the current element
  if (element.mouse_over) {

    // check if elements even has been triggered (bitwise operations)
    // print out trigger event and user events

    bool event_triggered =
        (element.trigger_event & m_logic_context.user_events) != 0;

    if (event_triggered) {
      // match LogicAction to the element action
      // we want to overwrite the current logic action as we only want one
      // action per tick
      m_logic_action = element.action;
      std::cout << "Mouse event triggered for element: "
                << magic_enum::enum_name(element.action) << std::endl;
    }
  }
  // Recursively process child elements
  for (UIElement &child : element.child_elements) {
    RecursiveProcessMouseEvents(child);
  }
}
} // namespace steamrot
