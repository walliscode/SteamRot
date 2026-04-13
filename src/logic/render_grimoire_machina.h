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
#include "Fragment.h"
#include "GrimoireMachina.h"
#include "Joint.h"
#include "MachinaForm.h"
#include "MachinaFormScaffold.h"
#include "ViewDirection.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
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
/// @brief Draw a single socket circle at the given world position.
///
/// Creates a circle shape on the fly, coloured based on the socket's hover
/// state (hover_color when hovered, base_color otherwise), and draws it onto
/// @p texture. Keeping the circle transient avoids stale cached positions.
///
/// @param texture      RenderTexture to draw on.
/// @param world_pos    World-space centre of the socket circle.
/// @param socket_state Runtime state of the socket (hover, connection status).
/////////////////////////////////////////////////
void DrawSocket(sf::RenderTexture &texture, sf::Vector2f world_pos,
                const SocketState &socket_state);

/////////////////////////////////////////////////
/// @brief Draw all sockets of a FragmentInstance on the crafting canvas.
///
/// World position of each socket is derived from the instance's transform
/// applied to the Fragment definition's local socket positions.
///
/// @param texture RenderTexture to draw on.
/// @param fragment_instance FragmentInstance whose sockets are drawn.
/////////////////////////////////////////////////
void DrawFragmentInstanceSockets(sf::RenderTexture &texture,
                                 FragmentInstance &fragment_instance);

/////////////////////////////////////////////////
/// @brief Draw all sockets of a JointInstance on the crafting canvas.
///
/// World position of each socket is derived from the instance's transform
/// applied to the Joint definition's local socket positions.
///
/// @param texture RenderTexture to draw on.
/// @param joint_instance JointInstance whose sockets are drawn.
/////////////////////////////////////////////////
void DrawJointInstanceSockets(sf::RenderTexture &texture,
                              JointInstance &joint_instance);

/////////////////////////////////////////////////
/// @brief Draw a placed FragmentInstance onto the crafting canvas.
///
/// Applies the instance's stored transform as RenderStates, draws the
/// fragment's Front view geometry at the placed position, then draws all
/// socket indicators on top.
///
/// @param texture           RenderTexture to draw on.
/// @param fragment_instance FragmentInstance to render.
/// @param draw_sockets      Whether to draw the instance's sockets. May be
/// false
/////////////////////////////////////////////////
void DrawFragmentInstance(sf::RenderTexture &texture,
                          FragmentInstance &fragment_instance,
                          const bool draw_sockets);

/////////////////////////////////////////////////
/// @brief Draw a placed JointInstance onto the crafting canvas.
///
/// Applies the instance's stored transform as RenderStates, draws the
/// joint's Front view geometry at the placed position, then draws all
/// socket indicators on top when draw_sockets is true.
///
/// @param texture        RenderTexture to draw on.
/// @param joint_instance JointInstance to render.
/// @param draw_sockets   Whether to draw the instance's sockets.
/////////////////////////////////////////////////
void DrawJointInstance(sf::RenderTexture &texture,
                       JointInstance &joint_instance,
                       const bool draw_sockets);

/////////////////////////////////////////////////
/// @brief Draw the scaffold state or a placeholder when no scaffold is active.
///
/// Draws the crafting canvas border unconditionally. When @p grimoire_machina
/// has no active scaffold, draws the no-machina-form placeholder box. When a
/// scaffold is present but has no joints yet, draws the growth point. Other
/// scaffold states are handled by downstream render functions.
///
/// @param texture          RenderTexture to draw to.
/// @param grimoire_machina GrimoireMachina whose scaffold state drives the
///                         rendering decision.
/////////////////////////////////////////////////
void DrawScaffoldOrPlaceholder(sf::RenderTexture &texture,
                               GrimoireMachina &grimoire_machina);

/////////////////////////////////////////////////
/// @brief Draw the given view of a Views object
///
/// @param texture sf::RenderTexture to draw on.
/// @param views Views object containing the vertex arrays to draw.
/// @param view_direction ViewDirection specifying which view to draw from the
/// Views object.
/////////////////////////////////////////////////
void DrawView(sf::RenderTexture &texture, const Views &views,
              ViewDirection view_direction);

/////////////////////////////////////////////////
/// @brief Draw the view of a Fragment for the given direction.
///
/// Draws the vertex array stored in @p fragment's movement_views for
/// @p view_direction onto @p texture. The geometry is drawn at its stored
/// positions (no transform is applied).
///
/// @param texture        RenderTexture to draw on.
/// @param fragment       Fragment whose movement view is drawn.
/// @param view_direction ViewDirection selecting which view to draw.
/////////////////////////////////////////////////
void DrawFragmentView(sf::RenderTexture &texture, const Fragment &fragment,
                      ViewDirection view_direction);

/////////////////////////////////////////////////
/// @brief Draw the view of a Fragment with custom render states.
///
/// Like the three-argument overload but applies @p states (e.g. a
/// translation transform) when drawing, so the geometry can be positioned
/// at an arbitrary location such as the cursor.
///
/// @param texture        RenderTexture to draw on.
/// @param fragment       Fragment whose movement view is drawn.
/// @param view_direction ViewDirection selecting which view to draw.
/// @param states         RenderStates applied when drawing (transform, etc.).
/////////////////////////////////////////////////
void DrawFragmentView(sf::RenderTexture &texture, const Fragment &fragment,
                      ViewDirection view_direction,
                      const sf::RenderStates &states);

/////////////////////////////////////////////////
/// @brief Draw the view of a Joint for the given direction.
///
/// Draws the vertex array stored in @p joint's movement_views for
/// @p view_direction onto @p texture. The geometry is drawn at its stored
/// positions (no transform is applied).
///
/// @param texture        RenderTexture to draw on.
/// @param joint          Joint whose movement view is drawn.
/// @param view_direction ViewDirection selecting which view to draw.
/////////////////////////////////////////////////
void DrawJointView(sf::RenderTexture &texture, const Joint &joint,
                   ViewDirection view_direction);

/////////////////////////////////////////////////
/// @brief Draw the view of a Joint with custom render states.
///
/// Like the three-argument overload but applies @p states (e.g. a
/// translation transform) when drawing, so the geometry can be positioned
/// at an arbitrary location such as the cursor.
///
/// @param texture        RenderTexture to draw on.
/// @param joint          Joint whose movement view is drawn.
/// @param view_direction ViewDirection selecting which view to draw.
/// @param states         RenderStates applied when drawing (transform, etc.).
/////////////////////////////////////////////////
void DrawJointView(sf::RenderTexture &texture, const Joint &joint,
                   ViewDirection view_direction,
                   const sf::RenderStates &states);

} // namespace steamrot::logic::render::grimoire_machina
