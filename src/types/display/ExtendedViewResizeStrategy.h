/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the ExtendedViewResizeStrategy class.
///
/// ExtendedViewResizeStrategy resizes the scene's render texture to match
/// the new window dimensions and then resets the sf::View to the texture's
/// default view.  Because the default view spans exactly the texture area in
/// world coordinates, each world unit always maps to one pixel regardless of
/// window size.
///
/// Effect: the rendered content stays the same apparent size on screen while
/// more (or less) of the world becomes visible as the window grows (or
/// shrinks).  This is the natural behaviour for map views or any scene where
/// the window acts as a variable-size viewport into a larger world.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "IResizeStrategy.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot {

/////////////////////////////////////////////////
/// @class ExtendedViewResizeStrategy
/// @brief Reveals more world on resize without scaling existing content.
///
/// The render texture is resized to the new window dimensions and the SFML
/// view is reset to the texture's default (1:1 world-to-pixel mapping).
/// Growing the window reveals more of the world; shrinking it hides parts of
/// it.  Pixel density and content scale remain constant.
/////////////////////////////////////////////////
class ExtendedViewResizeStrategy : public IResizeStrategy {
public:
  /////////////////////////////////////////////////
  /// @brief Default constructor.
  /////////////////////////////////////////////////
  ExtendedViewResizeStrategy() = default;

  /////////////////////////////////////////////////
  /// @brief Resize the render texture and reset to the default view.
  ///
  /// Resizes @p render_texture to @p new_size and applies
  /// @c getDefaultView() so that the view covers exactly the new texture
  /// area (one world unit per pixel).  More or less of the scene becomes
  /// visible without altering the scale of existing content.
  ///
  /// @param new_size        New window size in pixels.
  /// @param render_texture  Scene render texture to resize.
  /////////////////////////////////////////////////
  void OnResize(sf::Vector2u new_size,
                sf::RenderTexture &render_texture) override {
    render_texture.resize(new_size);

    // Reset to the default view so more (or less) world is revealed rather
    // than scaling the existing content.
    render_texture.setView(render_texture.getDefaultView());
  }
};

} // namespace steamrot
