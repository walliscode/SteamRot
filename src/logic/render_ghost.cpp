/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for rendering the MrGhost item.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_ghost.h"
#include "overload.h"
#include "render_grimoire_machina.h"

namespace steamrot::logic::render::ghost {

/////////////////////////////////////////////////
void draw_ghost_item(sf::RenderTexture &texture, MrGhost &mr_ghost) {

  std::visit(overload{[&texture](FragmentInstance &instance) {
                        grimoire_machina::draw_fragment_instance(
                            texture, instance, true);
                      },
                      [&texture](JointInstance &instance) {
                        grimoire_machina::draw_joint_instance(texture, instance,
                                                              true);
                      },
                      [](std::monostate &) {
                        // nothing to draw
                      }},
             mr_ghost.m_instance);
}

} // namespace steamrot::logic::render::ghost
