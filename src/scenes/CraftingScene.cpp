/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CraftingScene class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CraftingScene.h"
#include "LogicType.h"
#include <SFML/Graphics/Color.hpp>

namespace steamrot {
/////////////////////////////////////////////////
CraftingScene::CraftingScene(const GameContext &game_context)
    : Scene(game_context) {}

/////////////////////////////////////////////////
void CraftingScene::sAction() {

  // process action logic
  for (auto &action_logic :
       m_scene_resources.logic_map[LogicGrouping::Action]) {
    action_logic->RunLogic();
  }
}

/////////////////////////////////////////////////
void CraftingScene::sPositioning() {
  // process positioning logic
  for (auto &positioning_logic :
       m_scene_resources.logic_map[LogicGrouping::Positioning]) {
    positioning_logic->RunLogic();
  }
}

/////////////////////////////////////////////////
void CraftingScene::sCollision() {
  // process collision logic
  for (auto &collision_logic :
       m_scene_resources.logic_map[LogicGrouping::Collision]) {
    collision_logic->RunLogic();
  }
}

/////////////////////////////////////////////////
void CraftingScene::sRender() {

  // clear the render texture
  m_scene_resources.scene_texture.clear(sf::Color::Black);

  // process render logic
  for (auto &render_logic :
       m_scene_resources.logic_map[LogicGrouping::Render]) {

    render_logic->RunLogic();
  }

  // display the render texture
  m_scene_resources.scene_texture.display();
}
} // namespace steamrot
