/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CraftingScene class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CraftingScene.h"
#include "scene_change_packet_generated.h"

namespace steamrot {
/////////////////////////////////////////////////
CraftingScene::CraftingScene(const uuids::uuid &id,
                             const GameContext &game_context)
    : Scene(SceneType::SceneType_CRAFTING, id, game_context) {}

/////////////////////////////////////////////////
void CraftingScene::sAction() {

  // process action logic
  for (auto &action_logic : m_logic_map[LogicType::Action]) {
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
  for (auto &collision_logic : m_logic_map[LogicType::Collision]) {
    collision_logic->RunLogic();
  }
}

/////////////////////////////////////////////////
void CraftingScene::sRender() {
  // process render logic
  for (auto &render_logic : m_logic_map[LogicType::Render]) {
    render_logic->RunLogic();
  }
}
} // namespace steamrot
