/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the CappedScaleResizeStrategy class.
///
/// CappedScaleResizeStrategy is similar to DirectScaleResizeStrategy but
/// enforces a maximum render-texture size.  Once the window reaches the cap
/// the texture stops growing; the content is centred in the window and the
/// surrounding area is left to the window clear colour (letterboxing).
///
/// Use this strategy for scenes or UI elements that should not exceed a
/// particular resolution ceiling (e.g. pixel-art assets that look bad when
/// scaled beyond 2x).
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
#include <algorithm>

namespace steamrot {

/////////////////////////////////////////////////
/// @class CappedScaleResizeStrategy
/// @brief Scales content with the window up to a configurable maximum size.
///
/// Below the cap the strategy behaves identically to DirectScaleResizeStrategy.
/// Once either dimension of the window exceeds its cap the texture no longer
/// grows in that dimension, producing letterboxing.
/////////////////////////////////////////////////
class CappedScaleResizeStrategy : public IResizeStrategy {
public:
  /////////////////////////////////////////////////
  /// @brief Construct with an explicit size ceiling.
  ///
  /// @param max_size Maximum render-texture dimensions in pixels.
  ///                 Axis-independent: each dimension is capped separately.
  /////////////////////////////////////////////////
  explicit CappedScaleResizeStrategy(sf::Vector2u max_size)
      : m_max_size(max_size) {}

  /////////////////////////////////////////////////
  /// @brief Resize the render texture up to the configured ceiling.
  ///
  /// The texture is resized to min(new_size, m_max_size) on each axis.
  /// The existing view is restored after the resize so that the same world
  /// area is displayed (scaled up to the cap, then letterboxed beyond it).
  ///
  /// @param new_size        New window size in pixels.
  /// @param render_texture  Scene render texture to resize.
  /////////////////////////////////////////////////
  void OnResize(sf::Vector2u new_size,
                sf::RenderTexture &render_texture) override {
    sf::Vector2u capped_size{std::min(new_size.x, m_max_size.x),
                             std::min(new_size.y, m_max_size.y)};

    // Preserve the view across the resize.
    sf::View current_view = render_texture.getView();

    render_texture.resize(capped_size);

    render_texture.setView(current_view);
  }

  /////////////////////////////////////////////////
  /// @brief Return the configured size ceiling.
  ///
  /// @return Maximum render-texture dimensions in pixels.
  /////////////////////////////////////////////////
  sf::Vector2u GetMaxSize() const { return m_max_size; }

private:
  /////////////////////////////////////////////////
  /// @brief Maximum render-texture dimensions in pixels.
  /////////////////////////////////////////////////
  sf::Vector2u m_max_size;
};

} // namespace steamrot
