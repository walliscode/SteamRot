/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the StylesConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "StylesConfigurator.h"
#include "FlatbuffersDataLoader.h"
#include "PathProvider.h"
#include "UIStyle.h"
#include "types_generated.h"
#include <expected>
#include <iostream>

namespace steamrot {
/////////////////////////////////////////////////
std::expected<std::vector<std::string>, FailInfo>
StylesConfigurator::GetAllStyleNames() {

  // get ui_styles path from path provider
  PathProvider path_provider;
  auto ui_styles_path_result = path_provider.GetUIStylesDirectory();

  if (!ui_styles_path_result) {
    return std::unexpected(ui_styles_path_result.error());
  }

  // set up vector to return
  std::vector<std::string> style_names;

  // set up file extension to look for
  std::string file_extension = ".styles.bin";

  // go through the directory and check for files with the .styles.bin extension
  for (const auto &entry :
       std::filesystem::directory_iterator(ui_styles_path_result.value())) {
    std::cout << "Found file: " << entry.path() << std::endl;
    if (entry.is_regular_file() &&
        (entry.path().string().ends_with(file_extension))) {

      std::string entry_path = entry.path().filename().string();
      // remove the suffix from the filename to get the style name
      std::string style_name =
          entry_path.substr(0, entry_path.size() - file_extension.size());
      // add the style name to the vector
      style_names.push_back(style_name);
    }
  }
  return style_names;
}

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
std::expected<UIStyle, FailInfo>
StylesConfigurator::ConfigureStyle(const UIStyleData &style_data) {
  UIStyle ui_style;

  // ----- PanelStyle -----
  if (!style_data.panel_style())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound, "panel_style missing"});
  const auto *panel_fb = style_data.panel_style()->style();
  if (!panel_fb)
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "panel_style.style missing"});
  if (!panel_fb->background_color())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "panel_style.style.background_color missing"});
  if (!panel_fb->border_color())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "panel_style.style.border_color missing"});
  if (!panel_fb->inner_margin())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "panel_style.style.inner_margin missing"});
  if (!panel_fb->minimum_size())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "panel_style.style.minimum_size missing"});
  if (!panel_fb->maximum_size())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "panel_style.style.maximum_size missing"});

  ui_style.panel_style.background_color = ToColor(panel_fb->background_color());
  ui_style.panel_style.border_color = ToColor(panel_fb->border_color());
  ui_style.panel_style.border_thickness = panel_fb->border_thickness();
  ui_style.panel_style.radius_resolution = panel_fb->radius_resolution();
  ui_style.panel_style.inner_margin = ToVec2f(panel_fb->inner_margin());
  ui_style.panel_style.minimum_size = ToVec2f(panel_fb->minimum_size());
  ui_style.panel_style.maximum_size = ToVec2f(panel_fb->maximum_size());

  // ----- ButtonStyle -----
  if (!style_data.button_style())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound, "button_style missing"});
  const auto *button_fb = style_data.button_style();
  const auto *button_style_fb = button_fb->style();
  if (!button_style_fb)
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "button_style.style missing"});
  if (!button_style_fb->background_color())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "button_style.style.background_color missing"});
  if (!button_style_fb->border_color())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "button_style.style.border_color missing"});
  if (!button_style_fb->inner_margin())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "button_style.style.inner_margin missing"});
  if (!button_style_fb->minimum_size())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "button_style.style.minimum_size missing"});
  if (!button_style_fb->maximum_size())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "button_style.style.maximum_size missing"});
  if (!button_fb->text_color())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "button_style.text_color missing"});
  if (!button_fb->hover_color())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "button_style.hover_color missing"});
  if (!button_fb->font())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "button_style.font missing"});

  ui_style.button_style.background_color =
      ToColor(button_style_fb->background_color());
  ui_style.button_style.border_color = ToColor(button_style_fb->border_color());
  ui_style.button_style.border_thickness = button_style_fb->border_thickness();
  ui_style.button_style.radius_resolution =
      button_style_fb->radius_resolution();
  ui_style.button_style.inner_margin = ToVec2f(button_style_fb->inner_margin());
  ui_style.button_style.minimum_size = ToVec2f(button_style_fb->minimum_size());
  ui_style.button_style.maximum_size = ToVec2f(button_style_fb->maximum_size());
  ui_style.button_style.text_color = ToColor(button_fb->text_color());
  ui_style.button_style.hover_color = ToColor(button_fb->hover_color());
  ui_style.button_style.font = button_fb->font()->str();
  ui_style.button_style.font_size = button_fb->font_size();

  // ----- DropDownContainerStyle -----
  if (!style_data.drop_down_container_style())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_container_style missing"});
  const auto *dd_container_fb = style_data.drop_down_container_style();
  const auto *dd_container_style_fb = dd_container_fb->style();
  if (!dd_container_style_fb)
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_container_style.style missing"});
  if (!dd_container_style_fb->background_color())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_container_style.style.background_color missing"});
  if (!dd_container_style_fb->border_color())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_container_style.style.border_color missing"});
  if (!dd_container_style_fb->inner_margin())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_container_style.style.inner_margin missing"});
  if (!dd_container_style_fb->minimum_size())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_container_style.style.minimum_size missing"});
  if (!dd_container_style_fb->maximum_size())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_container_style.style.maximum_size missing"});

  ui_style.drop_down_container_style.background_color =
      ToColor(dd_container_style_fb->background_color());
  ui_style.drop_down_container_style.border_color =
      ToColor(dd_container_style_fb->border_color());
  ui_style.drop_down_container_style.border_thickness =
      dd_container_style_fb->border_thickness();
  ui_style.drop_down_container_style.radius_resolution =
      dd_container_style_fb->radius_resolution();
  ui_style.drop_down_container_style.inner_margin =
      ToVec2f(dd_container_style_fb->inner_margin());
  ui_style.drop_down_container_style.minimum_size =
      ToVec2f(dd_container_style_fb->minimum_size());
  ui_style.drop_down_container_style.maximum_size =
      ToVec2f(dd_container_style_fb->maximum_size());
  ui_style.drop_down_container_style.drop_symbol_ratio =
      dd_container_fb->drop_symbol_ratio();

  // ----- DropDownListStyle -----
  if (!style_data.drop_down_list_style())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_list_style missing"});
  const auto *dd_list_fb = style_data.drop_down_list_style();
  const auto *dd_list_style_fb = dd_list_fb->style();
  if (!dd_list_style_fb)
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_list_style.style missing"});
  if (!dd_list_style_fb->background_color())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_list_style.style.background_color missing"});
  if (!dd_list_style_fb->border_color())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_list_style.style.border_color missing"});
  if (!dd_list_style_fb->inner_margin())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_list_style.style.inner_margin missing"});
  if (!dd_list_style_fb->minimum_size())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_list_style.style.minimum_size missing"});
  if (!dd_list_style_fb->maximum_size())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_list_style.style.maximum_size missing"});
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

  ui_style.drop_down_list_style.background_color =
      ToColor(dd_list_style_fb->background_color());
  ui_style.drop_down_list_style.border_color =
      ToColor(dd_list_style_fb->border_color());
  ui_style.drop_down_list_style.border_thickness =
      dd_list_style_fb->border_thickness();
  ui_style.drop_down_list_style.radius_resolution =
      dd_list_style_fb->radius_resolution();
  ui_style.drop_down_list_style.inner_margin =
      ToVec2f(dd_list_style_fb->inner_margin());
  ui_style.drop_down_list_style.minimum_size =
      ToVec2f(dd_list_style_fb->minimum_size());
  ui_style.drop_down_list_style.maximum_size =
      ToVec2f(dd_list_style_fb->maximum_size());
  ui_style.drop_down_list_style.text_color = ToColor(dd_list_fb->text_color());
  ui_style.drop_down_list_style.hover_color =
      ToColor(dd_list_fb->hover_color());
  ui_style.drop_down_list_style.font = dd_list_fb->font()->str();
  ui_style.drop_down_list_style.font_size = dd_list_fb->font_size();

  // ----- DropDownItemStyle -----
  if (!style_data.drop_down_item_style())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_item_style missing"});
  const auto *dd_item_fb = style_data.drop_down_item_style();
  const auto *dd_item_style_fb = dd_item_fb->style();
  if (!dd_item_style_fb)
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_item_style.style missing"});
  if (!dd_item_style_fb->background_color())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_item_style.style.background_color missing"});
  if (!dd_item_style_fb->border_color())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_item_style.style.border_color missing"});
  if (!dd_item_style_fb->inner_margin())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_item_style.style.inner_margin missing"});
  if (!dd_item_style_fb->minimum_size())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_item_style.style.minimum_size missing"});
  if (!dd_item_style_fb->maximum_size())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_item_style.style.maximum_size missing"});
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

  ui_style.drop_down_item_style.background_color =
      ToColor(dd_item_style_fb->background_color());
  ui_style.drop_down_item_style.border_color =
      ToColor(dd_item_style_fb->border_color());
  ui_style.drop_down_item_style.border_thickness =
      dd_item_style_fb->border_thickness();
  ui_style.drop_down_item_style.radius_resolution =
      dd_item_style_fb->radius_resolution();
  ui_style.drop_down_item_style.inner_margin =
      ToVec2f(dd_item_style_fb->inner_margin());
  ui_style.drop_down_item_style.minimum_size =
      ToVec2f(dd_item_style_fb->minimum_size());
  ui_style.drop_down_item_style.maximum_size =
      ToVec2f(dd_item_style_fb->maximum_size());
  ui_style.drop_down_item_style.text_color = ToColor(dd_item_fb->text_color());
  ui_style.drop_down_item_style.hover_color =
      ToColor(dd_item_fb->hover_color());
  ui_style.drop_down_item_style.font = dd_item_fb->font()->str();
  ui_style.drop_down_item_style.font_size = dd_item_fb->font_size();

  // ----- DropDownButtonStyle -----
  if (!style_data.drop_down_button_style())
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_button_style missing"});
  const auto *dd_button_fb = style_data.drop_down_button_style();
  const auto *dd_button_style_fb = dd_button_fb->style();
  if (!dd_button_style_fb)
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "drop_down_button_style.style missing"});
  if (!dd_button_style_fb->background_color())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_button_style.style.background_color missing"});
  if (!dd_button_style_fb->border_color())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_button_style.style.border_color missing"});
  if (!dd_button_style_fb->inner_margin())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_button_style.style.inner_margin missing"});
  if (!dd_button_style_fb->minimum_size())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_button_style.style.minimum_size missing"});
  if (!dd_button_style_fb->maximum_size())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_button_style.style.maximum_size missing"});
  if (!dd_button_fb->triangle_color())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_button_style.triangle_color missing"});
  if (!dd_button_fb->hover_color())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "drop_down_button_style.hover_color missing"});

  ui_style.drop_down_button_style.background_color =
      ToColor(dd_button_style_fb->background_color());
  ui_style.drop_down_button_style.border_color =
      ToColor(dd_button_style_fb->border_color());
  ui_style.drop_down_button_style.border_thickness =
      dd_button_style_fb->border_thickness();
  ui_style.drop_down_button_style.radius_resolution =
      dd_button_style_fb->radius_resolution();
  ui_style.drop_down_button_style.inner_margin =
      ToVec2f(dd_button_style_fb->inner_margin());
  ui_style.drop_down_button_style.minimum_size =
      ToVec2f(dd_button_style_fb->minimum_size());
  ui_style.drop_down_button_style.maximum_size =
      ToVec2f(dd_button_style_fb->maximum_size());
  ui_style.drop_down_button_style.triangle_color =
      ToColor(dd_button_fb->triangle_color());
  ui_style.drop_down_button_style.hover_color =
      ToColor(dd_button_fb->hover_color());

  // ----- Name -----
  if (!style_data.name())
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound, "style_data.name missing"});
  ui_style.name = style_data.name()->str();

  return ui_style;
}

