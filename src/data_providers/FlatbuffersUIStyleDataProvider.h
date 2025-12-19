/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the StylesConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

#include "FailInfo.h"
#include "IUIStyleDataProvider.h"
#include "UIStyle.h"
#include "ui_style_generated.h"
#include <expected>
#include <vector>
namespace steamrot {

/////////////////////////////////////////////////
/// @class StylesConfigurator
/// @brief Responsible for providing UI styles configured from external data
///
/////////////////////////////////////////////////
class FlatbuffersUIStyleDataProvider : public IUIStyleDataProvider {

public:
  FlatbuffersUIStyleDataProvider(
      const std::unordered_map<std::string, std::shared_ptr<const sf::Font>>
          &fonts_map);

  std::expected<std::vector<UIStyle>, FailInfo> ProvideUIStyles() override;

  /////////////////////////////////////////////////
  /// @brief Validates and configures base Style properties from FlatBuffers
  /// data
  ///
  /// @param style_fb FlatBuffers StyleData to read from
  /// @param style Style object to configure
  /// @param style_name Name of the style type (for error messages)
  /// @return monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureBaseStyle(const StyleData *style_fb, Style &style,
                     const std::string &style_name);

  /////////////////////////////////////////////////
  /// @brief Configures PanelStyle from FlatBuffers data
  ///
  /// @param panel_fb FlatBuffers PanelStyleData to read from
  /// @param panel_style PanelStyle object to configure
  /// @return monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigurePanelStyle(const PanelStyleData *panel_fb, PanelStyle &panel_style);

  /////////////////////////////////////////////////
  /// @brief Configures ButtonStyle from FlatBuffers data
  ///
  /// @param button_fb FlatBuffers ButtonStyleData to read from
  /// @param button_style ButtonStyle object to configure
  /// @return monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureButtonStyle(const ButtonStyleData *button_fb,
                       ButtonStyle &button_style);

  /////////////////////////////////////////////////
  /// @brief Configures DropDownContainerStyle from FlatBuffers data
  ///
  /// @param dd_container_fb FlatBuffers DropDownContainerStyleData to read from
  /// @param dd_container_style DropDownContainerStyle object to configure
  /// @return monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo> ConfigureDropDownContainerStyle(
      const DropDownContainerStyleData *dd_container_fb,
      DropDownContainerStyle &dd_container_style);

  /////////////////////////////////////////////////
  /// @brief Configures DropDownListStyle from FlatBuffers data
  ///
  /// @param dd_list_fb FlatBuffers DropDownListStyleData to read from
  /// @param dd_list_style DropDownListStyle object to configure
  /// @return monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureDropDownListStyle(const DropDownListStyleData *dd_list_fb,
                             DropDownListStyle &dd_list_style);

  /////////////////////////////////////////////////
  /// @brief Configures DropDownItemStyle from FlatBuffers data
  ///
  /// @param dd_item_fb FlatBuffers DropDownItemStyleData to read from
  /// @param dd_item_style DropDownItemStyle object to configure
  /// @return monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureDropDownItemStyle(const DropDownItemStyleData *dd_item_fb,
                             DropDownItemStyle &dd_item_style);

  /////////////////////////////////////////////////
  /// @brief Configures DropDownButtonStyle from FlatBuffers data
  ///
  /// @param dd_button_fb FlatBuffers DropDownButtonStyleData to read from
  /// @param dd_button_style DropDownButtonStyle object to configure
  /// @return monostate on success, FailInfo on error
  /////////////////////////////////////////////////
  std::expected<std::monostate, FailInfo>
  ConfigureDropDownButtonStyle(const DropDownButtonStyleData *dd_button_fb,
                               DropDownButtonStyle &dd_button_style);
};
} // namespace steamrot
