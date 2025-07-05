/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIEventLogic class.
/////////////////////////////////////////////////
#include "UIActionLogic.h"
#include "ArchetypeHelpers.h"
#include "ArchetypeManager.h"
#include "BaseLogic.h"
#include "CGrimoireMachina.h"
#include "CUserInterface.h"
#include "DropDown.h"
#include "EntityHelpers.h"
#include "UIElement.h"
#include "UIElementFactory.h"
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
    : BaseLogic(logic_context) {}

/////////////////////////////////////////////////
void UIActionLogic::ProcessLogic() {

  ArchetypeID archetype_id = GenerateArchetypeIDfromTypes<CUserInterface>();

  // if it is not in the archetyps map, then skip
  if (m_logic_context.archetypes.contains(archetype_id)) {

    // get the archetype from the map
    Archetype &archetype = m_logic_context.archetypes[archetype_id];

    // cycle through all the entity indexs in the archetype
    for (size_t entity_id : archetype) {

      // get the CUserInterface component
      CUserInterface &ui_component = GetComponent<CUserInterface>(
          entity_id, m_logic_context.scene_entities);

      ProcessEvents(ui_component);
    }
  }
}
/////////////////////////////////////////////////
void UIActionLogic::ProcessEvents(CUserInterface &ui_component) {

  RecursiveProcessEvents(ui_component.m_root_element);
}
/////////////////////////////////////////////////
void UIActionLogic::RecursiveProcessEvents(UIElement &ui_element) {

  // cycle through EventBus
  for (auto const &event : m_logic_context.event_handler.GetEventBus()) {

    // guard statement to check if the event is a trigger event for the ui
    // element
    if (ui_element.trigger_event != event.m_event_type) {
      // check children
      for (UIElement &child : ui_element.child_elements) {
        // if the child element has the same trigger event, process it
        if (child.trigger_event == event.m_event_type) {
          RecursiveProcessEvents(child);
        }
      }
      // if no child element has the same trigger event, skip to next event
      continue;
    }

    // If the event is a user input event, process it
    // Process relevant events
    switch (event.m_event_type) {

    case (EventType::EventType_EVENT_USER_INPUT): {

      // check if any data is present
      if (event.m_event_data) {

        // Check if the event data is of type UserInputBitset
        if (std::holds_alternative<UserInputBitset>(
                event.m_event_data.value())) {
          UserInputBitset user_input_data =
              std::get<UserInputBitset>(event.m_event_data.value());

          // call functions which required user input data
          ProcessMouseEvents(ui_element, user_input_data);
        }
      }
    }
    default:
      break;
    }
  }
}

/////////////////////////////////////////////////
void UIActionLogic::ProcessMouseEvents(UIElement &element,
                                       UserInputBitset &user_input) {
  {

    // Process mouse events for the current element
    if (element.mouse_over) {

      // default to no matching data
      bool input_matches = false;

      // check that the element data is populated
      if (element.trigger_event_data.has_value()) {

        // get the trigger event data
        auto &trigger_event_data = element.trigger_event_data.value();

        // check that the trigger event data is of type UserInputBitset
        if (std::holds_alternative<UserInputBitset>(trigger_event_data)) {
          UserInputBitset trigger_data =
              std::get<UserInputBitset>(trigger_event_data);

          // check that they match
          input_matches = (trigger_data & user_input) != 0;
        }
      }
      if (input_matches) {
        // match LogicAction to the element action
        // we want to overwrite the current logic action as we only want one
        // action per tick
        std::cout << "Mouse event triggered for element: "
                  << magic_enum::enum_name(element.trigger_event) << std::endl;

        // pass through local action processing first (e.g. stays at Element
        // level)
        bool is_local_action = LocalUIActions(element);
      }
    }
  }

  /////////////////////////////////////////////////
  bool UIActionLogic::LocalUIActions(UIElement & element) {
    // Check if the element has a local action
    bool has_local_action{true};

    // create std::visit lamda function to handle different variant types in
    // element.element_type
    auto handle_element_type = [&](const auto &element_type) {
      using ElementType = std::decay_t<decltype(element_type)>;
      if constexpr (std::is_same_v<ElementType, DropDownContainer>) {
        // Handle DropDown specific logic
        HandleDropDownContainerActions(element);

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

    // Get ArcetypeID for CGrimoireMachina and resultant Archetype
    Archetype grimoire_id =
        m_logic_context
            .archetypes[GenerateArchetypeIDfromTypes<CGrimoireMachina>()];

    // pull just the first entity in the archetype
    if (!grimoire_id.empty()) {

      // Get the CGrimoireMachina component from the entity
      CGrimoireMachina &grimoire_component = GetComponent<CGrimoireMachina>(
          grimoire_id[0], m_logic_context.scene_entities);

      // Get the available fragments from the CGrimoireMachina component
      for (auto &fragmet : grimoire_component.m_all_fragments) {
        // Add the fragment name to the vector
        available_fragments.push_back(fragmet.first);
      }
    } else {
      std::cout << "No CGrimoireMachina found in the current scene."
                << std::endl;
    }
    return available_fragments;
  }
  /////////////////////////////////////////////////
  void UIActionLogic::HandleDropDownContainerActions(UIElement & element) {
    DropDownContainer &drop_down_container =
        std::get<DropDownContainer>(element.element_type);

    switch (element.action) {
    case ActionNames::ActionNames_ACTION_TOGGLE_DROP_DOWN: {
      drop_down_container.is_expanded = !drop_down_container.is_expanded;

      auto toggle_child_elements = [&](UIElement &child) {
        std::visit(
            [&](auto &actual_elem_type) {
              using T = std::decay_t<decltype(actual_elem_type)>;
              if constexpr (std::is_same_v<T, DropDownList>) {

                actual_elem_type.is_expanded = !actual_elem_type.is_expanded;
                child.children_active = actual_elem_type.is_expanded;

                // if the drop down is expanded, we want to populate the list
                if (actual_elem_type.is_expanded) {

                  // create instance of the UIElementFactory
                  UIElementFactory ui_element_factory;

                  // create a vector of available fragment names and populate
                  std::vector<std::string> available_fragments;

                  switch (actual_elem_type.data_populate_function) {
                  case DataPopulateFunction_PopulateWithFragmentData: {
                    std::cout << "Populating DropDownList with fragment data."
                              << std::endl;
                    available_fragments = GetAvailableFragments();
                  }

                  default: {
                    break;
                  }
                  }

                  // Create new DropDownItem for each available fragment
                  for (const std::string &fragment_name : available_fragments) {
                    // Create a new UIElement for the DropDownItem
                    UIElement drop_down_item_element =
                        ui_element_factory.CreateDropDownItem();
                    // Configure the DropDownItem properties
                    drop_down_item_element.element_type =
                        DropDownItem{fragment_name};
                    // add the DropDownItem to the child elements
                    child.child_elements.push_back(drop_down_item_element);
                  }
                  // } else {
                  //   // Clear the dropdown list if it is not expanded
                  //   child.child_elements.clear();
                } else {
                  // Clear the dropdown list if it is not expanded
                  child.child_elements.clear();
                }

              } else if constexpr (std::is_same_v<T, DropDownButton>) {
                actual_elem_type.is_expanded = !actual_elem_type.is_expanded;

              } else {
                std::cout << "Unknown element type in DropDownContainer: "
                          << magic_enum::enum_name(child.action) << std::endl;
              }
            },
            child.element_type);
      };

      for (UIElement &child : element.child_elements) {
        toggle_child_elements(child);
      }
      break;
    }
    default:
      break;
    }
  }

} // namespace steamrot
