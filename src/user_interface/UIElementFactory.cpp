///////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIElementFactory class
/////////////////////////////////////////////////
#include "UIElementFactory.h"
#include "ButtonElement.h"
#include "FailInfo.h"
#include "PanelElement.h"
#include "user_interface_generated.h"
#include <expected>

namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::unique_ptr<UIElement>, FailInfo>
CreateUIElement(const UIElementDataUnion &data_type, const void *data) {
  // arrange
  std::unique_ptr<UIElement> element;
  const UIElementData *base_data = nullptr;

  switch (data_type) {
  case UIElementDataUnion::UIElementDataUnion_PanelData: {
    auto panel_data = static_cast<const PanelData *>(data);
    auto panel = std::make_unique<PanelElement>();
    auto config_result = ConfigurePanelElement(*panel, *panel_data);
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    element = std::move(panel);
    base_data = panel_data->base_data(); // <-- Assign base pointer
    break;
  }
  case UIElementDataUnion::UIElementDataUnion_ButtonData: {
    // auto button_data = static_cast<const ButtonData *>(data);
    // auto button = std::make_unique<ButtonElement>();
    // auto config_result = ConfigureButtonElement(*button, *button_data);
    // if (!config_result.has_value())
    //   return std::unexpected(config_result.error());
    // element = std::move(button);
    // base_data = button_data->base_data(); // <-- Assign base pointer
    break;
  }
  // ... other cases ...
  default:
    return std::unexpected(
        FailInfo{FailMode::NonExistentEnumValue,
                 "CreateUIElement: Unsupported UI element type in union."});
  }

  // Only call this once!
  if (base_data) {
    auto base_config_result = ConfigureBaseUIElement(*element, *base_data);
    if (!base_config_result.has_value())
      return std::unexpected(base_config_result.error());
  }

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
