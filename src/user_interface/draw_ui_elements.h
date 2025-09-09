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

#include "ButtonStyle.h"
#include "UIElement.h"
#include "UIStyle.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <memory>
#include <sys/types.h>

namespace steamrot {
namespace draw_ui_elements {

/////////////////////////////////////////////////
/// @brief Draw nested UI elements recursively to a render texture
///
/// @param texture Render texture to draw to
/// @param element Element to draw
/// @param style Style to use for drawing
/////////////////////////////////////////////////
void DrawNestedUIElements(sf::RenderTexture &texture, const UIElement &element,
                          const UIStyle &style);

/////////////////////////////////////////////////
/// @brief Draw the border and background of a general UI element
///
/// @param texture Render texture to draw to
/// @param element Element to draw
/// @param style Style to use for drawing
/////////////////////////////////////////////////
void DrawBorderAndBackground(sf::RenderTexture &texture,
                             const UIElement &element, const Style &style);

/////////////////////////////////////////////////
/// @brief Draw the border and background of a button UI element
///
/// @param texture Render texture to draw to
/// @param element Element to draw
/// @param style ButtonStyle to use for drawing
/////////////////////////////////////////////////
void DrawBorderAndBackground(sf::RenderTexture &texture,
                             const UIElement &element,
                             const ButtonStyle &style);

void DrawText(sf::RenderTexture &texture, const std::string &text,
              const sf::Vector2f &position, const sf::Vector2f size,
              std::shared_ptr<const sf::Font> font, uint8_t font_size,
              const sf::Color &color);

void UpdateSizeAndPositionOfChildElements(const UIElement &element,
                                          const UIStyle &style);

} // namespace draw_ui_elements
} // namespace steamrot
