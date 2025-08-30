/////////////////////////////////////////////////
/// @file
/// @brief Declaration of draw_ui_elements namespace
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "ButtonElement.h"
#include "PanelElement.h"
#include "UIStyle.h"
#include <SFML/Graphics/RenderTexture.hpp>

namespace steamrot {
namespace draw_ui_elements {

void DrawText(sf::RenderTexture &texture, const std::string &text,
              const sf::Vector2f &position, const UIStyle &style);
/////////////////////////////////////////////////
/// @brief Overloaded function to draw a PanelElement on a RenderTexture
///
/// @param texture Reference to the RenderTexture to draw on
/// @param panel PanelElement to draw
/// @param style UIStyle providing values for drawing
/////////////////////////////////////////////////
void DrawUIElement(sf::RenderTexture &texture, const PanelElement &panel,
                   const UIStyle &style);

/////////////////////////////////////////////////
/// @brief Overloaded function to draw a ButtonElement on a RenderTexture
///
/// @param texture Reference to the RenderTexture to draw on
/// @param button ButtonElement to draw
/// @param style UIStyle providing values for drawing
/////////////////////////////////////////////////
void DrawUIElement(sf::RenderTexture &texture, const ButtonElement &button,
                   const UIStyle &style);
} // namespace draw_ui_elements
} // namespace steamrot
