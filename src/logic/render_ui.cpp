/////////////////////////////////////////////////
/// @file
/// @brief Implementation of functions for drawing UI elements
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_ui.h"
#include "ButtonElement.h"
#include "DropDownButtonElement.h"
#include "DropDownContainerElement.h"
#include "DropDownItemElement.h"
#include "DropDownListElement.h"
#include "PanelElement.h"
#include <cstdint>

namespace steamrot::logic::render::ui {

/////////////////////////////////////////////////
void DrawUIElementDispatch(sf::RenderTexture &texture, const UIElement &element,
                           const UIStyle &style) {
  // Type dispatch using dynamic_cast
  if (const auto *button = dynamic_cast<const ButtonElement *>(&element)) {
    DrawButtonElement(texture, *button, style);
  } else if (const auto *panel = dynamic_cast<const PanelElement *>(&element)) {
    DrawPanelElement(texture, *panel, style);
  } else if (const auto *list =
                 dynamic_cast<const DropDownListElement *>(&element)) {
    DrawDropDownListElement(texture, *list, style);
  } else if (const auto *item =
                 dynamic_cast<const DropDownItemElement *>(&element)) {
    DrawDropDownItemElement(texture, *item, style);
  } else if (const auto *dd_button =
                 dynamic_cast<const DropDownButtonElement *>(&element)) {
    DrawDropDownButtonElement(texture, *dd_button, style);
  } else if (const auto *container =
                 dynamic_cast<const DropDownContainerElement *>(&element)) {
    DrawDropDownContainerElement(texture, *container, style);
  }
}

/////////////////////////////////////////////////
void DrawNestedUIElements(sf::RenderTexture &texture, const UIElement &element,
                          const UIStyle &style) {
  // Draw the parent element first using dispatcher
  DrawUIElementDispatch(texture, element, style);

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
void DrawButtonElement(sf::RenderTexture &texture, const ButtonElement &button,
                       const UIStyle &style) {
  // Draw the border and background
  DrawBorderAndBackground(texture, button, style.button_style);

  // Draw the button text
  sf::Vector2f text_position{
      button.position.x + style.button_style.border_thickness +
          style.button_style.inner_margin.x,
      button.position.y + style.button_style.border_thickness +
          style.button_style.inner_margin.y};

  DrawText(texture, button.label, text_position, button.size,
           style.button_style.font, style.button_style.font_size,
           style.button_style.text_color);
}

/////////////////////////////////////////////////
void DrawPanelElement(sf::RenderTexture &texture, const PanelElement &panel,
                      const UIStyle &style) {

  DrawBorderAndBackground(texture, panel, style.panel_style);
}

/////////////////////////////////////////////////
void DrawDropDownListElement(sf::RenderTexture &texture,
                             const DropDownListElement &list,
                             const UIStyle &style) {
  DrawBorderAndBackground(texture, list, style.drop_down_list_style);

  // calculate the position for the text
  sf::Vector2f text_position{
      list.position.x + style.drop_down_list_style.border_thickness +
          style.drop_down_list_style.inner_margin.x,
      list.position.y + style.drop_down_list_style.border_thickness +
          style.drop_down_list_style.inner_margin.y};

  // set the label based on whether the dropdown is expanded
  std::string label =
      list.is_expanded ? list.expanded_label : list.unexpanded_label;

  DrawText(texture, label, text_position, list.size,
           style.drop_down_list_style.font,
           style.drop_down_list_style.font_size,
           style.drop_down_list_style.text_color);
}

/////////////////////////////////////////////////
void DrawDropDownItemElement(sf::RenderTexture &texture,
                             const DropDownItemElement &item,
                             const UIStyle &style) {
  DrawBorderAndBackground(texture, item, style.drop_down_item_style);

  DrawText(texture, item.label,
           sf::Vector2f{
               item.position.x + style.drop_down_item_style.border_thickness +
                   style.drop_down_item_style.inner_margin.x,
               item.position.y + style.drop_down_item_style.border_thickness +
                   style.drop_down_item_style.inner_margin.y},
           item.size, style.drop_down_item_style.font,
           style.drop_down_item_style.font_size,
           style.drop_down_item_style.text_color);
}

/////////////////////////////////////////////////
void DrawDropDownButtonElement(sf::RenderTexture &texture,
                               const DropDownButtonElement &button,
                               const UIStyle &style) {
  DrawBorderAndBackground(texture, button, style.drop_down_button_style);

  // calculate the radius of the triangle using the size, border thickness,
  // and inner margin of the button
  float triangle_radius =
      (button.size.x - 2 * style.drop_down_button_style.border_thickness -
       2 * style.drop_down_button_style.inner_margin.x) /
      2.0f;

  // create a triangle shape for the dropdown indicator
  sf::CircleShape triangle{triangle_radius, 3};
  triangle.setFillColor(style.drop_down_button_style.triangle_color);

  // set the origin to the center of the triangle
  triangle.setOrigin(triangle.getLocalBounds().getCenter());
  if (!button.is_expanded) {
    // rotate the triangle 180 degrees to point downwards if the dropdown is
    // not expanded
    triangle.setRotation(sf::degrees(180.0f));
  }
  // position the triangle in the centre of the button
  sf::FloatRect button_bounds{button.position, button.size};
  triangle.setPosition(button_bounds.getCenter());

  // draw the triangle on the texture
  texture.draw(triangle);
}

/////////////////////////////////////////////////
void DrawDropDownContainerElement(sf::RenderTexture &texture,
                                  const DropDownContainerElement &container,
                                  const UIStyle &style) {
  // Draw the border and background for the container
  DrawBorderAndBackground(texture, container, style.drop_down_container_style);
}

/////////////////////////////////////////////////
void DrawBorderAndBackground(sf::RenderTexture &texture,
                             const UIElement &element, const Style &style) {

  // Create the rectangle using the element's position and size
  sf::RectangleShape rectangle(element.size);
  rectangle.setPosition(element.position);
  rectangle.setFillColor(style.background_color);
  rectangle.setOutlineColor(style.border_color);
  // Border thickness is negative to draw inwards
  rectangle.setOutlineThickness(-style.border_thickness);

  // Draw the rectangle on the texture
  texture.draw(rectangle);
}

/////////////////////////////////////////////////
void DrawBorderAndBackground(sf::RenderTexture &texture,
                             const UIElement &element,
                             const ButtonStyle &style) {
  // Create the rectangle using the element's position and size
  sf::RectangleShape rectangle(element.size);
  rectangle.setPosition(element.position);
  // Change color if hovered
  element.is_mouse_over ? rectangle.setFillColor(style.hover_color)
                        : rectangle.setFillColor(style.background_color);
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
} // namespace steamrot::logic::render::ui
