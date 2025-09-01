/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the StylesConfigurator class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "StylesConfigurator.h"
#include "AssetManager.h"
#include "FlatbuffersDataLoader.h"
#include "PathProvider.h"
#include <catch2/catch_test_macros.hpp>
#include <vector>

TEST_CASE("StylesConfigurator can be instantiated", "[StylesConfigurator]") {
  // Arrange & Act
  steamrot::StylesConfigurator stylesConfigurator;

  SUCCEED("StylesConfigurator instantiated successfully");
}

TEST_CASE("StylesConfigurator ConfigureStyle returns expected result for "
          "default style (all fields checked)",
          "[StylesConfigurator][Fields]") {

  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  steamrot::StylesConfigurator styles_configurator;

  steamrot::AssetManager asset_manager{steamrot::EnvironmentType::Test};
  auto load_default_assets_result = asset_manager.LoadDefaultAssets();
  if (!load_default_assets_result)
    FAIL(load_default_assets_result.error().message);

  auto style_data_result = data_loader.ProvideUIStylesData("default");
  if (!style_data_result)
    FAIL(style_data_result.error().message);

  auto ui_style_result = styles_configurator.ConfigureStyle(
      *style_data_result.value(), asset_manager);
  if (!ui_style_result)
    FAIL(ui_style_result.error().message);

  const steamrot::UIStyleData &style_data = *style_data_result.value();
  const steamrot::UIStyle &ui_style = ui_style_result.value();

  // Helper lambda for Color comparison
  auto check_color = [](const auto &lhs, const auto &rhs) {
    REQUIRE(lhs.r == rhs->r());
    REQUIRE(lhs.g == rhs->g());
    REQUIRE(lhs.b == rhs->b());
    REQUIRE(lhs.a == rhs->a());
  };
  // Helper lambda for Vector2f comparison
  auto check_vec2f = [](const auto &lhs, const auto &rhs) {
    REQUIRE(lhs.x == (rhs->x()));
    REQUIRE(lhs.y == (rhs->y()));
  };

  // ----- PanelStyle -----
  const auto *panel_style_fb = style_data.panel_style();
  const auto &panel_style = ui_style.panel_style;
  check_color(panel_style.background_color,
              panel_style_fb->style()->background_color());
  check_color(panel_style.border_color,
              panel_style_fb->style()->border_color());
  REQUIRE(panel_style.border_thickness ==
          (panel_style_fb->style()->border_thickness()));
  REQUIRE(panel_style.radius_resolution ==
          panel_style_fb->style()->radius_resolution());
  check_vec2f(panel_style.inner_margin,
              panel_style_fb->style()->inner_margin());
  check_vec2f(panel_style.minimum_size,
              panel_style_fb->style()->minimum_size());
  check_vec2f(panel_style.maximum_size,
              panel_style_fb->style()->maximum_size());

  // ----- ButtonStyle -----
  const auto *button_style_fb = style_data.button_style();
  const auto &button_style = ui_style.button_style;
  check_color(button_style.background_color,
              button_style_fb->style()->background_color());
  check_color(button_style.border_color,
              button_style_fb->style()->border_color());
  REQUIRE(button_style.border_thickness ==
          (button_style_fb->style()->border_thickness()));
  REQUIRE(button_style.radius_resolution ==
          button_style_fb->style()->radius_resolution());
  check_vec2f(button_style.inner_margin,
              button_style_fb->style()->inner_margin());
  check_vec2f(button_style.minimum_size,
              button_style_fb->style()->minimum_size());
  check_vec2f(button_style.maximum_size,
              button_style_fb->style()->maximum_size());
  check_color(button_style.text_color, button_style_fb->text_color());
  check_color(button_style.hover_color, button_style_fb->hover_color());
  REQUIRE(button_style.font != nullptr);
  REQUIRE(button_style.font_size == button_style_fb->font_size());

  // ----- DropDownContainerStyle -----
  const auto *dd_container_style_fb = style_data.drop_down_container_style();
  const auto &dd_container_style = ui_style.drop_down_container_style;
  check_color(dd_container_style.background_color,
              dd_container_style_fb->style()->background_color());
  check_color(dd_container_style.border_color,
              dd_container_style_fb->style()->border_color());
  REQUIRE(dd_container_style.border_thickness ==
          (dd_container_style_fb->style()->border_thickness()));
  REQUIRE(dd_container_style.radius_resolution ==
          dd_container_style_fb->style()->radius_resolution());
  check_vec2f(dd_container_style.inner_margin,
              dd_container_style_fb->style()->inner_margin());
  check_vec2f(dd_container_style.minimum_size,
              dd_container_style_fb->style()->minimum_size());
  check_vec2f(dd_container_style.maximum_size,
              dd_container_style_fb->style()->maximum_size());
  REQUIRE(dd_container_style.drop_symbol_ratio ==
          (dd_container_style_fb->drop_symbol_ratio()));

  // ----- DropDownListStyle -----
  const auto *dd_list_style_fb = style_data.drop_down_list_style();
  const auto &dd_list_style = ui_style.drop_down_list_style;
  check_color(dd_list_style.background_color,
              dd_list_style_fb->style()->background_color());
  check_color(dd_list_style.border_color,
              dd_list_style_fb->style()->border_color());
  REQUIRE(dd_list_style.border_thickness ==
          (dd_list_style_fb->style()->border_thickness()));
  REQUIRE(dd_list_style.radius_resolution ==
          dd_list_style_fb->style()->radius_resolution());
  check_vec2f(dd_list_style.inner_margin,
              dd_list_style_fb->style()->inner_margin());
  check_vec2f(dd_list_style.minimum_size,
              dd_list_style_fb->style()->minimum_size());
  check_vec2f(dd_list_style.maximum_size,
              dd_list_style_fb->style()->maximum_size());
  check_color(dd_list_style.text_color, dd_list_style_fb->text_color());
  check_color(dd_list_style.hover_color, dd_list_style_fb->hover_color());
  REQUIRE(dd_list_style.font == dd_list_style_fb->font()->str());
  REQUIRE(dd_list_style.font_size == dd_list_style_fb->font_size());

  // ----- DropDownItemStyle -----
  const auto *dd_item_style_fb = style_data.drop_down_item_style();
  const auto &dd_item_style = ui_style.drop_down_item_style;
  check_color(dd_item_style.background_color,
              dd_item_style_fb->style()->background_color());
  check_color(dd_item_style.border_color,
              dd_item_style_fb->style()->border_color());
  REQUIRE(dd_item_style.border_thickness ==
          (dd_item_style_fb->style()->border_thickness()));
  REQUIRE(dd_item_style.radius_resolution ==
          dd_item_style_fb->style()->radius_resolution());
  check_vec2f(dd_item_style.inner_margin,
              dd_item_style_fb->style()->inner_margin());
  check_vec2f(dd_item_style.minimum_size,
              dd_item_style_fb->style()->minimum_size());
  check_vec2f(dd_item_style.maximum_size,
              dd_item_style_fb->style()->maximum_size());
  check_color(dd_item_style.text_color, dd_item_style_fb->text_color());
  check_color(dd_item_style.hover_color, dd_item_style_fb->hover_color());
  REQUIRE(dd_item_style.font == dd_item_style_fb->font()->str());
  REQUIRE(dd_item_style.font_size == dd_item_style_fb->font_size());

  // ----- DropDownButtonStyle -----
  const auto *dd_button_style_fb = style_data.drop_down_button_style();
  const auto &dd_button_style = ui_style.drop_down_button_style;
  check_color(dd_button_style.background_color,
              dd_button_style_fb->style()->background_color());
  check_color(dd_button_style.border_color,
              dd_button_style_fb->style()->border_color());
  REQUIRE(dd_button_style.border_thickness ==
          (dd_button_style_fb->style()->border_thickness()));
  REQUIRE(dd_button_style.radius_resolution ==
          dd_button_style_fb->style()->radius_resolution());
  check_vec2f(dd_button_style.inner_margin,
              dd_button_style_fb->style()->inner_margin());
  check_vec2f(dd_button_style.minimum_size,
              dd_button_style_fb->style()->minimum_size());
  check_vec2f(dd_button_style.maximum_size,
              dd_button_style_fb->style()->maximum_size());
  check_color(dd_button_style.triangle_color,
              dd_button_style_fb->triangle_color());
  check_color(dd_button_style.hover_color, dd_button_style_fb->hover_color());

  // ----- Name -----
  REQUIRE(ui_style.name == style_data.name()->str());
}

TEST_CASE("StylesConfigurator returns a map of all available styles",
          "[StylesConfigurator]") {
  // Arrange
  steamrot::FlatbuffersDataLoader data_loader{steamrot::EnvironmentType::Test};
  steamrot::StylesConfigurator styles_configurator;
  steamrot::AssetManager asset_manager{steamrot::EnvironmentType::Test};
  auto load_default_assets_result = asset_manager.LoadDefaultAssets();
  if (!load_default_assets_result)
    FAIL(load_default_assets_result.error().message);

  std::vector<std::string> expected_style_names = {"default"};

  // Act
  auto styles_map_result = styles_configurator.ProvideUIStylesMap(
      asset_manager, expected_style_names);
  if (!styles_map_result)
    FAIL(styles_map_result.error().message);
  const auto &styles_map = styles_map_result.value();
  // Assert
  REQUIRE(styles_map.size() == expected_style_names.size());
  for (const auto &name : expected_style_names) {
    auto it = styles_map.find(name);
    REQUIRE(it != styles_map.end());
  }
}
