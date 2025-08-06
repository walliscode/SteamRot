#pragma once
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the UIElement struct
/////////////////////////////////////////////////
#include "Button.h"
#include "DropDown.h"
#include "EventPacket.h"
#include "Panel.h"
#include "user_interface_generated.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <variant>
#include <vector>
namespace steamrot {

using ElementType = std::variant<Panel, Button, DropDownContainer, DropDownList,
                                 DropDownItem, DropDownButton>;

struct UIElement {
  std::string name{"UIElement"};

  /////////////////////////////////////////////////
  /// @brief UIELement type, contains extra data for the element, can only be
  /// one of the options.
  /////////////////////////////////////////////////
  ElementType element_type;

  /////////////////////////////////////////////////
  /// @brief Container for all child elements. Can be empty
  /////////////////////////////////////////////////
  std::vector<UIElement> child_elements;

  /////////////////////////////////////////////////
  /// @brief Position of the UI element in the window
  /////////////////////////////////////////////////
  sf::Vector2f position{0.f, 0.f};

  /////////////////////////////////////////////////
  /// @brief Size of the UI element.
  /////////////////////////////////////////////////
  sf::Vector2f size{0.f, 0.f};

  /////////////////////////////////////////////////
  /// @brief An optional ratio for the UI element, used in spacing strategies
  /////////////////////////////////////////////////
  std::optional<float> ratio{std::nullopt};

  /////////////////////////////////////////////////
  /// @brief Spacing and sizing strategy for the children elements defaulting to
  /// Even
  /////////////////////////////////////////////////
  SpacingAndSizingType spacing_strategy{
      SpacingAndSizingType::SpacingAndSizingType_Even};
  /////////////////////////////////////////////////
  /// @brief Layout type of the children elements
  /////////////////////////////////////////////////
  LayoutType layout{LayoutType::LayoutType_Vertical};

  /////////////////////////////////////////////////
  /// @brief Is mouse over this element, false if it over a child element
  /////////////////////////////////////////////////
  bool mouse_over{false};

  /////////////////////////////////////////////////
  /// @brief Helper variable to check if the mouse is over a child element
  /////////////////////////////////////////////////
  bool mouse_over_child{false};

  /////////////////////////////////////////////////
  /// @brief Helper variable for the UIRenderLogic and UICollisionLogic whether
  /// to recursively process the childred
  /////////////////////////////////////////////////
  bool children_active{false};

  /////////////////////////////////////////////////
  /// @brief Event that triggers this UIELement
  /////////////////////////////////////////////////
  EventType trigger_event{EventType::EventType_NONE};

  /////////////////////////////////////////////////
  /// @brief Optional data package to check against the trigger event.
  /////////////////////////////////////////////////
  EventData trigger_event_data{std::monostate{}};

  /////////////////////////////////////////////////
  /// @brief Potential response event that is sent to the EventBus
  /////////////////////////////////////////////////
  EventType response_event{EventType::EventType_NONE};

  /////////////////////////////////////////////////
  /// @brief Optional data package to be associated with the response event.
  /////////////////////////////////////////////////
  EventData response_event_data{std::monostate{}};
};
} // namespace steamrot
