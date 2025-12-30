/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for FlatbuffersUIStyleDataProvider
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "FlatbuffersUIStyleDataProvider.h"
#include "flatbuffers/buffer.h"
#include "types_generated.h"
#include "ui_style_generated.h"
#include <SFML/Graphics/Font.hpp>
#include <catch2/catch_test_macros.hpp>
#include <flatbuffers/flatbuffers.h>
#include <memory>
#include <unordered_map>

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigureBaseStyle with null "
          "style_fb returns error",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  Style style;
  auto result = provider.ConfigureBaseStyle(nullptr, style, "test_style");

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "test_style missing");
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigureBaseStyle with valid data "
          "configures correctly",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorData(fbb, 100, 150, 200, 255);
  auto border_color = steamrot::CreateColorData(fbb, 255, 128, 0, 255);
  auto inner_margin = steamrot::CreateVector2fData(fbb, 15.0f, 20.0f);
  auto minimum_size = steamrot::CreateVector2fData(fbb, 60.0f, 40.0f);
  auto maximum_size = steamrot::CreateVector2fData(fbb, 300.0f, 250.0f);

  auto style_offset =
      steamrot::CreateStyleData(fbb, background_color, border_color, 3.5f, 12,
                                inner_margin, minimum_size, maximum_size);

  fbb.Finish(style_offset);
  const steamrot::StyleData *style_fb =
      flatbuffers::GetRoot<steamrot::StyleData>(fbb.GetBufferPointer());

  Style style;
  auto result = provider.ConfigureBaseStyle(style_fb, style, "test_style");

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
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigurePanelStyle with null "
          "panel_fb returns error",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  PanelStyle panel_style;
  auto result = provider.ConfigurePanelStyle(nullptr, panel_style);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "panel_style missing");
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigurePanelStyle with valid data "
          "configures correctly",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorData(fbb, 50, 60, 70, 255);
  auto border_color = steamrot::CreateColorData(fbb, 200, 100, 50, 255);
  auto inner_margin = steamrot::CreateVector2fData(fbb, 8.0f, 8.0f);
  auto minimum_size = steamrot::CreateVector2fData(fbb, 100.0f, 100.0f);
  auto maximum_size = steamrot::CreateVector2fData(fbb, 400.0f, 400.0f);

  auto style_offset =
      steamrot::CreateStyleData(fbb, background_color, border_color, 1.5f, 8,
                                inner_margin, minimum_size, maximum_size);

  auto panel_offset = steamrot::CreatePanelStyleData(fbb, style_offset);
  fbb.Finish(panel_offset);

  const steamrot::PanelStyleData *panel_fb =
      flatbuffers::GetRoot<steamrot::PanelStyleData>(fbb.GetBufferPointer());

  PanelStyle panel_style;
  auto result = provider.ConfigurePanelStyle(panel_fb, panel_style);

  REQUIRE(result.has_value());
  REQUIRE(panel_style.background_color.r == 50);
  REQUIRE(panel_style.background_color.g == 60);
  REQUIRE(panel_style.background_color.b == 70);
  REQUIRE(panel_style.border_thickness == 1.5f);
  REQUIRE(panel_style.radius_resolution == 8);
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigureButtonStyle with null "
          "button_fb returns error",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  ButtonStyle button_style;
  auto result = provider.ConfigureButtonStyle(nullptr, button_style);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "button_style missing");
}

