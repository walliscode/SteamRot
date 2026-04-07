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
void UpdateSizeAndPositionOfChildElements(const UIElement &element,
                                          const UIStyle &style) {
  // guard clause for no children
  if (element.child_elements.empty()) {
    return;
  }

  // handle DropDownContainer Children
  if (dynamic_cast<const DropDownContainerElement *>(&element)) {
    // static cast for speed, only safe because of the above check
    auto dd_container = static_cast<const DropDownContainerElement *>(&element);

    // pull out ratio
    float ratio = style.drop_down_container_style.drop_symbol_ratio;

    // ignore Inner margins for dropdown container children
    float available_width =
        element.size.x - 2 * style.drop_down_container_style.border_thickness;
    float available_height =
        element.size.y - 2 * style.drop_down_container_style.border_thickness;

    sf::Vector2f available_size{available_width, available_height};

    // calculate the start position for the dropdown list
    sf::Vector2f dd_list_position{
        element.position.x + style.drop_down_container_style.border_thickness,
        element.position.y + style.drop_down_container_style.border_thickness};

    // calculate the size of the dropdown list
    sf::Vector2f dd_list_size{available_size.x * (1 - ratio), available_size.y};

    // set the size and position of the dropdown list child
    if (!dd_container->child_elements.empty()) {
      dd_container->child_elements[0]->size = dd_list_size;
      dd_container->child_elements[0]->position = dd_list_position;
    }

    // calculate the start position for the dropdown button
    sf::Vector2f dd_button_position{dd_list_position.x + dd_list_size.x,
                                    dd_list_position.y};

    // calculate the size of the dropdown button
    sf::Vector2f dd_button_size{available_size.x * ratio, available_size.y};

    // set the size and position of the dropdown button child
    if (dd_container->child_elements.size() > 1) {
      dd_container->child_elements[1]->size = dd_button_size;
      dd_container->child_elements[1]->position = dd_button_position;
    }
    return;
  }

  // add generic handling for any UIElement with children
  switch (element.layout) {
  case Layout::Vertical: {
    // calculate the available size for the children
    float available_width = element.size.x -
                            2 * style.panel_style.border_thickness -
                            2 * style.panel_style.inner_margin.x;
    float available_height = element.size.y -
                             2 * style.panel_style.border_thickness -
                             2 * style.panel_style.inner_margin.y;
    sf::Vector2f available_size{available_width, available_height};
    // calculate the start position for the children
    sf::Vector2f start_position{
        element.position.x + style.panel_style.border_thickness +
            style.panel_style.inner_margin.x,
        element.position.y + style.panel_style.border_thickness +
            style.panel_style.inner_margin.y};

    // calculate the height of each child based on the number of children, add
    // in the inner margin as spacing
    float child_height =
        (available_size.y - (element.child_elements.size() - 1) *
                                style.panel_style.inner_margin.y) /
        static_cast<float>(element.child_elements.size());
    // set the size and position of each child
    for (size_t i = 0; i < element.child_elements.size(); i++) {
      element.child_elements[i]->size.x = available_size.x;
      element.child_elements[i]->size.y = child_height;
      element.child_elements[i]->position = sf::Vector2f{
          start_position.x,
          start_position.y +
              i * (child_height + style.panel_style.inner_margin.y)};
    }
    break;
  }
  case Layout::Horizontal: {
    // calculate the available size for the children
    float available_width = element.size.x -
                            2 * style.panel_style.border_thickness -
                            2 * style.panel_style.inner_margin.x;
    float available_height = element.size.y -
                             2 * style.panel_style.border_thickness -
                             2 * style.panel_style.inner_margin.y;
    sf::Vector2f available_size{available_width, available_height};
    // calculate the start position for the children
    sf::Vector2f start_position{
        element.position.x + style.panel_style.border_thickness +
            style.panel_style.inner_margin.x,
        element.position.y + style.panel_style.border_thickness +
            style.panel_style.inner_margin.y};
    // calculate the width of each child based on the number of children, add
    // in the inner margin as spacing
    float child_width =
        (available_size.x - (element.child_elements.size() - 1) *
                                style.panel_style.inner_margin.x) /
        static_cast<float>(element.child_elements.size());
    // set the size and position of each child
    for (size_t i = 0; i < element.child_elements.size(); i++) {
      element.child_elements[i]->size.x = child_width;
      element.child_elements[i]->size.y = available_size.y;
      element.child_elements[i]->position =
          sf::Vector2f{start_position.x +
                           i * (child_width + style.panel_style.inner_margin.x),
                       start_position.y};
    }
    break;
  }
  case Layout::DropDown: {
    // for a dropdown, they are ordered vertically, inner margins are ignored
    // and the available space is a multiple of the parent inner size (e.g the
    // more children, the more space they take up)
    float available_width =
        element.size.x - 2 * style.panel_style.border_thickness;
    float available_height =
        element.size.y - 2 * style.panel_style.border_thickness;
    sf::Vector2f available_size{available_width, available_height};
    // calculate the start position for the children
    sf::Vector2f start_position{
        element.position.x + style.panel_style.border_thickness,
        element.position.y + style.panel_style.border_thickness};
    // set the size and position of each child
    for (size_t i = 0; i < element.child_elements.size(); i++) {
      element.child_elements[i]->size.x = available_size.x;
      element.child_elements[i]->size.y = available_size.y;
      element.child_elements[i]->position = sf::Vector2f{
          start_position.x, start_position.y + i * available_size.y};
    }
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
