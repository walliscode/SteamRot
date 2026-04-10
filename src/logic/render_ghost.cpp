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

  // Build a render state that translates fragment/joint geometry to the cursor
  sf::RenderStates states;
  states.transform.translate(mr_ghost.m_position);

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
          grimoire_machina::DrawFragmentView(texture, it->second,
                                             ViewDirection::Front, states);

        } else if constexpr (std::is_same_v<T, JointTag>) {
          auto it = grimoire_machina.m_all_joints.find(tag.key);
          if (it == grimoire_machina.m_all_joints.end()) {
            return;
          }
          grimoire_machina::DrawJointView(texture, it->second,
                                          ViewDirection::Front, states);
        }
      },
      mr_ghost.m_selection);
}

} // namespace steamrot::logic::render::ghost