//
TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigureButtonStyle with "
          "non-existent font returns error",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorData(fbb, 50, 60, 70, 255);
  auto border_color = steamrot::CreateColorData(fbb, 200, 100, 50, 255);
  auto text_color = steamrot::CreateColorData(fbb, 255, 255, 255, 255);
  auto hover_color = steamrot::CreateColorData(fbb, 200, 200, 200, 255);

  auto inner_margin = steamrot::CreateVector2fData(fbb, 8.0f, 8.0f);
  auto minimum_size = steamrot::CreateVector2fData(fbb, 100.0f, 50.0f);
  auto maximum_size = steamrot::CreateVector2fData(fbb, 300.0f, 150.0f);
  auto font_name = fbb.CreateString("non_existent_font");

  auto style_offset =
      steamrot::CreateStyleData(fbb, background_color, border_color, 2.0f, 10,
                                inner_margin, minimum_size, maximum_size);

  auto button_offset = steamrot::CreateButtonStyleData(
      fbb, style_offset, text_color, hover_color, font_name, 16);

  fbb.Finish(button_offset);

  const steamrot::ButtonStyleData *button_fb =
      flatbuffers::GetRoot<steamrot::ButtonStyleData>(fbb.GetBufferPointer());

  ButtonStyle button_style;
  auto result = provider.ConfigureButtonStyle(button_fb, button_style);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message ==
          "button_style.font not found: non_existent_font");
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigureButtonStyle with valid "
          "data configures correctly",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  // Create fonts map with a test font
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  auto test_font = std::make_shared<sf::Font>();
  fonts_map["test_font"] = test_font;

  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorData(fbb, 50, 60, 70, 255);
  auto border_color = steamrot::CreateColorData(fbb, 200, 100, 50, 255);
  auto text_color = steamrot::CreateColorData(fbb, 255, 255, 255, 255);
  auto hover_color = steamrot::CreateColorData(fbb, 200, 200, 200, 255);
  auto inner_margin = steamrot::CreateVector2fData(fbb, 8.0f, 8.0f);
  auto minimum_size = steamrot::CreateVector2fData(fbb, 100.0f, 50.0f);
  auto maximum_size = steamrot::CreateVector2fData(fbb, 300.0f, 150.0f);
  auto font_name = fbb.CreateString("test_font");

  auto style_offset =
      steamrot::CreateStyleData(fbb, background_color, border_color, 2.0f, 10,
                                inner_margin, minimum_size, maximum_size);

  auto button_offset = steamrot::CreateButtonStyleData(
      fbb, style_offset, text_color, hover_color, font_name, 18);

  fbb.Finish(button_offset);

  const steamrot::ButtonStyleData *button_fb =
      flatbuffers::GetRoot<steamrot::ButtonStyleData>(fbb.GetBufferPointer());

  ButtonStyle button_style;
  auto result = provider.ConfigureButtonStyle(button_fb, button_style);

  REQUIRE(result.has_value());
  REQUIRE(button_style.text_color.r == 255);
  REQUIRE(button_style.text_color.g == 255);
  REQUIRE(button_style.text_color.b == 255);
  REQUIRE(button_style.hover_color.r == 200);
  REQUIRE(button_style.font != nullptr);
  REQUIRE(button_style.font == test_font);
  REQUIRE(button_style.font_size == 18);
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigureDropDownContainerStyle "
          "with null returns error",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  DropDownContainerStyle dd_container_style;
  auto result =
      provider.ConfigureDropDownContainerStyle(nullptr, dd_container_style);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "drop_down_container_style missing");
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigureDropDownContainerStyle "
          "with valid data configures correctly",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorData(fbb, 80, 80, 80, 255);
  auto border_color = steamrot::CreateColorData(fbb, 180, 180, 180, 255);
  auto inner_margin = steamrot::CreateVector2fData(fbb, 5.0f, 5.0f);
  auto minimum_size = steamrot::CreateVector2fData(fbb, 120.0f, 30.0f);
  auto maximum_size = steamrot::CreateVector2fData(fbb, 350.0f, 80.0f);

  auto style_offset =
      steamrot::CreateStyleData(fbb, background_color, border_color, 1.0f, 6,
                                inner_margin, minimum_size, maximum_size);

  auto dd_container_offset =
      steamrot::CreateDropDownContainerStyleData(fbb, style_offset,
                                                 0.25f); // drop_symbol_ratio

  fbb.Finish(dd_container_offset);

  const steamrot::DropDownContainerStyleData *dd_container_fb =
      flatbuffers::GetRoot<steamrot::DropDownContainerStyleData>(
          fbb.GetBufferPointer());

  DropDownContainerStyle dd_container_style;
  auto result = provider.ConfigureDropDownContainerStyle(dd_container_fb,
                                                         dd_container_style);

  REQUIRE(result.has_value());
  REQUIRE(dd_container_style.drop_symbol_ratio == 0.25f);
  REQUIRE(dd_container_style.background_color.r == 80);
  REQUIRE(dd_container_style.border_thickness == 1.0f);
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigureDropDownListStyle with "
          "null returns error",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  DropDownListStyle dd_list_style;
  auto result = provider.ConfigureDropDownListStyle(nullptr, dd_list_style);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "drop_down_list_style missing");
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigureDropDownListStyle with "
          "missing font returns error",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorData(fbb, 90, 90, 90, 255);
  auto border_color = steamrot::CreateColorData(fbb, 160, 160, 160, 255);
  auto text_color = steamrot::CreateColorData(fbb, 255, 255, 255, 255);
  auto hover_color = steamrot::CreateColorData(fbb, 200, 200, 200, 255);

  auto inner_margin = steamrot::CreateVector2fData(fbb, 4.0f, 4.0f);
  auto minimum_size = steamrot::CreateVector2fData(fbb, 100.0f, 150.0f);
  auto maximum_size = steamrot::CreateVector2fData(fbb, 300.0f, 400.0f);
  auto font_name = fbb.CreateString("missing_font");

  auto style_offset =
      steamrot::CreateStyleData(fbb, background_color, border_color, 1.0f, 8,
                                inner_margin, minimum_size, maximum_size);

  auto dd_list_offset = steamrot::CreateDropDownListStyleData(
      fbb, style_offset, text_color, hover_color, font_name, 14);

  fbb.Finish(dd_list_offset);

  const steamrot::DropDownListStyleData *dd_list_fb =
      flatbuffers::GetRoot<steamrot::DropDownListStyleData>(
          fbb.GetBufferPointer());

  DropDownListStyle dd_list_style;
  auto result = provider.ConfigureDropDownListStyle(dd_list_fb, dd_list_style);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message ==
          "drop_down_list_style.font not found: missing_font");
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigureDropDownListStyle with "
          "valid data configures correctly",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  auto test_font = std::make_shared<sf::Font>();
  fonts_map["list_font"] = test_font;

  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorData(fbb, 90, 90, 90, 255);
  auto border_color = steamrot::CreateColorData(fbb, 160, 160, 160, 255);
  auto text_color = steamrot::CreateColorData(fbb, 255, 255, 255, 255);
  auto hover_color = steamrot::CreateColorData(fbb, 200, 200, 200, 255);

  auto inner_margin = steamrot::CreateVector2fData(fbb, 4.0f, 4.0f);
  auto minimum_size = steamrot::CreateVector2fData(fbb, 100.0f, 150.0f);
  auto maximum_size = steamrot::CreateVector2fData(fbb, 300.0f, 400.0f);
  auto font_name = fbb.CreateString("list_font");

  auto style_offset =
      steamrot::CreateStyleData(fbb, background_color, border_color, 1.0f, 8,
                                inner_margin, minimum_size, maximum_size);

  auto dd_list_offset = steamrot::CreateDropDownListStyleData(
      fbb, style_offset, text_color, hover_color, font_name, 14);

  fbb.Finish(dd_list_offset);

  const steamrot::DropDownListStyleData *dd_list_fb =
      flatbuffers::GetRoot<steamrot::DropDownListStyleData>(
          fbb.GetBufferPointer());

  DropDownListStyle dd_list_style;
  auto result = provider.ConfigureDropDownListStyle(dd_list_fb, dd_list_style);

  REQUIRE(result.has_value());
  REQUIRE(dd_list_style.text_color.r == 255);
  REQUIRE(dd_list_style.hover_color.r == 200);
  REQUIRE(dd_list_style.font == test_font);
  REQUIRE(dd_list_style.font_size == 14);
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigureDropDownItemStyle with "
          "null returns error",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  DropDownItemStyle dd_item_style;
  auto result = provider.ConfigureDropDownItemStyle(nullptr, dd_item_style);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "drop_down_item_style missing");
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigureDropDownItemStyle with "
          "null style returns error",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  flatbuffers::FlatBufferBuilder fbb;

  auto text_color = steamrot::CreateColorData(fbb, 240, 240, 240, 255);
  auto hover_color = steamrot::CreateColorData(fbb, 180, 180, 180, 255);
  auto font_name = fbb.CreateString("item_font");

  // Create with null style pointer
  auto dd_item_offset = steamrot::CreateDropDownItemStyleData(
      fbb, 0, text_color, hover_color, font_name, 12);

  fbb.Finish(dd_item_offset);

  const steamrot::DropDownItemStyleData *dd_item_fb =
      flatbuffers::GetRoot<steamrot::DropDownItemStyleData>(
          fbb.GetBufferPointer());

  DropDownItemStyle dd_item_style;
  auto result = provider.ConfigureDropDownItemStyle(dd_item_fb, dd_item_style);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "drop_down_item_style.style missing");
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigureDropDownItemStyle with "
          "valid data configures correctly",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  auto test_font = std::make_shared<sf::Font>();
  fonts_map["item_font"] = test_font;

  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorData(fbb, 70, 70, 70, 255);
  auto border_color = steamrot::CreateColorData(fbb, 140, 140, 140, 255);
  auto text_color = steamrot::CreateColorData(fbb, 240, 240, 240, 255);
  auto hover_color = steamrot::CreateColorData(fbb, 180, 180, 180, 255);

  auto inner_margin = steamrot::CreateVector2fData(fbb, 3.0f, 3.0f);
  auto minimum_size = steamrot::CreateVector2fData(fbb, 90.0f, 25.0f);
  auto maximum_size = steamrot::CreateVector2fData(fbb, 280.0f, 50.0f);
  auto font_name = fbb.CreateString("item_font");

  auto style_offset =
      steamrot::CreateStyleData(fbb, background_color, border_color, 0.5f, 6,
                                inner_margin, minimum_size, maximum_size);

  auto dd_item_offset = steamrot::CreateDropDownItemStyleData(
      fbb, style_offset, text_color, hover_color, font_name, 12);

  fbb.Finish(dd_item_offset);

  const steamrot::DropDownItemStyleData *dd_item_fb =
      flatbuffers::GetRoot<steamrot::DropDownItemStyleData>(
          fbb.GetBufferPointer());

  DropDownItemStyle dd_item_style;
  auto result = provider.ConfigureDropDownItemStyle(dd_item_fb, dd_item_style);

  REQUIRE(result.has_value());
  REQUIRE(dd_item_style.text_color.r == 240);
  REQUIRE(dd_item_style.hover_color.r == 180);
  REQUIRE(dd_item_style.font == test_font);
  REQUIRE(dd_item_style.font_size == 12);
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigureDropDownButtonStyle with"
          "null returns error",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  DropDownButtonStyle dd_button_style;
  auto result = provider.ConfigureDropDownButtonStyle(nullptr, dd_button_style);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "drop_down_button_style missing");
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigureDropDownButtonStyle with "
          "null style returns error",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  flatbuffers::FlatBufferBuilder fbb;

  auto triangle_color = steamrot::CreateColorData(fbb, 255, 200, 0, 255);
  auto hover_color = steamrot::CreateColorData(fbb, 170, 170, 170, 255);

  // Create with null style pointer
  auto dd_button_offset = steamrot::CreateDropDownButtonStyleData(
      fbb, 0, triangle_color, hover_color);

  fbb.Finish(dd_button_offset);

  const steamrot::DropDownButtonStyleData *dd_button_fb =
      flatbuffers::GetRoot<steamrot::DropDownButtonStyleData>(
          fbb.GetBufferPointer());

  DropDownButtonStyle dd_button_style;
  auto result =
      provider.ConfigureDropDownButtonStyle(dd_button_fb, dd_button_style);

  REQUIRE(!result.has_value());
  REQUIRE(result.error().mode == steamrot::FailMode::FlatbuffersDataNotFound);
  REQUIRE(result.error().message == "drop_down_button_style.style missing");
}

