////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "UIRenderLogic.h"
#include "BaseLogic.h"
#include "CUserInterface.h"
#include "DropDown.h"
#include "EntityHelpers.h"
#include "log_handler.h"
#include "themes_generated.h"
#include "user_interface_generated.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstddef>
#include <iostream>

namespace steamrot {

/////////////////////////////////////////////////////////////
UIRenderLogic::UIRenderLogic(const LogicContext logic_context)
    : Logic<CUserInterface>(logic_context) {
  // Initialize the UI engine with the provided flatbuffer configuration
  // This could include setting up styles, themes, and other UI elements
  if (!logic_context.ui_config) {
    log_handler::ProcessLog(spdlog::level::level_enum::err,
                            log_handler::LogCode::kNoCode,
                            "UIEngine: Null UI configuration provided, cannot "
                            "initialize UI styles");
    return;
  }
  AddStyles(logic_context.ui_config);
}
void UIRenderLogic::ProcessLogic() {

  // clear the render texture before drawing
  m_logic_context.scene_texture.clear();

  // Draw all UI elements to the render texture
  DrawUIElements();

  // display the render texture after drawing
  m_logic_context.scene_texture.display();
}

////////////////////////////////////////////////////////////
void UIRenderLogic::AddStyles(const themes::UIObjects *config) {
  std::cout << "UIEngine: Adding styles from config" << std::endl;
  // add guard to check if config is not null
  if (!config) {
    log_handler::ProcessLog(
        spdlog::level::level_enum::err, log_handler::LogCode::kNoCode,
        "UIEngine: Null config provided, cannot add styles");
    return;
  }

  // configure the styles from the flatbuffer config
  if (config->panel_style()) {
    ConfigurePanelStyle(config->panel_style());
  } else {
    log_handler::ProcessLog(
        spdlog::level::level_enum::err, log_handler::LogCode::kNoCode,
        "UIEngine: Null panel style provided, cannot configure panel style");
  }

  if (config->button_style()) {
    ConfigureButtonStyle(config->button_style());
  } else {
    log_handler::ProcessLog(
        spdlog::level::level_enum::err, log_handler::LogCode::kNoCode,
        "UIEngine: Null button style provided, cannot configure button style");
  }
  std::cout << "UIEngine: Configured button style" << std::endl;
  if (config->drop_down_container_style()) {
    ConfigureDropDownContainerStyle(config->drop_down_container_style());
  } else {
    log_handler::ProcessLog(
        spdlog::level::level_enum::err, log_handler::LogCode::kNoCode,
        "UIEngine: Null dropdown style provided, cannot configure dropdown "
        "style");
  }

  if (config->drop_down_list_style()) {
    ConfigureDropDownListStyle(config->drop_down_list_style());
  } else {
    log_handler::ProcessLog(
        spdlog::level::level_enum::err, log_handler::LogCode::kNoCode,
        "UIEngine: Null dropdown list style provided, cannot configure "
        "dropdown list style");
  }

  if (config->drop_down_item_style()) {
    ConfigureDropDownItemStyle(config->drop_down_item_style());
  } else {
    log_handler::ProcessLog(
        spdlog::level::level_enum::err, log_handler::LogCode::kNoCode,
        "UIEngine: Null dropdown item style provided, cannot configure "
        "dropdown item style");
  }

  if (config->drop_down_button_style()) {
    ConfigureDropDownButtonStyle(config->drop_down_button_style());
  } else {
    log_handler::ProcessLog(
        spdlog::level::level_enum::err, log_handler::LogCode::kNoCode,
        "UIEngine: Null dropdown button style provided, cannot configure "
        "dropdown button style");
  }

  std::cout << "UIEngine: Styles added successfully" << std::endl;
}

/////////////////////////////////////////////////
void UIRenderLogic::ConfigurePanelStyle(const themes::PanelStyle *panel_style) {
  // set the panel style from the flatbuffer config
  m_panel_style.background_color =
      sf::Color(panel_style->style()->background_color()->r(),
                panel_style->style()->background_color()->g(),
                panel_style->style()->background_color()->b(),
                panel_style->style()->background_color()->a());
  m_panel_style.border_color =
      sf::Color(panel_style->style()->border_color()->r(),
                panel_style->style()->border_color()->g(),
                panel_style->style()->border_color()->b(),
                panel_style->style()->border_color()->a());
  m_panel_style.border_thickness = panel_style->style()->border_thickness();
  m_panel_style.radius_resolution = panel_style->style()->radius_resolution();
  m_panel_style.inner_margin =
      sf::Vector2f(panel_style->style()->inner_margin()->x(),
                   panel_style->style()->inner_margin()->y());

  m_panel_style.minimum_size =
      sf::Vector2f(panel_style->style()->minimum_size()->x(),
                   panel_style->style()->minimum_size()->y());
  m_panel_style.maximum_size =
      sf::Vector2f(panel_style->style()->maximum_size()->x(),
                   panel_style->style()->maximum_size()->y());
  std::cout << "UIEngine: Panel style configured successfully" << std::endl;
}

/////////////////////////////////////////////////
void UIRenderLogic::ConfigureButtonStyle(
    const themes::ButtonStyle *button_style) {
  // set the button style from the flatbuffer config
  m_button_style.background_color =
      sf::Color(button_style->style()->background_color()->r(),
                button_style->style()->background_color()->g(),
                button_style->style()->background_color()->b(),
                button_style->style()->background_color()->a());
  m_button_style.border_color =
      sf::Color(button_style->style()->border_color()->r(),
                button_style->style()->border_color()->g(),
                button_style->style()->border_color()->b(),
                button_style->style()->border_color()->a());
  m_button_style.border_thickness = button_style->style()->border_thickness();
  m_button_style.radius_resolution = button_style->style()->radius_resolution();
  m_button_style.inner_margin =
      sf::Vector2f(button_style->style()->inner_margin()->x(),
                   button_style->style()->inner_margin()->y());
  m_button_style.text_color = sf::Color(
      button_style->text_color()->r(), button_style->text_color()->g(),
      button_style->text_color()->b(), button_style->text_color()->a());
  m_button_style.hover_color = sf::Color(
      button_style->hover_color()->r(), button_style->hover_color()->g(),
      button_style->hover_color()->b(), button_style->hover_color()->a());

  m_button_style.minimum_size =
      sf::Vector2f(button_style->style()->minimum_size()->x(),
                   button_style->style()->minimum_size()->y());
  m_button_style.maximum_size =
      sf::Vector2f(button_style->style()->maximum_size()->x(),
                   button_style->style()->maximum_size()->y());
  std::cout << "UIEngine: Button style configured successfully" << std::endl;
}

/////////////////////////////////////////////////
void UIRenderLogic::ConfigureDropDownContainerStyle(
    const themes::DropDownContainerStyle *dropdown_style) {

  // set the DropDown style from the flatbuffer config
  m_dropdown_style.background_color =
      sf::Color(dropdown_style->style()->background_color()->r(),
                dropdown_style->style()->background_color()->g(),
                dropdown_style->style()->background_color()->b(),
                dropdown_style->style()->background_color()->a());
  m_dropdown_style.border_color =
      sf::Color(dropdown_style->style()->border_color()->r(),
                dropdown_style->style()->border_color()->g(),
                dropdown_style->style()->border_color()->b(),
                dropdown_style->style()->border_color()->a());
  m_dropdown_style.border_thickness =
      dropdown_style->style()->border_thickness();
  m_dropdown_style.radius_resolution =
      dropdown_style->style()->radius_resolution();
  m_dropdown_style.inner_margin =
      sf::Vector2f(dropdown_style->style()->inner_margin()->x(),
                   dropdown_style->style()->inner_margin()->y());

  m_dropdown_style.drop_symbol_ratio = dropdown_style->drop_symbol_ratio();

  m_dropdown_style.minimum_size =
      sf::Vector2f(dropdown_style->style()->minimum_size()->x(),
                   dropdown_style->style()->minimum_size()->y());
  m_dropdown_style.maximum_size =
      sf::Vector2f(dropdown_style->style()->maximum_size()->x(),
                   dropdown_style->style()->maximum_size()->y());
  std::cout << "UIEngine: DropDown style configured successfully" << std::endl;
}

/////////////////////////////////////////////////
void UIRenderLogic::ConfigureDropDownListStyle(
    const themes::DropDownListStyle *dropdown_list_style) {
  // set the DropDownList style from the flatbuffer config
  m_dropdown_list_style.background_color =
      sf::Color(dropdown_list_style->style()->background_color()->r(),
                dropdown_list_style->style()->background_color()->g(),
                dropdown_list_style->style()->background_color()->b(),
                dropdown_list_style->style()->background_color()->a());
  m_dropdown_list_style.border_color =
      sf::Color(dropdown_list_style->style()->border_color()->r(),
                dropdown_list_style->style()->border_color()->g(),
                dropdown_list_style->style()->border_color()->b(),
                dropdown_list_style->style()->border_color()->a());
  m_dropdown_list_style.border_thickness =
      dropdown_list_style->style()->border_thickness();
  m_dropdown_list_style.radius_resolution =
      dropdown_list_style->style()->radius_resolution();
  m_dropdown_list_style.inner_margin =
      sf::Vector2f(dropdown_list_style->style()->inner_margin()->x(),
                   dropdown_list_style->style()->inner_margin()->y());
  m_dropdown_list_style.minimum_size =
      sf::Vector2f(dropdown_list_style->style()->minimum_size()->x(),
                   dropdown_list_style->style()->minimum_size()->y());
  m_dropdown_list_style.maximum_size =
      sf::Vector2f(dropdown_list_style->style()->maximum_size()->x(),
                   dropdown_list_style->style()->maximum_size()->y());

  m_dropdown_list_style.text_color =
      sf::Color(dropdown_list_style->text_color()->r(),
                dropdown_list_style->text_color()->g(),
                dropdown_list_style->text_color()->b(),
                dropdown_list_style->text_color()->a());
  m_dropdown_list_style.hover_color =
      sf::Color(dropdown_list_style->hover_color()->r(),
                dropdown_list_style->hover_color()->g(),
                dropdown_list_style->hover_color()->b(),
                dropdown_list_style->hover_color()->a());

  m_dropdown_list_style.font = dropdown_list_style->font()->str();

  std::cout << "UIEngine: DropDownList style configured successfully"
            << std::endl;
}

/////////////////////////////////////////////////
void UIRenderLogic::ConfigureDropDownItemStyle(
    const themes::DropDownItemStyle *dropdown_item_style) {
  // set the DropDownItem style from the flatbuffer config
  m_dropdown_item_style.background_color =
      sf::Color(dropdown_item_style->style()->background_color()->r(),
                dropdown_item_style->style()->background_color()->g(),
                dropdown_item_style->style()->background_color()->b(),
                dropdown_item_style->style()->background_color()->a());
  m_dropdown_item_style.border_color =
      sf::Color(dropdown_item_style->style()->border_color()->r(),
                dropdown_item_style->style()->border_color()->g(),
                dropdown_item_style->style()->border_color()->b(),
                dropdown_item_style->style()->border_color()->a());
  m_dropdown_item_style.border_thickness =
      dropdown_item_style->style()->border_thickness();
  m_dropdown_item_style.radius_resolution =
      dropdown_item_style->style()->radius_resolution();
  m_dropdown_item_style.inner_margin =
      sf::Vector2f(dropdown_item_style->style()->inner_margin()->x(),
                   dropdown_item_style->style()->inner_margin()->y());
  m_dropdown_item_style.minimum_size =
      sf::Vector2f(dropdown_item_style->style()->minimum_size()->x(),
                   dropdown_item_style->style()->minimum_size()->y());
  m_dropdown_item_style.maximum_size =
      sf::Vector2f(dropdown_item_style->style()->maximum_size()->x(),
                   dropdown_item_style->style()->maximum_size()->y());

  m_dropdown_item_style.text_color =
      sf::Color(dropdown_item_style->text_color()->r(),
                dropdown_item_style->text_color()->g(),
                dropdown_item_style->text_color()->b(),
                dropdown_item_style->text_color()->a());
  m_dropdown_item_style.hover_color =
      sf::Color(dropdown_item_style->hover_color()->r(),
                dropdown_item_style->hover_color()->g(),
                dropdown_item_style->hover_color()->b(),
                dropdown_item_style->hover_color()->a());

  std::cout << "UIEngine: DropDownItem style configured successfully"
            << std::endl;
}

/////////////////////////////////////////////////
void UIRenderLogic::ConfigureDropDownButtonStyle(
    const themes::DropDownButtonStyle *dropdown_button_style) {
  // set the DropDownButton style from the flatbuffer config
  m_dropdown_button_style.background_color =
      sf::Color(dropdown_button_style->style()->background_color()->r(),
                dropdown_button_style->style()->background_color()->g(),
                dropdown_button_style->style()->background_color()->b(),
                dropdown_button_style->style()->background_color()->a());
  m_dropdown_button_style.border_color =
      sf::Color(dropdown_button_style->style()->border_color()->r(),
                dropdown_button_style->style()->border_color()->g(),
                dropdown_button_style->style()->border_color()->b(),
                dropdown_button_style->style()->border_color()->a());
  m_dropdown_button_style.border_thickness =
      dropdown_button_style->style()->border_thickness();
  m_dropdown_button_style.radius_resolution =
      dropdown_button_style->style()->radius_resolution();
  m_dropdown_button_style.inner_margin =
      sf::Vector2f(dropdown_button_style->style()->inner_margin()->x(),
                   dropdown_button_style->style()->inner_margin()->y());
  m_dropdown_button_style.triangle_color =
      sf::Color(dropdown_button_style->triangle_color()->r(),
                dropdown_button_style->triangle_color()->g(),
                dropdown_button_style->triangle_color()->b(),
                dropdown_button_style->triangle_color()->a());
  m_dropdown_button_style.hover_color =
      sf::Color(dropdown_button_style->hover_color()->r(),
                dropdown_button_style->hover_color()->g(),
                dropdown_button_style->hover_color()->b(),
                dropdown_button_style->hover_color()->a());
  std::cout << "UIEngine: DropDownButton style configured successfully"
            << std::endl;
}

void UIRenderLogic::DrawUIElements() {

  // cycle through all the Archetype IDs associated with this logic
  // class
  for (const ArchetypeID &archetype_id : m_archetype_IDs) {

    // if it is not in the archetyps map, then skip
    if (!m_logic_context.archetypes.contains(archetype_id)) {
      continue;
    } else {
      // get the archetype from the map
      Archetype &archetype = m_logic_context.archetypes[archetype_id];

      // cycle through all the entity indexs in the archetype
      for (size_t entity_id : archetype) {

        // get the CUserInterface component
        CUserInterface &ui_component = GetComponent<CUserInterface>(
            entity_id, m_logic_context.scene_entities);

        // recursively draw the UI elements starting from the root
        // element
        RecursiveDrawUIElement(ui_component.m_root_element);
      }
    }
  }
}

/////////////////////////////////////////////////
void UIRenderLogic::RecursiveDrawUIElement(UIElement &element) {
  // provide placement elements for the children
  float border_thickness;
  sf::Vector2f inner_margin;

  // define visitor function to set the styles based on the element type
  auto UIVisitor = [&](auto &element_type) -> void {
    if constexpr (std::is_same_v<std::decay_t<decltype(element_type)>, Panel>) {

      DrawPanel(element);
      border_thickness = m_panel_style.border_thickness;
      inner_margin = m_panel_style.inner_margin;
    } else if constexpr (std::is_same_v<std::decay_t<decltype(element_type)>,
                                        Button>) {

      DrawButton(element);
      border_thickness = m_button_style.border_thickness;
      inner_margin = m_button_style.inner_margin;

    } else if constexpr (std::is_same_v<std::decay_t<decltype(element_type)>,
                                        DropDownContainer>) {

      DrawDropDownContainer(element);
      border_thickness = m_dropdown_style.border_thickness;
      inner_margin = m_dropdown_style.inner_margin;

    } else if constexpr (std::is_same_v<std::decay_t<decltype(element_type)>,
                                        DropDownList>) {
      DrawDropDownList(element);
      border_thickness = m_dropdown_list_style.border_thickness;
      inner_margin = m_dropdown_list_style.inner_margin;
    } else if constexpr (std::is_same_v<std::decay_t<decltype(element_type)>,
                                        DropDownItem>) {
      DrawDropDownItem(element);
      border_thickness = m_dropdown_item_style.border_thickness;
      inner_margin = m_dropdown_item_style.inner_margin;
    } else if constexpr (std::is_same_v<std::decay_t<decltype(element_type)>,
                                        DropDownButton>) {
      DrawDropDownButton(element);

      border_thickness = m_dropdown_button_style.border_thickness;
      inner_margin = m_dropdown_button_style.inner_margin;
    } else {
      std::cout << "Unknown element type for drawing" << std::endl;
    }
  };
  // call the visitor function with the element type
  std::visit(UIVisitor, element.element_type);

  // GUARD: return early if children are not active
  if (!element.children_active) {
    return;
  }

  // Get the spacing strategy based on the element's spacing strategy
  SpacingStrategy spacing_strategy =
      GetSpacingStrategy(element.spacing_strategy);

  // call the spacing strategy to adjust the size and position of the children
  spacing_strategy(element, inner_margin, border_thickness);

  // draw the child elements recursively
  for (size_t i = 0; i < element.child_elements.size(); ++i) {
    RecursiveDrawUIElement(element.child_elements[i]);
  }
}

/////////////////////////////////////////////////
void UIRenderLogic::DrawPanel(UIElement &element) {
  DrawBoxWithRadiusCorners(element);
}

/////////////////////////////////////////////////
void UIRenderLogic::DrawButton(UIElement &element) {

  // Draw the box first
  DrawBoxWithRadiusCorners(element);

  // get tht button details from the variant
  Button button_element = std::get<Button>(element.element_type);

  // Create a text object for the button label
  sf::Text button_text(
      m_logic_context.asset_manager.GetFont(m_button_style.font),
      button_element.label);

  button_text.setCharacterSize(24); // Set the character size
  button_text.setFillColor(m_button_style.text_color);

  // Center the text within the button
  button_text.setPosition(
      {element.position.x +
           (element.size.x - button_text.getLocalBounds().size.x) / 2,

       // Text is aligned so that the bottom of the text is the bottom of the
       // y bounds so we need to adjust the y position to account for that
       element.position.y +
           (element.size.y / 2 - button_text.getLocalBounds().size.y)});

  // Draw the button text to the render texture
  m_logic_context.scene_texture.draw(button_text);
}
/////////////////////////////////////////////////
void UIRenderLogic::DrawDropDownContainer(UIElement &element) {

  // adjust the size of th dropdown element
  AdjustSize(element);

  // Draw the box first
  DrawBoxWithRadiusCorners(element);
}

/////////////////////////////////////////////////
void UIRenderLogic::DrawDropDownList(UIElement &element) {
  // This should only ever be contained in a DropDownContainer, so we don't
  // adjust size for this element

  // Draw a standard rectangle for this
  sf::RectangleShape dropdown_shape;
  dropdown_shape.setSize(element.size);
  dropdown_shape.setPosition(element.position);
  dropdown_shape.setFillColor(m_dropdown_list_style.background_color);
  // no border for this element
  dropdown_shape.setOutlineThickness(0.f);
  // draw the rectangle to the render texture
  m_logic_context.scene_texture.draw(dropdown_shape);

  // get the dropdown list specific details from the variant
  DropDownList dropdown_list_element =
      std::get<DropDownList>(element.element_type);
  // Create a text object for the dropdown list label depending on whether its
  // expanded

  std::string label_to_use = dropdown_list_element.is_expanded
                                 ? dropdown_list_element.expanded_label
                                 : dropdown_list_element.label;

  sf::Text dropdown_text(

      m_logic_context.asset_manager.GetFont(m_dropdown_list_style.font),
      label_to_use);
  dropdown_text.setCharacterSize(
      m_dropdown_list_style.font_size); // Set the character size
  dropdown_text.setFillColor(m_dropdown_list_style.text_color);
  // Center the text within the dropdown list
  dropdown_text.setPosition(
      {element.position.x +
           (element.size.x - dropdown_text.getLocalBounds().size.x) / 2,
       // Text is aligned so that the bottom of the text is the bottom of the
       // y bounds so we need to adjust the y position to account for that
       element.position.y +
           (element.size.y / 2 - dropdown_text.getLocalBounds().size.y)});
  // Draw the dropdown list text to the render texture
  m_logic_context.scene_texture.draw(dropdown_text);
}

/////////////////////////////////////////////////
void UIRenderLogic::DrawDropDownItem(UIElement &element) {
  // This should only ever be contained in a DropDownList, so we don't
  // adjust size for this element
  // Draw a standard rectangle for this
  sf::RectangleShape dropdown_shape;
  dropdown_shape.setSize(element.size);
  dropdown_shape.setPosition(element.position);

  // set the fill color based on whether the mouse is over the element
  if (element.mouse_over) {
    dropdown_shape.setFillColor(m_dropdown_item_style.hover_color);
  } else {
    dropdown_shape.setFillColor(m_dropdown_item_style.background_color);
  }
  // no border for this element
  dropdown_shape.setOutlineThickness(0.f);
  // draw the rectangle to the render texture
  m_logic_context.scene_texture.draw(dropdown_shape);
  // get the dropdown item specific details from the variant
  DropDownItem dropdown_item_element =
      std::get<DropDownItem>(element.element_type);
  // Create a text object for the dropdown item label
  sf::Text dropdown_text(
      m_logic_context.asset_manager.GetFont(m_dropdown_item_style.font),
      dropdown_item_element.label);
  dropdown_text.setCharacterSize(12); // Set the character size
  dropdown_text.setFillColor(m_dropdown_item_style.text_color);
  // Center the text within the dropdown item
  dropdown_text.setPosition(
      {element.position.x +
           (element.size.x - dropdown_text.getLocalBounds().size.x) / 2,
       // Text is aligned so that the bottom of the text is the bottom of the
       // y bounds so we need to adjust the y position to account for that
       element.position.y +
           (element.size.y / 2 - dropdown_text.getLocalBounds().size.y)});
  // Draw the dropdown item text to the render texture
  m_logic_context.scene_texture.draw(dropdown_text);
}

/////////////////////////////////////////////////
void UIRenderLogic::DrawDropDownButton(UIElement &element) {

  // draw a standard rectangle and fill it with the background color
  sf::RectangleShape drop_down_button_container;
  drop_down_button_container.setSize(element.size);
  drop_down_button_container.setPosition(element.position);
  drop_down_button_container.setFillColor(
      m_dropdown_button_style.background_color);
  // no border for this element
  drop_down_button_container.setOutlineThickness(0.f);
  // draw the rectangle to the render texture
  m_logic_context.scene_texture.draw(drop_down_button_container);

  // now create the triangle for the dropdown button
  sf::VertexArray triangle_button(sf::PrimitiveType::Triangles,
                                  3); // create a triangle vertex array

  // set the position of the triangle vertices
  triangle_button[0].position = sf::Vector2f(element.position);
  triangle_button[1].position =
      sf::Vector2f(element.position.x + element.size.x, element.position.y);
  triangle_button[2].position =
      sf::Vector2f(element.position.x + (element.size.x / 2),
                   element.position.y + element.size.y);
  // set the fill color of the triangle
  if (element.mouse_over) {
    triangle_button[0].color = m_dropdown_button_style.hover_color;
    triangle_button[1].color = m_dropdown_button_style.hover_color;
    triangle_button[2].color = m_dropdown_button_style.hover_color;
  } else {
    triangle_button[0].color = m_dropdown_button_style.triangle_color;
    triangle_button[1].color = m_dropdown_button_style.triangle_color;
    triangle_button[2].color = m_dropdown_button_style.triangle_color;
  }

  // draw the triangle to the render texture
  m_logic_context.scene_texture.draw(triangle_button);
}
/////////////////////////////////////////////////
void UIRenderLogic::AdjustSize(UIElement &element) {
  // variales to hold the minimum and maximum sizes
  sf::Vector2f min_size;
  sf::Vector2f max_size;

  // the minimum and maximum sizes are contained within the relevant style
  // we get that using a visitor function and assign it to variables
  auto UIVisitor = [&](auto &element_type) -> void {
    if constexpr (std::is_same_v<std::decay_t<decltype(element_type)>, Panel>) {
      min_size = m_panel_style.minimum_size;
      max_size = m_panel_style.maximum_size;
    } else if constexpr (std::is_same_v<std::decay_t<decltype(element_type)>,
                                        Button>) {
      min_size = m_button_style.minimum_size;
      max_size = m_button_style.maximum_size;
    } else if constexpr (std::is_same_v<std::decay_t<decltype(element_type)>,
                                        DropDownContainer>) {

      min_size = m_dropdown_style.minimum_size;
      max_size = m_dropdown_style.maximum_size;

    } else {
      std::cout << "Unknown element type for size adjustment" << std::endl;
    }
  };

  // call the visitor function with the element type
  std::visit(UIVisitor, element.element_type);

  // adjust the size of the element based on the minimum and maximum sizes
  // if the size is smaller than the minimum size, set it to the minimum size
  if (element.size.x < min_size.x) {
    element.size.x = min_size.x;
  }
  if (element.size.y < min_size.y) {
    element.size.y = min_size.y;
  }
  // if the size is larger than the maximum size, set it to the maximum size
  if (element.size.x > max_size.x) {
    element.size.x = max_size.x;
  }
  if (element.size.y > max_size.y) {
    element.size.y = max_size.y;
  }
}

/////////////////////////////////////////////////
void UIRenderLogic::DrawBoxWithRadiusCorners(UIElement &ui_element) {
  // set element specific variables
  float radius;
  size_t resolution;
  sf::Color border_color;
  sf::Color background_color;
  sf::Vector2f size = ui_element.size;
  sf::Vector2f origin = ui_element.position;

  // define visitor function to set the styles based on the element type
  auto UIVisitor = [&](auto &element_type) -> void {
    if constexpr (std::is_same_v<std::decay_t<decltype(element_type)>, Panel>) {

      radius = m_panel_style.border_thickness;
      resolution = m_panel_style.radius_resolution;
      border_color = m_panel_style.border_color;
      background_color = m_panel_style.background_color;
    } else if constexpr (std::is_same_v<std::decay_t<decltype(element_type)>,
                                        Button>) {

      radius = m_button_style.border_thickness;
      resolution = m_button_style.radius_resolution;
      border_color = m_button_style.border_color;

      // background color based on mouse over state
      if (ui_element.mouse_over) {
        background_color = m_button_style.hover_color;
      } else {
        background_color = m_button_style.background_color;
      }
    } else if constexpr (std::is_same_v<std::decay_t<decltype(element_type)>,
                                        DropDownContainer>) {

      radius = m_dropdown_style.border_thickness;
      resolution = m_dropdown_style.radius_resolution;
      border_color = m_dropdown_style.border_color;

    } else {
      std::cout << "Unknown style type selected" << std::endl;
    }
  };
  // call the visitor function with the element type
  std::visit(UIVisitor, ui_element.element_type);

  // lambda function to draw one side (quarter circle and box) based off
  // centre
  auto draw_corner = [&](const sf::Vector2f &centre, const int x_sign,
                         const int y_sign) {
    // create a triangle fan for the rounded corner
    sf::VertexArray corner(sf::PrimitiveType::TriangleFan);

    // calculate x offset
    float x_offset = (x_sign * (size.x / 2)) - (x_sign * radius);
    // calculate y offset
    float y_offset = (y_sign * (size.y / 2)) - (y_sign * radius);
    // combine for offset
    sf::Vector2f offset(x_offset, y_offset);

    // set the origin of the corner
    corner.append(sf::Vertex(centre + offset, border_color));

    // add points to the corner for the rounded part
    for (size_t i = 0; i <= resolution; ++i) {

      // define the segmented angle
      float angle = (i * M_PI_2) / resolution;

      // calculate the x and y coefficients based on the angle
      float x = radius * cos(angle) * x_sign;
      float y = radius * sin(angle) * y_sign;

      sf::Vector2f segmented_point{centre.x + offset.x + x,
                                   centre.y + offset.y + y};

      // append the vertex to the corner array
      corner.append(sf::Vertex(segmented_point, border_color));
    }

    // draw the corner to the render texture
    m_logic_context.scene_texture.draw(corner);
  };

  // calculate the center of the box
  sf::Vector2f center = origin + sf::Vector2f(size.x / 2, size.y / 2);
  // draw the four sides of the box
  draw_corner(center, 1, 1);   // top right
  draw_corner(center, -1, 1);  // top left
  draw_corner(center, -1, -1); // bottom left
  draw_corner(center, 1, -1);  // bottom right

  // draw the four sides of the box
  sf::RectangleShape top_side(sf::Vector2f(size.x - 2 * radius, radius));
  top_side.setPosition({origin.x + radius, origin.y});
  top_side.setFillColor(border_color);
  m_logic_context.scene_texture.draw(top_side);

  sf::RectangleShape bottom_side(sf::Vector2f(size.x - 2 * radius, radius));
  bottom_side.setPosition({origin.x + radius, origin.y + size.y - radius});
  bottom_side.setFillColor(border_color);
  m_logic_context.scene_texture.draw(bottom_side);
  sf::RectangleShape left_side(sf::Vector2f(radius, size.y - 2 * radius));
  left_side.setPosition({origin.x, origin.y + radius});
  left_side.setFillColor(border_color);
  m_logic_context.scene_texture.draw(left_side);
  sf::RectangleShape right_side(sf::Vector2f(radius, size.y - 2 * radius));
  right_side.setPosition({origin.x + size.x - radius, origin.y + radius});
  right_side.setFillColor(border_color);
  m_logic_context.scene_texture.draw(right_side);

  // draw the background of the box taking into account the radius
  sf::RectangleShape background(
      sf::Vector2f(size.x - 2 * radius, size.y - 2 * radius));
  background.setPosition({origin.x + radius, origin.y + radius});
  background.setFillColor(background_color);
  m_logic_context.scene_texture.draw(background);
}

/////////////////////////////////////////////////
SpacingStrategy UIRenderLogic::GetSpacingStrategy(
    const SpacingAndSizingType &spacing_strategy) const {

  // return a spacing strategy based on the enum value
  switch (spacing_strategy) {
  case (SpacingAndSizingType::SpacingAndSizingType_Even): {
  }
    return EvenSpacingStrategy;

  case (SpacingAndSizingType::SpacingAndSizingType_Ratioed): {
    return RatioedSpacingStrategy;
  }
  case (SpacingAndSizingType::SpacingAndSizingType_DropDownList): {
    return DropDownSpacingStrategy;
  }
  default:

    return EvenSpacingStrategy;
  };
}

/////////////////////////////////////////////////
void UIRenderLogic::EvenSpacingStrategy(UIElement &ul_element,
                                        sf::Vector2f &inner_margin,
                                        float &parent_border_thickness) {

  // get the orientation of the children
  LayoutType layout = ul_element.layout;

  // get the children
  std::vector<UIElement> &children = ul_element.child_elements;

  // get the number of children
  size_t number_of_children = children.size();

  // calculate the size and posotion of each child based on the layout
  if (layout == LayoutType::LayoutType_Vertical) {
    // calculate the height of each child
    float child_height = (ul_element.size.y - (2 * parent_border_thickness) -
                          ((number_of_children + 1) * inner_margin.y)) /
                         number_of_children;
    // set the size and position of each child
    for (size_t i = 0; i < number_of_children; ++i) {
      children[i].size.x = ul_element.size.x - (2 * parent_border_thickness) -
                           (2 * inner_margin.x);
      children[i].size.y = child_height;
      children[i].position.x =
          ul_element.position.x + parent_border_thickness + inner_margin.x;
      children[i].position.y = ul_element.position.y + parent_border_thickness +
                               inner_margin.y +
                               (i * (child_height + inner_margin.y));
    }
  } else if (layout == LayoutType::LayoutType_Horizontal) {
    // calculate the width of each child
    float child_width = (ul_element.size.x - (2 * parent_border_thickness) -
                         ((number_of_children + 1) * inner_margin.x)) /
                        number_of_children;
    // set the size and position of each child
    for (size_t i = 0; i < number_of_children; ++i) {
      children[i].size.x = child_width;
      children[i].size.y = ul_element.size.y - (2 * parent_border_thickness) -
                           (2 * inner_margin.y);
      children[i].position.x = ul_element.position.x + parent_border_thickness +
                               inner_margin.x +
                               (i * (child_width + inner_margin.x));
      children[i].position.y =
          ul_element.position.y + parent_border_thickness + inner_margin.y;
    }
  }
}

/////////////////////////////////////////////////
void UIRenderLogic::RatioedSpacingStrategy(UIElement &ui_element,
                                           sf::Vector2f &inner_margin,
                                           float &parent_border_thickness) {
  // get the orientation of the children
  LayoutType layout = ui_element.layout;
  // get the children
  std::vector<UIElement> &children = ui_element.child_elements;
  // get the number of children
  size_t number_of_children = children.size();

  // need to account for ratios adding up to more than 1.0, or some not having
  // ratios
  // default: if sum is greater than 1.0, then we will scale down all the parts
  // default: if sum is less than 1.0, then we will scale up all the parts
  // default: if elements do not have ratio then we will asign an even ratio

  // calculate the correct ratio for each child
  float total_ratio = 0.0f;
  std::vector<float> ratios(
      number_of_children, (1.0f / number_of_children)); // default ratio of 1.0
  for (size_t i = 0; i < number_of_children; ++i) {
    // if the child has a ratio, then use it
    if (children[i].ratio.has_value() && children[i].ratio > 0.0f) {
      ratios[i] = children[i].ratio.value();
    }
    total_ratio += ratios[i];
  }
  // if the total ratio does not equal 1.0, then scale
  if (total_ratio != 1.0f) {
    for (size_t i = 0; i < number_of_children; ++i) {
      ratios[i] /= total_ratio;
    }
  }

  // now apply ratios depending on layout strategy
  if (layout == LayoutType::LayoutType_Vertical) {
    // width is constant for each child
    float child_width = ui_element.size.x - (2 * parent_border_thickness) -
                        (2 * inner_margin.x);

    // variable to keep track of position moving down
    float moving_vertical_position{ui_element.position.y +
                                   parent_border_thickness + inner_margin.y};

    for (size_t i = 0; i < number_of_children; ++i) {
      // calculate the height of each child based on the ratio
      float child_height = (ui_element.size.y - (2 * parent_border_thickness) -
                            ((number_of_children + 1) * inner_margin.y)) *
                           ratios[i];
      // set the size and position of each child
      children[i].size.x = child_width;
      children[i].size.y = child_height;
      children[i].position.x =
          ui_element.position.x + parent_border_thickness + inner_margin.x;
      children[i].position.y = moving_vertical_position;

      // add child_height and inner margin to moving_vertical_position
      moving_vertical_position =
          moving_vertical_position + child_height + inner_margin.x;
    }

  } else if (layout == LayoutType::LayoutType_Horizontal) {
    // height is constant for each child
    float child_height = ui_element.size.y - (2 * parent_border_thickness) -
                         (2 * inner_margin.y);

    // variable to keep track of position moving right
    float moving_horizontal_position{ui_element.position.x +
                                     parent_border_thickness + inner_margin.x};

    for (size_t i = 0; i < number_of_children; ++i) {
      // calculate the width of each child based on the ratio
      float child_width = (ui_element.size.x - (2 * parent_border_thickness) -
                           ((number_of_children + 1) * inner_margin.x)) *
                          ratios[i];

      // set the size and position of each child
      children[i].size.y = child_height;
      children[i].size.x = child_width;
      children[i].position.y =
          ui_element.position.y + parent_border_thickness + inner_margin.y;
      children[i].position.x = moving_horizontal_position;

      // add child_width and inner margin to moving_horizontal_position
      moving_horizontal_position =
          moving_horizontal_position + child_width + inner_margin.x;
    }
  }
}

void UIRenderLogic::DropDownSpacingStrategy(UIElement &ui_element,
                                            sf::Vector2f &inner_margin,
                                            float &parent_border_thickness) {

  // This strategy takes the parent element's size and positions the children
  // beneath it with no margins or borders.

  std::vector<UIElement> &children = ui_element.child_elements;
  size_t number_of_children = children.size();

  std::cout << "Drop Down list starts at: " << ui_element.position.x << ", "
            << ui_element.position.y << std::endl;
  // size is constant
  sf::Vector2f child_size = ui_element.size;

  // iterate through the children and set their size and position
  for (size_t i = 0; i < number_of_children; ++i) {
    // set the size of the child
    children[i].size = child_size;
    // set the position of the child
    children[i].position.x = ui_element.position.x;
    children[i].position.y =
        ui_element.position.y + (ui_element.size.y * (i + 1));
    std::cout << "UIEngine: Child " << i
              << " positioned at: " << children[i].position.x << ", "
              << children[i].position.y << std::endl;
  }
}
} // namespace steamrot