/////////////////////////////////////////////////
std::expected<std::unordered_map<std::string, UIStyle>, FailInfo>
StylesConfigurator::ProvideUIStylesMap() {
  // create map to return
  std::unordered_map<std::string, UIStyle> styles_map;

  // get all available style names
  auto style_names_result = GetAllStyleNames();
  if (!style_names_result) {
    return std::unexpected(style_names_result.error());
  }
  auto style_names = style_names_result.value();

  // set up data loader
  FlatbuffersDataLoader data_loader;

  // go through all style names, load their data and configure the styles
  for (const auto &style_name : style_names) {
    auto style_data_result = data_loader.ProvideUIStylesData(style_name);
    if (!style_data_result) {
      return std::unexpected(style_data_result.error());
    }
    auto ui_style_result = ConfigureStyle(*style_data_result.value());
    if (!ui_style_result) {
      return std::unexpected(ui_style_result.error());
    }
    // add the style to the map, if duplicatae return error
    if (styles_map.find(style_name) != styles_map.end()) {
      return std::unexpected(
          FailInfo{FailMode::ParameterOutOfBounds,
                   "Duplicate style name found when creating styles map: " +
                       style_name});
    }
    styles_map[style_name] = ui_style_result.value();
  }
  return styles_map;
}
} // namespace steamrot
