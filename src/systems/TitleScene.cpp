#include "TitleScene.h"
#include "actions_generated.h"
#include "logics_generated.h"
#include <iostream>

namespace steamrot {
TitleScene::TitleScene(const size_t pool_size, const SceneData *scene_data,
                       const uuids::uuid &id, const GameContext game_context)
    : Scene(pool_size, scene_data, id, game_context) {
  std::cout << "TitleScene constructor called with ID: " << id << std::endl;
  // Initialization code specific to TitleScreen can go here
}

////////////////////////////////////////////////////////////
void TitleScene::sMovement() {};

/////////////////////////////////////////////////
void TitleScene::sRender() {

  for (auto &render_logic : m_logics[LogicType::LogicType_Render]) {
    render_logic->RunLogic();
  }
}

/////////////////////////////////////////////////
void TitleScene::sCollision() {
  // process collision logic
  for (auto &collision_logic : m_logics[LogicType::LogicType_Collision]) {
    collision_logic->RunLogic();
  }
}

void TitleScene::sAction() {
  // process action logic
  for (auto &action_logic : m_logics[LogicType::LogicType_Action]) {
    action_logic->RunLogic();
  }
  // process actions after all action logic has been run
  ProcessActions();
}

void TitleScene::ProcessActions() {
  // reset actions and data
  m_scene_action = ActionNames{0};
  m_scene_data_package = SceneDataPackage{};
  // process actions
  ActionNames action = ScrapeLogicForActions().first;
  LogicData logic_data = ScrapeLogicForActions().second;

  switch (action) {

  case ActionNames_ACTION_QUIT_GAME: {
    std::cout << "Quitting game from TitleScene." << std::endl;
    m_game_context.game_window.close();
  }

  case ActionNames_ACTION_CHANGE_SCENE: {
    // Change Scene is handled by the SceneManager, so set Scene variables
    m_scene_action = action;
    m_scene_data_package.new_scene_type = logic_data.ui_data_package.scene_type;
  }

  default: {
    // do nothing
    break;
  }
  }
}
} // namespace steamrot
