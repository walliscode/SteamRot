/////////////////////////////////////////////////
/// @file
/// @brief Declaration of UI rendering functions
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "ButtonElement.h"
#include "ButtonStyle.h"
#include "DropDownButtonElement.h"
#include "DropDownContainerElement.h"
#include "DropDownItemElement.h"
#include "DropDownListElement.h"
#include "PanelElement.h"
#include "UIElement.h"
#include "UIStyle.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <memory>
#include <sys/types.h>

namespace steamrot {

namespace logic {
namespace render {

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
/// @brief Internal dispatcher that dispatches to type-specific drawing
/// functions
///
/// @param texture Render texture to draw to
/// @param element Element to draw
/// @param style Style to use for drawing
/////////////////////////////////////////////////
void DrawUIElementDispatch(sf::RenderTexture &texture, const UIElement &element,
                           const UIStyle &style);

/////////////////////////////////////////////////
/// @brief Draw a ButtonElement
///
/// @param texture Render texture to draw to
/// @param button Button element to draw
/// @param style Style to use for drawing
/////////////////////////////////////////////////
void DrawButtonElement(sf::RenderTexture &texture, const ButtonElement &button,
                       const UIStyle &style);

/////////////////////////////////////////////////
/// @brief Draw a PanelElement
///
/// @param texture Render texture to draw to
/// @param panel Panel element to draw
/// @param style Style to use for drawing
/////////////////////////////////////////////////
void DrawPanelElement(sf::RenderTexture &texture, const PanelElement &panel,
                      const UIStyle &style);

/////////////////////////////////////////////////
/// @brief Draw a DropDownListElement
///
/// @param texture Render texture to draw to
/// @param list DropDown list element to draw
/// @param style Style to use for drawing
/////////////////////////////////////////////////
void DrawDropDownListElement(sf::RenderTexture &texture,
                             const DropDownListElement &list,
                             const UIStyle &style);

/////////////////////////////////////////////////
/// @brief Draw a DropDownItemElement
///
/// @param texture Render texture to draw to
/// @param item DropDown item element to draw
/// @param style Style to use for drawing
/////////////////////////////////////////////////
void DrawDropDownItemElement(sf::RenderTexture &texture,
                             const DropDownItemElement &item,
                             const UIStyle &style);

/////////////////////////////////////////////////
/// @brief Draw a DropDownButtonElement
///
/// @param texture Render texture to draw to
/// @param button DropDown button element to draw
/// @param style Style to use for drawing
/////////////////////////////////////////////////
void DrawDropDownButtonElement(sf::RenderTexture &texture,
                               const DropDownButtonElement &button,
                               const UIStyle &style);

/////////////////////////////////////////////////
/// @brief Draw a DropDownContainerElement
///
/// @param texture Render texture to draw to
/// @param container DropDown container element to draw
/// @param style Style to use for drawing
/////////////////////////////////////////////////
void DrawDropDownContainerElement(sf::RenderTexture &texture,
                                  const DropDownContainerElement &container,
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

} // namespace render
} // namespace logic
} // namespace steamrot
