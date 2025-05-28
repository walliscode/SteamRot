////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "UIEngine.h"
#include <SFML/Graphics/RenderTexture.hpp>
namespace steamrot {
////////////////////////////////////////////////////////////
UIEngine::UIEngine(const json &config) {

  // Initialize the UI engine with the provided JSON configuration
  // This could include setting up styles, themes, and other UI elements
  AddStyles(config);
}
////////////////////////////////////////////////////////////
void UIEngine::AddStyles(const json &config) {
  // Implementation for adding styles from the JSON config can be added here
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
