/////////////////////////////////////////////////
/// @file
/// @brief Implementation of functions for drawing UI elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "draw_ui_elements.h"
#include "DropDownContainerElement.h"
#include "user_interface_generated.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>

namespace steamrot {
namespace draw_ui_elements {
/////////////////////////////////////////////////
void DrawNestedUIElements(sf::RenderTexture &texture, const UIElement &element,
                          const UIStyle &style) {
  // draw the parent element first
  element.DrawUIElement(texture, style);

  // update the size and position of the child elements
  UpdateSizeAndPositionOfChildElements(element, style);

  // if children are active, draw them
  if (element.children_active) {
    for (const auto &child : element.child_elements) {
      DrawNestedUIElements(texture, *child, style);
    }
  }
}

/////////////////////////////////////////////////
void DrawBorderAndBackground(sf::RenderTexture &texture,
                             const sf::Vector2f &position,
                             const sf::Vector2f &size, const Style &style) {
  // Create the rectangle using the element's position and size
  sf::RectangleShape rectangle(size);
  rectangle.setPosition(position);
  rectangle.setFillColor(style.background_color);
  rectangle.setOutlineColor(style.border_color);
  // Border thickness is negative to draw inwards
  rectangle.setOutlineThickness(-style.border_thickness);
  // Draw the rectangle on the texture
  texture.draw(rectangle);
}
/////////////////////////////////////////////////
void DrawText(sf::RenderTexture &texture, const std::string &text,
              const sf::Vector2f &position, const sf::Vector2f size,
              std::shared_ptr<const sf::Font> font, uint8_t font_size,
              const sf::Color &color) {

  // create the text object
  sf::Text text_object(*font, text, font_size);

  // set the fill color
  text_object.setFillColor(color);
  // set the origin to the center of the text
  text_object.setOrigin(text_object.getLocalBounds().getCenter());

  // calculate the center of the container
  sf::Vector2f container_center = sf::FloatRect(position, size).getCenter();
  // set the position of the text to the center of the container
  text_object.setPosition(container_center);

  texture.draw(text_object);
}

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

    // ignore Inner margines for dropdown container children
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
  case LayoutType_Vertical: {
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
  case LayoutType_Horizontal: {
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
  case LayoutType_DropDown: {
    // for a dropdown, they are ordered vertically, inner margins are ignored
    // and the avaiable space is a multiple of the parent inner size (e.g the
    // more children the more space they take up)
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
} // namespace draw_ui_elements
} // namespace steamrot
