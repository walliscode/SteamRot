///////////////////////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of the UIElementFactory class
///////////////////////////////////////////////////////////////////////////////
#include "UIElementFactory.h"
#include "EventHandler.h"
#include "EventPacket.h"
#include "FailInfo.h"
#include "SubscriberFactory.h"
#include "event_helpers.h"
#include "user_interface_generated.h"
#include <expected>
#include <iostream>
#include <string>
#include <variant>

namespace steamrot {

/////////////////////////////////////////////////
std::expected<std::unique_ptr<UIElement>, FailInfo>
CreateUIElement(const UIElementDataUnion &data_type, const void *data,
                EventHandler &event_hanlder) {

  std::unique_ptr<UIElement> element{nullptr};
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
  std::cout << "Swtich statment complete." << std::endl;

  // Only call this once!
  if (base_data) {

    std::cout << "Configuring base UI element." << std::endl;
    auto base_config_result =
        ConfigureBaseUIElement(*element, *base_data, event_hanlder);
    if (!base_config_result.has_value())
      return std::unexpected(base_config_result.error());
  }
  // return unexpected if element is still null
  if (!element) {
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "CreateUIElement: Element creation failed, element is null."});
  }
  std::cout << "UI element created successfully." << std::endl;
  return element;
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureBaseUIElement(UIElement &element, const UIElementData &data,
                       EventHandler &event_handler) {

  element.position = sf::Vector2f({data.position()->x(), data.position()->y()});
  element.size = sf::Vector2f({data.size()->x(), data.size()->y()});

  // set Subscription if subscriber_data exists and EventType is not none
  if (data.subscriber_data() && (data.subscriber_data()->event_type_data() !=
                                 EventType::EventType_NONE)) {

    // set EventType
    if (!data.subscriber_data()->event_type_data()) {
      return std::unexpected(FailInfo{
          FailMode::FlatbuffersDataNotFound,
          "UIElementData has subscriber_data but no event_type_data."});
    }
    EventType event_type = data.subscriber_data()->event_type_data();

    // create EventData by running the flatbuffers data through the converter
    auto event_data_conversion_result =
        ConvertFlatbuffersEventDataDataToEventData(
            data.subscriber_data()->event_data_data_type(),
            data.subscriber_data()->event_data_data());

    if (!event_data_conversion_result.has_value())
      return std::unexpected(event_data_conversion_result.error());
    EventData event_data = event_data_conversion_result.value();

    // create and register subscriber if not
    SubscriberFactory factory{event_handler};
    auto create_subscriber_result =
        factory.CreateAndRegisterSubscriber(event_type);

    if (!create_subscriber_result.has_value())
      return std::unexpected(create_subscriber_result.error());

    // reset the element's subscription to be sure
    element.subscription.reset();
    element.subscription = std::move(create_subscriber_result.value());
  }

  // set ResponseEvent if response_event_data exists and EventType is not none
  if (data.response_event_data() &&
      (data.response_event_data()->event_type() != EventType::EventType_NONE)) {

    // set EventType
    if (!data.response_event_data()->event_type()) {
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "UIElementData has response_event_data but no event_type."});
    }
    EventType event_type = data.response_event_data()->event_type();

    // create EventData by running the flatbuffers data through the converter
    auto event_data_conversion_result =
        ConvertFlatbuffersEventDataDataToEventData(
            data.response_event_data()->event_data_data_type(),
            data.response_event_data()->event_data_data());
    if (!event_data_conversion_result.has_value())
      return std::unexpected(event_data_conversion_result.error());

    EventData event_data = event_data_conversion_result.value();

    // create EventPacket and set on element
    EventPacket event_packet(event_type, event_data,
                             data.response_event_data()->event_lifetime());
    element.response_event = event_packet;
  }

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
      auto child_element_result =
          CreateUIElement(type, child_table, event_handler);
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

  return std::monostate{};
}

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureDropDownContainerElement(
    DropDownContainerElement &dropdown_container_element,
    const DropDownContainerData &data) {

  // return if children are empty
  if (!data.base_data()->children()) {
    return std::unexpected(FailInfo{
        FailMode::FlatbuffersDataNotFound,
        "DropDownContainerData must have exactly 2 children: a "
        "DropDownListData and a DropDownButtonData. This one has none."});
  }
  // ensure that the data contains only a list and a button as children
  if (data.base_data()->children()->size() != 2) {
    size_t num_children = data.base_data()->children()->size();
    return std::unexpected(
        FailInfo{FailMode::FlatbuffersDataNotFound,
                 "DropDownContainerData must have exactly 2 children: a "
                 "DropDownListData and a DropDownButtonData. This one has " +
                     std::to_string(num_children) + " children."});
  }
  // ensure the first child in the data is DropDownListData
  auto first_child_fb = data.base_data()->children()->Get(0);
  if (!first_child_fb ||
      first_child_fb->element_type() !=
          UIElementDataUnion::UIElementDataUnion_DropDownListData) {
    return std::unexpected(FailInfo{
        FailMode::FlatbuffersDataNotFound,
        "DropDownContainerData's first child must be a DropDownListData."});
  }
  // ensure the second child in the data is DropDownButtonData
  auto second_child_fb = data.base_data()->children()->Get(1);
  if (!second_child_fb ||
      second_child_fb->element_type() !=
          UIElementDataUnion::UIElementDataUnion_DropDownButtonData) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "DropDownContainerData's second child "
                                    "must be a DropDownButtonData."});
  }

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
