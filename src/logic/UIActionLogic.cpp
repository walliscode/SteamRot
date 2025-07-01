/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIEventLogic class.
/////////////////////////////////////////////////
#include "UIActionLogic.h"
#include "ArchetypeHelpers.h"
#include "BaseLogic.h"
#include "CUserInterface.h"
#include "DropDown.h"
#include "EntityHelpers.h"
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <magic_enum/magic_enum.hpp>

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

      ProcessMouseEvents(ui_component);
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
      std::cout << "Mouse event triggered for element: "
                << magic_enum::enum_name(element.action) << std::endl;
      LocalUIActions(element);
      m_logic_action = element.action;
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
void UIActionLogic::HandleDropDownContainerActions(UIElement &element) {
  DropDownContainer &drop_down_container =
      std::get<DropDownContainer>(element.element_type);

  switch (element.action) {
  case ActionNames::ActionNames_ACTION_TOGGLE_DROP_DOWN: {
    drop_down_container.is_expanded = !drop_down_container.is_expanded;

    auto toggle_child_elements = [](UIElement &child) {
      std::visit(
          [&](auto &actual_elem_type) {
            using T = std::decay_t<decltype(actual_elem_type)>;
            if constexpr (std::is_same_v<T, DropDownList>) {

              actual_elem_type.is_expanded = !actual_elem_type.is_expanded;
              child.children_active = actual_elem_type.is_expanded;
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
