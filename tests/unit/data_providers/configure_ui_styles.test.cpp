/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for configure_ui_styles functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_ui_styles.h"
#include "FlatbuffersDataLoader.h"
#include "UIStyle.h"
#include "types_generated.h"
#include "ui_style_generated.h"
#include <SFML/Graphics/Font.hpp>
#include <catch2/catch_test_macros.hpp>
#include <flatbuffers/flatbuffers.h>
#include <memory>
#include <unordered_map>

/////////////////////////////////////////////////
// ConfigureBaseStyle tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureBaseStyle fails with null data",
          "[unit][configure_ui_styles]") {
  Style style;

  auto result =
      steamrot::data::configure::ConfigureBaseStyle(nullptr, style, "test");

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "test missing");
}

TEST_CASE("ConfigureBaseStyle configures from valid data",
          "[unit][configure_ui_styles]") {
  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorDataFbs(fbb, 100, 150, 200, 255);
  auto border_color = steamrot::CreateColorDataFbs(fbb, 255, 128, 0, 255);
  auto inner_margin = steamrot::CreateVector2fDataFbs(fbb, 15.0f, 20.0f);
  auto minimum_size = steamrot::CreateVector2fDataFbs(fbb, 60.0f, 40.0f);
  auto maximum_size = steamrot::CreateVector2fDataFbs(fbb, 300.0f, 250.0f);
  auto hover_color = steamrot::CreateColorDataFbs(fbb, 50, 100, 150, 255);

  auto style_offset =
      steamrot::CreateStyleDataFbs(fbb, background_color, border_color, 3.5f, 12,
                                inner_margin, minimum_size, maximum_size,
                                hover_color);

  fbb.Finish(style_offset);
  const steamrot::StyleDataFbs *style_fb =
      flatbuffers::GetRoot<steamrot::StyleDataFbs>(fbb.GetBufferPointer());

  Style style;
  auto result = steamrot::data::configure::ConfigureBaseStyle(
      style_fb, style, "test_style");

  REQUIRE(result.has_value());
  REQUIRE(style.background_color.r == 100);
  REQUIRE(style.background_color.g == 150);
  REQUIRE(style.background_color.b == 200);
  REQUIRE(style.background_color.a == 255);
  REQUIRE(style.border_color.r == 255);
  REQUIRE(style.border_color.g == 128);
  REQUIRE(style.border_color.b == 0);
  REQUIRE(style.border_color.a == 255);
  REQUIRE(style.border_thickness == 3.5f);
  REQUIRE(style.radius_resolution == 12);
  REQUIRE(style.inner_margin.x == 15.0f);
  REQUIRE(style.inner_margin.y == 20.0f);
  REQUIRE(style.minimum_size.x == 60.0f);
  REQUIRE(style.minimum_size.y == 40.0f);
  REQUIRE(style.maximum_size.x == 300.0f);
  REQUIRE(style.maximum_size.y == 250.0f);
  REQUIRE(style.hover_color.r == 50);
  REQUIRE(style.hover_color.g == 100);
  REQUIRE(style.hover_color.b == 150);
  REQUIRE(style.hover_color.a == 255);
}

TEST_CASE("ConfigureBaseStyle defaults hover_color to background_color when not set",
          "[unit][configure_ui_styles]") {
  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorDataFbs(fbb, 100, 150, 200, 255);
  auto border_color = steamrot::CreateColorDataFbs(fbb, 255, 128, 0, 255);
  auto inner_margin = steamrot::CreateVector2fDataFbs(fbb, 15.0f, 20.0f);
  auto minimum_size = steamrot::CreateVector2fDataFbs(fbb, 60.0f, 40.0f);
  auto maximum_size = steamrot::CreateVector2fDataFbs(fbb, 300.0f, 250.0f);

  // No hover_color provided - should default to background_color
  auto style_offset =
      steamrot::CreateStyleDataFbs(fbb, background_color, border_color, 3.5f, 12,
                                inner_margin, minimum_size, maximum_size);

  fbb.Finish(style_offset);
  const steamrot::StyleDataFbs *style_fb =
      flatbuffers::GetRoot<steamrot::StyleDataFbs>(fbb.GetBufferPointer());

  Style style;
  auto result = steamrot::data::configure::ConfigureBaseStyle(
      style_fb, style, "test_style");

  REQUIRE(result.has_value());
  REQUIRE(style.hover_color.r == style.background_color.r);
  REQUIRE(style.hover_color.g == style.background_color.g);
  REQUIRE(style.hover_color.b == style.background_color.b);
  REQUIRE(style.hover_color.a == style.background_color.a);
}

