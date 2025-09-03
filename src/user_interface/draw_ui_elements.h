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

#include "UIElement.h"
#include "UIStyle.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <memory>
#include <sys/types.h>

namespace steamrot {
namespace draw_ui_elements {

void DrawUIRecursively(sf::RenderTexture &texture, const UIElement &element,
                       const UIStyle &style);

void DrawBorderAndBackground(sf::RenderTexture &texture,
                             const sf::Vector2f &position,
                             const sf::Vector2f &size, const Style &style);

void DrawText(sf::RenderTexture &texture, const std::string &text,
              const sf::Vector2f &position, const sf::Vector2f size,
              std::shared_ptr<const sf::Font> font, uint8_t font_size,
              const sf::Color &color);

void UpdateSizeAndPositionOfChildElements(const UIElement &element,
                                          const UIStyle &style);

} // namespace draw_ui_elements
} // namespace steamrot
