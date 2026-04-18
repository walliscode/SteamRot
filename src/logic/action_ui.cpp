/////////////////////////////////////////////////
/// @file
/// @brief Implementation of action handling functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "action_ui.h"
#include "DataPopulationFunctions.h"
#include "DropDownButtonElement.h"
#include "DropDownItemElement.h"
#include "action_grimoire_machina.h"

namespace steamrot::logic::action::ui {

/////////////////////////////////////////////////
void ProcessUIActionsAndEvents(UIElement &ui_element,
                               EventHandler &event_handler,
                               const SceneContext &scene_context) {

  // disabled elements cannot be interacted with
  if (ui_element.is_disabled) {
    return;
  }

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

  // disabled elements cannot be interacted with; skip the entire subtree
  if (ui_element.is_disabled) {
    return;
  }

  // add specific processing for certain element types before generic recursion
  // intercept DropDownContainerElement before generic recursion
  if (auto *container = dynamic_cast<DropDownContainerElement *>(&ui_element)) {
    ProcessDropDownContainerElementActions(*container, scene_context);
    return; // ← generic recursion never runs for this element
  }
  // bool to keep track if any child was processed
  bool child_processed = false;

  // Only iterate children when they are active (visible). If children are
  // inactive, their hover state has already been cleared by CheckMouseOver so
  // we fall through to process the parent element directly.
  if (ui_element.children_active) {
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
  }
  // if no child was processed, process the parent
  if (!child_processed) {
    // this will occur if no child was processed (or no children exist), or
    // when children are inactive
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

      // store the expanded state of the list before syncing with the button
      bool was_list_expanded = dropdown_list_element->is_expanded;

      // if the button is expanded, the list should be expanded
      dropdown_list_element->is_expanded = dropdown_button_element->is_expanded;

      // call the list element actions whenever the expanded state changes
      // (populate on expand, clear on collapse)
      if (dropdown_list_element->is_expanded != was_list_expanded) {

        ProcessDropDownListElementActions(*dropdown_list_element,
                                          scene_context);
      }

      // Process DropDownItemElement clicks when the list is expanded
      bool item_selected = false;
      if (dropdown_list_element->is_expanded) {
        for (auto &child : dropdown_list_element->child_elements) {
          if (auto *item = dynamic_cast<DropDownItemElement *>(child.get())) {
            if (item->is_mouse_over && item->subscription &&
                item->subscription->m_active) {
              ProcessDropDownItemElementActions(*item,
                                                scene_context.event_handler);
              item_selected = true;
              break;
            }
          }
        }
      }

      // Collapse the dropdown after a selection so the list closes
      if (item_selected) {
        dropdown_button_element->is_expanded = false;
        dropdown_list_element->is_expanded = false;
        ProcessDropDownListElementActions(*dropdown_list_element,
                                          scene_context);
      }
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
    const SceneContext &scene_context) {

  if (!dropdown_list_element.is_expanded) {
    // list is collapsing: clear items and deactivate children
    dropdown_list_element.child_elements.clear();
    dropdown_list_element.children_active = false;
    return;
  }

  // set up any variables needed
  std::vector<std::string> fragment_names;
  std::vector<std::string> joint_names;

  // get GrimoireMachina once for both lookup and instance construction
  GrimoireMachina *grimoire_ptr =
      scene_context.asset_manager.GetGrimoireMachina().has_value()
          ? scene_context.asset_manager.GetGrimoireMachina().value()
          : nullptr;

  // switch on the DataPopulationFunction enum
  switch (dropdown_list_element.data_population_function) {

  case DataPopulationFunction::GetAllFragmentNames:
    if (grimoire_ptr)
      fragment_names =
          grimoire_machina::GetAllFragmentNames(*grimoire_ptr);

    break;

  case DataPopulationFunction::GetAllJointNames:
    if (grimoire_ptr)
      joint_names = grimoire_machina::GetAllJointNames(*grimoire_ptr);

    break;

  default:
    // do nothing as there may be functions in the enum that are not relevant to
    // this UI element
    break;
  }

  // deal with variables
  for (const auto &fragment_name : fragment_names) {
    // create a new DropDownItemElement for each fragment name and add to the
    // child elements of the list
    auto dropdown_item = std::make_unique<DropDownItemElement>();
    dropdown_item->priority = 2;
    dropdown_item->label = fragment_name;

    // store a lightweight tag; action_ghost resolves it to a FragmentInstance
    dropdown_item->ghost_selection_tag = FragmentTag{fragment_name};

    // create a subscriber so the item reacts to USER_INPUT SELECT events
    auto subscriber = std::make_shared<Subscriber>();
    subscriber->event_type = EventType::USER_INPUT;
    subscriber->filter_payload =
        InputPayload{InputPayload::InputAction::SELECT};
    // [TODO:] Sort out a way of passing the reg result
    auto reg_select_result =
        scene_context.event_handler.RegisterSubscriber(subscriber);
    dropdown_item->subscription = subscriber;

    dropdown_list_element.child_elements.push_back(std::move(dropdown_item));
  }

  for (const auto &joint_name : joint_names) {
    // create a new DropDownItemElement for each joint name and add to the
    // child elements of the list
    auto dropdown_item = std::make_unique<DropDownItemElement>();
    dropdown_item->priority = 2;
    dropdown_item->label = joint_name;

    // store a lightweight tag; action_ghost resolves it to a JointInstance
    dropdown_item->ghost_selection_tag = JointTag{joint_name};

    // create a subscriber so the item reacts to USER_INPUT SELECT events
    auto subscriber = std::make_shared<Subscriber>();
    subscriber->event_type = EventType::USER_INPUT;
    subscriber->filter_payload =
        InputPayload{InputPayload::InputAction::SELECT};
    // [TODO:] Sort out a way of passing the reg result
    auto reg_select_result =
        scene_context.event_handler.RegisterSubscriber(subscriber);
    dropdown_item->subscription = subscriber;

    dropdown_list_element.child_elements.push_back(std::move(dropdown_item));
  }

  // activate children so they are rendered
  dropdown_list_element.children_active = true;
}

/////////////////////////////////////////////////
void ProcessDropDownItemElementActions(DropDownItemElement &item,
                                       EventHandler &event_handler) {

  if (!item.is_mouse_over)
    return;

  if (!item.subscription || !item.subscription->m_active)
    return;

  // Emit a GHOST SELECT event carrying the item's ghost_selection_tag
  EventPacket packet;
  packet.type = EventType::GHOST;
  packet.payload =
      GhostPayload{GhostPayload::GhostAction::SELECT, item.ghost_selection_tag};
  event_handler.AddEvent(packet);

  item.subscription->m_active = false;
}

} // namespace steamrot::logic::action::ui
