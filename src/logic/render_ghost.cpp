/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for rendering the MrGhost item.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_ghost.h"
#include "ViewDirection.h"
#include "render_grimoire_machina.h"
#include <SFML/Graphics/RenderStates.hpp>
#include <variant>

namespace steamrot::logic::render::ghost {

/////////////////////////////////////////////////
void DrawGhostItem(sf::RenderTexture &texture, const MrGhost &mr_ghost,
                   GrimoireMachina &grimoire_machina) {

  static constexpr float k_corner_offset = 5.f;

  std::visit(
      [&](const auto &tag) {
        using T = std::decay_t<decltype(tag)>;

        if constexpr (std::is_same_v<T, std::monostate>) {
          return; // nothing selected — draw nothing

        } else if constexpr (std::is_same_v<T, FragmentTag>) {
          auto it = grimoire_machina.m_all_fragments.find(tag.key);
          if (it == grimoire_machina.m_all_fragments.end()) {
            return;
          }
          const sf::FloatRect bounds =
              it->second.movement_views[ViewDirection::Front].getBounds();
          sf::RenderStates states;
          states.transform.translate(mr_ghost.m_position - bounds.position -
                                     bounds.size -
                                     sf::Vector2f(k_corner_offset,
                                                  k_corner_offset));
          grimoire_machina::DrawFragmentView(texture, it->second,
                                             ViewDirection::Front, states);

        } else if constexpr (std::is_same_v<T, JointTag>) {
          auto it = grimoire_machina.m_all_joints.find(tag.key);
          if (it == grimoire_machina.m_all_joints.end()) {
            return;
          }
          const sf::FloatRect bounds =
              it->second.movement_views[ViewDirection::Front].getBounds();
          sf::RenderStates states;
          states.transform.translate(mr_ghost.m_position - bounds.position -
                                     bounds.size -
                                     sf::Vector2f(k_corner_offset,
                                                  k_corner_offset));
          grimoire_machina::DrawJointView(texture, it->second,
                                          ViewDirection::Front, states);
        }
      },
      mr_ghost.m_selection);
}

} // namespace steamrot::logic::render::ghost
