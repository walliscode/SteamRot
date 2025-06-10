////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "UIRenderLogic.h"
#include "log_handler.h"
#include "spdlog/common.h"
#include <SFML/Graphics/RenderTexture.hpp>

namespace steamrot {
UIRenderLogic::UIRenderLogic() : Logic<CUserInterface>() {
  // Default constructor initializes the UI engine with default styles
  // This could be extended to load styles from a configuration file or theme
  log_handler::ProcessLog(spdlog::level::level_enum::info,
                          log_handler::LogCode::kNoCode,
                          "UIEngine: Default constructor called");
}

/////////////////////////////////////////////////////////////
UIRenderLogic::UIRenderLogic(const themes::UIObjects *config)
    : Logic<CUserInterface>() {
  // Initialize the UI engine with the provided flatbuffer configuration
  // This could include setting up styles, themes, and other UI elements
  AddStyles(config);
}
void UIRenderLogic::ProcessLogic(
    components::containers::EntityMemoryPool &entities,
    const EntityIndicies &entity_indicies) {
  // Process the logic for the UI elements
  // This could include updating button states, handling input, etc.
  // For now, we will just log that the logic is being processed
  log_handler::ProcessLog(spdlog::level::level_enum::info,
                          log_handler::LogCode::kNoCode,
                          "UIEngine: Processing UI Logic");
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
  // add button styles from flatbuffer config
  m_button_style.border_color.r = config->button()->border_color()->r();
  m_button_style.border_color.g = config->button()->border_color()->g();
  m_button_style.border_color.b = config->button()->border_color()->b();
  m_button_style.border_color.a = config->button()->border_color()->a();
  m_button_style.background_color.r = config->button()->background_color()->r();
  m_button_style.background_color.g = config->button()->background_color()->g();
  m_button_style.background_color.b = config->button()->background_color()->b();
  m_button_style.background_color.a = config->button()->background_color()->a();
  m_button_style.text_color.r = config->button()->text_color()->r();
  m_button_style.text_color.g = config->button()->text_color()->g();
  m_button_style.text_color.b = config->button()->text_color()->b();
  m_button_style.text_color.a = config->button()->text_color()->a();

  log_handler::ProcessLog(
      spdlog::level::level_enum::info, log_handler::LogCode::kNoCode,
      "UIEngine: Button styles added from flatbuffer config");
}
////////////////////////////////////////////////////////////
void UIRenderLogic::DrawUILayer(sf::RenderTexture &render_texture,
                                std::vector<CUserInterface> &ui_elements) {
  // Implementation for drawing the UI layer to the render texture can be
  // added here This could include drawing buttons, text fields, and other UI
  // components
}
////////////////////////////////////////////////////////////
void UIRenderLogic::DrawDropDownMenu(sf::RenderTexture &render_texture) {};

////////////////////////////////////////////////////////////
void UIRenderLogic::DrawTestButton(sf::RenderTexture &ui_layer) {

  // Crea  // Create a button shape
  sf::RectangleShape button(sf::Vector2f(200, 50));

  button.setFillColor(m_button_style.background_color);
  button.setOutlineColor(m_button_style.border_color);
  button.setOutlineThickness(2);
  button.setPosition({100.f, 100.f}); // Position the button

  // Draw the button and text to the UI layer
  ui_layer.draw(button);
}

void UIRenderLogic::Draw(sf::RenderTexture &ui_layer) {
  // clear the UI layer before drawing
  ui_layer.clear();

  // Draw the test button to the UI layer
  DrawTestButton(ui_layer);

  // Finish drawing to the UI layer with display
  ui_layer.display();
}
} // namespace steamrot
