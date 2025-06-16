////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "UIRenderLogic.h"
#include "BaseLogic.h"
#include "CUserInterface.h"
#include "EntityHelpers.h"
#include "log_handler.h"

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
  std::cout << "UIEngine: Adding Panel config" << std::endl;
  // add panel styles from flatbuffer config
  m_panel_style.background_color.r =
      config->panel_style()->background_color()->r();
  m_panel_style.background_color.g =
      config->panel_style()->background_color()->g();
  m_panel_style.background_color.b =
      config->panel_style()->background_color()->b();
  m_panel_style.background_color.a =
      config->panel_style()->background_color()->a();
  m_panel_style.border_color.r = config->panel_style()->border_color()->r();
  m_panel_style.border_color.g = config->panel_style()->border_color()->g();
  m_panel_style.border_color.b = config->panel_style()->border_color()->b();
  m_panel_style.border_color.a = config->panel_style()->border_color()->a();
  m_panel_style.border_thickness = config->panel_style()->border_thickness();
  m_panel_style.radius_resolution = config->panel_style()->radius_resolution();

  std::cout << "UIEngine: Panel config added" << std::endl;
  std::cout << "UIEngine: Adding Button config" << std::endl;
  // add button styles from flatbuffer config
  m_button_style.border_color.r = config->button_style()->border_color()->r();
  m_button_style.border_color.g = config->button_style()->border_color()->g();
  m_button_style.border_color.b = config->button_style()->border_color()->b();
  m_button_style.border_color.a = config->button_style()->border_color()->a();
  m_button_style.background_color.r =
      config->button_style()->background_color()->r();
  m_button_style.background_color.g =
      config->button_style()->background_color()->g();
  m_button_style.background_color.b =
      config->button_style()->background_color()->b();
  m_button_style.background_color.a =
      config->button_style()->background_color()->a();
  m_button_style.text_color.r = config->button_style()->text_color()->r();
  m_button_style.text_color.g = config->button_style()->text_color()->g();
  m_button_style.text_color.b = config->button_style()->text_color()->b();
  m_button_style.text_color.a = config->button_style()->text_color()->a();

  log_handler::ProcessLog(
      spdlog::level::level_enum::info, log_handler::LogCode::kNoCode,
      "UIEngine: Button styles added from flatbuffer config");
}

void UIRenderLogic::DrawUIElements() {

  // define lambda function for recursively drawing UI elements
  std::function<void(const UIElement &)> draw_ui_element =
      [&](const UIElement &element) {
        // Draw the element based on its type
        switch (element.element_type) {
        case UIElementType::UIElementType_Panel:
          DrawPanel(element);
          break;
        case UIElementType::UIElementType_DropDownMenu:
          DrawDropDownMenu();
          break;
        // Add more cases for other UI element types as needed
        default:
          log_handler::ProcessLog(
              spdlog::level::level_enum::warn, log_handler::LogCode::kNoCode,
              "UIEngine: Unknown UI element type encountered");
          break;
        }
        // Recursively draw child elements if they exist
        for (const auto &child : element.child_elements) {
          draw_ui_element(child);
        }
      };
  // cycle through all the Archetype IDs associated with this logic class
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

        // recursively draw the UI elements starting from the root element
        draw_ui_element(ui_component.root_element);
      }
    }
  }
}

void UIRenderLogic::DrawPanel(const UIElement &element) {

  DrawBoxWithRadiusCorners(element, {100.f, 100.f}, {500.f, 300.f},
                           m_panel_style.radius_resolution);
}

////////////////////////////////////////////////////////////
void UIRenderLogic::DrawDropDownMenu() {};

/////////////////////////////////////////////////
void UIRenderLogic::DrawBoxWithRadiusCorners(const UIElement &ui_element,
                                             const sf::Vector2f &origin,
                                             const sf::Vector2f &size,
                                             const size_t &resolution) {

  // get thickness/radius from the UIElement
  float radius;
  if (ui_element.element_type == UIElementType::UIElementType_Panel) {
    radius = m_panel_style.border_thickness;
  } else {
    radius = 10.0f; // default radius for other types
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
    corner.append(sf::Vertex(centre + offset, m_panel_style.border_color));

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
      corner.append(sf::Vertex(segmented_point, m_panel_style.border_color));
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
  top_side.setFillColor(m_panel_style.border_color);
  m_logic_context.scene_texture.draw(top_side);

  sf::RectangleShape bottom_side(sf::Vector2f(size.x - 2 * radius, radius));
  bottom_side.setPosition({origin.x + radius, origin.y + size.y - radius});
  bottom_side.setFillColor(m_panel_style.border_color);
  m_logic_context.scene_texture.draw(bottom_side);
  sf::RectangleShape left_side(sf::Vector2f(radius, size.y - 2 * radius));
  left_side.setPosition({origin.x, origin.y + radius});
  left_side.setFillColor(m_panel_style.border_color);
  m_logic_context.scene_texture.draw(left_side);
  sf::RectangleShape right_side(sf::Vector2f(radius, size.y - 2 * radius));
  right_side.setPosition({origin.x + size.x - radius, origin.y + radius});
  right_side.setFillColor(m_panel_style.border_color);
  m_logic_context.scene_texture.draw(right_side);

  // draw the background of the box taking into account the radius
  sf::RectangleShape background(
      sf::Vector2f(size.x - 2 * radius, size.y - 2 * radius));
  background.setPosition({origin.x + radius, origin.y + radius});
  background.setFillColor(m_panel_style.background_color);
  m_logic_context.scene_texture.draw(background);
}

} // namespace steamrot