/////////////////////////////////////////////////
// ConfigurePanelStyle tests
/////////////////////////////////////////////////

TEST_CASE("ConfigurePanelStyle fails with null data",
          "[unit][configure_ui_styles]") {
  PanelStyle panel_style;

  auto result =
      steamrot::data::configure::ConfigurePanelStyle(nullptr, panel_style);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "panel_style missing");
}

TEST_CASE("ConfigurePanelStyle configures from valid data",
          "[unit][configure_ui_styles]") {
  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorDataFbs(fbb, 50, 60, 70, 255);
  auto border_color = steamrot::CreateColorDataFbs(fbb, 200, 100, 50, 255);
  auto inner_margin = steamrot::CreateVector2fDataFbs(fbb, 8.0f, 8.0f);
  auto minimum_size = steamrot::CreateVector2fDataFbs(fbb, 100.0f, 100.0f);
  auto maximum_size = steamrot::CreateVector2fDataFbs(fbb, 400.0f, 400.0f);

  auto style_offset =
      steamrot::CreateStyleDataFbs(fbb, background_color, border_color, 1.5f, 8,
                                inner_margin, minimum_size, maximum_size);

  auto panel_offset = steamrot::CreatePanelStyleDataFbs(fbb, style_offset);
  fbb.Finish(panel_offset);

  const steamrot::PanelStyleDataFbs *panel_fb =
      flatbuffers::GetRoot<steamrot::PanelStyleDataFbs>(fbb.GetBufferPointer());

  PanelStyle panel_style;
  auto result =
      steamrot::data::configure::ConfigurePanelStyle(panel_fb, panel_style);

  REQUIRE(result.has_value());
  REQUIRE(panel_style.background_color.r == 50);
  REQUIRE(panel_style.background_color.g == 60);
  REQUIRE(panel_style.background_color.b == 70);
  REQUIRE(panel_style.border_thickness == 1.5f);
  REQUIRE(panel_style.radius_resolution == 8);
}

/////////////////////////////////////////////////
// ConfigureButtonStyle tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureButtonStyle fails with null data",
          "[unit][configure_ui_styles]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  ButtonStyle button_style;

  auto result = steamrot::data::configure::ConfigureButtonStyle(
      nullptr, button_style, fonts_map);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "button_style missing");
}

TEST_CASE("ConfigureButtonStyle fails with non-existent font",
          "[unit][configure_ui_styles]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;

  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorDataFbs(fbb, 50, 60, 70, 255);
  auto border_color = steamrot::CreateColorDataFbs(fbb, 200, 100, 50, 255);
  auto text_color = steamrot::CreateColorDataFbs(fbb, 255, 255, 255, 255);
  auto hover_color = steamrot::CreateColorDataFbs(fbb, 200, 200, 200, 255);

  auto inner_margin = steamrot::CreateVector2fDataFbs(fbb, 8.0f, 8.0f);
  auto minimum_size = steamrot::CreateVector2fDataFbs(fbb, 100.0f, 50.0f);
  auto maximum_size = steamrot::CreateVector2fDataFbs(fbb, 300.0f, 150.0f);
  auto font_name = fbb.CreateString("non_existent_font");

  auto style_offset =
      steamrot::CreateStyleDataFbs(fbb, background_color, border_color, 2.0f, 10,
                                inner_margin, minimum_size, maximum_size,
                                hover_color);

  auto button_offset = steamrot::CreateButtonStyleDataFbs(
      fbb, style_offset, text_color, font_name, 16);

  fbb.Finish(button_offset);

  const steamrot::ButtonStyleDataFbs *button_fb =
      flatbuffers::GetRoot<steamrot::ButtonStyleDataFbs>(fbb.GetBufferPointer());

  ButtonStyle button_style;
  auto result = steamrot::data::configure::ConfigureButtonStyle(
      button_fb, button_style, fonts_map);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message ==
          "button_style.font not found: non_existent_font");
}

