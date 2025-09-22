/////////////////////////////////////////////////
/// @file
/// @brief Unit tests for the UIElementFactory class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "UIElementFactory.h"
#include "ButtonElement.h"
#include "PanelElement.h"
#include "TestContext.h"
#include "TestUIElementDataProvider.h"
#include "catch2/catch_test_macros.hpp"
#include "ui_element_factory_helpers.h"
#include "user_interface_generated.h"
#include <iostream>
#include <memory>
#include <string>

using namespace steamrot::tests;

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

  // create test context
  steamrot::tests::TestContext test_context;
  // check that the map from the EventHandler is empty
  REQUIRE(test_context.GetGameContext()
              .event_handler.GetSubcriberRegister()
              .size() == 0);

  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *panel_data =
      TestUIElementDataFactory::CreateTestPanelData(builder);
  REQUIRE(panel_data != nullptr);

  // act
  auto element_result = CreateUIElement(
      steamrot::UIElementDataUnion::UIElementDataUnion_PanelData, panel_data,
      test_context.GetGameContext().event_handler);
  if (!element_result.has_value()) {
    FAIL(element_result.error().message);
  }
  // assert
  REQUIRE(test_context.GetGameContext()
              .event_handler.GetSubcriberRegister()
              .size() == 1);

  // pull out as PanelElement
  auto panel_element =
      dynamic_cast<steamrot::PanelElement *>(element_result.value().get());
  // ensure it is not null
  REQUIRE(panel_element != nullptr);
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

  steamrot::tests::TestContext test_context;
  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *button_data =
      TestUIElementDataFactory::CreateTestButtonData(builder, "TestButton");
  REQUIRE(button_data != nullptr);
  std::cout << "Creating UI Element" << std::endl;
  auto element_result = CreateUIElement(
      steamrot::UIElementDataUnion::UIElementDataUnion_ButtonData, button_data,
      test_context.GetGameContext().event_handler);

  if (!element_result.has_value()) {
    FAIL(element_result.error().message);
  }

  std::cout << "Attempting t o dynamic cast to ButtonElement" << std::endl;

  auto button_element =
      dynamic_cast<steamrot::ButtonElement *>(element_result.value().get());

  // ensure it is not null
  REQUIRE(button_element != nullptr);
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
  steamrot::tests::TestContext test_context;
  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *ddlist_data =
      TestUIElementDataFactory::CreateTestDropDownListData(builder, "TestList",
                                                           "ExpandedLabel");
  REQUIRE(ddlist_data != nullptr);

  auto element_result = CreateUIElement(
      steamrot::UIElementDataUnion::UIElementDataUnion_DropDownListData,
      ddlist_data, test_context.GetGameContext().event_handler);
  if (!element_result.has_value()) {
    FAIL(element_result.error().message);
  }

  auto ddlist_element = dynamic_cast<steamrot::DropDownListElement *>(
      element_result.value().get());
  // ensure it is not null
  REQUIRE(ddlist_element != nullptr);
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
  steamrot::tests::TestContext test_context;
  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *ddcontainer_data =
      TestUIElementDataFactory::CreateTestDropDownContainerData(builder);
  REQUIRE(ddcontainer_data != nullptr);

  auto element_result = CreateUIElement(
      steamrot::UIElementDataUnion::UIElementDataUnion_DropDownContainerData,
      ddcontainer_data, test_context.GetGameContext().event_handler);
  if (!element_result.has_value()) {
    FAIL(element_result.error().message);
  }

  auto ddcontainer_element = dynamic_cast<steamrot::DropDownContainerElement *>(
      element_result.value().get());

  // ensure it is not null
  REQUIRE(ddcontainer_element != nullptr);
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
  steamrot::tests::TestContext test_context;
  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *dditem_data =
      TestUIElementDataFactory::CreateTestDropDownItemData(builder, "TestItem");
  REQUIRE(dditem_data != nullptr);

  auto element_result = CreateUIElement(
      steamrot::UIElementDataUnion::UIElementDataUnion_DropDownItemData,
      dditem_data, test_context.GetGameContext().event_handler);
  if (!element_result.has_value()) {
    FAIL(element_result.error().message);
  }

  auto dditem_element = dynamic_cast<steamrot::DropDownItemElement *>(
      element_result.value().get());
  REQUIRE(dditem_element != nullptr);
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
  steamrot::tests::TestContext test_context;
  flatbuffers::FlatBufferBuilder builder{1024};
  const auto *ddbutton_data =
      TestUIElementDataFactory::CreateTestDropDownButtonData(builder, true);
  REQUIRE(ddbutton_data != nullptr);

  auto element_result = CreateUIElement(
      steamrot::UIElementDataUnion::UIElementDataUnion_DropDownButtonData,
      ddbutton_data, test_context.GetGameContext().event_handler);
  if (!element_result.has_value()) {
    FAIL(element_result.error().message);
  }

  auto ddbutton_element = dynamic_cast<steamrot::DropDownButtonElement *>(
      element_result.value().get());
  REQUIRE(ddbutton_element != nullptr);
  steamrot::tests::TestDropDownButtonElementProperties(*ddbutton_element,
                                                       *ddbutton_data);
  steamrot::tests::TestUIELementProperites(*ddbutton_element,
                                           *ddbutton_data->base_data());
}
TEST_CASE("UIElementFactory::CreateUIElement - Deeply Nested Panel",
          "[UIElementFactory][nested]") {
  steamrot::tests::TestContext test_context;
  flatbuffers::FlatBufferBuilder builder{4096};

  // Create deeply nested PanelData (with DropDownContainer, DropDownList,
  // Button, DropDownItem, DropDownButton)
  const auto *nested_panel_data =
      TestUIElementDataFactory::CreateDeeplyNestedTestPanel(builder);
  REQUIRE(nested_panel_data != nullptr);

  // act: create the element using the factory
  auto element_result = CreateUIElement(
      steamrot::UIElementDataUnion::UIElementDataUnion_PanelData,
      nested_panel_data, test_context.GetGameContext().event_handler);
  if (!element_result.has_value()) {
    FAIL(element_result.error().message);
  }

  // assert: check that we got a PanelElement with nested structure
  auto panel_element =
      dynamic_cast<steamrot::PanelElement *>(element_result.value().get());
  REQUIRE(panel_element != nullptr);

  // Deep recursive test: check all nested elements match their data
  // This uses the helpers discussed in previous answers.
  steamrot::tests::TestNestedElementProperties(
      *panel_element, nested_panel_data,
      steamrot::UIElementDataUnion::UIElementDataUnion_PanelData);
}
