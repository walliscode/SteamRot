/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for rendering the MrGhost item.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_ghost.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Color.hpp>
#include <variant>

namespace steamrot::logic::render::ghost {

/////////////////////////////////////////////////
void DrawGhostItem(sf::RenderTexture &texture, const MrGhost &mr_ghost,
                   GrimoireMachina & /*grimoire_machina*/) {

  // Nothing to draw when no item is selected
  if (std::holds_alternative<std::monostate>(mr_ghost.m_selection)) {
    return;
  }

  // Choose a colour that indicates whether a fragment or joint is selected:
  // cyan for fragments, yellow for joints
  sf::Color ghost_color = std::visit(
      [](const auto &tag) -> sf::Color {
        using T = std::decay_t<decltype(tag)>;
        if constexpr (std::is_same_v<T, FragmentTag>) {
          return sf::Color(0, 200, 200, 180);
        } else if constexpr (std::is_same_v<T, JointTag>) {
          return sf::Color(200, 200, 0, 180);
        } else {
          return sf::Color::Transparent;
        }
      },
      mr_ghost.m_selection);

  // Draw a semi-transparent circle centred on the cursor as a placeholder.
  // This will be replaced with proper geometry once ghost-space vertex arrays
  // are available.
  sf::CircleShape circle(20.f);
  circle.setFillColor(ghost_color);
  circle.setOrigin(circle.getLocalBounds().getCenter());
  circle.setPosition(mr_ghost.m_position);

  texture.draw(circle);
}

} // namespace steamrot::logic::render::ghost
