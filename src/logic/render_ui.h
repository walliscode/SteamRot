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
#include "DropDownButtonElement.h"
#include "DropDownContainerElement.h"
#include "DropDownItemElement.h"
#include "DropDownListElement.h"
#include "PanelElement.h"
#include "UIPriorityTier.h"
#include "UIElement.h"
#include "UIStyle.h"
#include "containers.h"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include <memory>
#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

namespace steamrot::logic::render::ui {

/////////////////////////////////////////////////
/// @brief Draw nested UI elements recursively to a render texture
///
/// Positioning of child elements must be calculated before calling this
/// function (see positioning_ui.h).
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
/// @brief Draw a semi-transparent grey overlay on a disabled UI element
///
/// Called after all child elements have been drawn so the overlay covers
/// the entire element hierarchy.
///
/// @param texture Render texture to draw to
/// @param element The disabled element whose bounds are used for the overlay
/////////////////////////////////////////////////
void DrawDisabledOverlay(sf::RenderTexture &texture, const UIElement &element);

void DrawText(sf::RenderTexture &texture, const std::string &text,
              const sf::Vector2f &position, const sf::Vector2f size,
              std::shared_ptr<const sf::Font> font, uint8_t font_size,
              const sf::Color &color);

/////////////////////////////////////////////////
/// @brief Draw all visible UI entities in a single tier pass.
///
/// Iterates all provided entity indices and draws only entities whose
/// CUserInterface::m_priority_tier matches the requested tier.
///
/// @param entity_indexes Entity indices to inspect for this pass.
/// @param scene_entities EntityMemoryPool containing CUserInterface components.
/// @param scene_texture  Render texture to draw to.
/// @param ui_styles      Map of all available UIStyles (keyed by name).
/// @param tier           Tier being drawn in this pass.
/////////////////////////////////////////////////
void DrawAllUIEntitiesInTier(
    const std::vector<size_t> &entity_indexes, EntityMemoryPool &scene_entities,
    sf::RenderTexture &scene_texture,
    const std::unordered_map<std::string, UIStyle> &ui_styles,
    UIPriorityTier tier);

} // namespace steamrot::logic::render::ui
