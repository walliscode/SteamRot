#include "TitleScene.h"
#include "logics_generated.h"

namespace steamrot {
TitleScene::TitleScene(const uuids::uuid &id, const GameContext game_context)
    : Scene(id, game_context) {}

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