TEST_CASE("ConfigureButtonStyle configures from valid data",
          "[unit][configure_ui_styles]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  auto test_font = std::make_shared<sf::Font>();
  fonts_map["test_font"] = test_font;

  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorDataFbs(fbb, 50, 60, 70, 255);
  auto border_color = steamrot::CreateColorDataFbs(fbb, 200, 100, 50, 255);
  auto text_color = steamrot::CreateColorDataFbs(fbb, 255, 255, 255, 255);
  auto hover_color = steamrot::CreateColorDataFbs(fbb, 200, 200, 200, 255);
  auto inner_margin = steamrot::CreateVector2fDataFbs(fbb, 8.0f, 8.0f);
  auto minimum_size = steamrot::CreateVector2fDataFbs(fbb, 100.0f, 50.0f);
  auto maximum_size = steamrot::CreateVector2fDataFbs(fbb, 300.0f, 150.0f);
  auto font_name = fbb.CreateString("test_font");

  auto style_offset =
      steamrot::CreateStyleDataFbs(fbb, background_color, border_color, 2.0f, 10,
                                inner_margin, minimum_size, maximum_size,
                                hover_color);

  auto button_offset = steamrot::CreateButtonStyleDataFbs(
      fbb, style_offset, text_color, font_name, 18);

  fbb.Finish(button_offset);

  const steamrot::ButtonStyleDataFbs *button_fb =
      flatbuffers::GetRoot<steamrot::ButtonStyleDataFbs>(fbb.GetBufferPointer());

  ButtonStyle button_style;
  auto result = steamrot::data::configure::ConfigureButtonStyle(
      button_fb, button_style, fonts_map);

  REQUIRE(result.has_value());
  REQUIRE(button_style.text_color.r == 255);
  REQUIRE(button_style.text_color.g == 255);
  REQUIRE(button_style.text_color.b == 255);
  REQUIRE(button_style.hover_color.r == 200);
  REQUIRE(button_style.font != nullptr);
  REQUIRE(button_style.font == test_font);
  REQUIRE(button_style.font_size == 18);
}

/////////////////////////////////////////////////
// ConfigureDropDownContainerStyle tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureDropDownContainerStyle fails with null data",
          "[unit][configure_ui_styles]") {
  DropDownContainerStyle dd_container_style;

  auto result = steamrot::data::configure::ConfigureDropDownContainerStyle(
      nullptr, dd_container_style);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "drop_down_container_style missing");
}

TEST_CASE("ConfigureDropDownContainerStyle configures from valid data",
          "[unit][configure_ui_styles]") {
  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorDataFbs(fbb, 80, 80, 80, 255);
  auto border_color = steamrot::CreateColorDataFbs(fbb, 180, 180, 180, 255);
  auto inner_margin = steamrot::CreateVector2fDataFbs(fbb, 5.0f, 5.0f);
  auto minimum_size = steamrot::CreateVector2fDataFbs(fbb, 120.0f, 30.0f);
  auto maximum_size = steamrot::CreateVector2fDataFbs(fbb, 350.0f, 80.0f);

  auto style_offset =
      steamrot::CreateStyleDataFbs(fbb, background_color, border_color, 1.0f, 6,
                                inner_margin, minimum_size, maximum_size);

  auto dd_container_offset =
      steamrot::CreateDropDownContainerStyleDataFbs(fbb, style_offset, 0.25f);

  fbb.Finish(dd_container_offset);

  const steamrot::DropDownContainerStyleDataFbs *dd_container_fb =
      flatbuffers::GetRoot<steamrot::DropDownContainerStyleDataFbs>(
          fbb.GetBufferPointer());

  DropDownContainerStyle dd_container_style;
  auto result = steamrot::data::configure::ConfigureDropDownContainerStyle(
      dd_container_fb, dd_container_style);

  REQUIRE(result.has_value());
  REQUIRE(dd_container_style.drop_symbol_ratio == 0.25f);
  REQUIRE(dd_container_style.background_color.r == 80);
  REQUIRE(dd_container_style.border_thickness == 1.0f);
}

/////////////////////////////////////////////////
// ConfigureDropDownListStyle tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureDropDownListStyle fails with null data",
          "[unit][configure_ui_styles]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  DropDownListStyle dd_list_style;

  auto result = steamrot::data::configure::ConfigureDropDownListStyle(
      nullptr, dd_list_style, fonts_map);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "drop_down_list_style missing");
}

