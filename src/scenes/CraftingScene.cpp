/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CraftingScene class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CraftingScene.h"

namespace steamrot {
/////////////////////////////////////////////////
CraftingScene::CraftingScene(const size_t pool_size, const uuids::uuid &id,
                             const GameContext game_context)
    : Scene(pool_size, id, game_context) {}

/////////////////////////////////////////////////
void CraftingScene::sAction() {

  // process action logic
  for (auto &action_logic : m_logics[LogicType::LogicType_Action]) {
    action_logic->RunLogic();
  }
}

/////////////////////////////////////////////////
void CraftingScene::sMovement() {
  // process movement logic
}

/////////////////////////////////////////////////
void CraftingScene::sCollision() {
  // process collision logic
  for (auto &collision_logic : m_logics[LogicType::LogicType_Collision]) {
    collision_logic->RunLogic();
  }
}

/////////////////////////////////////////////////
void CraftingScene::sRender() {
  // process render logic
  for (auto &render_logic : m_logics[LogicType::LogicType_Render]) {
    render_logic->RunLogic();
  }
}
} // namespace steamrot
