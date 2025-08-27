/////////////////////////////////////////////////
/// @file
/// @brief Implementation of helper functions for UIElementFactory unit tests
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ui_element_factory_helpers.h"
#include <string>

namespace steamrot {
namespace tests {

void TestUIELementProperites(const UIElement &element,
                             const UIElementData &data) {
  // Check position
  REQUIRE(element.position.x == data.position()->x());
  REQUIRE(element.position.y == data.position()->y());
  // Check size
  REQUIRE(element.size.x == data.size()->x());
  REQUIRE(element.size.y == data.size()->y());
  // Check spacing strategy
  REQUIRE(element.spacing_strategy == data.spacing_strategy());
  // Check layout
  REQUIRE(element.layout == data.layout());
  // Check children_active
  REQUIRE(element.children_active == data.children_active());
}

void TestPanelElementProperties(const PanelElement &element,
                                const PanelData &data) {
  // Currently no specific properties to test
  SUCCEED("PanelElement currently has no specific properties to test.");
}

void TestButtonElementProperties(const ButtonElement &element,
                                 const ButtonData &data) {
  REQUIRE(element.label == (data.label() ? data.label()->str() : ""));
}

void TestDropDownListElementProperties(const DropDownListElement &element,
                                       const DropDownListData &data) {
  REQUIRE(element.unexpanded_label ==
          (data.label() ? data.label()->str() : ""));
  REQUIRE(element.expanded_label ==
          (data.expanded_label() ? data.expanded_label()->str() : ""));
  REQUIRE(element.data_populate_function == data.data_populate_function());
}

void TestDropDownContainerElementProperties(
    const DropDownContainerElement &element,
    const DropDownContainerData &data) {
  // No specific properties for DropDownContainerElement
  SUCCEED(
      "DropDownContainerElement currently has no specific properties to test.");
}

void TestDropDownItemElementProperties(const DropDownItemElement &element,
                                       const DropDownItemData &data) {
  REQUIRE(element.label == (data.label() ? data.label()->str() : ""));
}

void TestDropDownButtonElementProperties(const DropDownButtonElement &element,
                                         const DropDownButtonData &data) {
  REQUIRE(element.is_expanded == data.is_expanded());
}

} // namespace tests
} // namespace steamrot
