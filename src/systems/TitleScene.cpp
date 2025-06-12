#include "TitleScene.h"
#include "logics_generated.h"
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

void TitleScene::sRender() {

  for (auto &render_logic : m_logics[LogicType::LogicType_Render]) {
    render_logic->RunLogic();
  }
}
} // namespace steamrot
