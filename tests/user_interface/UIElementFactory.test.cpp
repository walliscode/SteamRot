/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the UIElementFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIElementFactory.h"
#include "ButtonElement.h"
#include "DropDownElement.h"
#include "PanelElement.h"
#include "TestUIElementDataProvider.h"
#include "UIElement.h"
#include "catch2/catch_test_macros.hpp"
#include "ui_element_factory_helpers.h"
#include "user_interface_generated.h"
#include <memory>
#include <string>

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

TEST_CASE("UIElementFactory::ConfigureButtonElement", "[UIElementFactory]") {
  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *button_data =
      TestUIElementDataFactory::CreateTestButtonData(builder, "TestButton");
  REQUIRE(button_data != nullptr);

  steamrot::ButtonElement button_element;
  auto result = steamrot::ConfigureButtonElement(button_element, *button_data);
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  steamrot::tests::TestButtonElementProperties(button_element, *button_data);
}

TEST_CASE("UIElementFactory::CreateUIElement - Button", "[UIElementFactory]") {
  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *button_data =
      TestUIElementDataFactory::CreateTestButtonData(builder, "TestButton");
  REQUIRE(button_data != nullptr);

  auto element_result = CreateUIElement(
      steamrot::UIElementDataUnion::UIElementDataUnion_ButtonData, button_data);
  if (!element_result.has_value()) {
    FAIL(element_result.error().message);
  }
  REQUIRE(dynamic_cast<steamrot::ButtonElement *>(
              element_result.value().get()) != nullptr);
  auto button_element =
      dynamic_cast<steamrot::ButtonElement *>(element_result.value().get());
  steamrot::tests::TestButtonElementProperties(*button_element, *button_data);
  steamrot::tests::TestUIELementProperites(*button_element,
                                           *button_data->base_data());
}

TEST_CASE("UIElementFactory::ConfigureDropDownListElement",
          "[UIElementFactory]") {
  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *ddlist_data =
      TestUIElementDataFactory::CreateTestDropDownListData(builder, "TestList",
                                                           "ExpandedLabel");
  REQUIRE(ddlist_data != nullptr);

  steamrot::DropDownListElement ddlist_element;
  auto result =
      steamrot::ConfigureDropDownListElement(ddlist_element, *ddlist_data);
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  steamrot::tests::TestDropDownListElementProperties(ddlist_element,
                                                     *ddlist_data);
}

TEST_CASE("UIElementFactory::CreateUIElement - DropDownList",
          "[UIElementFactory]") {
  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *ddlist_data =
      TestUIElementDataFactory::CreateTestDropDownListData(builder, "TestList",
                                                           "ExpandedLabel");
  REQUIRE(ddlist_data != nullptr);

  auto element_result = CreateUIElement(
      steamrot::UIElementDataUnion::UIElementDataUnion_DropDownListData,
      ddlist_data);
  if (!element_result.has_value()) {
    FAIL(element_result.error().message);
  }
  REQUIRE(dynamic_cast<steamrot::DropDownListElement *>(
              element_result.value().get()) != nullptr);
  auto ddlist_element = dynamic_cast<steamrot::DropDownListElement *>(
      element_result.value().get());
  steamrot::tests::TestDropDownListElementProperties(*ddlist_element,
                                                     *ddlist_data);
  steamrot::tests::TestUIELementProperites(*ddlist_element,
                                           *ddlist_data->base_data());
}

TEST_CASE("UIElementFactory::ConfigureDropDownContainerElement",
          "[UIElementFactory]") {
  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *ddcontainer_data =
      TestUIElementDataFactory::CreateTestDropDownContainerData(builder);
  REQUIRE(ddcontainer_data != nullptr);

  steamrot::DropDownContainerElement ddcontainer_element;
  auto result = steamrot::ConfigureDropDownContainerElement(ddcontainer_element,
                                                            *ddcontainer_data);
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  steamrot::tests::TestDropDownContainerElementProperties(ddcontainer_element,
                                                          *ddcontainer_data);
}

