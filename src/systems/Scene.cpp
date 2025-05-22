////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "Scene.h"

namespace steamrot {

////////////////////////////////////////////////////////////
Scene::Scene(const size_t &pool_size, const json &config_data,
             const uuids::uuid &id)
    : m_action_manager(config_data["actions"]), m_entity_manager(pool_size),
      m_id(id) {}

////////////////////////////////////////////////////////////
size_t Scene::id_counter = 0;

////////////////////////////////////////////////////////////
bool Scene::GetActive() const { return m_active; }

////////////////////////////////////////////////////////////
void Scene::SetActive(bool active) { m_active = active; }

////////////////////////////////////////////////////////////
const uuids::uuid Scene::GetSceneID() { return m_id; }

////////////////////////////////////////////////////////////
void to_json(nlohmann::json &j, const Scene &scene) {
  j = nlohmann::json{
      {"type", "Scene"},

      {"m_paused", scene.m_paused},
      {"m_active", scene.m_active},
      {"m_interactable", scene.m_interactable},
      {"m_current_frame", scene.m_current_frame},
  };
}

} // namespace steamrot
