/////////////////////////////////////////////////
/// @file
/// @brief Declaration of UIElementRenderCache struct
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>

namespace steamrot {

/////////////////////////////////////////////////
/// @brief Cache for storing SFML rendering objects to avoid per-frame allocation
///
/// This cache stores pre-computed SFML objects (shapes, text) that can be reused
/// across frames, significantly reducing memory allocations and CPU overhead.
/// Objects are only recreated when the underlying UI element properties change.
/////////////////////////////////////////////////
struct UIElementRenderCache {
  /////////////////////////////////////////////////
  /// @brief Cached rectangle shape for border and background
  ///
  /// Stores the pre-configured rectangle with position, size, colors, and outline.
  /// Reused across frames until element geometry or style changes.
  /////////////////////////////////////////////////
  std::optional<sf::RectangleShape> rectangle;

  /////////////////////////////////////////////////
  /// @brief Cached text object for labels and text rendering
  ///
  /// Stores the pre-configured text with font, size, color, and position.
  /// Reused across frames until text content or style changes.
  /////////////////////////////////////////////////
  std::optional<sf::Text> text;

  /////////////////////////////////////////////////
  /// @brief Cached circle shape for dropdown indicators and icons
  ///
  /// Stores the pre-configured circle/triangle with size, color, and rotation.
  /// Reused across frames until element state changes.
  /////////////////////////////////////////////////
  std::optional<sf::CircleShape> shape;

  /////////////////////////////////////////////////
  /// @brief Flag indicating if cached objects need to be regenerated
  ///
  /// Set to true when element properties change (position, size, text, style).
  /// Render functions check this flag to determine if cache needs updating.
  /////////////////////////////////////////////////
  bool is_dirty{true};

  /////////////////////////////////////////////////
  /// @brief Mark cache as dirty, requiring regeneration on next render
  /////////////////////////////////////////////////
  void Invalidate() { is_dirty = true; }

  /////////////////////////////////////////////////
  /// @brief Clear all cached objects
  /////////////////////////////////////////////////
  void Clear() {
    rectangle.reset();
    text.reset();
    shape.reset();
    is_dirty = true;
  }
};

} // namespace steamrot
