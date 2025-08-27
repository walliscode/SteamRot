///////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIElementFactory class
/////////////////////////////////////////////////
#include "UIElementFactory.h"
#include "FailInfo.h"
#include "PanelElement.h"
#include "user_interface_generated.h"
#include <expected>

namespace steamrot {
/////////////////////////////////////////////////
std::expected<UIElement, FailInfo>
UIElementFactory::CreateUIStructure(const UIElementData &element_data) {
  return UIElement{};
}

/////////////////////////////////////////////////
std::expected<std::unique_ptr<UIElement>, FailInfo>
CreateUIElement(const UIElementDataUnion &data_type, const void *data) {

  // create return pointer
  std::unique_ptr<UIElement> element;

  // use flatbuffers union to determine type and create specific element
  switch (data_type) {
  case UIElementDataUnion::UIElementDataUnion_PanelData: {
    // cast data to specific type base pointer
    auto panel_data = static_cast<const PanelData *>(data);
    auto panel = std::make_unique<PanelElement>();
    auto config_result = ConfigurePanelElement(*panel, *panel_data);
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    element = std::move(panel); // Move to base pointer
    break;
  }

  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "CreateUIElement: Unsupported UI element type in union."});
  }

  // Configure base properties
  auto base_config_result = ConfigureBaseUIElement(
      *element, *(static_cast<const UIElementData *>(data)));

  return element;
}
/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureBaseUIElement(UIElement &element, const UIElementData &data) {

  element.position = sf::Vector2f({data.position()->x(), data.position()->y()});
  element.size = sf::Vector2f({data.size()->x(), data.size()->y()});
  element.spacing_strategy = data.spacing_strategy();
  element.layout = data.layout();
  element.children_active = data.children_active();

  return std::monostate{};
};

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigurePanelElement(PanelElement &panel_element, const PanelData &data) {
  return std::monostate{};
};
} // namespace steamrot
