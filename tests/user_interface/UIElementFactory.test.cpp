/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the UIElementFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIElementFactory.h"
#include "UIElement.h"
#include "types_generated.h"
#include "ui_element_factory_helpers.h"
#include "user_interface_generated.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UIElementFactory::ConfigureBaseUIElement", "[UIElementFactory]") {
  // Arrange
  flatbuffers::FlatBufferBuilder builder(1024);
  flatbuffers::Offset<Vector2fData> position =
      CreateVector2fData(builder, 10.0f, 20.0f);
  flatbuffers::Offset<Vector2fData> size =
      CreateVector2fData(builder, 100.0f, 200.0f);

  flatbuffers::Offset<steamrot::UIElementData> ui_element_data =
      CreateUIElementData(
          builder, position, size, true,
          steamrot::SpacingAndSizingType::SpacingAndSizingType_DropDownList,
          steamrot::LayoutType::LayoutType_Grid);

  builder.Finish(ui_element_data);
  auto *data_ptr = builder.GetBufferPointer();
  auto base_element_data =
      flatbuffers::GetRoot<steamrot::UIElementData>(data_ptr);

  steamrot::UIElement element;

  // Act
  auto result = steamrot::ConfigureBaseUIElement(element, *base_element_data);
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  steamrot::tests::TestUIELementProperites(element, *base_element_data);
}

TEST_CASE("UIElementFactory::ConfigurePanelElement", "[UIElementFactory]") {
  // Arrange
  flatbuffers::FlatBufferBuilder builder(1024);
  flatbuffers::Offset<Vector2fData> position =
      CreateVector2fData(builder, 10.0f, 20.0f);
  flatbuffers::Offset<Vector2fData> size =
      CreateVector2fData(builder, 100.0f, 200.0f);
  flatbuffers::Offset<steamrot::UIElementData> ui_element_data =
      CreateUIElementData(
          builder, position, size, true,
          steamrot::SpacingAndSizingType::SpacingAndSizingType_DropDownList,
          steamrot::LayoutType::LayoutType_Grid);
  flatbuffers::Offset<steamrot::PanelData> panel_data =
      CreatePanelData(builder, ui_element_data);
  builder.Finish(panel_data);
  auto *data_ptr = builder.GetBufferPointer();
  auto panel_data_ptr = flatbuffers::GetRoot<steamrot::PanelData>(data_ptr);
  steamrot::PanelElement panel_element;
  // Act
  auto result = steamrot::ConfigurePanelElement(panel_element, *panel_data_ptr);
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  SUCCEED("PanelElement currently has no specific properties to test.");
}

TEST_CASE("UIElementFactory::CreateUIElement - Panel", "[UIElementFactory]") {
  // Arrange
  flatbuffers::FlatBufferBuilder builder(1024);
  flatbuffers::Offset<Vector2fData> position =
      CreateVector2fData(builder, 10.0f, 20.0f);
  flatbuffers::Offset<Vector2fData> size =
      CreateVector2fData(builder, 100.0f, 200.0f);
  flatbuffers::Offset<steamrot::UIElementData> ui_element_data =
      CreateUIElementData(
          builder, position, size, true,
          steamrot::SpacingAndSizingType::SpacingAndSizingType_DropDownList,
          steamrot::LayoutType::LayoutType_Grid);
  flatbuffers::Offset<steamrot::PanelData> panel_data =
      CreatePanelData(builder, ui_element_data);
  flatbuffers::Offset<steamrot::UIElementData> root_ui_element_data =
      CreateUIElementData(
          builder, position, size, true,
          steamrot::SpacingAndSizingType::SpacingAndSizingType_DropDownList,
          steamrot::LayoutType::LayoutType_Grid);
  flatbuffers::Offset<steamrot::UIElementDataUnion> data_type =
      steamrot::CreateUIElementDataUnion(
          builder, steamrot::UIElementDataUnion_PanelData, panel_data.Union());
  builder.Finish(root_ui_element_data);
  auto *data_ptr = builder.GetBufferPointer();
  auto root_element_data =
      flatbuffers::GetRoot<steamrot::UIElementData>(data_ptr);
  // Act
  auto result = steamrot::CreateUIElement(*data_type, root_element_data);
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  auto element_ptr = std::move(result.value());
  REQUIRE(element_ptr != nullptr);
  REQUIRE(element_ptr->GetElementType() ==
          steamrot::UIElementDataUnion::UIElementDataUnion_PanelData);
  // Assert
  auto *panel_element =
      dynamic_cast<steamrot::PanelElement *>(element_ptr.get());
  REQUIRE(panel_element != nullptr);
  steamrot::tests::TestUIELementProperites(*panel_element, *root_element_data);
}
