/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIEventLogic class.
/////////////////////////////////////////////////
#include "UIActionLogic.h"
#include "ArchetypeHelpers.h"
#include "ArchetypeManager.h"
#include "CGrimoireMachina.h"
#include "CUserInterface.h"
#include "DropDownItemElement.h"
#include "DropDownListElement.h"
#include "Logic.h"
#include "emp_helpers.h"
#include "entity_helpers.h"
#include <SFML/Window/Mouse.hpp>
#include <iostream>

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

      // Perform any aciton logic here, processing nested elements recursively
      ProcessNestedUIActionsAndEvents(*ui_component.m_root_element,
                                      m_logic_context.event_handler,
                                      m_logic_context);
    }
  }
}

/////////////////////////////////////////////////
void ProcessUIActionsAndEvents(UIElement &ui_element,
                               EventHandler &event_handler,
                               const LogicContext &logic_context) {

  // check the subscription first
  if (!ui_element.subscription) {

    return;
  }

  // if there is a subscription, then it must be active
  if (!ui_element.subscription->IsActive()) {
    return;
  }

  // use a dynamic cast to determine the type of UIElement
  if (ButtonElement *button_element =
          dynamic_cast<ButtonElement *>(&ui_element)) {
    ProcessButtonElementActions(*button_element, event_handler);
  } else if (DropDownListElement *dropdown_list_element =
                 dynamic_cast<DropDownListElement *>(&ui_element)) {
    ProcessDropDownListElementActions(*dropdown_list_element, logic_context);
  }

  // FINALLY set the subscriber to inactive
  auto set_inactive_result = ui_element.subscription->SetInactive();
}

/////////////////////////////////////////////////
void ProcessNestedUIActionsAndEvents(UIElement &ui_element,
                                     EventHandler &event_handler,
                                     const LogicContext &logic_context) {
  // bool to keep track if any child was processed
  bool child_processed = false;

  // cycle through all child elements and process recursively
  for (auto &child : ui_element.child_elements) {

    // Check if this child has an active subscription before processing
    bool child_has_active_subscription =
        child->subscription && child->subscription->IsActive();

    // go as deep as possible first, this will stop when no children are
    // detected
    ProcessNestedUIActionsAndEvents(*child, event_handler, logic_context);

    // If the child had an active subscription, it (or one of its descendants)
    // was processed
    if (child_has_active_subscription) {
      // for the parent to evaluate - child was processed
      child_processed = true;
      // if a child was processed, no need to check further children
      break;
    }
  }

  if (!child_processed) {
    // this will occur if no child was processed (or no children exist)
    ProcessUIActionsAndEvents(ui_element, event_handler, logic_context);
  }
}

/////////////////////////////////////////////////
void ProcessButtonElementActions(ButtonElement &button_element,
                                 EventHandler &event_handler) {

  // for now, all buttons need a mouse over to be clicked, so this will be the
  // top level flow control
  std::cout << "Processing Button Element Actions" << std::endl;
  if (button_element.is_mouse_over) {

    std::cout << "Button is mouse over, checking for click" << std::endl;
    // check if button has an event packet. for now, all event packets are sent
    // to the global event bus
    if (button_element.response_event.has_value())

      std::cout << "Button has response event, adding to event bus"
                << std::endl;
    event_handler.AddEvent(button_element.response_event.value());
  }
}

/////////////////////////////////////////////////
void ProcessDropDownListElementActions(
    DropDownListElement &dropdown_list_element,
    const LogicContext &logic_context) {

  // Only populate if the function is set and not None
  if (dropdown_list_element.data_populate_function ==
      DataPopulateFunction::DataPopulateFunction_None) {
    return;
  }

  // Dispatch to appropriate data population function based on enum
  switch (dropdown_list_element.data_populate_function) {
  case DataPopulateFunction::DataPopulateFunction_PopulateWithFragmentData: {
    // Find CGrimoireMachina in the scene
    ArchetypeID grimoire_archetype_id =
        GenerateArchetypeIDfromTypes<CGrimoireMachina>();

    const auto it = logic_context.archetypes.find(grimoire_archetype_id);
    if (it != logic_context.archetypes.end()) {
      const Archetype &archetype = it->second;

      // Get the first entity with CGrimoireMachina (should only be one)
      if (!archetype.empty()) {
        size_t entity_id = *archetype.begin();
        const CGrimoireMachina &grimoire_machina =
            emp_helpers::GetComponent<CGrimoireMachina>(
                entity_id, logic_context.scene_entities);

        // Get all fragment names
        std::vector<std::string> fragment_names =
            entity_helpers::GetAllFragmentNames(grimoire_machina);

        // Clear existing child elements
        dropdown_list_element.child_elements.clear();

        // Create DropDownItemElements for each fragment
        for (const std::string &fragment_name : fragment_names) {
          auto item = std::make_unique<DropDownItemElement>();
          item->label = fragment_name;
          item->value = fragment_name;
          dropdown_list_element.child_elements.push_back(std::move(item));
        }
      }
    }
    break;
  }
  case DataPopulateFunction::DataPopulateFunction_PopulateWithJointData: {
    // Find CGrimoireMachina in the scene
    ArchetypeID grimoire_archetype_id =
        GenerateArchetypeIDfromTypes<CGrimoireMachina>();

    const auto it = logic_context.archetypes.find(grimoire_archetype_id);
    if (it != logic_context.archetypes.end()) {
      const Archetype &archetype = it->second;

      // Get the first entity with CGrimoireMachina (should only be one)
      if (!archetype.empty()) {
        size_t entity_id = *archetype.begin();
        const CGrimoireMachina &grimoire_machina =
            emp_helpers::GetComponent<CGrimoireMachina>(
                entity_id, logic_context.scene_entities);

        // Get all joint names
        std::vector<std::string> joint_names =
            entity_helpers::GetAllJointNames(grimoire_machina);

        // Clear existing child elements
        dropdown_list_element.child_elements.clear();

        // Create DropDownItemElements for each joint
        for (const std::string &joint_name : joint_names) {
          auto item = std::make_unique<DropDownItemElement>();
          item->label = joint_name;
          item->value = joint_name;
          dropdown_list_element.child_elements.push_back(std::move(item));
        }
      }
    }
    break;
  }
  case DataPopulateFunction::DataPopulateFunction_None:
    // Already handled above
    break;
  default:
    std::cout << "Warning: Unhandled DataPopulateFunction value: "
              << static_cast<int>(dropdown_list_element.data_populate_function)
              << std::endl;
    break;
  }
}

} // namespace steamrot
