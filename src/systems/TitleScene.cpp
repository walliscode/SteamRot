#include "TitleScene.h"
#include <iostream>

namespace steamrot {
TitleScene::TitleScene(const size_t pool_size, const SceneData *scene_data,
                       const uuids::uuid &id)
    : Scene(pool_size, scene_data, id) {
  std::cout << "TitleScene constructor called with ID: " << id << std::endl;
  // Initialization code specific to TitleScreen can go here
}

////////////////////////////////////////////////////////////
void TitleScene::sMovement() {};

} // namespace steamrot
