/////////////////////////////////////////////////
/// @file
/// @brief Declaration of the IResizeStrategy interface.
///
/// IResizeStrategy is the strategy-pattern interface for responding to
/// window resize events.  Concrete implementations determine how each
/// scene's render texture and view are adjusted when the game window
/// changes size.
///
/// Three concrete strategies ship with the engine:
///   - DirectScaleResizeStrategy   – scales content with the window
///   - CappedScaleResizeStrategy   – scales up to a configurable ceiling
///   - ExtendedViewResizeStrategy  – keeps content size, reveals more world
///
/// Scenes hold one strategy via std::shared_ptr<IResizeStrategy>.
/// Swap the strategy at any time with Scene::SetResizeStrategy().
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Preprocessor Directives
/////////////////////////////////////////////////
#pragma once

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/Vector2.hpp>

namespace steamrot {

/////////////////////////////////////////////////
/// @class IResizeStrategy
/// @brief Strategy interface for handling window resize events.
///
/// Implementors decide how a scene's render texture (and, optionally, its
/// SFML view) is modified when the game window is resized.
/////////////////////////////////////////////////
class IResizeStrategy {
public:
  /////////////////////////////////////////////////
  /// @brief Virtual destructor.
  /////////////////////////////////////////////////
  virtual ~IResizeStrategy() = default;

  /////////////////////////////////////////////////
  /// @brief Called when the game window is resized.
  ///
  /// Implementations may resize @p render_texture, adjust its sf::View,
  /// or perform any other response needed for their specific scaling policy.
  ///
  /// @param new_size        The new window dimensions in pixels.
  /// @param render_texture  The scene's render texture to modify.
  /////////////////////////////////////////////////
  virtual void OnResize(sf::Vector2u new_size,
                        sf::RenderTexture &render_texture) = 0;
};

} // namespace steamrot
