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
#include "ViewDirection.h"
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

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
                         GrimoireMachina &grimoire_machina);

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
/// @brief Draw a placed FragmentInstance onto the crafting canvas.
///
/// Applies the instance's stored transform as RenderStates, draws the
/// fragment's Front view geometry at the placed position, then draws all
/// socket indicators on top when @p draw_sockets is true.
///
/// @param texture           RenderTexture to draw on.
/// @param fragment_instance FragmentInstance to render.
/// @param draw_sockets      Whether to draw the instance's sockets.
/////////////////////////////////////////////////
void draw_fragment_instance(sf::RenderTexture &texture,
                            FragmentInstance &fragment_instance,
                            bool draw_sockets);

/////////////////////////////////////////////////
/// @brief Draw a placed JointInstance onto the crafting canvas.
///
/// Applies the instance's stored transform as RenderStates, draws the
/// joint's Front view geometry at the placed position, then draws all
/// socket indicators on top when @p draw_sockets is true.
///
/// @param texture        RenderTexture to draw on.
/// @param joint_instance JointInstance to render.
/// @param draw_sockets   Whether to draw the instance's sockets.
/////////////////////////////////////////////////
void draw_joint_instance(sf::RenderTexture &texture,
                         JointInstance &joint_instance, bool draw_sockets);

/////////////////////////////////////////////////
/// @brief Draw all sockets of a FragmentInstance on the crafting canvas.
///
/// World position of each socket is derived from the instance's transform
/// applied to the Fragment definition's local socket positions.
///
/// @param texture           RenderTexture to draw on.
/// @param fragment_instance FragmentInstance whose sockets are drawn.
/////////////////////////////////////////////////
void draw_fragment_instance_sockets(sf::RenderTexture &texture,
                                    FragmentInstance &fragment_instance);

/////////////////////////////////////////////////
/// @brief Draw all sockets of a JointInstance on the crafting canvas.
///
/// World position of each socket is derived from the instance's transform
/// applied to the Joint definition's local socket positions.
///
/// @param texture        RenderTexture to draw on.
/// @param joint_instance JointInstance whose sockets are drawn.
/////////////////////////////////////////////////
void draw_joint_instance_sockets(sf::RenderTexture &texture,
                                 JointInstance &joint_instance);

/////////////////////////////////////////////////
/// @brief Draw a single socket circle at the given world position.
///
/// Creates a circle shape on the fly, coloured based on the socket's hover
/// state (hover_color when hovered, base_color otherwise), and draws it onto
/// @p texture.
///
/// @param texture      RenderTexture to draw on.
/// @param world_pos    World-space centre of the socket circle.
/// @param socket_state Runtime state of the socket (hover, connection status).
/////////////////////////////////////////////////
void draw_socket(sf::RenderTexture &texture, sf::Vector2f world_pos,
                 const SocketState &socket_state);

/////////////////////////////////////////////////
/// @brief Draw the given view of a Views object.
///
/// @param texture        RenderTexture to draw on.
/// @param views          Views object containing the vertex arrays to draw.
/// @param view_direction ViewDirection specifying which view to draw.
/////////////////////////////////////////////////
void draw_view(sf::RenderTexture &texture, const Views &views,
               ViewDirection view_direction);

/////////////////////////////////////////////////
/// @brief Draw the given view of a Views object with custom render states.
///
/// Like the three-argument overload but applies @p states (e.g. a translation
/// transform) when drawing, so the geometry can be positioned at an arbitrary
/// location such as the cursor.
///
/// @param texture        RenderTexture to draw on.
/// @param views          Views object containing the vertex arrays to draw.
/// @param view_direction ViewDirection specifying which view to draw.
/// @param states         RenderStates applied when drawing (transform, etc.).
/////////////////////////////////////////////////
void draw_view(sf::RenderTexture &texture, const Views &views,
               ViewDirection view_direction, const sf::RenderStates &states);

} // namespace steamrot::logic::render::grimoire_machina
