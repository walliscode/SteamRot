/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for rendering the MrGhost item.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "render_ghost.h"
#include "overload.h"

namespace steamrot::logic::render::ghost {

/////////////////////////////////////////////////
void draw_ghost_item(sf::RenderTexture &texture, MrGhost &mr_ghost) {

  std::visit(overload{[&texture](FragmentInstance &instance) {
                        instance.DrawInstance(texture, true);
                      },
                      [&texture](JointInstance &instance) {
                        instance.DrawInstance(texture, true);
                      },
                      [](std::monostate &) {
                        // nothing to draw
                      }},
             mr_ghost.m_instance);
}

} // namespace steamrot::logic::render::ghost
