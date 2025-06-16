////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "UIRenderLogic.h"
#include "BaseLogic.h"
#include "CUserInterface.h"
#include "EntityHelpers.h"
#include "log_handler.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
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
  // Create a panel shape with the specified style
  sf::RectangleShape panel_shape;

  // using the elements position and size
  if (element.position) {
    panel_shape.setPosition({element.position->x, element.position->y});
  }
  if (element.size) {
    panel_shape.setSize({element.size->x, element.size->y});
  }
  panel_shape.setFillColor(m_panel_style.background_color);
  panel_shape.setOutlineColor(m_panel_style.border_color);
  panel_shape.setOutlineThickness(m_panel_style.border_thickness);
  // Draw the panel to the render texture
  m_logic_context.scene_texture.draw(panel_shape);
}

////////////////////////////////////////////////////////////
void UIRenderLogic::DrawDropDownMenu() {};

} // namespace steamrot
