/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CraftingScene class
/////////////////////////////////////////////////

#include "CraftingScene.h"

namespace steamrot {
/////////////////////////////////////////////////
CraftingScene::CraftingScene(const size_t pool_size,
                             const SceneData *scene_data, const uuids::uuid &id,
                             const GameContext game_context)
    : Scene(pool_size, scene_data, id, game_context) {}

/////////////////////////////////////////////////
void CraftingScene::ProcessActions() {
  // process actions
  ActionNames action = ScrapeLogicForActions();
}

/////////////////////////////////////////////////
void CraftingScene::sAction() {

  // process action logic
  for (auto &action_logic : m_logics[LogicType::LogicType_Action]) {
    action_logic->RunLogic();
  }
  // process actions after all action logic has been run
  ProcessActions();
}

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
