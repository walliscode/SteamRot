/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CraftingScene class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CraftingScene.h"
#include "logic_execution.h"
#include "scene_change_packet_generated.h"
#include <SFML/Graphics/Color.hpp>

namespace steamrot {
/////////////////////////////////////////////////
CraftingScene::CraftingScene(const uuids::uuid &id,
                             const GameContext &game_context)
    : Scene(SceneType::SceneType_CRAFTING, id, game_context) {}

/////////////////////////////////////////////////
void CraftingScene::sAction() {
  // Execute action logics using extracted free function
  logic::execution::ExecuteActionLogics(m_logic_map);
}

/////////////////////////////////////////////////
void CraftingScene::sMovement() {
  // Execute movement logics using extracted free function
  logic::execution::ExecuteMovementLogics(m_logic_map);
}

/////////////////////////////////////////////////
void CraftingScene::sCollision() {
  // Execute collision logics using extracted free function
  logic::execution::ExecuteCollisionLogics(m_logic_map);
}

/////////////////////////////////////////////////
void CraftingScene::sRender() {
  // clear the render texture
  m_scene_resources.scene_texture.clear(sf::Color::Black);
  // process render logic
  for (auto &render_logic : m_logic_map[LogicType::Render]) {
    render_logic->RunLogic();
  }
}
} // namespace steamrot
