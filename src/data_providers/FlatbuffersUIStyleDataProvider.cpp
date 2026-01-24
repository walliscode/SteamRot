/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the FlatbuffersUIStyleDataProvider class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersUIStyleDataProvider.h"
#include "FailInfo.h"
#include "FlatbuffersDataLoader.h"
#include "UIStyle.h"
#include "configure_ui_styles.h"
#include <expected>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
FlatbuffersUIStyleDataProvider::FlatbuffersUIStyleDataProvider(
    std::unordered_map<std::string, std::shared_ptr<const sf::Font>> &fonts_map)
    : IUIStyleDataProvider(fonts_map) {};

/////////////////////////////////////////////////
std::expected<std::vector<UIStyle>, FailInfo>
FlatbuffersUIStyleDataProvider::CreateUIStyles() {

  // Create return object
  std::vector<UIStyle> ui_styles;

  // Configure UI styles
  auto configure_result = ConfigureUIStyles(ui_styles);
  if (!configure_result.has_value()) {
    return std::unexpected(configure_result.error());
  }

  return ui_styles;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersUIStyleDataProvider::ConfigureUIStyles(
    std::vector<UIStyle> &ui_styles) {

  // Get UIStyle data from FlatBuffers
  auto get_data_result = m_loader.ProvideUIStylesData();
  if (!get_data_result)
    return std::unexpected(get_data_result.error());
  auto all_ui_styles = get_data_result.value();

  for (const auto style_data : all_ui_styles) {

    // Create UIStyle to fill
    UIStyle ui_style;

    // Handle name
    if (!style_data->name())
      return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                      "style_data.name missing"});
    ui_style.name = style_data->name()->str();

    // Configure PanelStyle
    auto panel_result = data::configure::ConfigurePanelStyle(
        style_data->panel_style(), ui_style.panel_style);
    if (!panel_result.has_value())
      return std::unexpected(panel_result.error());

    // Configure ButtonStyle
    auto button_result = data::configure::ConfigureButtonStyle(
        style_data->button_style(), ui_style.button_style, fonts_map);
    if (!button_result.has_value())
      return std::unexpected(button_result.error());

    // Configure DropDownContainerStyle
    if (!style_data->drop_down_container_style())
      return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                      "style_data.drop_down_container_style "
                                      "missing"});
    auto dd_container_result = data::configure::ConfigureDropDownContainerStyle(
        style_data->drop_down_container_style(),
        ui_style.drop_down_container_style);
    if (!dd_container_result.has_value())
      return std::unexpected(dd_container_result.error());

    // Configure DropDownListStyle
    if (!style_data->drop_down_list_style())
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "style_data.drop_down_list_style missing"});
    auto dd_list_result = data::configure::ConfigureDropDownListStyle(
        style_data->drop_down_list_style(), ui_style.drop_down_list_style,
        fonts_map);
    if (!dd_list_result.has_value())
      return std::unexpected(dd_list_result.error());

    // Configure DropDownItemStyle
    if (!style_data->drop_down_item_style())
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "style_data.drop_down_item_style missing"});
    auto dd_item_result = data::configure::ConfigureDropDownItemStyle(
        style_data->drop_down_item_style(), ui_style.drop_down_item_style,
        fonts_map);
    if (!dd_item_result.has_value())
      return std::unexpected(dd_item_result.error());

    // Configure DropDownButtonStyle
    if (!style_data->drop_down_button_style())
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "style_data.drop_down_button_style missing"});
    auto dd_button_result = data::configure::ConfigureDropDownButtonStyle(
        style_data->drop_down_button_style(), ui_style.drop_down_button_style);
    if (!dd_button_result.has_value())
      return std::unexpected(dd_button_result.error());

    ui_styles.push_back(ui_style);
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::vector<UIStyle>, FailInfo>
FlatbuffersUIStyleDataProvider::ProvideUIStyles() {
  // Call the new CreateUIStyles method for backward compatibility
  return CreateUIStyles();
}

} // namespace steamrot
