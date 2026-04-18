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
#include "MrGhost.h"
#include <SFML/Graphics/RenderTexture.hpp>

namespace steamrot::logic::render::ghost {

/////////////////////////////////////////////////
/// @brief Draw the ghost item at the position encoded in its instance transform.
///
/// Dispatches to draw_fragment_instance or draw_joint_instance depending on
/// which alternative is active in mr_ghost.m_instance. Draws nothing when the
/// instance is monostate (empty selection).
///
/// @param texture  RenderTexture to draw on.
/// @param mr_ghost MrGhost carrying the current instance (transform already
///                 updated by GhostPositioningLogic this tick).
/////////////////////////////////////////////////
void draw_ghost_item(sf::RenderTexture &texture, MrGhost &mr_ghost);

} // namespace steamrot::logic::render::ghost
