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
#include "UIRenderContext.h"
#include "UIStyle.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <memory>
#include <sys/types.h>

namespace steamrot {

namespace logic {
namespace ui {
namespace render {

/////////////////////////////////////////////////
/// @brief Draw nested UI elements recursively to a render texture
///
/// @param context Rendering context containing texture and style
/// @param element Element to draw
/////////////////////////////////////////////////
void DrawNestedUIElements(const UIRenderContext &context,
                          const UIElement &element);

/////////////////////////////////////////////////
/// @brief Internal dispatcher that dispatches to type-specific drawing
/// functions
///
/// @param context Rendering context containing texture and style
/// @param element Element to draw
/////////////////////////////////////////////////
void DrawUIElementDispatch(const UIRenderContext &context,
                           const UIElement &element);

/////////////////////////////////////////////////
/// @brief Draw a ButtonElement
///
/// @param context Rendering context containing texture and style
/// @param button Button element to draw
/////////////////////////////////////////////////
void DrawButtonElement(const UIRenderContext &context,
                       const ButtonElement &button);

/////////////////////////////////////////////////
/// @brief Draw a PanelElement
///
/// @param context Rendering context containing texture and style
/// @param panel Panel element to draw
/////////////////////////////////////////////////
void DrawPanelElement(const UIRenderContext &context, const PanelElement &panel);

/////////////////////////////////////////////////
/// @brief Draw a DropDownListElement
///
/// @param context Rendering context containing texture and style
/// @param list DropDown list element to draw
/////////////////////////////////////////////////
void DrawDropDownListElement(const UIRenderContext &context,
                             const DropDownListElement &list);

/////////////////////////////////////////////////
/// @brief Draw a DropDownItemElement
///
/// @param context Rendering context containing texture and style
/// @param item DropDown item element to draw
/////////////////////////////////////////////////
void DrawDropDownItemElement(const UIRenderContext &context,
                             const DropDownItemElement &item);

/////////////////////////////////////////////////
/// @brief Draw a DropDownButtonElement
///
/// @param context Rendering context containing texture and style
/// @param button DropDown button element to draw
/////////////////////////////////////////////////
void DrawDropDownButtonElement(const UIRenderContext &context,
                               const DropDownButtonElement &button);

/////////////////////////////////////////////////
/// @brief Draw a DropDownContainerElement
///
/// @param context Rendering context containing texture and style
/// @param container DropDown container element to draw
/////////////////////////////////////////////////
void DrawDropDownContainerElement(const UIRenderContext &context,
                                  const DropDownContainerElement &container);

/////////////////////////////////////////////////
/// @brief Draw the border and background of a general UI element
///
/// @param context Rendering context containing texture
/// @param element Element to draw
/// @param style Style to use for drawing
/////////////////////////////////////////////////
void DrawBorderAndBackground(const UIRenderContext &context,
                             const UIElement &element, const Style &style);

/////////////////////////////////////////////////
/// @brief Draw the border and background of a button UI element
///
/// @param context Rendering context containing texture
/// @param element Element to draw
/// @param style ButtonStyle to use for drawing
/////////////////////////////////////////////////
void DrawBorderAndBackground(const UIRenderContext &context,
                             const UIElement &element,
                             const ButtonStyle &style);

/////////////////////////////////////////////////
/// @brief Draw text with the specified parameters
///
/// @param context Rendering context containing texture
/// @param text Text string to draw
/// @param position Position to draw at
/// @param size Size of the container
/// @param font Font to use
/// @param font_size Font size
/// @param color Text color
/////////////////////////////////////////////////
void DrawText(const UIRenderContext &context, const std::string &text,
              const sf::Vector2f &position, const sf::Vector2f size,
              std::shared_ptr<const sf::Font> font, uint8_t font_size,
              const sf::Color &color);

/////////////////////////////////////////////////
/// @brief Update size and position of child elements based on layout
///
/// @param element Parent element containing children
/// @param style Style to use for layout calculations
/////////////////////////////////////////////////
void UpdateSizeAndPositionOfChildElements(const UIElement &element,
                                          const UIStyle &style);

} // namespace render
} // namespace ui
} // namespace logic
} // namespace steamrot
