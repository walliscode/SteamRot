////////////////////////////////////////////////////////////
/// @file
/// @brief Declaration of functions to configure UI elements
////////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
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

namespace steamrot::data::configure {

////////////////////////////////////////////////////////////
/// @brief Configure the base properties of a UIElement
///
/// @param element UIElement to configure
/// @param data FlatBuffers data to configure from
/// @param event_handler Reference to EventHandler for creating Subscribers
/// @param create_ui_element_callback Callback function to create child elements
/// @return std::expected with monostate on success, or FailInfo on error
////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureBaseUIElement(
    UIElement &element, const UIElementData &data, EventHandler &event_handler,
    std::function<std::expected<std::unique_ptr<UIElement>, FailInfo>(
        const UIElementDataUnion &, const void *)>
        create_ui_element_callback);

////////////////////////////////////////////////////////////
/// @brief Configure a Panel UIElement from FlatBuffers data
///
/// @param panel_element PanelElement to configure
/// @param data FlatBuffers data to configure from
/// @return std::expected with monostate on success, or FailInfo on error
////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigurePanelElement(PanelElement &panel_element, const PanelData &data);

////////////////////////////////////////////////////////////
/// @brief Configure a Button UIElement from FlatBuffers data
///
/// @param button_element ButtonElement to configure
/// @param data FlatBuffers data to configure from
/// @return std::expected with monostate on success, or FailInfo on error
////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureButtonElement(ButtonElement &button_element, const ButtonData &data);

////////////////////////////////////////////////////////////
/// @brief Configure a DropDownList UIElement from FlatBuffers data
///
/// @param dropdown_list_element DropDownListElement to configure
/// @param data FlatBuffers data to configure from
/// @return std::expected with monostate on success, or FailInfo on error
////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureDropDownListElement(
    DropDownListElement &dropdown_list_element, const DropDownListData &data);

////////////////////////////////////////////////////////////
/// @brief Configure a DropDownContainer UIElement from FlatBuffers data
///
/// @param dropdown_container_element DropDownContainerElement to configure
/// @param data FlatBuffers data to configure from
/// @return std::expected with monostate on success, or FailInfo on error
////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureDropDownContainerElement(
    DropDownContainerElement &dropdown_container_element,
    const DropDownContainerData &data);

////////////////////////////////////////////////////////////
/// @brief Configure a DropDownItem UIElement from FlatBuffers data
///
/// @param dropdown_item_element DropDownItemElement to configure
/// @param data FlatBuffers data to configure from
/// @return std::expected with monostate on success, or FailInfo on error
////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureDropDownItemElement(
    DropDownItemElement &dropdown_item_element, const DropDownItemData &data);

////////////////////////////////////////////////////////////
/// @brief Configure a DropDownButton UIElement from FlatBuffers data
///
/// @param dropdown_button_element DropDownButtonElement to configure
/// @param data FlatBuffers data to configure from
/// @return std::expected with monostate on success, or FailInfo on error
////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureDropDownButtonElement(
    DropDownButtonElement &dropdown_button_element,
    const DropDownButtonData &data);

////////////////////////////////////////////////////////////
/// @brief Convert FlatBuffers LayoutFbs to native Layout
///
/// @param fbs_layout FlatBuffers layout value
/// @return Native Layout enum value
////////////////////////////////////////////////////////////
Layout ConvertLayout(int8_t fbs_layout);

////////////////////////////////////////////////////////////
/// @brief Convert FlatBuffers SpacingAndSizingFbs to native SpacingAndSizing
///
/// @param fbs_spacing FlatBuffers spacing value
/// @return Native SpacingAndSizing enum value
////////////////////////////////////////////////////////////
SpacingAndSizing ConvertSpacingAndSizing(int8_t fbs_spacing);

} // namespace steamrot::data::configure
