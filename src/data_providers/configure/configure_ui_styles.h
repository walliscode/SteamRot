/////////////////////////////////////////////////
/// @file
/// @brief Declaration of functions to configure UI styles.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FailInfo.h"
#include "UIStyle.h"
#include "ui_style_generated.h"
#include <SFML/Graphics/Font.hpp>
#include <expected>
#include <memory>
#include <unordered_map>
#include <variant>

namespace steamrot::data::configure {

/////////////////////////////////////////////////
/// @brief Validates and configures base Style properties from FlatBuffers data
///
/// @param style_fb FlatBuffers StyleDataFbs to read from
/// @param style Style object to configure
/// @param style_name Name of the style type (for error messages)
/// @return monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureBaseStyle(const StyleDataFbs *style_fb, Style &style,
                   const std::string &style_name);

/////////////////////////////////////////////////
/// @brief Configures PanelStyle from FlatBuffers data
///
/// @param panel_fb FlatBuffers PanelStyleDataFbs to read from
/// @param panel_style PanelStyle object to configure
/// @return monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigurePanelStyle(const PanelStyleDataFbs *panel_fb, PanelStyle &panel_style);

/////////////////////////////////////////////////
/// @brief Configures ButtonStyle from FlatBuffers data
///
/// @param button_fb FlatBuffers ButtonStyleDataFbs to read from
/// @param button_style ButtonStyle object to configure
/// @param fonts_map Map of font names to font shared pointers
/// @return monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureButtonStyle(
    const ButtonStyleDataFbs *button_fb, ButtonStyle &button_style,
    std::unordered_map<std::string, std::shared_ptr<const sf::Font>>
        &fonts_map);

/////////////////////////////////////////////////
/// @brief Configures DropDownContainerStyle from FlatBuffers data
///
/// @param dd_container_fb FlatBuffers DropDownContainerStyleDataFbs to read from
/// @param dd_container_style DropDownContainerStyle object to configure
/// @return monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureDropDownContainerStyle(
    const DropDownContainerStyleDataFbs *dd_container_fb,
    DropDownContainerStyle &dd_container_style);

/////////////////////////////////////////////////
/// @brief Configures DropDownListStyle from FlatBuffers data
///
/// @param dd_list_fb FlatBuffers DropDownListStyleDataFbs to read from
/// @param dd_list_style DropDownListStyle object to configure
/// @param fonts_map Map of font names to font shared pointers
/// @return monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureDropDownListStyle(
    const DropDownListStyleDataFbs *dd_list_fb, DropDownListStyle &dd_list_style,
    std::unordered_map<std::string, std::shared_ptr<const sf::Font>>
        &fonts_map);

/////////////////////////////////////////////////
/// @brief Configures DropDownItemStyle from FlatBuffers data
///
/// @param dd_item_fb FlatBuffers DropDownItemStyleDataFbs to read from
/// @param dd_item_style DropDownItemStyle object to configure
/// @param fonts_map Map of font names to font shared pointers
/// @return monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureDropDownItemStyle(
    const DropDownItemStyleDataFbs *dd_item_fb, DropDownItemStyle &dd_item_style,
    std::unordered_map<std::string, std::shared_ptr<const sf::Font>>
        &fonts_map);

/////////////////////////////////////////////////
/// @brief Configures DropDownButtonStyle from FlatBuffers data
///
/// @param dd_button_fb FlatBuffers DropDownButtonStyleDataFbs to read from
/// @param dd_button_style DropDownButtonStyle object to configure
/// @return monostate on success, FailInfo on error
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureDropDownButtonStyle(const DropDownButtonStyleDataFbs *dd_button_fb,
                             DropDownButtonStyle &dd_button_style);

} // namespace steamrot::data::configure
