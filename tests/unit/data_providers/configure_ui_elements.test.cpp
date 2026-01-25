/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for configure_ui_elements functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "configure_ui_elements.h"
#include "ButtonElement.h"
#include "DropDownButtonElement.h"
#include "DropDownContainerElement.h"
#include "DropDownItemElement.h"
#include "DropDownListElement.h"
#include "PanelElement.h"
#include "user_interface_generated.h"
#include <catch2/catch_test_macros.hpp>
#include <fstream>

std::pair<std::unique_ptr<char[]>, const steamrot::UserInterfaceFbs *>
LoadTestData(const std::string &filename) {
  std::filesystem::path test_file_path = __FILE__;
  std::filesystem::path data_dir = test_file_path.parent_path() / "data";
  std::filesystem::path bin_file_path = data_dir / filename;

  std::ifstream infile(bin_file_path, std::ios::binary | std::ios::in);
  if (!infile.is_open()) {
    throw std::runtime_error("Failed to open file: " + bin_file_path.string());
  }

  infile.seekg(0, std::ios::end);
  auto length = infile.tellg();
  if (length <= 0) {
    throw std::runtime_error("Empty or invalid file: " +
                             bin_file_path.string());
  }

  infile.seekg(0, std::ios::beg);
  auto data = std::make_unique<char[]>(static_cast<size_t>(length));
  infile.read(data.get(), length);
  infile.close();

  const steamrot::UserInterfaceFbs *ui_element_data =
      steamrot::GetUserInterfaceFbs(data.get());

  return {std::move(data), ui_element_data};
}

std::pair<std::unique_ptr<char[]>, const steamrot::UserInterfaceFbs *>
LoadUIElementTestData() {
  return LoadTestData("ui_element_test_data.bin");
}

TEST_CASE("ConvertLayout converts FlatBuffers layout correctly",
          "[unit][configure_ui_elements]") {
  REQUIRE(steamrot::data::configure::ConvertLayout(steamrot::LayoutFbs_None) ==
          steamrot::Layout::None);
  REQUIRE(steamrot::data::configure::ConvertLayout(
              steamrot::LayoutFbs_Horizontal) == steamrot::Layout::Horizontal);
  REQUIRE(steamrot::data::configure::ConvertLayout(
              steamrot::LayoutFbs_Vertical) == steamrot::Layout::Vertical);
  REQUIRE(steamrot::data::configure::ConvertLayout(steamrot::LayoutFbs_Grid) ==
          steamrot::Layout::Grid);
  REQUIRE(steamrot::data::configure::ConvertLayout(
              steamrot::LayoutFbs_DropDown) == steamrot::Layout::DropDown);
}

TEST_CASE("ConvertSpacingAndSizing converts FlatBuffers spacing correctly",
          "[unit][configure_ui_elements]") {
  REQUIRE(steamrot::data::configure::ConvertSpacingAndSizing(
              steamrot::SpacingAndSizingFbs_None) ==
          steamrot::SpacingAndSizing::None);
  REQUIRE(steamrot::data::configure::ConvertSpacingAndSizing(
              steamrot::SpacingAndSizingFbs_Even) ==
          steamrot::SpacingAndSizing::Even);
  REQUIRE(steamrot::data::configure::ConvertSpacingAndSizing(
              steamrot::SpacingAndSizingFbs_Ratioed) ==
          steamrot::SpacingAndSizing::Ratioed);
  REQUIRE(steamrot::data::configure::ConvertSpacingAndSizing(
              steamrot::SpacingAndSizingFbs_DropDownList) ==
          steamrot::SpacingAndSizing::DropDownList);
}

TEST_CASE("ConfigurePanelElement handles basic configuration",
          "[unit][configure_ui_elements]") {
  auto [data, ui_element_data] = LoadUIElementTestData();
  REQUIRE(ui_element_data != nullptr);

  auto panel_data = ui_element_data->root_ui_element();
  REQUIRE(panel_data != nullptr);

  steamrot::PanelElement panel_element;
  auto result = steamrot::data::configure::ConfigurePanelElement(panel_element,
                                                                 *panel_data);
  REQUIRE(result.has_value());
}

TEST_CASE("ConfigureButtonElement configures button label",
          "[unit][configure_ui_elements]") {
  auto [data, ui_element_data] = LoadUIElementTestData();
  REQUIRE(ui_element_data != nullptr);
  REQUIRE(ui_element_data->root_ui_element()->base_data()->children()->size() ==
          5);

  auto children = ui_element_data->root_ui_element()->base_data()->children();
  auto button_fb = children->Get(0);
  REQUIRE(button_fb != nullptr);
  REQUIRE(button_fb->element_type() ==
          steamrot::UIElementDataUnion::UIElementDataUnion_ButtonData);

  auto button_data =
      static_cast<const steamrot::ButtonData *>(button_fb->element());
  REQUIRE(button_data != nullptr);

  steamrot::ButtonElement button_element;
  auto result = steamrot::data::configure::ConfigureButtonElement(
      button_element, *button_data);
  REQUIRE(result.has_value());
  REQUIRE(button_element.label == "Test Tab");
}

