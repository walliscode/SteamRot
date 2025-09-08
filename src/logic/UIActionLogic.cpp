/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIEventLogic class.
/////////////////////////////////////////////////
#include "UIActionLogic.h"
#include "ArchetypeHelpers.h"
#include "ArchetypeManager.h"
#include "CGrimoireMachina.h"
#include "CUserInterface.h"

#include "Logic.h"
#include "UIElement.h"
#include "emp_helpers.h"
#include "event_helpers.h"
#include "events_generated.h"

#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <magic_enum/magic_enum.hpp>

#include <vector>

using namespace magic_enum::bitwise_operators;
namespace steamrot {
/////////////////////////////////////////////////
UIActionLogic::UIActionLogic(const LogicContext logic_context)
    : Logic(logic_context) {}

/////////////////////////////////////////////////
void UIActionLogic::ProcessLogic() {

  ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CUserInterface>();

  const auto it = m_logic_context.archetypes.find(archetype_id);
  // if it is not in the archetyps map, then skip
  if (it != m_logic_context.archetypes.end()) {

    // get the archetype from the map
    const Archetype &archetype = it->second;

    // cycle through all the entity indexs in the archetype
    for (size_t entity_id : archetype) {

      // get the CUserInterface component
      CUserInterface &ui_component = emp_helpers::GetComponent<CUserInterface>(
          entity_id, m_logic_context.scene_entities);

      ProcessEvents(ui_component);
    }
  }
}
/////////////////////////////////////////////////
void UIActionLogic::ProcessEvents(CUserInterface &ui_component) {
  // print out the size of the event bus for debugging if it is greater than 1
  // cycle through EventBus
  for (auto const &event : m_logic_context.event_handler.GetGlobalEventBus()) {
    // print the event type for debugging, except for EVENT_USER_INPUT
    if (event.m_event_type != EventType::EventType_EVENT_USER_INPUT) {
    }
    RecursiveProcessEvents(*ui_component.m_root_element, event);
  }
}
/////////////////////////////////////////////////
void UIActionLogic::RecursiveProcessEvents(UIElement &ui_element,
                                           const EventPacket &event) {

  // guard statement to check if the event is a trigger event for the ui
  // element

  // check children
}

// If the event is a user in0.1f, process it
// Process relevant events

/////////////////////////////////////////////////
void UIActionLogic::ProcessMouseEvents(UIElement &element,
                                       UserInputBitset &user_input) {

  // check if the mouse is over the element
  if (element.is_mouse_over) {

    // default to no events matching
    bool events_match = false;

    // check that the element data is populated
  }
}

/////////////////////////////////////////////////
bool UIActionLogic::LocalUIActions(UIElement &element) {
  // Check if the element has a local action
  bool has_local_action{true};

  return has_local_action;
}

/////////////////////////////////////////////////
std::vector<std::string> UIActionLogic::GetAvailableFragments() {

  // Create a vector to hold the names of available fragments
  std::vector<std::string> available_fragments;

  // Get the archetype ID for CGrimoireMachina
  ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CGrimoireMachina>();
  // Get the archetype from the logic context
  const auto it = m_logic_context.archetypes.find(archetype_id);

  // pull just the first entity in the archetype
  if (it != m_logic_context.archetypes.end()) {
    const Archetype &archetype = it->second;

    // Get the CGrimoireMachina component from the entity
    CGrimoireMachina &grimoire_component =
        emp_helpers::GetComponent<CGrimoireMachina>(
            archetype_id[0], m_logic_context.scene_entities);

    // Get the available fragments from the CGrimoireMachina component
    for (auto &fragmet : grimoire_component.m_all_fragments) {
      // Add the fragment name to the vector
      available_fragments.push_back(fragmet.first);
    }
  } else {
    std::cout << "No CGrimoireMachina found in the current scene." << std::endl;
  }
  return available_fragments;
}
/////////////////////////////////////////////////
void UIActionLogic::ToggleDropDown(UIElement &element) {};
} // namespace steamrot
