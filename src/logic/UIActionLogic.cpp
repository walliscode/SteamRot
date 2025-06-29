/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIEventLogic class.
/////////////////////////////////////////////////
#include "UIActionLogic.h"
#include "DropDown.h"
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
      LocalUIActions(element);
      m_logic_action = element.action;
      std::cout << "Mouse event triggered for element: "
                << magic_enum::enum_name(element.action) << std::endl;

      m_logic_data.ui_data_package = element.ui_data_package;
    }
  }
  // Recursively process child elements
  for (UIElement &child : element.child_elements) {
    RecursiveProcessMouseEvents(child);
  }
}

/////////////////////////////////////////////////
bool UIActionLogic::LocalUIActions(UIElement &element) {
  // Check if the element has a local action
  bool has_local_action{true};

  if (element.action == ActionNames::ActionNames_ACTION_TOGGLE_DROP_DOWN) {
    if constexpr (std::is_same_v<std::decay_t<decltype(element.element_type)>,
                                 DropDownContainer>) {
      DropDownContainer &dropdown_container =
          std::get<DropDownContainer>(element.element_type);

      // toggle the dropdown state and children active state
      dropdown_container.is_expanded = !dropdown_container.is_expanded;
      element.children_active = !element.children_active;

    } else {
      // if the action is not a local action, then we do not have a local action
      has_local_action = false;
    }
  }
  return has_local_action;
}
} // namespace steamrot
