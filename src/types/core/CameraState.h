/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the CameraState struct.
/////////////////////////////////////////////////

#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/View.hpp>
#include <algorithm>

namespace steamrot {

/////////////////////////////////////////////////
/// @struct CameraState
/// @brief Holds the per-scene camera/view state.
///
/// Currently tracks a single zoom level applied to the world view on the
/// scene render texture.  UI rendering resets to the texture default view so
/// that UI elements are never affected by the zoom.
/////////////////////////////////////////////////
struct CameraState {

  /////////////////////////////////////////////////
  /// @brief Minimum allowed zoom level (zoomed fully in).
  /////////////////////////////////////////////////
  static constexpr float kMinZoom{0.1f};

  /////////////////////////////////////////////////
  /// @brief Maximum allowed zoom level (zoomed fully out).
  /////////////////////////////////////////////////
  static constexpr float kMaxZoom{10.0f};

  /////////////////////////////////////////////////
  /// @brief Fractional change applied per unit of scroll delta.
  /////////////////////////////////////////////////
  static constexpr float kZoomStep{0.1f};

  /////////////////////////////////////////////////
  /// @brief Current zoom level.
  ///
  /// A value of 1.0 is the default (no zoom).  Values > 1.0 zoom out
  /// (more world visible); values < 1.0 zoom in (less world visible).
  /////////////////////////////////////////////////
  float m_zoom_level{1.0f};

  /////////////////////////////////////////////////
  /// @brief Apply a mouse-scroll delta to the zoom level.
  ///
  /// Positive delta (scroll up) zooms in; negative (scroll down) zooms out.
  /// The result is clamped to [kMinZoom, kMaxZoom].
  ///
  /// @param scroll_delta Raw scroll delta from the SFML mouse-wheel event.
  /////////////////////////////////////////////////
  void ApplyScrollDelta(float scroll_delta) {
    m_zoom_level *= (1.0f - scroll_delta * kZoomStep);
    m_zoom_level = std::clamp(m_zoom_level, kMinZoom, kMaxZoom);
  }

  /////////////////////////////////////////////////
  /// @brief Build a zoomed world view for the given render texture.
  ///
  /// Returns the texture default view with the current zoom level applied.
  /// Zooming is centred on the texture midpoint.
  ///
  /// @param texture Render texture whose default view is used as the base.
  /// @return sf::View with zoom applied.
  /////////////////////////////////////////////////
  sf::View GetWorldView(const sf::RenderTexture &texture) const {
    sf::View view = texture.getDefaultView();
    view.zoom(m_zoom_level);
    return view;
  }

  /////////////////////////////////////////////////
  /// @brief Convert a screen-space pixel position to world-space coordinates.
  ///
  /// Applies the current world view (with zoom) to map the given screen-pixel
  /// position to its corresponding world-space floating-point position.
  /// This is the single authoritative screen-to-world conversion; callers
  /// should use this instead of calling mapPixelToCoords directly.
  ///
  /// @param screen_pos Screen-space pixel position (e.g. mouse cursor).
  /// @param texture    Render texture whose world view is used for the mapping.
  /// @return World-space position corresponding to @p screen_pos.
  /////////////////////////////////////////////////
  sf::Vector2f MapToWorldCoords(sf::Vector2i screen_pos,
                                const sf::RenderTexture &texture) const {
    return texture.mapPixelToCoords(screen_pos, GetWorldView(texture));
  }
};

} // namespace steamrot
