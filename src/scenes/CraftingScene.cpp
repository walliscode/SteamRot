/////////////////////////////////////////////////
/// @file
/// @brief Implementation of the CraftingScene class
/////////////////////////////////////////////////

/////////////////////////////////////////////////
/// Headers
/////////////////////////////////////////////////
#include "CraftingScene.h"
#include "LogicType.h"
#include "positioning_camera.h"
#include "render/render_background.h"
#include <SFML/Graphics/Color.hpp>
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

  RunLogicsForTypes(m_scene_resources.logic_map[LogicGrouping::Collision],
                    {LogicType::GrimoireMachinaCollision});
}

/////////////////////////////////////////////////
void CraftingScene::sCollision() {
  RunLogicsForTypes(m_scene_resources.logic_map[LogicGrouping::Collision],
                    {LogicType::UICollision});
}

/////////////////////////////////////////////////
void CraftingScene::sRender() {

  // clear the render texture
  m_scene_resources.scene_texture.clear(sf::Color::Black);

  logic::positioning::camera::apply_world_view(
      m_scene_resources.scene_texture, m_scene_state.camera_state);

  // draw the background grid
  logic::render::draw_grid_background({50, 50}, 1, sf::Color(255, 255, 255, 50),
                                      m_scene_resources.scene_texture);

  RunLogicsExceptTypes(m_scene_resources.logic_map[LogicGrouping::Render],
                       {LogicType::UIRender});

  logic::positioning::camera::apply_ui_view(m_scene_resources.scene_texture);
  RunLogicsForTypes(m_scene_resources.logic_map[LogicGrouping::Render],
                    {LogicType::UIRender});

  // display the render texture
  m_scene_resources.scene_texture.display();
}
} // namespace steamrot
