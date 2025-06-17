////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "UIRenderLogic.h"
#include "BaseLogic.h"
#include "CUserInterface.h"
#include "EntityHelpers.h"
#include "log_handler.h"
#include "user_interface_generated.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
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
  std::cout << "UIEngine: Initializing with provided flatbuffer config"
            << std::endl;
  AddStyles(logic_context.ui_config);
  std::cout << "UIEngine: Styles added from flatbuffer config" << std::endl;
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
  // add guard to check if config is not null
  if (!config) {
    log_handler::ProcessLog(
        spdlog::level::level_enum::err, log_handler::LogCode::kNoCode,
        "UIEngine: Null config provided, cannot add styles");
    return;
  }

  // set the panel style from the flatbuffer config
  m_panel_style.background_color =
      sf::Color(config->panel_style()->style()->background_color()->r(),
                config->panel_style()->style()->background_color()->g(),
                config->panel_style()->style()->background_color()->b(),
                config->panel_style()->style()->background_color()->a());
  m_panel_style.border_color =
      sf::Color(config->panel_style()->style()->border_color()->r(),
                config->panel_style()->style()->border_color()->g(),
                config->panel_style()->style()->border_color()->b(),
                config->panel_style()->style()->border_color()->a());
  m_panel_style.border_thickness =
      config->panel_style()->style()->border_thickness();
  m_panel_style.radius_resolution =
      config->panel_style()->style()->radius_resolution();
  m_panel_style.inner_margin =
      sf::Vector2f(config->panel_style()->style()->inner_margin()->x(),
                   config->panel_style()->style()->inner_margin()->y());

  // set the button style from the flatbuffer config
  m_button_style.background_color =
      sf::Color(config->button_style()->style()->background_color()->r(),
                config->button_style()->style()->background_color()->g(),
                config->button_style()->style()->background_color()->b(),
                config->button_style()->style()->background_color()->a());

  m_button_style.border_color =
      sf::Color(config->button_style()->style()->border_color()->r(),
                config->button_style()->style()->border_color()->g(),
                config->button_style()->style()->border_color()->b(),
                config->button_style()->style()->border_color()->a());

  m_button_style.border_thickness =
      config->button_style()->style()->border_thickness();

  m_button_style.radius_resolution =
      config->button_style()->style()->radius_resolution();

  m_button_style.inner_margin =
      sf::Vector2f(config->button_style()->style()->inner_margin()->x(),
                   config->button_style()->style()->inner_margin()->y());

  m_button_style.text_color =
      sf::Color(config->button_style()->text_color()->r(),
                config->button_style()->text_color()->g(),
                config->button_style()->text_color()->b(),
                config->button_style()->text_color()->a());

  m_button_style.hover_color =
      sf::Color(config->button_style()->hover_color()->r(),
                config->button_style()->hover_color()->g(),
                config->button_style()->hover_color()->b(),
                config->button_style()->hover_color()->a());

  log_handler::ProcessLog(
      spdlog::level::level_enum::info, log_handler::LogCode::kNoCode,
      "UIEngine: Button styles added from flatbuffer config");
}

