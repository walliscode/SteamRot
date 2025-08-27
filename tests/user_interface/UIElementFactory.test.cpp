/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the UIElementFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIElementFactory.h"
#include "TestUIElementDataProvider.h"
#include "UIElement.h"
#include "catch2/catch_test_macros.hpp"
#include "ui_element_factory_helpers.h"
#include "user_interface_generated.h"
#include <memory>

using namespace steamrot::tests;

TEST_CASE("UIElementFactory::ConfigureBaseUIElement", "[UIElementFactory]") {
  flatbuffers::FlatBufferBuilder builder{1024};
  // Create base UIElementData (do NOT finish buffer in factory)
  auto base_element_offset =
      TestUIElementDataFactory::CreateTestUIElementData(builder);

  // Finish the buffer in the test
  builder.Finish(base_element_offset);
  const auto *base_element_data =
      flatbuffers::GetRoot<steamrot::UIElementData>(builder.GetBufferPointer());
  REQUIRE(base_element_data != nullptr);

  steamrot::UIElement element;
  auto result = steamrot::ConfigureBaseUIElement(element, *base_element_data);
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  // Now you can call helper functions to test contents
  steamrot::tests::TestUIELementProperites(element, *base_element_data);
}

TEST_CASE("UIElementFactory::ConfigurePanelElement", "[UIElementFactory]") {
  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *panel_data =
      TestUIElementDataFactory::CreateTestPanelData(builder);
  REQUIRE(panel_data != nullptr);

  steamrot::PanelElement panel_element;
  auto result = steamrot::ConfigurePanelElement(panel_element, *panel_data);
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  // test contents with helper function
  steamrot::tests::TestPanelElementProperties(panel_element, *panel_data);
}

// TEST_CASE("UIElementFactory::ConfigureButtonElement", "[UIElementFactory]") {
//   flatbuffers::FlatBufferBuilder builder{1024};
//   const auto *button_data =
//       TestUIElementDataFactory::CreateTestButtonData(builder);
//   REQUIRE(button_data != nullptr);
//
//   steamrot::ButtonElement button_element;
//   auto result = steamrot::ConfigureButtonElement(button_element,
//   *button_data); if (!result.has_value()) {
//     FAIL(result.error().message);
//   }
//   SUCCEED("ButtonElement constructed and configured.");
// }
//
TEST_CASE("UIElementFactory::CreateUIElement - Panel", "[UIElementFactory]") {
  // arrange
  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *panel_data =
      TestUIElementDataFactory::CreateTestPanelData(builder);
  REQUIRE(panel_data != nullptr);

  // act
  auto element_result = CreateUIElement(
      steamrot::UIElementDataUnion::UIElementDataUnion_PanelData, panel_data);
  if (!element_result.has_value()) {
    FAIL(element_result.error().message);
  }
  // assert
  // check that it produces a PanelElement
  REQUIRE(dynamic_cast<steamrot::PanelElement *>(
              element_result.value().get()) != nullptr);
  // pull out as PanelElement
  auto panel_element =
      dynamic_cast<steamrot::PanelElement *>(element_result.value().get());
  // test contents with helper function
  steamrot::tests::TestPanelElementProperties(*panel_element, *panel_data);
  // test base properties too
  steamrot::tests::TestUIELementProperites(*panel_element,
                                           *panel_data->base_data());
}
