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

  return CreateUIElement(UIElementDataUnionFbs::UIElementDataUnion_PanelData,
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
  auto create_callback = [this](const UIElementDataUnionFbs &data_type,
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
    const UIElementDataUnionFbs &data_type, const void *data) {

  std::unique_ptr<UIElement> element{nullptr};
  const UIElementDataFbs *base_data = nullptr;

  switch (data_type) {
  case UIElementDataUnionFbs::UIElementDataUnion_PanelData: {
    auto panel_data = static_cast<const PanelDataFbs *>(data);
    auto panel = std::make_unique<PanelElement>();
    auto config_result =
        data::configure::ConfigurePanelElement(*panel, *panel_data);
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    element = std::move(panel);
    base_data = panel_data->base_data();
    break;
  }
  case UIElementDataUnionFbs::UIElementDataUnion_ButtonData: {
    auto button_data = static_cast<const ButtonDataFbs *>(data);
    auto button = std::make_unique<ButtonElement>();
    auto config_result =
        data::configure::ConfigureButtonElement(*button, *button_data);
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    element = std::move(button);
    base_data = button_data->base_data();
    break;
  }
  case UIElementDataUnionFbs::UIElementDataUnion_DropDownListData: {
    auto ddlist_data = static_cast<const DropDownListDataFbs *>(data);
    auto ddlist = std::make_unique<DropDownListElement>();
    auto config_result =
        data::configure::ConfigureDropDownListElement(*ddlist, *ddlist_data);
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    element = std::move(ddlist);
    base_data = ddlist_data->base_data();
    break;
  }
  case UIElementDataUnionFbs::UIElementDataUnion_DropDownContainerData: {
    auto ddcont_data = static_cast<const DropDownContainerDataFbs *>(data);
    auto ddcont = std::make_unique<DropDownContainerElement>();
    auto config_result = data::configure::ConfigureDropDownContainerElement(
        *ddcont, *ddcont_data);
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    element = std::move(ddcont);
    base_data = ddcont_data->base_data();
    break;
  }
  case UIElementDataUnionFbs::UIElementDataUnion_DropDownItemData: {
    auto dditem_data = static_cast<const DropDownItemDataFbs *>(data);
    auto dditem = std::make_unique<DropDownItemElement>();
    auto config_result =
        data::configure::ConfigureDropDownItemElement(*dditem, *dditem_data);
    if (!config_result.has_value())
      return std::unexpected(config_result.error());
    element = std::move(dditem);
    base_data = dditem_data->base_data();
    break;
  }
  case UIElementDataUnionFbs::UIElementDataUnion_DropDownButtonData: {
    auto ddbtn_data = static_cast<const DropDownButtonDataFbs *>(data);
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
    // Create a callback that captures 'this' and calls CreateUIElement
    auto create_callback = [this](const UIElementDataUnionFbs &data_type,
                                  const void *data)
        -> std::expected<std::unique_ptr<UIElement>, FailInfo> {
      return this->CreateUIElement(data_type, data);
    };

    auto base_config_result = data::configure::ConfigureBaseUIElement(
        *element, *base_data, m_event_handler, create_callback);
    if (!base_config_result.has_value())
      return std::unexpected(base_config_result.error());
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
