////////////////////////////////////////////////////////////
// headers
////////////////////////////////////////////////////////////
#include "TexturesPackage.h"
#include "log_handler.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <memory>
#include <utility>
namespace steamrot {

////////////////////////////////////////////////////////////
const std::map<size_t, std::unique_ptr<sf::RenderTexture>> &
TexturesPackage::GetTextures() {

  return m_texture_map;
}

////////////////////////////////////////////////////////////
void TexturesPackage::AddTexture(size_t id,
                                 std::unique_ptr<sf::RenderTexture> texture) {

  // if key already exists throw runtime error
  if (m_texture_map.find(id) != m_texture_map.end()) {
    log_handler::ProcessLog(spdlog::level::level_enum::err,
                            log_handler::LogCode::kDuplicateMapKey,
                            "Duplicate scene id in map: " + std::to_string(id));
  } else {
    // add texture to map
    m_texture_map[id] = std::move(texture);
  }
}
} // namespace steamrot
