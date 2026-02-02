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
TitleScene::TitleScene(const GameContext &game_context) : Scene(game_context) {}

/////////////////////////////////////////////////
void TitleScene::sMovement() {};

/////////////////////////////////////////////////
void TitleScene::sRender() {
  // clear the render texture and the start of each Scene render step
  m_scene_resources.scene_texture.clear(sf::Color::Black);

  for (auto &render_logic : m_scene_resources.logic_map[LogicType::Render]) {

    render_logic->RunLogic();
  }

  // display the render texture at the end of each Scene render step
  m_scene_resources.scene_texture.display();
}

/////////////////////////////////////////////////
void TitleScene::sCollision() {

  // process collision logick
  for (auto &collision_logic :
       m_scene_resources.logic_map[LogicType::Collision]) {
    collision_logic->RunLogic();
  }
}

/////////////////////////////////////////////////
void TitleScene::sAction() {
  // process action logic
  for (auto &action_logic : m_scene_resources.logic_map[LogicType::Action]) {
    action_logic->RunLogic();
  }
}

} // namespace steamrot
