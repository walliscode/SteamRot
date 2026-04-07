/////////////////////////////////////////////////
/// @file
/// @brief Implementation of free functions for resize logic operations.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "resize_logic.h"
#include "FailInfo.h"

namespace steamrot::logic::resize {

/////////////////////////////////////////////////
std::expected<std::monostate, FailInfo>
ResizeSceneTexture(sf::RenderTexture &scene_texture,
                   const sf::Vector2u &new_size) {
  if (!scene_texture.resize(new_size)) {
    return std::unexpected(
        FailInfo{FailMode::ResourceCreationFailure,
                 "resize_logic::ResizeSceneTexture: failed to resize scene "
                 "render texture"});
  }
  return std::monostate{};
}

} // namespace steamrot::logic::resize
