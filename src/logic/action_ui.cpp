/////////////////////////////////////////////////
/// @file
/// @brief Implementation of action handling functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_ui.h"
#include "DropDownButtonElement.h"

namespace steamrot::logic::action::ui {

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

  // use a dynamic cast to determine the type of UIElement
  if (ButtonElement *button_element =
          dynamic_cast<ButtonElement *>(&ui_element)) {

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

  // add specific processing for certain element types before generic recursion
  // intercept DropDownContainerElement before generic recursion
  if (auto *container = dynamic_cast<DropDownContainerElement *>(&ui_element)) {
    ProcessDropDownContainerElementActions(*container, scene_context);
    return; // ← generic recursion never runs for this element
  }
  // bool to keep track if any child was processed
  bool child_processed = false;

  // cycle through all child elements and process recursively
  for (auto &child : ui_element.child_elements) {

    // Check if this child is ready to be processed (compound boolean)
    // This needs to be checked before the recursive call as the subscription
    // will be set  inactive after processing
    bool child_has_active_subscription = child->subscription &&
                                         child->subscription->m_active &&
                                         child->is_mouse_over;

    // go as deep as possible first, this will stop when no children are
    // detected
    ProcessNestedUIActionsAndEvents(*child, event_handler, scene_context);

    if (child_has_active_subscription) {
      // that means that UIElement was processed and we don't want to process
      // any sibling, parents or descendants
      child_processed = true;
      // if a child was processed, no need to check further children
      break;
    }
  }
  // if no child was processed, process the parent
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
    // check if button has event packets. for now, all event packets are sent
    // to the global event bus
    for (const auto &event_packet : button_element.response_events) {
      event_handler.AddEvent(event_packet);
    }
  }
}

/////////////////////////////////////////////////
void ProcessDropDownContainerElementActions(
    DropDownContainerElement &dropdown_container_element,
    const SceneContext &scene_context) {

  // pull out the DropDownButtonElement child and the DropDownListElement child,
  // if they exist. The button should be the second child and the list should be
  // the first
  auto *dropdown_button_element = dynamic_cast<DropDownButtonElement *>(
      dropdown_container_element.child_elements[1].get());
  auto *dropdown_list_element = dynamic_cast<DropDownListElement *>(
      dropdown_container_element.child_elements[0].get());

  // process the button element first, as it will control the expanded state of
  // the container and list
  ProcessDropDownButtonElementActions(*dropdown_button_element);

  // if the DropDownButtonElement child is expanded, then the container should
  // be expanded
  if (dropdown_button_element) {

    if (dropdown_list_element) {
      // if the button is expanded, the list should be expanded
      dropdown_list_element->is_expanded = dropdown_button_element->is_expanded;
    }

    // deal with parent container
    dropdown_container_element.is_expanded =
        dropdown_button_element->is_expanded;
  }
}

void ProcessDropDownButtonElementActions(
    DropDownButtonElement &dropdown_button_element) {
  // for now, the only action for a dropdown button is to expand the list, so
  // this will be the top level flow control
  if (dropdown_button_element.is_mouse_over &&
      dropdown_button_element.subscription &&
      dropdown_button_element.subscription->m_active) {
    // toggle the expanded state
    dropdown_button_element.is_expanded = !dropdown_button_element.is_expanded;
  }
}

/////////////////////////////////////////////////
void ProcessDropDownListElementActions(
    DropDownListElement &dropdown_list_element,
    const SceneContext &scene_context) {}

} // namespace steamrot::logic::action::ui
