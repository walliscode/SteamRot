/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the TitleScene class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "TitleScene.h"
#include "scene_change_packet_generated.h"

namespace steamrot {
/////////////////////////////////////////////////
TitleScene::TitleScene(const uuids::uuid &id, const GameContext &game_context)
    : Scene(SceneType::SceneType_TITLE, id, game_context) {}

////////////////////////////////////////////////////////////
void TitleScene::sMovement() {};

/////////////////////////////////////////////////
void TitleScene::sRender() {
  // clear the render texture and the start of each Scene render step
  m_render_texture.clear(sf::Color::Black);

  if (m_logic_map.find(LogicType::Render) == m_logic_map.end()) {
  }
  for (auto &render_logic : m_logic_map[LogicType::Render]) {

    render_logic->RunLogic();
  }
}

/////////////////////////////////////////////////
void TitleScene::sCollision() {

  // process collision logick
  for (auto &collision_logic : m_logic_map[LogicType::Collision]) {
    collision_logic->RunLogic();
  }
}

/////////////////////////////////////////////////
void TitleScene::sAction() {
  // process action logic
  for (auto &action_logic : m_logic_map[LogicType::Action]) {
    action_logic->RunLogic();
  }
}

} // namespace steamrot
