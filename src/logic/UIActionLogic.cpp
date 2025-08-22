/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIEventLogic class.
/////////////////////////////////////////////////
#include "UIActionLogic.h"
#include "ArchetypeHelpers.h"
#include "ArchetypeManager.h"
#include "CGrimoireMachina.h"
#include "CUserInterface.h"
#include "DropDown.h"
#include "Logic.h"
#include "UIElement.h"
#include "UIElementFactory.h"
#include "emp_helpers.h"
#include "event_helpers.h"
#include "events_generated.h"
#include "user_interface_generated.h"

#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <variant>
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
    RecursiveProcessEvents(ui_component.m_root_element, event);
  }
}
/////////////////////////////////////////////////
void UIActionLogic::RecursiveProcessEvents(UIElement &ui_element,
                                           const EventPacket &event) {

  // guard statement to check if the event is a trigger event for the ui
  // element

  if (ui_element.trigger_event != event.m_event_type) {
    // check children
    for (UIElement &child : ui_element.child_elements) {
      RecursiveProcessEvents(child, event);
    }
    // return early if the event is not a trigger event for the ui element
    return;
  }

  // If the event is a user input event, process it
  // Process relevant events
  switch (event.m_event_type) {

  case (EventType::EventType_EVENT_USER_INPUT): {

    // check if any data is present
    if (std::holds_alternative<UserInputBitset>(event.m_event_data)) {

      // Check if the event data is of type UserInputBitset
      if (std::holds_alternative<UserInputBitset>(event.m_event_data)) {
        UserInputBitset user_input_data =
            std::get<UserInputBitset>(event.m_event_data);

        // call functions which required user input data
        ProcessMouseEvents(ui_element, user_input_data);
      }
    }
    break;
  }
  case (EventType::EventType_EVENT_TOGGLE_DROPDOWN): {
    // check if any data is present
    if (!std::holds_alternative<std::monostate>(event.m_event_data)) {

      // Check if the event data is of type UIElementName
      if (std::holds_alternative<UIElementName>(event.m_event_data)) {

        UIElementName element_name =
            std::get<UIElementName>(event.m_event_data);

        // check if the element name matches the current element
        if (ui_element.name == element_name) {

          // check if ui element type is a DropDownContainer
          if (std::holds_alternative<DropDownList>(ui_element.element_type) ||
              std::holds_alternative<DropDownButton>(ui_element.element_type)) {
            // call the toggle drop down function
            ToggleDropDown(ui_element);
          }
        }
      }
    }
    break;
  }

  default:
    break;
  }
}

/////////////////////////////////////////////////
void UIActionLogic::ProcessMouseEvents(UIElement &element,
                                       UserInputBitset &user_input) {

  // check if the mouse is over the element
  if (element.mouse_over) {

    // default to no events matching
    bool events_match = false;

    // check that the element data is populated
    if (!std::holds_alternative<std::monostate>(element.trigger_event_data)) {

      // get the trigger event data
      auto &trigger_event_data = element.trigger_event_data;

      // check that the trigger event data is of type UserInputBitset
      if (std::holds_alternative<UserInputBitset>(trigger_event_data)) {
        UserInputBitset trigger_data =
            std::get<UserInputBitset>(trigger_event_data);

        // check that they match
        events_match = (trigger_data & user_input) != 0;
      }
    }
    if (events_match) {
      // if events match generate a new event packet to add to the event bus
      EventPacket event_packet{2};
      event_packet.m_event_type = element.response_event;
      event_packet.m_event_data = element.response_event_data;

      // print out the EventPacket for debugging
      std::cout << "Generated EventPacket: "
                << "EventType: "
                << magic_enum::enum_name(event_packet.m_event_type)
                << ", EventData: " << event_packet.m_event_data.index()
                << std::endl;
      m_logic_context.event_handler.AddToGlobalEventBus({event_packet});
    }
  }
}

/////////////////////////////////////////////////
bool UIActionLogic::LocalUIActions(UIElement &element) {
  // Check if the element has a local action
  bool has_local_action{true};

  // create std::visit lamda function to handle different variant types in
  // element.element_type
  auto handle_element_type = [&](const auto &element_type) {
    using ElementType = std::decay_t<decltype(element_type)>;
    if constexpr (std::is_same_v<ElementType, DropDownContainer>) {
      // Handle DropDown specific logic
      ToggleDropDown(element);

    } else {
      // If the type is not recognized, we can set has_local_action to false
      has_local_action = false;
    }
  };
  // Visit the variant to handle the element type
  std::visit(handle_element_type, element.element_type);
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
void UIActionLogic::ToggleDropDown(UIElement &element) {
  auto toggle_element = [&](UIElement &element) {
    std::visit(
        [&](auto &actual_elem_type) {
          using T = std::decay_t<decltype(actual_elem_type)>;
          if constexpr (std::is_same_v<T, DropDownList>) {
            actual_elem_type.is_expanded = !actual_elem_type.is_expanded;
            element.children_active = actual_elem_type.is_expanded;

            if (actual_elem_type.is_expanded) {
              UIElementFactory ui_element_factory;
              std::vector<std::string> available_fragments;

              switch (actual_elem_type.data_populate_function) {
              case DataPopulateFunction_PopulateWithFragmentData:
                std::cout << "Populating DropDownList with fragment data."
                          << std::endl;
                available_fragments = GetAvailableFragments();
                break;
              default:
                break;
              }

              for (const std::string &fragment_name : available_fragments) {
                auto ddi_result = ui_element_factory.CreateDropDownItem();
                //[TODO:handle unexpected result]

                UIElement drop_down_item_element =
                    std::move(ddi_result.value());

                drop_down_item_element.element_type =
                    DropDownItem{fragment_name};
                element.child_elements.push_back(drop_down_item_element);
              }
            } else {
              element.child_elements.clear();
            }
          } else if constexpr (std::is_same_v<T, DropDownButton>) {
            actual_elem_type.is_expanded = !actual_elem_type.is_expanded;
          }
        },
        element.element_type);
  };

  // Call the lambda for the given element
  toggle_element(element);
}
} // namespace steamrot
