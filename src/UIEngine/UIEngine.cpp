////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "UIEngine.h"
#include "log_handler.h"
#include "spdlog/common.h"
#include <SFML/Graphics/RenderTexture.hpp>
namespace steamrot {
////////////////////////////////////////////////////////////
UIEngine::UIEngine(const json &config) {

  // Initialize the UI engine with the provided JSON configuration
  // This could include setting up styles, themes, and other UI elements
  AddStyles(config);
}

/////////////////////////////////////////////////////////////
UIEngine::UIEngine(const themes::UIObjects *config) {
  // Initialize the UI engine with the provided flatbuffer configuration
  // This could include setting up styles, themes, and other UI elements
  AddStyles(config);
}
////////////////////////////////////////////////////////////
void UIEngine::AddStyles(const json &config) {
  // Implementation for adding styles from the JSON config can be added here
}

////////////////////////////////////////////////////////////
void UIEngine::AddStyles(const themes::UIObjects *config) {
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
void UIEngine::DrawUILayer(sf::RenderTexture &render_texture,
                           std::vector<CUserInterface> &ui_elements) {
  // Implementation for drawing the UI layer to the render texture can be
  // added here This could include drawing buttons, text fields, and other UI
  // components
}
////////////////////////////////////////////////////////////
void UIEngine::DrawDropDownMenu(sf::RenderTexture &render_texture) {};
} // namespace steamrot
