////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "TexturesPackage.h"
#include "log_handler.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <functional>

namespace steamrot {

////////////////////////////////////////////////////////////
const std::map<uuids::uuid, std::reference_wrapper<sf::RenderTexture>> &
TexturesPackage::GetTextures() {

  return m_texture_map;
}

////////////////////////////////////////////////////////////
void TexturesPackage::AddTexture(const uuids::uuid &scene_id,
                                 sf::RenderTexture &texture) {

  // if key already exists throw runtime error
  if (m_texture_map.find(scene_id) != m_texture_map.end()) {
    log_handler::ProcessLog(spdlog::level::level_enum::err,
                            log_handler::LogCode::kDuplicateMapKey,
                            "Duplicate scene id in map: ");
  } else {
    // add texture to map
    m_texture_map.emplace(scene_id, std::ref(texture));
  }
}
} // namespace steamrot
