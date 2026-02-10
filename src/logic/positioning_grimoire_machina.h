/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for positioning grimoire machina
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////

#include "CUserInterface.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
namespace steamrot::logic::positioning::grimoire_machina {

/////////////////////////////////////////////////
/// @brief Returns a usable rectangle shape for the crafting canvas.
///
/// The crafting canvas is where the  active machina form will be positioned.
///
/// @param texture_coordinates The rectangle shape containing the texture
/// coordinates of the RenderTexture
/// @param crafting_ui_elements CUserInterface elements that are part of the
/// crafting UI.
/////////////////////////////////////////////////
sf::FloatRect CalculateCraftingCanvasSizeAndPosition(
    const sf::FloatRect &texture_coordinates,
    const std::vector<CUserInterface> &crafting_ui_elements);

} // namespace steamrot::logic::positioning::grimoire_machina
