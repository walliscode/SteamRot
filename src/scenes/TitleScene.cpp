/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TitleScene class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TitleScene.h"

namespace steamrot {
/////////////////////////////////////////////////
TitleScene::TitleScene(const uuids::uuid &id, const GameContext game_context)
    : Scene(id, game_context) {}

/////////////////////////////////////////////////
SceneType TitleScene::GetSceneType() const {
  static const SceneType scene_type = SceneType::SceneType_TITLE;
  return scene_type;
}
////////////////////////////////////////////////////////////
void TitleScene::sMovement() {};

/////////////////////////////////////////////////
void TitleScene::sRender() {

  for (auto &render_logic : m_logics[LogicType::Render]) {
    render_logic->RunLogic();
  }
}

/////////////////////////////////////////////////
void TitleScene::sCollision() {
  // process collision logic
  for (auto &collision_logic : m_logics[LogicType::Collision]) {
    collision_logic->RunLogic();
  }
}

/////////////////////////////////////////////////
void TitleScene::sAction() {
  // process action logic
  for (auto &action_logic : m_logics[LogicType::Action]) {
    action_logic->RunLogic();
  }
}

} // namespace steamrot
