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
#include "GrimoireMachina.h"
#include "MachinaForm.h"
#include "MachinaFormScaffold.h"
#include <SFML/Graphics.hpp>

namespace steamrot::logic::render::grimoire_machina {

/////////////////////////////////////////////////
/// @brief Top-level render entry point called by GrimoireMachinaRenderLogic.
///
/// Decides what to render based on the state of @p grimoire_machina:
/// - No active scaffold  → draws the no-machina-form indicator at the world
///   origin.
/// - Scaffold present    → draws every placed joint and fragment instance.
///
/// @param texture          RenderTexture to draw to.
/// @param grimoire_machina GrimoireMachina whose scaffold state drives the
///                         rendering decision.
/////////////////////////////////////////////////
void render_machina_form(sf::RenderTexture &texture,
                         GrimoireMachina &grimoire_machina,
                         const sf::Font &font);

/////////////////////////////////////////////////
/// @brief Draw a small indicator box at the world origin (0, 0) to show that
/// no machina form is currently active.
///
/// The box is drawn in world space, centred on (0, 0), so that the camera
/// can use the origin as a reference point.
///
/// @param texture RenderTexture to draw on.
/////////////////////////////////////////////////
void draw_no_machina_form_indicator(sf::RenderTexture &texture);

/////////////////////////////////////////////////
/// @brief Draw elements which represent an empty active machina form.
///
/// @param texture Reference to the render texture on which the elements will be
/// drawn.
/// @param form MachinaForm to draw.
/////////////////////////////////////////////////
void draw_empty_active_machina_form(sf::RenderTexture &texture,
                                    MachinaForm &form);

/////////////////////////////////////////////////
/// @brief Draw a status box with the given parameters.
///
/// the status box is a surrounding rectangle with a small trapezium to capture
/// any associated text
/// @param box The rectangle to draw the status box around in world space.
/// @param color The color of the status box border.
/// @param text  The text to display in the status box.
/// @param font The font to use for the text in the status box.
/// @param texture The render texture to draw on.
/////////////////////////////////////////////////
void draw_status_box(sf::FloatRect box, sf::Color color,
                     const std::string &text, const sf::Font &font,
                     sf::RenderTexture &texture);

/////////////////////////////////////////////////
/// @brief Switch container function to pick the correct status box to draw
/// based on the current state of the structural analysis
///
/// @param state Enumeration of the current state of the structural analysis
/// @param box FloatRect to pass to the draw_status_box function
/// @param font Font to pass to the draw_status_box function
/// @param texture RenderTexture to pass to the draw_status_box function
/////////////////////////////////////////////////
void pick_and_draw_status_box(const MachinaFormScaffold &scaffold,
                              const sf::Font &font, sf::RenderTexture &texture);

} // namespace steamrot::logic::render::grimoire_machina