TEST_CASE("ConfigureDropDownListElement configures labels",
          "[unit][configure_ui_elements]") {
  auto [data, ui_element_data] = LoadUIElementTestData();
  REQUIRE(ui_element_data != nullptr);

  auto children = ui_element_data->root_ui_element()->base_data()->children();
  auto ddlist_fb = children->Get(1);
  REQUIRE(ddlist_fb != nullptr);
  REQUIRE(ddlist_fb->element_type() ==
          steamrot::UIElementDataUnion::UIElementDataUnion_DropDownListData);

  auto ddlist_data =
      static_cast<const steamrot::DropDownListData *>(ddlist_fb->element());
  REQUIRE(ddlist_data != nullptr);

  steamrot::DropDownListElement ddlist_element;
  auto result = steamrot::data::configure::ConfigureDropDownListElement(
      ddlist_element, *ddlist_data);
  REQUIRE(result.has_value());
  REQUIRE(ddlist_element.unexpanded_label == "Select Option");
  REQUIRE(ddlist_element.expanded_label == "Options:");
}

TEST_CASE("ConfigureDropDownItemElement configures label",
          "[unit][configure_ui_elements]") {
  auto [data, ui_element_data] = LoadUIElementTestData();
  REQUIRE(ui_element_data != nullptr);

  auto children = ui_element_data->root_ui_element()->base_data()->children();
  auto dditem_fb = children->Get(2);
  REQUIRE(dditem_fb != nullptr);
  REQUIRE(
      steamrot::EnumNameUIElementDataUnion(dditem_fb->element_type()) ==
      steamrot::EnumNameUIElementDataUnion(
          steamrot::UIElementDataUnion::UIElementDataUnion_DropDownItemData));

  auto dditem_data =
      static_cast<const steamrot::DropDownItemData *>(dditem_fb->element());
  REQUIRE(dditem_data != nullptr);

  steamrot::DropDownItemElement dditem_element;
  auto result = steamrot::data::configure::ConfigureDropDownItemElement(
      dditem_element, *dditem_data);
  REQUIRE(result.has_value());
  REQUIRE(dditem_element.label == "item...");
}

TEST_CASE("ConfigureDropDownButtonElement configures expanded state",
          "[unit][configure_ui_elements]") {
  auto [data, ui_element_data] = LoadUIElementTestData();
  REQUIRE(ui_element_data != nullptr);

  auto children = ui_element_data->root_ui_element()->base_data()->children();
  auto ddbtn_fb = children->Get(3);
  REQUIRE(ddbtn_fb != nullptr);
  REQUIRE(ddbtn_fb->element_type() ==
          steamrot::UIElementDataUnion::UIElementDataUnion_DropDownButtonData);

  auto ddbtn_data =
      static_cast<const steamrot::DropDownButtonData *>(ddbtn_fb->element());
  REQUIRE(ddbtn_data != nullptr);
  REQUIRE(ddbtn_data->is_expanded() == true);
  steamrot::DropDownButtonElement ddbtn_element;
  auto result = steamrot::data::configure::ConfigureDropDownButtonElement(
      ddbtn_element, *ddbtn_data);
  REQUIRE(result.has_value());
  REQUIRE(ddbtn_element.is_expanded == true);
}

TEST_CASE("ConfigureDropDownContainerElement validates children",
          "[unit][configure_ui_elements]") {
  auto [data, ui_element_data] = LoadUIElementTestData();
  REQUIRE(ui_element_data != nullptr);

  auto children = ui_element_data->root_ui_element()->base_data()->children();
  auto ddcont_fb = children->Get(4);
  REQUIRE(ddcont_fb != nullptr);
  REQUIRE(
      ddcont_fb->element_type() ==
      steamrot::UIElementDataUnion::UIElementDataUnion_DropDownContainerData);

  auto ddcont_data = static_cast<const steamrot::DropDownContainerData *>(
      ddcont_fb->element());
  REQUIRE(ddcont_data != nullptr);

  steamrot::DropDownContainerElement ddcont_element;
  auto result = steamrot::data::configure::ConfigureDropDownContainerElement(
      ddcont_element, *ddcont_data);

  if (!result.has_value()) {
    FAIL(result.error().message);
  }
}
