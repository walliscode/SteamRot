#pragma once
/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the UIElement struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#include "uuid.h"
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "Layout.h"
#include "SpacingAndSizing.h"
#include "Subscriber.h"
#include "UIElementRenderCache.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

namespace steamrot {

/////////////////////////////////////////////////
/// @class UIElement
/// @brief Base struct for all UI elements, contains common properties
/////////////////////////////////////////////////
struct UIElement {
  virtual ~UIElement() {};
  uuids::uuid id{uuids::uuid_system_generator{}()};
  /////////////////////////////////////////////////
  /// @brief Position of the UI element in the window
  /////////////////////////////////////////////////
  sf::Vector2f position{0.f, 0.f};

  /////////////////////////////////////////////////
  /// @brief Size of the UI element.
  /////////////////////////////////////////////////
  sf::Vector2f size{0.f, 0.f};

  /////////////////////////////////////////////////
  /// @brief Specific event this element is subscribed to
  /////////////////////////////////////////////////
  std::shared_ptr<Subscriber> subscription{nullptr};

  /////////////////////////////////////////////////
  /// @brief EventPackets to be pushed to the EventBus if criteria are met
  /////////////////////////////////////////////////
  std::vector<EventPacket> response_events;

  /////////////////////////////////////////////////
  /// @brief Is mouse over this element, false if it over a child element
  /////////////////////////////////////////////////
  bool is_mouse_over{false};

  /////////////////////////////////////////////////
  /// @brief Helper variable for the UIRenderLogic and UICollisionLogic whether
  /// to recursively process the childred
  /////////////////////////////////////////////////
  bool children_active{false};

  /////////////////////////////////////////////////
  /// @brief Container for all child elements. Can be empty
  /////////////////////////////////////////////////
  std::vector<std::unique_ptr<UIElement>> child_elements;

  /////////////////////////////////////////////////
  /// @brief Spacing and sizing strategy for the children elements defaulting to
  /// Even
  /////////////////////////////////////////////////
  SpacingAndSizing spacing_strategy{SpacingAndSizing::Even};

  /////////////////////////////////////////////////
  /// @brief Layout type of the children elements
  /////////////////////////////////////////////////
  Layout layout{Layout::Vertical};

  /////////////////////////////////////////////////
  /// @brief Render cache for storing SFML objects to avoid per-frame allocation
  ///
  /// This cache is mutable to allow const render functions to update it.
  /// Marked mutable because caching is an implementation detail that doesn't
  /// affect the logical const-ness of the UIElement.
  /////////////////////////////////////////////////
  mutable UIElementRenderCache render_cache;

  /////////////////////////////////////////////////
  /// @brief Create a deep copy of this UI element and its children
  ///
  /// @return A new unique_ptr to a cloned UIElement
  /////////////////////////////////////////////////
  virtual std::unique_ptr<UIElement> Clone() const = 0;

  /////////////////////////////////////////////////
  /// @brief Helper to copy base UIElement data to a target element
  ///
  /// @param target The target UIElement to copy data into
  /////////////////////////////////////////////////
  void CloneBaseUIElementData(UIElement &target) const {
    target.position = position;
    target.size = size;
    target.subscription = subscription; // shared_ptr copies fine
    target.response_events = response_events;
    target.is_mouse_over = is_mouse_over;
    target.children_active = children_active;
    target.spacing_strategy = spacing_strategy;
    target.layout = layout;

    // Deep copy children
    target.child_elements.clear();
    target.child_elements.reserve(child_elements.size());
    for (const auto &child : child_elements) {
      if (child) {
        target.child_elements.push_back(child->Clone());
      }
    }
  }
};
} // namespace steamrot
