/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for rendering the MrGhost item.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_ghost.h"
#include "render_grimoire_machina.h"

namespace steamrot::logic::render::ghost {

/////////////////////////////////////////////////
void draw_ghost_item(sf::RenderTexture &texture, MrGhost &mr_ghost) {

  std::visit(
      [&texture](auto &instance) {
        using T = std::decay_t<decltype(instance)>;
        if constexpr (std::is_same_v<T, FragmentInstance>) {
          if (!instance.fragment)
            return;
          grimoire_machina::draw_fragment_instance(texture, instance, true);
        } else if constexpr (std::is_same_v<T, JointInstance>) {
          if (!instance.joint)
            return;
          grimoire_machina::draw_joint_instance(texture, instance, true);
        }
        // std::monostate: nothing to draw
      },
      mr_ghost.m_instance);
}

} // namespace steamrot::logic::render::ghost