TEST_CASE("UIElementFactory::CreateUIElement - DropDownContainer",
          "[UIElementFactory]") {
  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *ddcontainer_data =
      TestUIElementDataFactory::CreateTestDropDownContainerData(builder);
  REQUIRE(ddcontainer_data != nullptr);

  auto element_result = CreateUIElement(
      steamrot::UIElementDataUnion::UIElementDataUnion_DropDownContainerData,
      ddcontainer_data);
  if (!element_result.has_value()) {
    FAIL(element_result.error().message);
  }
  REQUIRE(dynamic_cast<steamrot::DropDownContainerElement *>(
              element_result.value().get()) != nullptr);
  auto ddcontainer_element = dynamic_cast<steamrot::DropDownContainerElement *>(
      element_result.value().get());
  steamrot::tests::TestDropDownContainerElementProperties(*ddcontainer_element,
                                                          *ddcontainer_data);
  steamrot::tests::TestUIELementProperites(*ddcontainer_element,
                                           *ddcontainer_data->base_data());
}

TEST_CASE("UIElementFactory::ConfigureDropDownItemElement",
          "[UIElementFactory]") {
  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *dditem_data =
      TestUIElementDataFactory::CreateTestDropDownItemData(builder, "TestItem");
  REQUIRE(dditem_data != nullptr);

  steamrot::DropDownItemElement dditem_element;
  auto result =
      steamrot::ConfigureDropDownItemElement(dditem_element, *dditem_data);
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  steamrot::tests::TestDropDownItemElementProperties(dditem_element,
                                                     *dditem_data);
}

TEST_CASE("UIElementFactory::CreateUIElement - DropDownItem",
          "[UIElementFactory]") {
  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *dditem_data =
      TestUIElementDataFactory::CreateTestDropDownItemData(builder, "TestItem");
  REQUIRE(dditem_data != nullptr);

  auto element_result = CreateUIElement(
      steamrot::UIElementDataUnion::UIElementDataUnion_DropDownItemData,
      dditem_data);
  if (!element_result.has_value()) {
    FAIL(element_result.error().message);
  }
  REQUIRE(dynamic_cast<steamrot::DropDownItemElement *>(
              element_result.value().get()) != nullptr);
  auto dditem_element = dynamic_cast<steamrot::DropDownItemElement *>(
      element_result.value().get());
  steamrot::tests::TestDropDownItemElementProperties(*dditem_element,
                                                     *dditem_data);
  steamrot::tests::TestUIELementProperites(*dditem_element,
                                           *dditem_data->base_data());
}

TEST_CASE("UIElementFactory::ConfigureDropDownButtonElement",
          "[UIElementFactory]") {
  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *ddbutton_data =
      TestUIElementDataFactory::CreateTestDropDownButtonData(builder, true);
  REQUIRE(ddbutton_data != nullptr);

  steamrot::DropDownButtonElement ddbutton_element;
  auto result = steamrot::ConfigureDropDownButtonElement(ddbutton_element,
                                                         *ddbutton_data);
  if (!result.has_value()) {
    FAIL(result.error().message);
  }
  steamrot::tests::TestDropDownButtonElementProperties(ddbutton_element,
                                                       *ddbutton_data);
}

TEST_CASE("UIElementFactory::CreateUIElement - DropDownButton",
          "[UIElementFactory]") {
  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *ddbutton_data =
      TestUIElementDataFactory::CreateTestDropDownButtonData(builder, true);
  REQUIRE(ddbutton_data != nullptr);

  auto element_result = CreateUIElement(
      steamrot::UIElementDataUnion::UIElementDataUnion_DropDownButtonData,
      ddbutton_data);
  if (!element_result.has_value()) {
    FAIL(element_result.error().message);
  }
  REQUIRE(dynamic_cast<steamrot::DropDownButtonElement *>(
              element_result.value().get()) != nullptr);
  auto ddbutton_element = dynamic_cast<steamrot::DropDownButtonElement *>(
      element_result.value().get());
  steamrot::tests::TestDropDownButtonElementProperties(*ddbutton_element,
                                                       *ddbutton_data);
  steamrot::tests::TestUIELementProperites(*ddbutton_element,
                                           *ddbutton_data->base_data());
}
