///////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIElementFactory class
///////////////////////////////////////////////////////////////////////////////
#include "UIElementFactory.h"
#include "FailInfo.h"
#include "user_interface_generated.h"
#include <expected>
#include <string>

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
    base_data = panel_data->base_data();
    break;
  }
  case UIElementDataUnion::UIElementDataUnion_ButtonData: {
    auto button_data = static_cast<const ButtonData *>(data);
    auto button = std::make_unique<ButtonElement>();
    auto config_result = ConfigureButtonElement(*button, *button_data);
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    element = std::move(button);
    base_data = button_data->base_data();
    break;
  }
  case UIElementDataUnion::UIElementDataUnion_DropDownListData: {
    auto ddlist_data = static_cast<const DropDownListData *>(data);
    auto ddlist = std::make_unique<DropDownListElement>();
    auto config_result = ConfigureDropDownListElement(*ddlist, *ddlist_data);
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    element = std::move(ddlist);
    base_data = ddlist_data->base_data();
    break;
  }
  case UIElementDataUnion::UIElementDataUnion_DropDownContainerData: {
    auto ddcont_data = static_cast<const DropDownContainerData *>(data);
    auto ddcont = std::make_unique<DropDownContainerElement>();
    auto config_result =
        ConfigureDropDownContainerElement(*ddcont, *ddcont_data);
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    element = std::move(ddcont);
    base_data = ddcont_data->base_data();
    break;
  }
  case UIElementDataUnion::UIElementDataUnion_DropDownItemData: {
    auto dditem_data = static_cast<const DropDownItemData *>(data);
    auto dditem = std::make_unique<DropDownItemElement>();
    auto config_result = ConfigureDropDownItemElement(*dditem, *dditem_data);
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    element = std::move(dditem);
    base_data = dditem_data->base_data();
    break;
  }
  case UIElementDataUnion::UIElementDataUnion_DropDownButtonData: {
    auto ddbtn_data = static_cast<const DropDownButtonData *>(data);
    auto ddbtn = std::make_unique<DropDownButtonElement>();
    auto config_result = ConfigureDropDownButtonElement(*ddbtn, *ddbtn_data);
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    element = std::move(ddbtn);
    base_data = ddbtn_data->base_data();
    break;
  }
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

  // Recursively create and attach children
  if (data.children()) {
    for (auto child_fb : *data.children()) {
      if (!child_fb)
        continue;
      auto type = child_fb->element_type();
      auto child_table = child_fb->element();
      if (!child_table)
        continue;
      auto child_element_result = CreateUIElement(type, child_table);
      if (!child_element_result.has_value())
        return std::unexpected(child_element_result.error());
      element.child_elements.push_back(std::move(child_element_result.value()));
    }
  }

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigurePanelElement(PanelElement &panel_element, const PanelData &data) {
  // No extra fields for panel
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureButtonElement(ButtonElement &button_element, const ButtonData &data) {
  if (data.label()) {
    button_element.label = data.label()->str();
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureDropDownListElement(DropDownListElement &dropdown_list_element,
                             const DropDownListData &data) {
  if (data.label()) {
    dropdown_list_element.unexpanded_label = data.label()->str();
  }
  if (data.expanded_label()) {
    dropdown_list_element.expanded_label = data.expanded_label()->str();
  }
  dropdown_list_element.data_populate_function = data.data_populate_function();
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureDropDownContainerElement(
    DropDownContainerElement &dropdown_container_element,
    const DropDownContainerData &data) {
  // No extra fields for container
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureDropDownItemElement(DropDownItemElement &dropdown_item_element,
                             const DropDownItemData &data) {
  if (data.label()) {
    dropdown_item_element.label = data.label()->str();
  }
  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureDropDownButtonElement(DropDownButtonElement &dropdown_button_element,
                               const DropDownButtonData &data) {
  dropdown_button_element.is_expanded = data.is_expanded();
  return std::monostate{};
}

} // namespace steamrot
