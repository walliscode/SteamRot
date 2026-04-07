/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for positioning UI elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "positioning_ui.h"
#include "DropDownContainerElement.h"

namespace steamrot::logic::positioning::ui {

/////////////////////////////////////////////////
sf::Vector2f CalculateAvailableSize(const sf::Vector2f &element_size,
                                    float border_thickness,
                                    const sf::Vector2f &inner_margin) {
  return {element_size.x - 2 * border_thickness - 2 * inner_margin.x,
          element_size.y - 2 * border_thickness - 2 * inner_margin.y};
}

/////////////////////////////////////////////////
sf::Vector2f CalculateStartPosition(const sf::Vector2f &element_position,
                                    float border_thickness,
                                    const sf::Vector2f &inner_margin) {
  return {element_position.x + border_thickness + inner_margin.x,
          element_position.y + border_thickness + inner_margin.y};
}

/////////////////////////////////////////////////
void PositionDropDownContainerChildren(const DropDownContainerElement &element,
                                       const UIStyle &style) {
  float border_thickness =
      style.drop_down_container_style.border_thickness;
  float ratio = style.drop_down_container_style.drop_symbol_ratio;

  // ignore inner margins for dropdown container children
  sf::Vector2f available_size =
      CalculateAvailableSize(element.size, border_thickness);
  sf::Vector2f start_position =
      CalculateStartPosition(element.position, border_thickness);

  // calculate the size and position of the dropdown list (child 0)
  sf::Vector2f dd_list_size{available_size.x * (1 - ratio), available_size.y};
  if (!element.child_elements.empty()) {
    element.child_elements[0]->size = dd_list_size;
    element.child_elements[0]->position = start_position;
  }

  // calculate the size and position of the dropdown button (child 1)
  sf::Vector2f dd_button_position{start_position.x + dd_list_size.x,
                                  start_position.y};
  sf::Vector2f dd_button_size{available_size.x * ratio, available_size.y};
  if (element.child_elements.size() > 1) {
    element.child_elements[1]->size = dd_button_size;
    element.child_elements[1]->position = dd_button_position;
  }
}

/////////////////////////////////////////////////
void PositionVerticalLayoutChildren(const UIElement &element,
                                    const UIStyle &style) {
  if (element.child_elements.empty()) {
    return;
  }

  float border_thickness = style.panel_style.border_thickness;
  sf::Vector2f inner_margin = style.panel_style.inner_margin;

  sf::Vector2f available_size =
      CalculateAvailableSize(element.size, border_thickness, inner_margin);
  sf::Vector2f start_position =
      CalculateStartPosition(element.position, border_thickness, inner_margin);

  // divide available height equally among children with inner_margin.y spacing
  float child_height =
      (available_size.y -
       (element.child_elements.size() - 1) * inner_margin.y) /
      static_cast<float>(element.child_elements.size());

  for (size_t i = 0; i < element.child_elements.size(); i++) {
    element.child_elements[i]->size.x = available_size.x;
    element.child_elements[i]->size.y = child_height;
    element.child_elements[i]->position = sf::Vector2f{
        start_position.x,
        start_position.y + i * (child_height + inner_margin.y)};
  }
}

/////////////////////////////////////////////////
void PositionHorizontalLayoutChildren(const UIElement &element,
                                      const UIStyle &style) {
  if (element.child_elements.empty()) {
    return;
  }

  float border_thickness = style.panel_style.border_thickness;
  sf::Vector2f inner_margin = style.panel_style.inner_margin;

  sf::Vector2f available_size =
      CalculateAvailableSize(element.size, border_thickness, inner_margin);
  sf::Vector2f start_position =
      CalculateStartPosition(element.position, border_thickness, inner_margin);

  // divide available width equally among children with inner_margin.x spacing
  float child_width =
      (available_size.x -
       (element.child_elements.size() - 1) * inner_margin.x) /
      static_cast<float>(element.child_elements.size());

  for (size_t i = 0; i < element.child_elements.size(); i++) {
    element.child_elements[i]->size.x = child_width;
    element.child_elements[i]->size.y = available_size.y;
    element.child_elements[i]->position =
        sf::Vector2f{start_position.x + i * (child_width + inner_margin.x),
                     start_position.y};
  }
}

/////////////////////////////////////////////////
void PositionDropDownLayoutChildren(const UIElement &element,
                                    const UIStyle &style) {
  float border_thickness = style.panel_style.border_thickness;

  // DropDown layout ignores inner margins
  sf::Vector2f available_size =
      CalculateAvailableSize(element.size, border_thickness);
  sf::Vector2f start_position =
      CalculateStartPosition(element.position, border_thickness);

  for (size_t i = 0; i < element.child_elements.size(); i++) {
    element.child_elements[i]->size.x = available_size.x;
    element.child_elements[i]->size.y = available_size.y;
    element.child_elements[i]->position = sf::Vector2f{
        start_position.x, start_position.y + i * available_size.y};
  }
}

/////////////////////////////////////////////////
void UpdateSizeAndPositionOfChildElements(const UIElement &element,
                                          const UIStyle &style) {
  // guard clause for no children
  if (element.child_elements.empty()) {
    return;
  }

  // handle DropDownContainer children with type-specific positioning
  if (const auto *dd_container =
          dynamic_cast<const DropDownContainerElement *>(&element)) {
    PositionDropDownContainerChildren(*dd_container, style);
    return;
  }

  // generic layout-based dispatch for any other UIElement with children
  switch (element.layout) {
  case Layout::Vertical: {
    PositionVerticalLayoutChildren(element, style);
    break;
  }
  case Layout::Horizontal: {
    PositionHorizontalLayoutChildren(element, style);
    break;
  }
  case Layout::DropDown: {
    PositionDropDownLayoutChildren(element, style);
    break;
  }
  default: {
    // for unsupported layout types, do nothing
    break;
  }
  }
}

/////////////////////////////////////////////////
void PositionNestedUIElements(const UIElement &element, const UIStyle &style) {
  // position the children of this element
  UpdateSizeAndPositionOfChildElements(element, style);

  // recursively position active children
  if (element.children_active) {
    for (const auto &child : element.child_elements) {
      PositionNestedUIElements(*child, style);
    }
  }
}

} // namespace steamrot::logic::positioning::ui
