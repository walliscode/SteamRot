/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the DirectScaleResizeStrategy class.
///
/// DirectScaleResizeStrategy resizes the scene's render texture to match the
/// new window dimensions and restores the existing SFML view so that the same
/// world area is rendered at the new (larger or smaller) pixel count.
///
/// Effect: all rendered content scales proportionally with the window.
/// Use this strategy for UI-heavy scenes where elements should fill the
/// available screen space.
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
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot {

/////////////////////////////////////////////////
/// @class DirectScaleResizeStrategy
/// @brief Scales rendered content proportionally with the window size.
///
/// When the window grows the render texture is enlarged, but the sf::View
/// is kept at the same world-space dimensions it had before the resize.
/// Each world unit therefore maps to more screen pixels, making everything
/// appear larger.
/////////////////////////////////////////////////
class DirectScaleResizeStrategy : public IResizeStrategy {
public:
  /////////////////////////////////////////////////
  /// @brief Default constructor.
  /////////////////////////////////////////////////
  DirectScaleResizeStrategy() = default;

  /////////////////////////////////////////////////
  /// @brief Resize the render texture and restore the previous view.
  ///
  /// Saves the current sf::View, resizes @p render_texture to @p new_size,
  /// then reapplies the saved view.  The same world area is now displayed
  /// across more (or fewer) pixels, producing a proportional scale effect.
  ///
  /// @param new_size        New window size in pixels.
  /// @param render_texture  Scene render texture to resize.
  /////////////////////////////////////////////////
  void OnResize(sf::Vector2u new_size,
                sf::RenderTexture &render_texture) override {
    // Capture view before resize (resize resets the texture's view).
    sf::View current_view = render_texture.getView();

    render_texture.resize(new_size);

    // Restore the old view so the same world area is still rendered.
    render_texture.setView(current_view);
  }
};

} // namespace steamrot
