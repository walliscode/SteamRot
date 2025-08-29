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
#include "PanelElement.h"
#include "UIStyle.h"
#include <SFML/Graphics/RenderTexture.hpp>

namespace steamrot {
namespace draw_ui_elements {

/////////////////////////////////////////////////
/// @brief Overloaded function to draw a PanelElement on a RenderTexture
///
/// @param texture Reference to the RenderTexture to draw on
/// @param panel PanelElement to draw
/// @param style UIStyle providing values for drawing
/////////////////////////////////////////////////
void DrawUIElement(sf::RenderTexture &texture, const PanelElement &panel,
                   const UIStyle &style);
} // namespace draw_ui_elements
} // namespace steamrot
