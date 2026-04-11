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

void Draw(sf::RenderTexture &texture, Socket &socket);

/////////////////////////////////////////////////
/// @brief Draw all sockets of a FragmentInstance on the crafting canvas.
///
/// @param texture RenderTexture to draw on.
/// @param fragment_instance FragmentInstance whose sockets are drawn.
/////////////////////////////////////////////////
void Draw(sf::RenderTexture &texture, FragmentInstance &fragment_instance);

/////////////////////////////////////////////////
/// @brief Draw all sockets of a JointInstance on the crafting canvas.
///
/// @param texture RenderTexture to draw on.
/// @param joint_instance JointInstance whose sockets are drawn.
/////////////////////////////////////////////////
void Draw(sf::RenderTexture &texture, JointInstance &joint_instance);

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
