/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the SpatialAnalysisScene class.
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "SpatialAnalysisScene.h"
#include "LogicType.h"
#include "positioning_camera.h"
#include <algorithm>

namespace steamrot {
namespace {

/////////////////////////////////////////////////
bool MatchesAnyLogicType(const logic::Logic &logic,
                         std::initializer_list<LogicType> types) {
  return std::find(types.begin(), types.end(), logic.GetLogicType()) !=
         types.end();
}

/////////////////////////////////////////////////
void RunLogicsForTypes(const logic::LogicVector &logics,
                       std::initializer_list<LogicType> types) {
  for (const auto &logic : logics) {
    if (MatchesAnyLogicType(*logic, types)) {
      logic->RunLogic();
    }
  }

  /////////////////////////////////////////////////
  void RunLogicsExceptTypes(const logic::LogicVector &logics,
                            std::initializer_list<LogicType> excluded_types) {
    for (const auto &logic : logics) {
      if (!MatchesAnyLogicType(*logic, excluded_types)) {
        logic->RunLogic();
      }
    }
  }
}

} // anonymous namespace

/////////////////////////////////////////////////
SpatialAnalysisScene::SpatialAnalysisScene(const GameContext &game_context)
    : Scene(game_context) {}

/////////////////////////////////////////////////
void SpatialAnalysisScene::sCollision() {
  RunLogicsForTypes(m_scene_resources.logic_map[LogicGrouping::Collision],
                    {LogicType::UICollision});
}

/////////////////////////////////////////////////
void SpatialAnalysisScene::sRender() {
  // clear the render texture and the start of each Scene render step
  m_scene_resources.scene_texture.clear(sf::Color::Black);

  logic::positioning::camera::apply_world_view(
      m_scene_resources.scene_texture, m_scene_state.camera_state);
  RunLogicsExceptTypes(m_scene_resources.logic_map[LogicGrouping::Render],
                       {LogicType::UIRender});

  logic::positioning::camera::apply_ui_view(m_scene_resources.scene_texture);
  RunLogicsForTypes(m_scene_resources.logic_map[LogicGrouping::Render],
                    {LogicType::UIRender});

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

/////////////////////////////////////////////////
void SpatialAnalysisScene::sPositioning() {
  for (auto &positioning_logic :
       m_scene_resources.logic_map[LogicGrouping::Positioning]) {
    positioning_logic->RunLogic();
  }

  RunLogicsForTypes(m_scene_resources.logic_map[LogicGrouping::Collision],
                    {LogicType::GrimoireMachinaCollision});
}
} // namespace steamrot
