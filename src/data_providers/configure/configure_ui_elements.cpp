////////////////////////////////////////////////////////////
/// @file
/// @brief Implementation of functions to configure UI elements
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
/// Headers
////////////////////////////////////////////////////////////
#include "configure_ui_elements.h"
#include "EventPacket.h"
#include "Subscriber.h"
#include "configure_event.h"
#include "configure_subscriber.h"
#include <string>

namespace steamrot::data::configure {

namespace {

////////////////////////////////////////////////////////////
/// @brief Convert legacy integer UI element priority into tier ordering.
///
/// Legacy values are mapped as:
/// - priority < 0  -> UIPriorityTier::Background
/// - priority == 0 -> UIPriorityTier::Normal
/// - priority == 1 -> UIPriorityTier::Elevated
/// - priority >= 2 -> UIPriorityTier::Modal
///
/// @param priority Legacy integer priority from FlatBuffers data.
/// @return Converted UIPriorityTier for fixed-pass UI processing.
////////////////////////////////////////////////////////////
UIPriorityTier PriorityTierFromLegacyPriority(const int priority) {
  if (priority < 0)
    return UIPriorityTier::Background;
  if (priority == 0)
    return UIPriorityTier::Normal;
  if (priority == 1)
    return UIPriorityTier::Elevated;
  return UIPriorityTier::Modal;
}

} // namespace

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

/////////////////////////////////////////////////
DataPopulationFunction ConvertDataPopulationFunction(
    DataPopulationFunctionFbs fbs_data_population_function) {
  switch (fbs_data_population_function) {
  case DataPopulationFunctionFbs_None:
    return DataPopulationFunction::None;
  case DataPopulationFunctionFbs_GetAllFragmentNames:
    return DataPopulationFunction::GetAllFragmentNames;
  case DataPopulationFunctionFbs_GetAllJointNames:
    return DataPopulationFunction::GetAllJointNames;
  default:
    return DataPopulationFunction::None;
  }
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureBaseUIElement(
    UIElement &element, const UIElementDataFbs &data,
    EventHandler &event_handler,
    std::function<std::expected<std::unique_ptr<UIElement>, FailInfo>(
        const UIElementDataUnionFbs &, const void *)>
        create_ui_element_callback) {

  element.position = sf::Vector2f({data.position()->x(), data.position()->y()});
  element.size = sf::Vector2f({data.size()->x(), data.size()->y()});

  // set is_mouse_over if available
  if (data.is_mouse_over())
    element.is_mouse_over = data.is_mouse_over();

  // set Subscription if subscriber_data exists and EventTypeFbs is not none
  if (data.subscriber_data() &&
      (data.subscriber_data()->event_type() != EventTypeFbs_NONE)) {

    // Create vector with single subscriber
    std::vector<const SubscriberFbs *> subscribers_fbs{data.subscriber_data()};

    // Create Subscriber and configure
    std::shared_ptr<Subscriber> subscriber = std::make_shared<Subscriber>();

    auto result = data::configure::ConfigureSubscriber(*subscriber,
                                                       data.subscriber_data());
    // propogate errors
    if (!result.has_value())
      return std::unexpected(result.error());

    element.subscription = subscriber;
    auto register_result = event_handler.RegisterSubscriber(subscriber);
    if (!register_result.has_value())
      return std::unexpected(register_result.error());
  }

  // set ResponseEvents if response_event_data exists and has entries
  if (data.response_event_data() && data.response_event_data()->size() > 0) {

    // Iterate through all event packet data entries
    for (auto event_packet_data : *data.response_event_data()) {
      if (!event_packet_data) {
        continue;
      }

      // Configure EventPacket from EventPacketFbs data
      EventPacket event_packet;
      auto configure_result =
          ConfigureEventPacket(event_packet, event_packet_data);
      if (!configure_result.has_value()) {
        return std::unexpected(configure_result.error());
      }

      // Add configured event packet to element's response events
      element.response_events.push_back(event_packet);
    }
  }

  element.children_active = data.children_active();
  element.m_priority_tier = PriorityTierFromLegacyPriority(data.priority());
  element.is_disabled = data.is_disabled();

  // Recursively create and attach children
  if (data.children()) {
    for (auto child_fb : *data.children()) {
      if (!child_fb)
        continue;
      auto type = child_fb->element_type();
      auto child_table = child_fb->element();
      if (!child_table)
        continue;
      auto child_element_result = create_ui_element_callback(type, child_table);
      if (!child_element_result.has_value())
        return std::unexpected(child_element_result.error());
      element.child_elements.push_back(std::move(child_element_result.value()));
    }
  }

  return std::monostate{};
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigurePanelElement(PanelElement &panel_element, const PanelDataFbs &data) {
  // layout() and spacing_strategy() return scalar enum values (not pointers)
  // so no null-check is required; missing fields default to 0 (None/None).
  panel_element.layout = ConvertLayout(data.layout());
  panel_element.spacing_strategy = ConvertSpacingAndSizing(data.spacing_strategy());
  return std::monostate{};
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureButtonElement(ButtonElement &button_element,
                       const ButtonDataFbs &data) {
  if (data.label()) {
    button_element.label = data.label()->str();
  }
  return std::monostate{};
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureDropDownListElement(DropDownListElement &dropdown_list_element,
                             const DropDownListDataFbs &data) {
  if (data.unexpanded_label()) {
    dropdown_list_element.unexpanded_label = data.unexpanded_label()->str();
  }
  if (data.expanded_label()) {
    dropdown_list_element.expanded_label = data.expanded_label()->str();
  }
  // convert data population function from flatbuffers enum to C++ enum
  dropdown_list_element.data_population_function =
      ConvertDataPopulationFunction(data.data_population_function());

  return std::monostate{};
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo> ConfigureDropDownContainerElement(
    DropDownContainerElement &dropdown_container_element,
    const DropDownContainerDataFbs &data) {

  // return if children are empty
  if (!data.base_data()->children()) {
    return std::unexpected(FailInfo{
        FailMode::FlatbuffersDataNotFound,
        "DropDownContainerDataFbs must have exactly 2 children: a "
        "DropDownListDataFbs and a DropDownButtonDataFbs. This one has none."});
  }
  // ensure that the data contains only a list and a button as children
  if (data.base_data()->children()->size() != 2) {
    size_t num_children = data.base_data()->children()->size();
    return std::unexpected(FailInfo{
        FailMode::FlatbuffersDataNotFound,
        "DropDownContainerDataFbs must have exactly 2 children: a "
        "DropDownListDataFbs and a DropDownButtonDataFbs. This one has " +
            std::to_string(num_children) + " children."});
  }
  // ensure the first child in the data is DropDownListDataFbs
  auto first_child_fb = data.base_data()->children()->Get(0);
  if (!first_child_fb ||
      first_child_fb->element_type() !=
          UIElementDataUnionFbs::UIElementDataUnionFbs_DropDownListDataFbs) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "DropDownContainerDataFbs's first child "
                                    "must be a DropDownListDataFbs."});
  }
  // ensure the second child in the data is DropDownButtonDataFbs
  auto second_child_fb = data.base_data()->children()->Get(1);
  if (!second_child_fb ||
      second_child_fb->element_type() !=
          UIElementDataUnionFbs::UIElementDataUnionFbs_DropDownButtonDataFbs) {
    return std::unexpected(FailInfo{FailMode::FlatbuffersDataNotFound,
                                    "DropDownContainerDataFbs's second child "
                                    "must be a DropDownButtonDataFbs."});
  }

  return std::monostate{};
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureDropDownItemElement(DropDownItemElement &dropdown_item_element,
                             const DropDownItemDataFbs &data) {
  if (data.label()) {
    dropdown_item_element.label = data.label()->str();
  }
  return std::monostate{};
}

////////////////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ConfigureDropDownButtonElement(DropDownButtonElement &dropdown_button_element,
                               const DropDownButtonDataFbs &data) {
  dropdown_button_element.is_expanded = data.is_expanded();
  return std::monostate{};
}

} // namespace steamrot::data::configure
