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
             const uuids::uuid &id, const AssetManager &asset_manager)
    : m_action_manager(scene_data->actions()),
      m_entity_manager(pool_size, scene_data->entity_collection()),
      m_asset_manager(asset_manager), m_logic_factory(), m_data_manager(),
      m_id(id) {
  std::cout << "Scene constructor called with ID: " << id << std::endl;

  // update map of Logic classes
  if (scene_data->logic_collection() != nullptr) {
    std::cout << "Logic collection found, creating logic context." << std::endl;
    // prepare variables for logic context
    const themes::UIObjects *ui_objects =
        m_data_manager.ProvideThemeData(scene_data->ui_theme()->str());
    std::cout << "UI Objects provided." << std::endl;

    // Create the logic context with the current scene data
    LogicContext logic_context{
        m_entity_manager.GetEntityMemoryPool(),
        m_entity_manager.GetArchetypeManager().GetArchetypes(),
        this->m_render_texture, ui_objects, m_asset_manager};

    std::cout << "Logic context created." << std::endl;
    m_logics = m_logic_factory.CreateLogicMap(*scene_data->logic_collection(),
                                              logic_context);
    std::cout << "Logic map created with " << m_logics.size() << " logics."
              << std::endl;
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
