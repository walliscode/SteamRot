////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "Scene.h"
#include "BaseLogic.h"
#include "EntityManager.h"
#include "actions_generated.h"
#include "themes_generated.h"
#include <iostream>
#include <utility>

namespace steamrot {

////////////////////////////////////////////////////////////
Scene::Scene(const size_t &pool_size, const SceneData *scene_data,
             const uuids::uuid &id, const GameContext game_context)
    : m_action_manager(scene_data->actions()),
      m_entity_manager(pool_size, scene_data->entity_collection()),
      m_game_context(game_context), m_logic_factory(), m_id(id) {

  // update map of Logic classes
  if (scene_data->logic_collection() != nullptr) {
    std::cout << "Logic collection found, creating logic context." << std::endl;
    // prepare variables for logic context
    const themes::UIObjects *ui_objects =
        m_game_context.data_manager.ProvideThemeData(
            scene_data->ui_theme()->str());
    std::cout << "UI Objects provided." << std::endl;

    // Create the logic context with the current scene data
    LogicContext logic_context{
        m_entity_manager.GetEntityMemoryPool(),
        m_entity_manager.GetArchetypeManager().GetArchetypes(),
        this->m_render_texture,
        m_game_context.game_window,
        ui_objects,
        m_game_context.asset_manager,
        m_game_context.user_events};

    m_logics = m_logic_factory.CreateLogicMap(*scene_data->logic_collection(),
                                              logic_context);
  } else {
    std::cout << "No logic found" << std::endl;
  }
  std::cout << "Scene fully defined" << std::endl;
}

sf::RenderTexture &Scene::GetRenderTexture() { return m_render_texture; }

////////////////////////////////////////////////////////////
bool Scene::GetActive() const { return m_active; }

////////////////////////////////////////////////////////////
void Scene::SetActive(bool active) { m_active = active; }

////////////////////////////////////////////////////////////
const uuids::uuid Scene::GetSceneID() { return m_id; }

/////////////////////////////////////////////////
const std::pair<ActionNames, LogicData> Scene::ScrapeLogicForActions() const {

  // create a default action and data pair to return
  std::pair<ActionNames, LogicData> action_and_data =
      std::make_pair(ActionNames{0}, LogicData{});

  // get the first vector of logics from the map
  for (const auto &logic_pair : m_logics) {
    const auto &logic = logic_pair.second;

    // see if any of the logic instances have an action. if so return early
    for (const auto &logic_instance : logic) {

      ActionNames logic_action = logic_instance->GetLogicAction();

      // 0 should be registered as no action
      if (logic_action != 0) {

        // fill in data
        action_and_data.first = logic_action;
        action_and_data.second = logic_instance->GetLogicData();

        // early return the action and data pair
        return action_and_data;
      }
    }
  }
  return action_and_data;
}

/////////////////////////////////////////////////
const ActionNames &Scene::GetSceneAction() const { return m_scene_action; }

/////////////////////////////////////////////////
const SceneDataPackage &Scene::GetSceneDataPackage() const {
  return m_scene_data_package;
}

} // namespace steamrot
