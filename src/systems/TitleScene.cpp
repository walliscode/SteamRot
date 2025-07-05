#include "TitleScene.h"
#include "logics_generated.h"
#include <iostream>

namespace steamrot {
TitleScene::TitleScene(const size_t pool_size, const SceneData *scene_data,
                       const uuids::uuid &id, const GameContext game_context)
    : Scene(pool_size, scene_data, id, game_context) {
  std::cout << "TitleScene constructor called with ID: " << id << std::endl;
  // Initialization code specific to TitleScreen can go here
}

////////////////////////////////////////////////////////////
void TitleScene::sMovement() {};

/////////////////////////////////////////////////
void TitleScene::sRender() {

  for (auto &render_logic : m_logics[LogicType::LogicType_Render]) {
    render_logic->RunLogic();
  }
}

/////////////////////////////////////////////////
void TitleScene::sCollision() {
  // process collision logic
  for (auto &collision_logic : m_logics[LogicType::LogicType_Collision]) {
    collision_logic->RunLogic();
  }
}

void TitleScene::sAction() {
  // process action logic
  for (auto &action_logic : m_logics[LogicType::LogicType_Action]) {
    action_logic->RunLogic();
  }
}

} // namespace steamrot
