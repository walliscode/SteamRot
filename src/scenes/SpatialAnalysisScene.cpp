/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the SpatialAnalysisScene class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SpatialAnalysisScene.h"

namespace steamrot {

/////////////////////////////////////////////////
SpatialAnalysisScene::SpatialAnalysisScene(const GameContext &game_context)
    : Scene(game_context) {}

/////////////////////////////////////////////////
void SpatialAnalysisScene::sPositioning() {
  for (auto &positioning_logic :
       m_scene_resources.logic_map[LogicGrouping::Positioning]) {
    positioning_logic->RunLogic();
  }
}

/////////////////////////////////////////////////
void SpatialAnalysisScene::sCollision() {

  // process collision logick
  for (auto &collision_logic :
       m_scene_resources.logic_map[LogicGrouping::Collision]) {
    collision_logic->RunLogic();
  }
}

/////////////////////////////////////////////////
void SpatialAnalysisScene::sRender() {
  // clear the render texture and the start of each Scene render step
  m_scene_resources.scene_texture.clear(sf::Color::Black);

  for (auto &render_logic :
       m_scene_resources.logic_map[LogicGrouping::Render]) {

    render_logic->RunLogic();
  }

  // display the render texture at the end of each Scene render step
  m_scene_resources.scene_texture.display();
}

/////////////////////////////////////////////////
void SpatialAnalysisScene::sAction() {
  // process action logic
  for (auto &action_logic :
       m_scene_resources.logic_map[LogicGrouping::Action]) {
    action_logic->RunLogic();
  }
}
} // namespace steamrot
