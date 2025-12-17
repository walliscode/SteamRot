////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of StylesConfigurator wrapper class
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "StylesConfigurator.h"
#include "FlatbuffersDataLoader.h"
#include "FlatbuffersUIStyleDataProvider.h"
#include "types_generated.h"

namespace steamrot {

////////////////////////////////////////////////////////////
std::expected<UIStyle, FailInfo>
StylesConfigurator::ConfigureStyle(const UIStyleData &style_data,
                                   const AssetManager &asset_manager) {

  // Get fonts map from AssetManager
  const auto &fonts_map = asset_manager.GetFonts();

  // Create FlatbuffersUIStyleDataProvider with fonts
  FlatbuffersUIStyleDataProvider provider(fonts_map);

  // Create a UIStyle to configure
  UIStyle ui_style;

  // Handle name
  if (!style_data.name())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "style_data.name missing"});
  ui_style.name = style_data.name()->str();

  // Configure PanelStyle
  auto panel_result =
      provider.ConfigurePanelStyle(style_data.panel_style(), ui_style.panel_style);
  if (!panel_result.has_value())
    return std::unexpected(panel_result.error());

  // Configure ButtonStyle
  auto button_result =
      provider.ConfigureButtonStyle(style_data.button_style(), ui_style.button_style);
  if (!button_result.has_value())
    return std::unexpected(button_result.error());

  // Configure DropDownContainerStyle
  auto dd_container_result = provider.ConfigureDropDownContainerStyle(
      style_data.drop_down_container_style(), ui_style.drop_down_container_style);
  if (!dd_container_result.has_value())
    return std::unexpected(dd_container_result.error());

  // Configure DropDownListStyle
  auto dd_list_result = provider.ConfigureDropDownListStyle(
      style_data.drop_down_list_style(), ui_style.drop_down_list_style);
  if (!dd_list_result.has_value())
    return std::unexpected(dd_list_result.error());

  // Configure DropDownItemStyle
  auto dd_item_result = provider.ConfigureDropDownItemStyle(
      style_data.drop_down_item_style(), ui_style.drop_down_item_style);
  if (!dd_item_result.has_value())
    return std::unexpected(dd_item_result.error());

  // Configure DropDownButtonStyle
  auto dd_button_result = provider.ConfigureDropDownButtonStyle(
      style_data.drop_down_button_style(), ui_style.drop_down_button_style);
  if (!dd_button_result.has_value())
    return std::unexpected(dd_button_result.error());

  return ui_style;
}

////////////////////////////////////////////////////////////
std::expected<std::unordered_map<std::string, UIStyle>, FailInfo>
StylesConfigurator::ProvideUIStylesMap(
    const AssetManager &asset_manager,
    const std::vector<std::string> &style_names) {

  FlatbuffersDataLoader data_loader;
  std::unordered_map<std::string, UIStyle> styles_map;

  for (const auto &style_name : style_names) {
    auto style_data_result = data_loader.ProvideUIStylesData(style_name);
    if (!style_data_result)
      return std::unexpected(style_data_result.error());

    auto ui_style_result =
        ConfigureStyle(*style_data_result.value(), asset_manager);
    if (!ui_style_result)
      return std::unexpected(ui_style_result.error());

    styles_map[style_name] = ui_style_result.value();
  }

  return styles_map;
}

} // namespace steamrot
