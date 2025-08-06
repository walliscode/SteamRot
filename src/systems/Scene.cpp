////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "Scene.h"
#include "BaseLogic.h"
#include "EntityManager.h"
#include "themes_generated.h"
#include <iostream>

namespace steamrot {

////////////////////////////////////////////////////////////
Scene::Scene(const size_t &pool_size, const SceneData *scene_data,
             const uuids::uuid &id, const GameContext game_context)
    : m_entity_manager(pool_size), m_game_context(game_context),
      m_logic_factory(), m_id(id) {

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
        this->m_entity_manager.GetArchetypeManager().GetArchetypes(),
        this->m_render_texture,
        m_game_context.game_window,
        ui_objects,
        m_game_context.asset_manager,
        m_game_context.event_handler};

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

} // namespace steamrot
