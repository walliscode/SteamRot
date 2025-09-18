/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIEventLogic class.
/////////////////////////////////////////////////
#include "UIActionLogic.h"
#include "ArchetypeHelpers.h"
#include "ArchetypeManager.h"
#include "CUserInterface.h"
#include "Logic.h"
#include "emp_helpers.h"
#include <SFML/Window/Mouse.hpp>
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
    }
  }
}

/////////////////////////////////////////////////
void ProcessUIActionsAndEvents(UIElement &ui_element,
                               EventHandler &event_handler) {

  // check the subscription first
  if (ui_element.subscription == std::nullopt)
    return;

  // if there is a subscription, then it must be active
  if (!ui_element.subscription.value()->IsActive())
    return;

  // use a dynamic cast to determine the type of UIElement
  if (ButtonElement *button_element =
          dynamic_cast<ButtonElement *>(&ui_element)) {
    ProcessButtonElementActions(*button_element, event_handler);
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
    if (button_element.response_event.has_value())

      event_handler.AddToGlobalEventBus(
          {button_element.response_event.value()});
  }
}

} // namespace steamrot
