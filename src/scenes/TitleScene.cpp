/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TitleScene class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TitleScene.h"
#include "logic_execution.h"
#include "scene_change_packet_generated.h"

namespace steamrot {
/////////////////////////////////////////////////
TitleScene::TitleScene(const uuids::uuid &id, const GameContext &game_context)
    : Scene(SceneType::SceneType_TITLE, id, game_context) {}

/////////////////////////////////////////////////
void TitleScene::sMovement() {
  // Execute movement logics using extracted free function
  logic::execution::ExecuteMovementLogics(m_logic_map);
}

/////////////////////////////////////////////////
void TitleScene::sRender() {
  // clear the render texture at the start of each Scene render step
  m_scene_resources.scene_texture.clear(sf::Color::Black);

  // Execute render logics using extracted free function
  logic::execution::ExecuteRenderLogics(m_logic_map);
}

/////////////////////////////////////////////////
void TitleScene::sCollision() {
  // Execute collision logics using extracted free function
  logic::execution::ExecuteCollisionLogics(m_logic_map);
}

/////////////////////////////////////////////////
void TitleScene::sAction() {
  // Execute action logics using extracted free function
  logic::execution::ExecuteActionLogics(m_logic_map);
}

} // namespace steamrot