TEST_CASE("FlatbuffersUIStyleDataProvider::ConfigureDropDownButtonStyle with"
          "valid data configures correctly",
          "[unit][FlatbuffersUIStyleDataProvider]") {
  std::unordered_map<std::string, std::shared_ptr<const sf::Font>> fonts_map;
  steamrot::FlatbuffersUIStyleDataProvider provider(fonts_map);

  flatbuffers::FlatBufferBuilder fbb;

  auto background_color = steamrot::CreateColorData(fbb, 60, 60, 60, 255);
  auto border_color = steamrot::CreateColorData(fbb, 120, 120, 120, 255);
  auto triangle_color = steamrot::CreateColorData(fbb, 255, 200, 0, 255);
  auto hover_color = steamrot::CreateColorData(fbb, 170, 170, 170, 255);

  auto inner_margin = steamrot::CreateVector2fData(fbb, 2.0f, 2.0f);
  auto minimum_size = steamrot::CreateVector2fData(fbb, 30.0f, 30.0f);
  auto maximum_size = steamrot::CreateVector2fData(fbb, 50.0f, 50.0f);

  auto style_offset =
      steamrot::CreateStyleData(fbb, background_color, border_color, 1.0f, 5,
                                inner_margin, minimum_size, maximum_size);

  auto dd_button_offset = steamrot::CreateDropDownButtonStyleData(
      fbb, style_offset, triangle_color, hover_color);

  fbb.Finish(dd_button_offset);

  const steamrot::DropDownButtonStyleData *dd_button_fb =
      flatbuffers::GetRoot<steamrot::DropDownButtonStyleData>(
          fbb.GetBufferPointer());

  DropDownButtonStyle dd_button_style;
  auto result =
      provider.ConfigureDropDownButtonStyle(dd_button_fb, dd_button_style);

  REQUIRE(result.has_value());
  REQUIRE(dd_button_style.triangle_color.r == 255);
  REQUIRE(dd_button_style.triangle_color.g == 200);
  REQUIRE(dd_button_style.triangle_color.b == 0);
  REQUIRE(dd_button_style.hover_color.r == 170);
}
