#include "TitleScene.h"

namespace steamrot {
TitleScene::TitleScene(const size_t pool_size, const SceneData *scene_data,
                       const uuids::uuid &id)
    : Scene(pool_size, scene_data, id) {
  // Initialization code specific to TitleScreen can go here
}

////////////////////////////////////////////////////////////
void TitleScene::sMovement() {};

////////////////////////////////////////////////////////////
std::unique_ptr<sf::RenderTexture> TitleScene::sDrawToTexture() {

  // a place holder as this needs to be drawn to
  return std::make_unique<sf::RenderTexture>();
}
} // namespace steamrot
