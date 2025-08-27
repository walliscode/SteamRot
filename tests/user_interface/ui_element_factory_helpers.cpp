/////////////////////////////////////////////////
/// @file
/// @brief Implementation of helper functions for UIElementFactory unit tests
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ui_element_factory_helpers.h"

namespace steamrot {
namespace tests {
/////////////////////////////////////////////////

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
} // namespace tests
} // namespace steamrot
