/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the StylesConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersUIStyleDataProvider.h"
#include "FlatbuffersDataLoader.h"
#include "UIStyle.h"
#include "types_generated.h"
#include <expected>
#include <iostream>
#include <vector>

namespace steamrot {
/////////////////////////////////////////////////
static sf::Color ToColor(const ColorData *color_fb) {
  sf::Color color;
  color.r = color_fb->r();
  color.g = color_fb->g();
  color.b = color_fb->b();
  color.a = color_fb->a();
  return color;
}
/////////////////////////////////////////////////
static sf::Vector2f ToVec2f(const Vector2fData *vec_fb) {
  sf::Vector2f vec;
  vec.x = vec_fb->x();
  vec.y = vec_fb->y();
  return vec;
}

/////////////////////////////////////////////////
FlatbuffersUIStyleDataProvider::FlatbuffersUIStyleDataProvider(
    std::unordered_map<std::string, std::shared_ptr<const sf::Font>> &fonts_map)
    : IUIStyleDataProvider(fonts_map) {};

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersUIStyleDataProvider::ConfigureBaseStyle(
    const StyleData *style_fb, Style &style, const std::string &style_name) {

  if (!style_fb)
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound, style_name + " missing"});

  if (!style_fb->background_color())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    style_name + ".background_color missing"});
  style.background_color = ToColor(style_fb->background_color());

  if (!style_fb->border_color())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    style_name + ".border_color missing"});
  style.border_color = ToColor(style_fb->border_color());

  style.border_thickness = style_fb->border_thickness();

  style.radius_resolution = style_fb->radius_resolution();

  if (!style_fb->inner_margin())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    style_name + ".inner_margin missing"});
  style.inner_margin = ToVec2f(style_fb->inner_margin());

  if (!style_fb->minimum_size())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    style_name + ".minimum_size missing"});
  style.minimum_size = ToVec2f(style_fb->minimum_size());

  if (!style_fb->maximum_size())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    style_name + ".maximum_size missing"});
  style.maximum_size = ToVec2f(style_fb->maximum_size());

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersUIStyleDataProvider::ConfigurePanelStyle(
    const PanelStyleData *panel_fb, PanelStyle &panel_style) {

  if (!panel_fb)
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound, "panel_style missing"});

  if (!panel_fb->style())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "panel_style.style missing"});

  const auto *panel_style_fb = panel_fb->style();
  auto base_result =
      ConfigureBaseStyle(panel_style_fb, panel_style, "panel_style.style");

  if (!base_result.has_value())
    return std::unexpected(base_result.error());

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersUIStyleDataProvider::ConfigureButtonStyle(
    const ButtonStyleData *button_fb, ButtonStyle &button_style) {

  if (!button_fb)
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound, "button_style missing"});

  if (!button_fb->style())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "button_style.style missing"});

  const auto *button_style_fb = button_fb->style();
  auto base_result =
      ConfigureBaseStyle(button_style_fb, button_style, "button_style.style");

  if (!base_result.has_value())
    return std::unexpected(base_result.error());

  if (!button_fb->text_color())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "button_style.text_color missing"});

  if (!button_fb->hover_color())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "button_style.hover_color missing"});

  if (!button_fb->font())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "button_style.font missing"});

  button_style.text_color = ToColor(button_fb->text_color());
  button_style.hover_color = ToColor(button_fb->hover_color());

  // Search for the font in the map
  auto it = fonts_map.find(button_fb->font()->str());
  if (it == fonts_map.end())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "button_style.font not found: " + button_fb->font()->str()});

  button_style.font = it->second;
  button_style.font_size = button_fb->font_size();

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersUIStyleDataProvider::ConfigureDropDownContainerStyle(
    const DropDownContainerStyleData *dd_container_fb,
    DropDownContainerStyle &dd_container_style) {

  if (!dd_container_fb)
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_container_style missing"});

  if (!dd_container_fb->style())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_container_style.style missing"});

  const auto *dd_container_style_fb = dd_container_fb->style();
  auto base_result =
      ConfigureBaseStyle(dd_container_style_fb, dd_container_style,
                         "drop_down_container_style.style");

  if (!base_result.has_value())
    return std::unexpected(base_result.error());

  dd_container_style.drop_symbol_ratio = dd_container_fb->drop_symbol_ratio();

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersUIStyleDataProvider::ConfigureDropDownListStyle(
    const DropDownListStyleData *dd_list_fb, DropDownListStyle &dd_list_style) {

  if (!dd_list_fb)
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_list_style missing"});

  if (!dd_list_fb->style())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_list_style.style missing"});

  const auto *dd_list_style_fb = dd_list_fb->style();
  auto base_result = ConfigureBaseStyle(dd_list_style_fb, dd_list_style,
                                        "drop_down_list_style.style");

  if (!base_result.has_value())
    return std::unexpected(base_result.error());

  if (!dd_list_fb->text_color())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_list_style.text_color missing"});

  if (!dd_list_fb->hover_color())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_list_style.hover_color missing"});

  if (!dd_list_fb->font())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_list_style.font missing"});

  dd_list_style.text_color = ToColor(dd_list_fb->text_color());
  dd_list_style.hover_color = ToColor(dd_list_fb->hover_color());

  auto get_dd_list_font_result = fonts_map.find(dd_list_fb->font()->str());
  if (get_dd_list_font_result == fonts_map.end())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_list_style.font not found: " +
                                        dd_list_fb->font()->str()});

  dd_list_style.font = get_dd_list_font_result->second;
  dd_list_style.font_size = dd_list_fb->font_size();

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersUIStyleDataProvider::ConfigureDropDownItemStyle(
    const DropDownItemStyleData *dd_item_fb, DropDownItemStyle &dd_item_style) {

  if (!dd_item_fb)
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_item_style missing"});

  if (!dd_item_fb->style())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_item_style.style missing"});

  const auto *dd_item_style_fb = dd_item_fb->style();
  auto base_result = ConfigureBaseStyle(dd_item_style_fb, dd_item_style,
                                        "drop_down_item_style.style");

  if (!base_result.has_value())
    return std::unexpected(base_result.error());

  if (!dd_item_fb->text_color())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_item_style.text_color missing"});

  if (!dd_item_fb->hover_color())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_item_style.hover_color missing"});

  if (!dd_item_fb->font())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_item_style.font missing"});

  dd_item_style.text_color = ToColor(dd_item_fb->text_color());
  dd_item_style.hover_color = ToColor(dd_item_fb->hover_color());

  auto get_dd_item_font_result = fonts_map.find(dd_item_fb->font()->str());
  if (get_dd_item_font_result == fonts_map.end())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_item_style.font not found: " +
                                        dd_item_fb->font()->str()});

  dd_item_style.font = get_dd_item_font_result->second;
  dd_item_style.font_size = dd_item_fb->font_size();

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersUIStyleDataProvider::ConfigureDropDownButtonStyle(
    const DropDownButtonStyleData *dd_button_fb,
    DropDownButtonStyle &dd_button_style) {

  if (!dd_button_fb)
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_button_style missing"});

  if (!dd_button_fb->style())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_button_style.style missing"});

  const auto *dd_button_style_fb = dd_button_fb->style();
  auto base_result = ConfigureBaseStyle(dd_button_style_fb, dd_button_style,
                                        "drop_down_button_style.style");

  if (!base_result.has_value())
    return std::unexpected(base_result.error());

  if (!dd_button_fb->triangle_color())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_button_style.triangle_color missing"});

  if (!dd_button_fb->hover_color())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_button_style.hover_color missing"});

  dd_button_style.triangle_color = ToColor(dd_button_fb->triangle_color());
  dd_button_style.hover_color = ToColor(dd_button_fb->hover_color());

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::vector<UIStyle>, FailInfo>
FlatbuffersUIStyleDataProvider::ProvideUIStyles() {

  // Create FlatbuffersDataLoader
  FlatbuffersDataLoader data_loader;

  std::cout << "Providing UI styles from flatbuffers..." << std::endl;
  // Get UIStyle data
  auto get_data_result = data_loader.ProvideUIStylesData();
  if (!get_data_result)
    return std::unexpected(get_data_result.error());
  auto all_ui_styles = get_data_result.value();

  std::cout << "Configuring UI styles from flatbuffers..." << std::endl;
  // Create vector to add styles to
  std::vector<UIStyle> ui_styles;

  for (const auto style_data : all_ui_styles) {

    // Create UIStyle to fill
    UIStyle ui_style;

    // Handle name
    if (!style_data->name())
      return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                      "style_data.name missing"});
    ui_style.name = style_data->name()->str();

    // Configure PanelStyle
    auto panel_result =
        ConfigurePanelStyle(style_data->panel_style(), ui_style.panel_style);
    if (!panel_result.has_value())
      return std::unexpected(panel_result.error());

    // Configure ButtonStyle
    auto button_result =
        ConfigureButtonStyle(style_data->button_style(), ui_style.button_style);
    if (!button_result.has_value())
      return std::unexpected(button_result.error());

    // Configure DropDownContainerStyle
    if (!style_data->drop_down_container_style())
      return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                      "style_data.drop_down_container_style "
                                      "missing"});
    auto dd_container_result =
        ConfigureDropDownContainerStyle(style_data->drop_down_container_style(),
                                        ui_style.drop_down_container_style);
    if (!dd_container_result.has_value())
      return std::unexpected(dd_container_result.error());

    // Configure DropDownListStyle
    if (!style_data->drop_down_list_style())
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "style_data.drop_down_list_style missing"});
    auto dd_list_result = ConfigureDropDownListStyle(
        style_data->drop_down_list_style(), ui_style.drop_down_list_style);
    if (!dd_list_result.has_value())
      return std::unexpected(dd_list_result.error());

    // Configure DropDownItemStyle
    if (!style_data->drop_down_item_style())
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "style_data.drop_down_item_style missing"});
    auto dd_item_result = ConfigureDropDownItemStyle(
        style_data->drop_down_item_style(), ui_style.drop_down_item_style);
    if (!dd_item_result.has_value())
      return std::unexpected(dd_item_result.error());

    // Configure DropDownButtonStyle
    if (!style_data->drop_down_button_style())
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "style_data.drop_down_button_style missing"});
    auto dd_button_result = ConfigureDropDownButtonStyle(
        style_data->drop_down_button_style(), ui_style.drop_down_button_style);
    if (!dd_button_result.has_value())
      return std::unexpected(dd_button_result.error());

    ui_styles.push_back(ui_style);
  }

  std::cout << "Provided " << ui_styles.size() << " UI styles from flatbuffers."
            << std::endl;

  return ui_styles;
}

} // namespace steamrot