TEST_CASE("ConfigureDropDownListStyle fails with missing font",
          "[unit][configure_ui_styles]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;

  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorDataFbs(fbb, 90, 90, 90, 255);
  auto border_color = steamrot::CreateColorDataFbs(fbb, 160, 160, 160, 255);
  auto text_color = steamrot::CreateColorDataFbs(fbb, 255, 255, 255, 255);
  auto hover_color = steamrot::CreateColorDataFbs(fbb, 200, 200, 200, 255);

  auto inner_margin = steamrot::CreateVector2fDataFbs(fbb, 4.0f, 4.0f);
  auto minimum_size = steamrot::CreateVector2fDataFbs(fbb, 100.0f, 150.0f);
  auto maximum_size = steamrot::CreateVector2fDataFbs(fbb, 300.0f, 400.0f);
  auto font_name = fbb.CreateString("missing_font");

  auto style_offset =
      steamrot::CreateStyleDataFbs(fbb, background_color, border_color, 1.0f, 8,
                                inner_margin, minimum_size, maximum_size,
                                hover_color);

  auto dd_list_offset = steamrot::CreateDropDownListStyleDataFbs(
      fbb, style_offset, text_color, font_name, 14);

  fbb.Finish(dd_list_offset);

  const steamrot::DropDownListStyleDataFbs *dd_list_fb =
      flatbuffers::GetRoot<steamrot::DropDownListStyleDataFbs>(
          fbb.GetBufferPointer());

  DropDownListStyle dd_list_style;
  auto result = steamrot::data::configure::ConfigureDropDownListStyle(
      dd_list_fb, dd_list_style, fonts_map);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message ==
          "drop_down_list_style.font not found: missing_font");
}

TEST_CASE("ConfigureDropDownListStyle configures from valid data",
          "[unit][configure_ui_styles]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  auto test_font = std::make_shared<sf::Font>();
  fonts_map["list_font"] = test_font;

  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorDataFbs(fbb, 90, 90, 90, 255);
  auto border_color = steamrot::CreateColorDataFbs(fbb, 160, 160, 160, 255);
  auto text_color = steamrot::CreateColorDataFbs(fbb, 255, 255, 255, 255);
  auto hover_color = steamrot::CreateColorDataFbs(fbb, 200, 200, 200, 255);

  auto inner_margin = steamrot::CreateVector2fDataFbs(fbb, 4.0f, 4.0f);
  auto minimum_size = steamrot::CreateVector2fDataFbs(fbb, 100.0f, 150.0f);
  auto maximum_size = steamrot::CreateVector2fDataFbs(fbb, 300.0f, 400.0f);
  auto font_name = fbb.CreateString("list_font");

  auto style_offset =
      steamrot::CreateStyleDataFbs(fbb, background_color, border_color, 1.0f, 8,
                                inner_margin, minimum_size, maximum_size,
                                hover_color);

  auto dd_list_offset = steamrot::CreateDropDownListStyleDataFbs(
      fbb, style_offset, text_color, font_name, 14);

  fbb.Finish(dd_list_offset);

  const steamrot::DropDownListStyleDataFbs *dd_list_fb =
      flatbuffers::GetRoot<steamrot::DropDownListStyleDataFbs>(
          fbb.GetBufferPointer());

  DropDownListStyle dd_list_style;
  auto result = steamrot::data::configure::ConfigureDropDownListStyle(
      dd_list_fb, dd_list_style, fonts_map);

  REQUIRE(result.has_value());
  REQUIRE(dd_list_style.text_color.r == 255);
  REQUIRE(dd_list_style.hover_color.r == 200);
  REQUIRE(dd_list_style.font == test_font);
  REQUIRE(dd_list_style.font_size == 14);
}

/////////////////////////////////////////////////
// ConfigureDropDownItemStyle tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureDropDownItemStyle fails with null data",
          "[unit][configure_ui_styles]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  DropDownItemStyle dd_item_style;

  auto result = steamrot::data::configure::ConfigureDropDownItemStyle(
      nullptr, dd_item_style, fonts_map);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "drop_down_item_style missing");
}

