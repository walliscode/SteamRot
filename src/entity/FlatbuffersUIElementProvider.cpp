////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of FlatbuffersUIElementProvider class
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "FlatbuffersUIElementProvider.h"
#include "ButtonElement.h"
#include "DropDownButtonElement.h"
#include "DropDownContainerElement.h"
#include "DropDownItemElement.h"
#include "DropDownListElement.h"
#include "PanelElement.h"
#include "configure_ui_elements.h"
#include "user_interface_generated.h"
#include <spdlog/spdlog.h>
#include <string>

namespace steamrot {

////////////////////////////////////////////////////////////
FlatbuffersUIElementProvider::FlatbuffersUIElementProvider(
    EventHandler &event_handler, const UserInterfaceFbs &ui_data)
    : IUIElementProvider(event_handler), m_ui_data(ui_data) {}

////////////////////////////////////////////////////////////
std::expected<std::unique_ptr<UIElement>, FailInfo>
FlatbuffersUIElementProvider::CreateRootUIElement() {
  if (!m_ui_data.root_ui_element()) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "UserInterfaceData missing root_ui_element"});
  }

  return CreateUIElement(UIElementDataUnion::UIElementDataUnion_PanelData,
                         m_ui_data.root_ui_element());
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
FlatbuffersUIElementProvider::ConfigureRootUIElement(UIElement &root_element) {
  if (!m_ui_data.root_ui_element()) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "UserInterfaceData missing root_ui_element"});
  }

  if (!m_ui_data.root_ui_element()->base_data()) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "UserInterfaceData root_ui_element missing base_data"});
  }

  // Create a callback that captures 'this' and calls CreateUIElement
  auto create_callback = [this](const UIElementDataUnion &data_type,
                                const void *data)
      -> std::expected<std::unique_ptr<UIElement>, FailInfo> {
    return this->CreateUIElement(data_type, data);
  };

  return data::configure::ConfigureBaseUIElement(
      root_element, *m_ui_data.root_ui_element()->base_data(),
      m_event_handler, create_callback);
}

////////////////////////////////////////////////////////////
std::expected<std::unique_ptr<UIElement>, FailInfo>
FlatbuffersUIElementProvider::CreateUIElement(
    const UIElementDataUnion &data_type, const void *data) {

  std::unique_ptr<UIElement> element{nullptr};
  const UIElementData *base_data = nullptr;

  switch (data_type) {
  case UIElementDataUnion::UIElementDataUnion_PanelData: {
    auto panel_data = static_cast<const PanelData *>(data);
    auto panel = std::make_unique<PanelElement>();
    auto config_result =
        data::configure::ConfigurePanelElement(*panel, *panel_data);
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    element = std::move(panel);
    base_data = panel_data->base_data();
    break;
  }
  case UIElementDataUnion::UIElementDataUnion_ButtonData: {
    auto button_data = static_cast<const ButtonData *>(data);
    auto button = std::make_unique<ButtonElement>();
    
    spdlog::debug("[CreateUIElement] Creating ButtonElement at {} with label: '{}'", 
                  static_cast<const void*>(button.get()),
                  button_data->label() ? button_data->label()->c_str() : "NO_LABEL");
    
    auto config_result =
        data::configure::ConfigureButtonElement(*button, *button_data);
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    element = std::move(button);
    base_data = button_data->base_data();
    break;
  }
  case UIElementDataUnion::UIElementDataUnion_DropDownListData: {
    auto ddlist_data = static_cast<const DropDownListData *>(data);
    auto ddlist = std::make_unique<DropDownListElement>();
    auto config_result =
        data::configure::ConfigureDropDownListElement(*ddlist, *ddlist_data);
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    element = std::move(ddlist);
    base_data = ddlist_data->base_data();
    break;
  }
  case UIElementDataUnion::UIElementDataUnion_DropDownContainerData: {
    auto ddcont_data = static_cast<const DropDownContainerData *>(data);
    auto ddcont = std::make_unique<DropDownContainerElement>();
    auto config_result = data::configure::ConfigureDropDownContainerElement(
        *ddcont, *ddcont_data);
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    element = std::move(ddcont);
    base_data = ddcont_data->base_data();
    break;
  }
  case UIElementDataUnion::UIElementDataUnion_DropDownItemData: {
    auto dditem_data = static_cast<const DropDownItemData *>(data);
    auto dditem = std::make_unique<DropDownItemElement>();
    auto config_result =
        data::configure::ConfigureDropDownItemElement(*dditem, *dditem_data);
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    element = std::move(dditem);
    base_data = dditem_data->base_data();
    break;
  }
  case UIElementDataUnion::UIElementDataUnion_DropDownButtonData: {
    auto ddbtn_data = static_cast<const DropDownButtonData *>(data);
    auto ddbtn = std::make_unique<DropDownButtonElement>();
    auto config_result =
        data::configure::ConfigureDropDownButtonElement(*ddbtn, *ddbtn_data);
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

  // Only call this once! for configuring the base data
  if (base_data) {
    spdlog::debug("[CreateUIElement] Configuring base data for element at {}", 
                  static_cast<const void*>(element.get()));
    
    // Create a callback that captures 'this' and calls CreateUIElement
    auto create_callback = [this](const UIElementDataUnion &data_type,
                                  const void *data)
        -> std::expected<std::unique_ptr<UIElement>, FailInfo> {
      return this->CreateUIElement(data_type, data);
    };

    auto base_config_result = data::configure::ConfigureBaseUIElement(
        *element, *base_data, m_event_handler, create_callback);
    if (!base_config_result.has_value())
      return std::unexpected(base_config_result.error());
      
    spdlog::debug("[CreateUIElement] After ConfigureBaseUIElement - element at {} has response_event: {}", 
                  static_cast<const void*>(element.get()),
                  element->response_event.has_value());
  }

  // return unexpected if element is still null
  if (!element) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "CreateUIElement: Element creation failed, element is null."});
  }

  return element;
}

} // namespace steamrot