void UIRenderLogic::DrawUIElements() {

  // define lambda function for recursively drawing UI elements
  std::function<void(const UIElement &, const sf::Vector2f &,
                     const sf::Vector2f &)>
      draw_ui_element = [&](const UIElement &element,
                            const sf::Vector2f &origin,
                            const sf::Vector2f &size) {
        // provide placement elements for the children
        float border_thickness;
        sf::Vector2f inner_margin;

        // Draw the element based on its type
        switch (element.element_type) {

        // Panel
        case UIElementType::UIElementType_Panel:
          DrawPanel(element, origin, size);

          border_thickness = m_panel_style.border_thickness;
          inner_margin = m_panel_style.inner_margin;

          break;

          // DropDownMenu
        case UIElementType::UIElementType_DropDownMenu:
          DrawDropDownMenu();
          break;

          // Button
        case UIElementType::UIElementType_Button: {

          DrawButton(element, origin, size);
          border_thickness = m_button_style.border_thickness;
          inner_margin = m_button_style.inner_margin;

          break;
        }
        // Add more cases for other UI element types as needed
        default:
          log_handler::ProcessLog(
              spdlog::level::level_enum::warn, log_handler::LogCode::kNoCode,
              "UIEngine: Unknown UI element type encountered");
          break;
        }

        // pull out number of children for convenience
        size_t number_of_children = element.child_elements.size();

        // create child origin and size variables
        sf::Vector2f child_origin;
        sf::Vector2f child_size;
        // Recursively draw child elements if they exist
        for (size_t i = 0; i < number_of_children; ++i) {

          switch (element.layout) {

          case LayoutType::LayoutType_Vertical: {

            // calculate size of the child element in a vertical layout
            float child_width =
                size.x - (2 * border_thickness) - (2 * inner_margin.x);

            float child_height = (size.y - (2 * border_thickness) -
                                  ((number_of_children + 1) * inner_margin.y)) /
                                 element.child_elements.size();

            child_size = sf::Vector2f(child_width, child_height);

            // calculate the position for the child element in a vertical
            // layout
            float child_origin_x = origin.x + border_thickness + inner_margin.x;

            float child_origin_y = origin.y + border_thickness +
                                   inner_margin.y +
                                   (i * (child_height + inner_margin.y));

            child_origin = sf::Vector2f(child_origin_x, child_origin_y);
            break;
          }
          case LayoutType::LayoutType_Horizontal: {

            // calculate size of the child element in a horizontal layout
            float child_width = (size.x - (2 * border_thickness) -
                                 ((number_of_children + 1) * inner_margin.x)) /
                                element.child_elements.size();
            float child_height =
                size.y - (2 * border_thickness) - (2 * inner_margin.y);

            child_size = sf::Vector2f(child_width, child_height);

            // calculate the position for the child element in a horizontal
            // layout
            float child_origin_x = origin.x + border_thickness +
                                   inner_margin.x +
                                   (i * (child_width + inner_margin.x));
            float child_origin_y = origin.y + border_thickness + inner_margin.y;
            child_origin = sf::Vector2f(child_origin_x, child_origin_y);
            break;
          }
          case LayoutType::LayoutType_Grid: {
            break;
          }
          default:
            break;
          }
          // finally, draw the child element recursivelye
          draw_ui_element(element.child_elements[i], child_origin, child_size);
        }
      };
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
        CUserInterface ui_component = GetComponent<CUserInterface>(
            entity_id, m_logic_context.scene_entities);

        // recursively draw the UI elements starting from the root
        // element
        draw_ui_element(ui_component.root_element,
                        ui_component.root_element.position,
                        ui_component.root_element.size);
      }
    }
  }
}

void UIRenderLogic::DrawPanel(const UIElement &element,
                              const sf::Vector2f &origin,
                              const sf::Vector2f &size) {

  DrawBoxWithRadiusCorners(element, origin, size,
                           m_panel_style.radius_resolution);
}

////////////////////////////////////////////////////////////
void UIRenderLogic::DrawDropDownMenu() {};

/////////////////////////////////////////////////
void UIRenderLogic::DrawButton(const UIElement &element,
                               const sf::Vector2f &origin,
                               const sf::Vector2f &size) {

  // Draw the box first
  DrawBoxWithRadiusCorners(element, origin, size,
                           m_button_style.radius_resolution);
}
/////////////////////////////////////////////////
void UIRenderLogic::DrawBoxWithRadiusCorners(const UIElement &ui_element,
                                             const sf::Vector2f &origin,
                                             const sf::Vector2f &size,
                                             const size_t &resolution) {

  // set element specific variables
  float radius;
  sf::Color border_color;
  sf::Color background_color;

  switch (ui_element.element_type) {

  // Panel
  case UIElementType::UIElementType_Panel: {
    radius = m_panel_style.border_thickness;
    border_color = m_panel_style.border_color;
    background_color = m_panel_style.background_color;
    break;
  }
    // Button
  case UIElementType::UIElementType_Button: {
    radius = m_button_style.border_thickness;
    border_color = m_button_style.border_color;
    background_color = m_button_style.background_color;
    break;
  }

  default: {
    radius = 0.f;
    border_color = sf::Color::Green;
    background_color = sf::Color::Transparent;
    break;
  }
  }
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

} // namespace steamrot
