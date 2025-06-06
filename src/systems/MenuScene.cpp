////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "MenuScene.h"
#include "Scene.h"
#include <SFML/Graphics/RenderTexture.hpp>
#include <cstddef>
#include <memory>

namespace steamrot {
MenuScene::MenuScene(size_t pool_size, const SceneData *scene_data,
                     const uuids::uuid &id)
    : Scene(pool_size, scene_data, id) {}
////////////////////////////////////////////////////////////
void MenuScene::sMovement() {};

////////////////////////////////////////////////////////////
std::unique_ptr<sf::RenderTexture> MenuScene::sDrawToTexture() {

  // a place holder as this needs to be drawn to
  return std::make_unique<sf::RenderTexture>();
}

} // namespace steamrot
