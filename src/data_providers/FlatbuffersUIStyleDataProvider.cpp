/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the StylesConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersUIStyleDataProvider.h"
#include "FailInfo.h"
#include "FlatbuffersDataLoader.h"
#include "UIStyle.h"
#include "types_generated.h"
#include <expected>
#include <vector>

namespace steamrot {

namespace {
/////////////////////////////////////////////////
std::expected<sf::Color, FailInfo> ToColor(const ColorData *color_fb,
                                            const std::string &context) {

  if (!color_fb)
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    context + " ColorData is null"});

  sf::Color color;
  color.r = color_fb->r();
  color.g = color_fb->g();
  color.b = color_fb->b();
  color.a = color_fb->a();
  return color;
}
/////////////////////////////////////////////////
std::expected<sf::Vector2f, FailInfo> ToVec2f(const Vector2fData *vec_fb,
                                               const std::string &context) {
  if (!vec_fb)
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    context + " Vector2fData is null"});

  sf::Vector2f vec;
  vec.x = vec_fb->x();
  vec.y = vec_fb->y();
  return vec;
}
} // namespace

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

  auto bg_color_result = ToColor(style_fb->background_color(),
                                  style_name + ".background_color");
  if (!bg_color_result.has_value())
    return std::unexpected(bg_color_result.error());

  style.background_color = bg_color_result.value();

  if (!style_fb->border_color())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    style_name + ".border_color missing"});

  auto border_color_result = ToColor(style_fb->border_color(),
                                      style_name + ".border_color");
  if (!border_color_result.has_value())
    return std::unexpected(border_color_result.error());
  style.border_color = border_color_result.value();

  if (!style_fb->border_thickness()) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    style_name + ".border_thickness missing"});
  } else {
    style.border_thickness = style_fb->border_thickness();
  }

  if (!style_fb->radius_resolution())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    style_name + ".radius_resolution missing"});
  style.radius_resolution = style_fb->radius_resolution();

  if (!style_fb->inner_margin())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    style_name + ".inner_margin missing"});

  auto inner_margin_result = ToVec2f(style_fb->inner_margin(),
                                      style_name + ".inner_margin");
  if (!inner_margin_result.has_value())
    return std::unexpected(inner_margin_result.error());
  style.inner_margin = inner_margin_result.value();

  if (!style_fb->minimum_size())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    style_name + ".minimum_size missing"});

  auto min_size_result = ToVec2f(style_fb->minimum_size(),
                                  style_name + ".minimum_size");
  if (!min_size_result.has_value())
    return std::unexpected(min_size_result.error());
  style.minimum_size = min_size_result.value();

  if (!style_fb->maximum_size())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    style_name + ".maximum_size missing"});

  auto max_size_result = ToVec2f(style_fb->maximum_size(),
                                  style_name + ".maximum_size");
  if (!max_size_result.has_value())
    return std::unexpected(max_size_result.error());
  style.maximum_size = max_size_result.value();

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

  if (!button_fb->font_size())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "button_style.font_size missing or set to 0"});

  auto text_color_result = ToColor(button_fb->text_color(),
                                    "button_style.text_color");
  if (!text_color_result.has_value())
    return std::unexpected(text_color_result.error());
  button_style.text_color = text_color_result.value();

  auto hover_color_result = ToColor(button_fb->hover_color(),
                                     "button_style.hover_color");
  if (!hover_color_result.has_value())
    return std::unexpected(hover_color_result.error());
  button_style.hover_color = hover_color_result.value();

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

  auto text_color_result = ToColor(dd_list_fb->text_color(),
                                    "drop_down_list_style.text_color");
  if (!text_color_result.has_value())
    return std::unexpected(text_color_result.error());
  dd_list_style.text_color = text_color_result.value();

  auto hover_color_result = ToColor(dd_list_fb->hover_color(),
                                     "drop_down_list_style.hover_color");
  if (!hover_color_result.has_value())
    return std::unexpected(hover_color_result.error());
  dd_list_style.hover_color = hover_color_result.value();

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

  auto text_color_result = ToColor(dd_item_fb->text_color(),
                                    "drop_down_item_style.text_color");
  if (!text_color_result.has_value())
    return std::unexpected(text_color_result.error());
  dd_item_style.text_color = text_color_result.value();

  auto hover_color_result = ToColor(dd_item_fb->hover_color(),
                                     "drop_down_item_style.hover_color");
  if (!hover_color_result.has_value())
    return std::unexpected(hover_color_result.error());
  dd_item_style.hover_color = hover_color_result.value();

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

  auto triangle_color_result = ToColor(dd_button_fb->triangle_color(),
                                        "drop_down_button_style.triangle_color");
  if (!triangle_color_result.has_value())
    return std::unexpected(triangle_color_result.error());
  dd_button_style.triangle_color = triangle_color_result.value();

  auto hover_color_result = ToColor(dd_button_fb->hover_color(),
                                     "drop_down_button_style.hover_color");
  if (!hover_color_result.has_value())
    return std::unexpected(hover_color_result.error());
  dd_button_style.hover_color = hover_color_result.value();

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::vector<UIStyle>, FailInfo>
FlatbuffersUIStyleDataProvider::ProvideUIStyles() {

  // Create FlatbuffersDataLoader
  FlatbuffersDataLoader data_loader;

  // Get UIStyle data
  auto get_data_result = data_loader.ProvideUIStylesData();
  if (!get_data_result)
    return std::unexpected(get_data_result.error());
  auto all_ui_styles = get_data_result.value();

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

  return ui_styles;
}

} // namespace steamrot
