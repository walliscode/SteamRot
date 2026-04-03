/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the GhostItemRenderLogic class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "GhostItemRenderLogic.h"
#include "GhostItemState.h"
#include <SFML/Graphics/RectangleShape.hpp>

namespace steamrot::logic {

/////////////////////////////////////////////////
GhostItemRenderLogic::GhostItemRenderLogic(const SceneContext scene_context)
    : Logic(scene_context) {}

/////////////////////////////////////////////////
void GhostItemRenderLogic::ProcessLogic() {

  const GhostItemState &ghost_state = m_scene_context.ghost_item_state;

  if (!ghost_state.m_is_active) {
    return;
  }

  // Draw a semi-transparent rectangle centred on the ghost position.
  // The size and colour are intentionally neutral so that every scene that
  // adds this Logic gets a usable default without customisation.
  constexpr float kGhostSize = 32.f;
  constexpr float kHalfSize = kGhostSize / 2.f;

  sf::RectangleShape ghost_shape(sf::Vector2f(kGhostSize, kGhostSize));
  ghost_shape.setPosition(
      {ghost_state.m_position.x - kHalfSize,
       ghost_state.m_position.y - kHalfSize});
  ghost_shape.setFillColor(sf::Color(200, 200, 200, 150));
  ghost_shape.setOutlineColor(sf::Color(255, 255, 255, 200));
  ghost_shape.setOutlineThickness(1.f);

  m_scene_context.scene_texture.draw(ghost_shape);
}

} // namespace steamrot::logic
