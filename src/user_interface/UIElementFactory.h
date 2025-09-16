/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the UIElementFactory class
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
#include "FailInfo.h"
#include "PanelElement.h"
#include "UIElement.h"
#include "user_interface_generated.h"
#include <expected>
#include <memory>
#include <variant>

namespace steamrot {

class UIElementFactory {

public:
  /////////////////////////////////////////////////
  /// @brief Constructor taking in an EventHandler reference
  ///
  /// @param event_handler Ref
  /////////////////////////////////////////////////
  UIElementFactory() = default;
};

/////////////////////////////////////////////////
/// @brief Create a UIElement (optionally nested) from the provided flatbuffer
/// data
///
/// @param data_type The flatbuffers union type for the element
/// @param data Pointer to the root flatbuffer table of the element
/// @return A std::expected containing a unique_ptr to a UIElement, or FailInfo
/// on error
/////////////////////////////////////////////////
std::expected<std::unique_ptr<UIElement>, FailInfo>
CreateUIElement(const UIElementDataUnion &data_type, const void *data,
                EventHandler &event_handler);

/////////////////////////////////////////////////
/// @brief Configure the base properties of a UIElement from the provided
/// flatbuffers data
///
/// @param element UIElement to configure
/// @param data Flatbuffers data to configure from
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureBaseUIElement(UIElement &element, const UIElementData &data,
                       EventHandler &event_handler);

/////////////////////////////////////////////////
/// @brief Configure a Panel UIElement from the provided flatbuffers data
///
/// @param panel_element PanelElement to configure
/// @param data Flatbuffers data to configure from
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigurePanelElement(PanelElement &panel_element, const PanelData &data);

/////////////////////////////////////////////////
/// @brief Configure a Button UIElement from the provided flatbuffers data
///
/// @param button_element ButtonElement to configure
/// @param data Flatbuffers data to configure from
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureButtonElement(ButtonElement &button_element, const ButtonData &data);

/////////////////////////////////////////////////
/// @brief Configure a DropDownList UIElement from the provided flatbuffers data
///
/// @param dropdown_list_element DropDownListElement to configure
/// @param data Flatbuffers data to configure from
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureDropDownListElement(DropDownListElement &dropdown_list_element,
                             const DropDownListData &data);

/////////////////////////////////////////////////
/// @brief Configure a DropDownContainer UIElement from the provided flatbuffers
/// data
///
/// @param dropdown_container_element DropDownContainerElement to configure
/// @param data Flatbuffers data to configure from
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureDropDownContainerElement(
    DropDownContainerElement &dropdown_container_element,
    const DropDownContainerData &data);

/////////////////////////////////////////////////
/// @brief Configure a DropDownItem UIElement from the provided flatbuffers data
///
/// @param dropdown_item_element DropDownItemElement to configure
/// @param data Flatbuffers data to configure from
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureDropDownItemElement(DropDownItemElement &dropdown_item_element,
                             const DropDownItemData &data);

/////////////////////////////////////////////////
/// @brief Configure a DropDownButton UIElement from the provided flatbuffers
/// data
///
/// @param dropdown_button_element DropDownButtonElement to configure
/// @param data Flatbuffers data to configure from
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureDropDownButtonElement(DropDownButtonElement &dropdown_button_element,
                               const DropDownButtonData &data);

} // namespace steamrot
