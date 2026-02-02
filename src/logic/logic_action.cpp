/////////////////////////////////////////////////
/// @file
/// @brief Implementation of action handling functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "logic_action.h"
#include "DropDownItemElement.h"
#include "archetypes.h"
#include "entity_memory.h"
#include "logic_ui.h"
#include <iostream>

namespace steamrot {
namespace logic {
namespace action {

/////////////////////////////////////////////////
void ProcessUIActionsAndEvents(UIElement &ui_element,
                               EventHandler &event_handler,
                               const SceneContext &scene_context) {

  // check the subscription first
  if (!ui_element.subscription) {
    return;
  }

  // if there is a subscription, then it must be active
  if (!ui_element.subscription->m_active) {
    return;
  }

  // Only debug when subscription is active (which means user interaction occurred)
  std::cout << "[DEBUG QUIT] ProcessUIActionsAndEvents: Subscription is ACTIVE for event type: "
            << EnumNameEventType(ui_element.subscription->m_trigger_event_type)
            << std::endl;

  // use a dynamic cast to determine the type of UIElement
  if (ButtonElement *button_element =
          dynamic_cast<ButtonElement *>(&ui_element)) {

    std::cout << "[DEBUG QUIT] Element is a ButtonElement, calling ProcessButtonElementActions"
              << std::endl;
    ProcessButtonElementActions(*button_element, event_handler);
  } else if (DropDownListElement *dropdown_list_element =
                 dynamic_cast<DropDownListElement *>(&ui_element)) {
    ProcessDropDownListElementActions(*dropdown_list_element, scene_context);
  }

  // FINALLY set the subscriber to inactive
  ui_element.subscription->m_active = false;
}

/////////////////////////////////////////////////
void ProcessNestedUIActionsAndEvents(UIElement &ui_element,
                                     EventHandler &event_handler,
                                     const SceneContext &scene_context) {
  // bool to keep track if any child was processed
  bool child_processed = false;

  // cycle through all child elements and process recursively
  for (auto &child : ui_element.child_elements) {

    // Check if this child has an active subscription before processing
    bool child_has_active_subscription =
        child->subscription && child->subscription->m_active;

    // go as deep as possible first, this will stop when no children are
    // detected
    ProcessNestedUIActionsAndEvents(*child, event_handler, scene_context);

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
    ProcessUIActionsAndEvents(ui_element, event_handler, scene_context);
  }
}

/////////////////////////////////////////////////
void ProcessButtonElementActions(ButtonElement &button_element,
                                 EventHandler &event_handler) {

  // for now, all buttons need a mouse over to be clicked, so this will be the
  // top level flow control
  
  if (button_element.is_mouse_over) {
    // check if button has an event packet. for now, all event packets are sent
    // to the global event bus
    if (button_element.response_event.has_value()) {
      const auto& event_type = button_element.response_event.value().event_type;
      
      // Only debug for QUIT_GAME events
      if (event_type == EventType::QUIT_GAME) {
        std::cout << "[DEBUG QUIT] Mouse is over QUIT button!" << std::endl;
        std::cout << "[DEBUG QUIT] Button has QUIT_GAME response_event!" << std::endl;
        std::cout << "[DEBUG QUIT] Adding QUIT_GAME event to EventHandler" << std::endl;
      }
      
      event_handler.AddEvent(button_element.response_event.value());
    }
  }
}

/////////////////////////////////////////////////
void ProcessDropDownListElementActions(
    DropDownListElement &dropdown_list_element,
    const SceneContext &scene_context) {

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
        archetypes::GenerateArchetypeIDfromTypes<CGrimoireMachina>();

    const auto it = scene_context.archetypes.find(grimoire_archetype_id);
    if (it != scene_context.archetypes.end()) {
      const Archetype &archetype = it->second;

      // Get the first entity with CGrimoireMachina (should only be one)
      if (!archetype.empty()) {
        size_t entity_id = *archetype.begin();
        const CGrimoireMachina &grimoire_machina =
            entity::memory::GetComponent<CGrimoireMachina>(
                entity_id, scene_context.scene_entities);

        // Get all fragment names
        std::vector<std::string> fragment_names =
            logic::ui::GetAllFragmentNames(grimoire_machina);

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
        archetypes::GenerateArchetypeIDfromTypes<CGrimoireMachina>();

    const auto it = scene_context.archetypes.find(grimoire_archetype_id);
    if (it != scene_context.archetypes.end()) {
      const Archetype &archetype = it->second;

      // Get the first entity with CGrimoireMachina (should only be one)
      if (!archetype.empty()) {
        size_t entity_id = *archetype.begin();
        const CGrimoireMachina &grimoire_machina =
            entity::memory::GetComponent<CGrimoireMachina>(
                entity_id, scene_context.scene_entities);

        // Get all joint names
        std::vector<std::string> joint_names =
            logic::ui::GetAllJointNames(grimoire_machina);

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

} // namespace action
} // namespace logic
} // namespace steamrot
