////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of functions to configure UI elements
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "configure_ui_elements.h"
#include "EventPacket.h"
#include "event_factory.h"
#include "event_type_conversion.h"
#include "subscriber_factory.h"
#include <spdlog/spdlog.h>
#include <string>

namespace steamrot::data::configure {

////////////////////////////////////////////////////////////
Layout ConvertLayout(int8_t fbs_layout) {
  switch (fbs_layout) {
  case LayoutFbs_None:
    return Layout::None;
  case LayoutFbs_Horizontal:
    return Layout::Horizontal;
  case LayoutFbs_Vertical:
    return Layout::Vertical;
  case LayoutFbs_Grid:
    return Layout::Grid;
  case LayoutFbs_DropDown:
    return Layout::DropDown;
  default:
    return Layout::None;
  }
}

////////////////////////////////////////////////////////////
SpacingAndSizing ConvertSpacingAndSizing(int8_t fbs_spacing) {
  switch (fbs_spacing) {
  case SpacingAndSizingFbs_None:
    return SpacingAndSizing::None;
  case SpacingAndSizingFbs_Even:
    return SpacingAndSizing::Even;
  case SpacingAndSizingFbs_Ratioed:
    return SpacingAndSizing::Ratioed;
  case SpacingAndSizingFbs_DropDownList:
    return SpacingAndSizing::DropDownList;
  default:
    return SpacingAndSizing::None;
  }
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureBaseUIElement(
    UIElement &element, const UIElementData &data, EventHandler &event_handler,
    std::function<std::expected<std::unique_ptr<UIElement>, FailInfo>(
        const UIElementDataUnion &, const void *)>
        create_ui_element_callback) {

  spdlog::debug("[ConfigureBaseUIElement] Configuring element at address: {}", 
                static_cast<const void*>(&element));

  element.position = sf::Vector2f({data.position()->x(), data.position()->y()});
  element.size = sf::Vector2f({data.size()->x(), data.size()->y()});

  // set is_mouse_over if available
  if (data.is_mouse_over())
    element.is_mouse_over = data.is_mouse_over();

  // set Subscription if subscriber_data exists and EventTypeFbs is not none
  if (data.subscriber_data() && (data.subscriber_data()->event_type_data() !=
                                 EventTypeFbs_EVENT_NONE)) {

    spdlog::debug("[ConfigureBaseUIElement] Element {} has subscriber_data", 
                  static_cast<const void*>(&element));

    // Create vector with single subscriber
    std::vector<const SubscriberFbs *> subscribers_fbs{data.subscriber_data()};

    // Create Subscriber via factory
    auto result = subscriber_factory::CreateSubscriber(subscribers_fbs[0]);

    // propogate errors
    if (!result.has_value())
      return std::unexpected(result.error());

    // create shared_ptr, add to element and register with event handler
    std::shared_ptr<Subscriber> subscriber =
        std::make_shared<Subscriber>(result.value());
    element.subscription = subscriber;
    
    spdlog::debug("[ConfigureBaseUIElement] Created subscription at {} for element {}", 
                  static_cast<const void*>(subscriber.get()),
                  static_cast<const void*>(&element));
    
    auto register_result = event_handler.RegisterSubscriber(subscriber);
    if (!register_result.has_value())
      return std::unexpected(register_result.error());
  } else {
    spdlog::debug("[ConfigureBaseUIElement] Element {} has NO subscriber_data", 
                  static_cast<const void*>(&element));
  }

  // set ResponseEvent if response_event_data exists and EventTypeFbs is not none
  if (data.response_event_data() &&
      (data.response_event_data()->event_type() != EventTypeFbs_EVENT_NONE)) {

    spdlog::debug("[ConfigureBaseUIElement] Element {} has response_event_data", 
                  static_cast<const void*>(&element));

    // set EventTypeFbs
    if (!data.response_event_data()->event_type()) {
      return std::unexpected(
          FailInfo{FailMode::FlatbuffersDataNotFound,
                   "UIElementData has response_event_data but no event_type."});
    }
    
    // Convert EventTypeFbs to native EventType
    auto event_type_result = event::ConvertEventTypeFbsToEventType(data.response_event_data()->event_type());
    if (!event_type_result.has_value()) {
      return std::unexpected(event_type_result.error());
    }
    EventType event_type = event_type_result.value();

    spdlog::debug("[ConfigureBaseUIElement] Element {} event_type: {}", 
                  static_cast<const void*>(&element), static_cast<int>(event_type));

    // create EventData by running the flatbuffers data through the factory
    auto event_data_conversion_result = event::CreateEventData(
        data.response_event_data()->event_data_data_type(),
        data.response_event_data()->event_data_data());
    if (!event_data_conversion_result.has_value())
      return std::unexpected(event_data_conversion_result.error());

    EventData event_data = event_data_conversion_result.value();

    // create EventPacket and set on element
    EventPacket event_packet(event_type, event_data,
                             data.response_event_data()->event_lifetime());
    element.response_event = event_packet;
    
    spdlog::debug("[ConfigureBaseUIElement] Set response_event on element {} with event_type: {}", 
                  static_cast<const void*>(&element), static_cast<int>(event_type));
  } else {
    spdlog::debug("[ConfigureBaseUIElement] Element {} has NO response_event_data", 
                  static_cast<const void*>(&element));
  }

  element.spacing_strategy = ConvertSpacingAndSizing(data.spacing_strategy());
  element.layout = ConvertLayout(data.layout());
  element.children_active = data.children_active();

  spdlog::debug("[ConfigureBaseUIElement] Before processing children - element {} response_event: {}", 
                static_cast<const void*>(&element), element.response_event.has_value());

  // Recursively create and attach children
  if (data.children()) {
    spdlog::debug("[ConfigureBaseUIElement] Element {} has {} children to process", 
                  static_cast<const void*>(&element), data.children()->size());
    
    size_t child_index = 0;
    for (auto child_fb : *data.children()) {
      if (!child_fb) {
        spdlog::debug("[ConfigureBaseUIElement] Child {} is null, skipping", child_index);
        child_index++;
        continue;
      }
      auto type = child_fb->element_type();
      auto child_table = child_fb->element();
      if (!child_table) {
        spdlog::debug("[ConfigureBaseUIElement] Child {} has null element table, skipping", child_index);
        child_index++;
        continue;
      }
      
      spdlog::debug("[ConfigureBaseUIElement] Processing child {} with type {}", 
                    child_index, static_cast<int>(type));
      
      auto child_element_result = create_ui_element_callback(type, child_table);
      if (!child_element_result.has_value())
        return std::unexpected(child_element_result.error());
      
      auto& created_child = child_element_result.value();
      spdlog::debug("[ConfigureBaseUIElement] Created child {} at address {}", 
                    child_index, static_cast<const void*>(created_child.get()));
      
      element.child_elements.push_back(std::move(created_child));
      child_index++;
    }
    
    spdlog::debug("[ConfigureBaseUIElement] Finished processing {} children for element {}", 
                  element.child_elements.size(), static_cast<const void*>(&element));
  }

  spdlog::debug("[ConfigureBaseUIElement] After processing children - element {} response_event: {}", 
                static_cast<const void*>(&element), element.response_event.has_value());

  return std::monostate{};
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigurePanelElement(PanelElement &panel_element, const PanelData &data) {
  // No extra fields for panel
  return std::monostate{};
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureButtonElement(ButtonElement &button_element, const ButtonData &data) {
  if (data.label()) {
    button_element.label = data.label()->str();
    spdlog::debug("[ConfigureButtonElement] Button at {} configured with label: '{}'", 
                  static_cast<const void*>(&button_element), button_element.label);
  }
  return std::monostate{};
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureDropDownListElement(
    DropDownListElement &dropdown_list_element, const DropDownListData &data) {
  if (data.label()) {
    dropdown_list_element.unexpanded_label = data.label()->str();
  }
  if (data.expanded_label()) {
    dropdown_list_element.expanded_label = data.expanded_label()->str();
  }

  dropdown_list_element.data_populate_function = data.data_populate_function();

  return std::monostate{};
}

////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureDropDownItemElement(
    DropDownItemElement &dropdown_item_element, const DropDownItemData &data) {
  if (data.label()) {
    dropdown_item_element.label = data.label()->str();
  }
  return std::monostate{};
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureDropDownButtonElement(
    DropDownButtonElement &dropdown_button_element,
    const DropDownButtonData &data) {
  dropdown_button_element.is_expanded = data.is_expanded();
  return std::monostate{};
}

} // namespace steamrot::data::configure
