////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "Scene.h"
#include "EntityManager.h"
#include <iostream>

namespace steamrot {

////////////////////////////////////////////////////////////
Scene::Scene(const size_t &pool_size, const SceneData *scene_data,
             const uuids::uuid &id)
    : m_action_manager(scene_data->actions()),
      m_entity_manager(pool_size, scene_data->entities()), m_logic_factory(),
      m_id(id) {
  std::cout << "Scene constructor called with ID: " << id << std::endl;

  // update map of Logic classes
  if (scene_data->logics() != nullptr) {
    m_logics = m_logic_factory.CreateLogicMap(*scene_data->logics());
  }
}

////////////////////////////////////////////////////////////
bool Scene::GetActive() const { return m_active; }

////////////////////////////////////////////////////////////
void Scene::SetActive(bool active) { m_active = active; }

////////////////////////////////////////////////////////////
const uuids::uuid Scene::GetSceneID() { return m_id; }

} // namespace steamrot
