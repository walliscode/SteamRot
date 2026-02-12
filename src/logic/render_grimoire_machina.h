/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the free functions related to the rendering of the
/// grimoire machina.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "MachinaForm.h"
#include "MachinaFormScaffold.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

namespace steamrot::logic::render::grimoire_machina {

/////////////////////////////////////////////////
/// @brief Draw elements which represent an empty active machina form.
///
/// @param texture Reference to the render texture on which the elements will be
/// drawn.
/// @param form MachinaForm to draw
/////////////////////////////////////////////////
void DrawEmptyActiveMachinaForm(sf::RenderTexture &texture, MachinaForm &form);

/////////////////////////////////////////////////
/// @brief Draw a border around the crafting canvas.
///
/// @param texture RenderTexture to draw on.
/// @param crafting_canvas FloatRect representing the crafting canvas.
/////////////////////////////////////////////////
void DrawCraftingCanvasBorder(sf::RenderTexture &texture,
                              sf::FloatRect &crafting_canvas);

/////////////////////////////////////////////////
/// @brief Draw a box around the crafting canvas to indicate that no machina
/// form is active.
///
/// @param texture Texture to draw on.
/// @param crafting_canvas FloatRect representing the crafting canvas.
/////////////////////////////////////////////////
void DrawNoMachinaFormBox(sf::RenderTexture &texture,
                          sf::FloatRect &crafting_canvas);

/////////////////////////////////////////////////
/// @brief Draw the growth point on the crafting canvas.
///
/// @param texture RenderTexture to draw on.
/// @param growth_point GrowthPoint to draw.
/////////////////////////////////////////////////
void DrawGrowthPoint(sf::RenderTexture &texture, GrowthPoint &growth_point);

} // namespace steamrot::logic::render::grimoire_machina
