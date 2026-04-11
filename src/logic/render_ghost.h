/////////////////////////////////////////////////
/// @file
/// @brief Declaration of free functions for rendering the MrGhost item.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GrimoireMachina.h"
#include "MrGhost.h"
#include <SFML/Graphics/RenderTexture.hpp>

namespace steamrot::logic::render::ghost {

/////////////////////////////////////////////////
/// @brief Draw the ghost item at the cursor position.
///
/// Renders the fragment or joint geometry at mr_ghost.m_position when a
/// selection is active, along with pale blue socket indicator circles at each
/// socket position. Draws nothing when the selection is empty (monostate).
///
/// @param texture          RenderTexture to draw on.
/// @param mr_ghost         MrGhost carrying the current selection and position.
/// @param grimoire_machina GrimoireMachina providing fragment/joint data
///                         (reserved for future geometry rendering).
/////////////////////////////////////////////////
void DrawGhostItem(sf::RenderTexture &texture, const MrGhost &mr_ghost,
                   GrimoireMachina &grimoire_machina);

} // namespace steamrot::logic::render::ghost