TEST_CASE("ConfigureDropDownItemStyle configures from valid data",
          "[unit][configure_ui_styles]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  auto test_font = std::make_shared<sf::Font>();
  fonts_map["item_font"] = test_font;

  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorDataFbs(fbb, 70, 70, 70, 255);
  auto border_color = steamrot::CreateColorDataFbs(fbb, 140, 140, 140, 255);
  auto text_color = steamrot::CreateColorDataFbs(fbb, 240, 240, 240, 255);
  auto hover_color = steamrot::CreateColorDataFbs(fbb, 180, 180, 180, 255);

  auto inner_margin = steamrot::CreateVector2fDataFbs(fbb, 3.0f, 3.0f);
  auto minimum_size = steamrot::CreateVector2fDataFbs(fbb, 90.0f, 25.0f);
  auto maximum_size = steamrot::CreateVector2fDataFbs(fbb, 280.0f, 50.0f);
  auto font_name = fbb.CreateString("item_font");

  auto style_offset =
      steamrot::CreateStyleDataFbs(fbb, background_color, border_color, 0.5f, 6,
                                inner_margin, minimum_size, maximum_size,
                                hover_color);

  auto dd_item_offset = steamrot::CreateDropDownItemStyleDataFbs(
      fbb, style_offset, text_color, font_name, 12);

  fbb.Finish(dd_item_offset);

  const steamrot::DropDownItemStyleDataFbs *dd_item_fb =
      flatbuffers::GetRoot<steamrot::DropDownItemStyleDataFbs>(
          fbb.GetBufferPointer());

  DropDownItemStyle dd_item_style;
  auto result = steamrot::data::configure::ConfigureDropDownItemStyle(
      dd_item_fb, dd_item_style, fonts_map);

  REQUIRE(result.has_value());
  REQUIRE(dd_item_style.text_color.r == 240);
  REQUIRE(dd_item_style.hover_color.r == 180);
  REQUIRE(dd_item_style.font == test_font);
  REQUIRE(dd_item_style.font_size == 12);
}

/////////////////////////////////////////////////
// ConfigureDropDownButtonStyle tests
/////////////////////////////////////////////////

TEST_CASE("ConfigureDropDownButtonStyle fails with null data",
          "[unit][configure_ui_styles]") {
  DropDownButtonStyle dd_button_style;

  auto result = steamrot::data::configure::ConfigureDropDownButtonStyle(
      nullptr, dd_button_style);

  REQUIRE_FALSE(result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "drop_down_button_style missing");
}

TEST_CASE("ConfigureDropDownButtonStyle configures from valid data",
          "[unit][configure_ui_styles]") {
  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorDataFbs(fbb, 60, 60, 60, 255);
  auto border_color = steamrot::CreateColorDataFbs(fbb, 120, 120, 120, 255);
  auto triangle_color = steamrot::CreateColorDataFbs(fbb, 255, 200, 0, 255);
  auto hover_color = steamrot::CreateColorDataFbs(fbb, 170, 170, 170, 255);

  auto inner_margin = steamrot::CreateVector2fDataFbs(fbb, 2.0f, 2.0f);
  auto minimum_size = steamrot::CreateVector2fDataFbs(fbb, 30.0f, 30.0f);
  auto maximum_size = steamrot::CreateVector2fDataFbs(fbb, 50.0f, 50.0f);

  auto style_offset =
      steamrot::CreateStyleDataFbs(fbb, background_color, border_color, 1.0f, 5,
                                inner_margin, minimum_size, maximum_size,
                                hover_color);

  auto dd_button_offset = steamrot::CreateDropDownButtonStyleDataFbs(
      fbb, style_offset, triangle_color);

  fbb.Finish(dd_button_offset);

  const steamrot::DropDownButtonStyleDataFbs *dd_button_fb =
      flatbuffers::GetRoot<steamrot::DropDownButtonStyleDataFbs>(
          fbb.GetBufferPointer());

  DropDownButtonStyle dd_button_style;
  auto result = steamrot::data::configure::ConfigureDropDownButtonStyle(
      dd_button_fb, dd_button_style);

  REQUIRE(result.has_value());
  REQUIRE(dd_button_style.triangle_color.r == 255);
  REQUIRE(dd_button_style.triangle_color.g == 200);
  REQUIRE(dd_button_style.triangle_color.b == 0);
  REQUIRE(dd_button_style.hover_color.r == 170);
}
