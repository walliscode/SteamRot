#pragma once
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the UIElement struct
/////////////////////////////////////////////////
#include "Button.h"
#include "DropDown.h"
#include "Panel.h"
#include "SceneType.h"
#include "actions_generated.h"
#include "event_helpers.h"
#include "user_interface_generated.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <variant>
#include <vector>
namespace steamrot {

using ElementType = std::variant<Panel, Button, DropDownContainer, DropDownList,
                                 DropDownItem, DropDownButton>;

/////////////////////////////////////////////////
/// @class UIElementDataPackage
/// @brief A bit of a catch all for the UI element data.
///
/// As the game develops, this can be split out, but we'll see how much data we
/// need to store
/////////////////////////////////////////////////
struct UIElementDataPackage {

  /////////////////////////////////////////////////
  /// @brief Contains scene_type information, for when we want to switch to a
  /// fresh scene.
  /////////////////////////////////////////////////
  std::optional<SceneType> scene_type{std::nullopt};

  /////////////////////////////////////////////////
  /// @brief Choices for the DropDown UIElement. String for now but will
  /// probably become a custom struct so we can add pictures
  /////////////////////////////////////////////////
  std::optional<std::vector<std::string>> drop_down_choices{std::nullopt};
};
struct UIElement {

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
  LayoutType layout{0};

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
  /// @brief Trigger event for the UI element
  ///
  /// This is designed to be used in conjuction with the ActionNames. If
  /// trigger_event is true then signal the action
  /////////////////////////////////////////////////
  EventBitset trigger_event{0};

  /////////////////////////////////////////////////
  /// @brief Action that should be performed when the UI Element is interacted
  /// with
  /////////////////////////////////////////////////
  ActionNames action{0};

  /////////////////////////////////////////////////
  /// @brief Data package for the UI element, all members should be optional so
  /// test with if statements
  /////////////////////////////////////////////////
  UIElementDataPackage ui_data_package{};
};
} // namespace steamrot
