/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIEventLogic class.
/////////////////////////////////////////////////
#include "UIEventLogic.h"
#include "EntityHelpers.h"
#include <SFML/Window/Mouse.hpp>

namespace steamrot {
/////////////////////////////////////////////////
UIEventLogic::UIEventLogic(const LogicContext logic_context)
    : Logic<CUserInterface>(logic_context) {}

/////////////////////////////////////////////////
void UIEventLogic::ProcessLogic() {

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
void UIEventLogic::ProcessMouseEvents(CUserInterface &ui_component) {}

/////////////////////////////////////////////////
void UIEventLogic::RecursiveProcessMouseEvents(UIElement &element) {
  // Process mouse events for the current element
  if (element.mouse_over) {
    // Handle mouse over logic here
  }
  // Recursively process child elements
  for (UIElement &child : element.child_elements) {
    RecursiveProcessMouseEvents(child);
  }
}
} // namespace steamrot
