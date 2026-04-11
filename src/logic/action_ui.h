/////////////////////////////////////////////////
/// @file
/// @brief Delaration of action handling functions.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ButtonElement.h"
#include "DropDownButtonElement.h"
#include "DropDownContainerElement.h"
#include "DropDownItemElement.h"
#include "DropDownListElement.h"
#include "EventHandler.h"
#include "SceneContext.h"

namespace steamrot::logic::action::ui {

/////////////////////////////////////////////////
/// @brief Dispatches the variant to the correct action processing function.
///
/// @param ui_element Element to process.
/// @param event_handler Event handler to process actions with.
/// @param scene_context SceneContext containing scene entities and archetypes
/////////////////////////////////////////////////
void ProcessUIActionsAndEvents(UIElement &ui_element,
                               EventHandler &event_handler,
                               const SceneContext &scene_context);

/////////////////////////////////////////////////
/// @brief Process actions for a UI element and its nested children recursively.
///
/// This function processes UI elements in a depth-first manner, ensuring that
/// child elements are processed before their parents. If a child element is
/// processed, the parent element will not be processed to avoid overlapping
/// element actions.
///
/// @param ui_element Element to process along with its children.
/// @param event_handler Event handler to process actions with.
/// @param scene_context SceneContext containing scene entities and archetypes
/////////////////////////////////////////////////
void ProcessNestedUIActionsAndEvents(UIElement &ui_element,
                                     EventHandler &event_handler,
                                     const SceneContext &scene_context);

/////////////////////////////////////////////////
/// @brief Process actions for a ButtonElement
///
/// This function checks if the button is in a state to trigger its response
/// event. Thee subscriber will have already been checked before this function
/// is called.
///
/// @param button_element ButtonElement to process
/////////////////////////////////////////////////
void ProcessButtonElementActions(ButtonElement &button_element,
                                 EventHandler &event_handler);

/////////////////////////////////////////////////
/// @brief Process actions for a DropDownContainerElement
///
/// @param dropdown_container_element DropDownContainerElement to process
/// @param scene_context Instance of SceneContext containing scene entities and
/// archetypes
/////////////////////////////////////////////////
void ProcessDropDownContainerElementActions(
    DropDownContainerElement &dropdown_container_element,
    const SceneContext &scene_context);

/////////////////////////////////////////////////
/// @brief Process actions for a DropDownButtonElement
///
/// @param dropdown_button_element DropDownButtonElement to process
/////////////////////////////////////////////////
void ProcessDropDownButtonElementActions(
    DropDownButtonElement &dropdown_button_element);

/////////////////////////////////////////////////
/// @brief Process actions for a DropDownListElement
///
/// This function handles data population for dropdown lists based on their
/// DataPopulateFunction enum value.
///
/// @param dropdown_list_element DropDownListElement to process
/// @param scene_context SceneContext containing scene entities and archetypes
/////////////////////////////////////////////////
void ProcessDropDownListElementActions(
    DropDownListElement &dropdown_list_element,
    const SceneContext &scene_context);

/////////////////////////////////////////////////
/// @brief Process actions for a DropDownItemElement.
///
/// When the item is hovered and its subscriber is active, emits a
/// GHOST SELECT EventPacket onto the event bus carrying the item's
/// ghost_selection_tag, then deactivates the subscriber.
///
/// @param item          DropDownItemElement to process.
/// @param event_handler EventHandler used to post the GHOST event.
/////////////////////////////////////////////////
void ProcessDropDownItemElementActions(DropDownItemElement &item,
                                       EventHandler &event_handler);
} // namespace steamrot::logic::action::ui
