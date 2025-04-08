#include "Scene.h"
Scene::Scene(const std::string &name, const size_t &pool_size)
    : m_name(name), m_action_manager(name), m_entity_manager(pool_size, name) {}

bool Scene::getActive() const { return m_active; }

void Scene::setActive(bool active) { m_active = active; }

void to_json(nlohmann::json &j, const Scene &scene) {
  j = nlohmann::json{
      {"type", "Scene"},
      {"m_name", scene.m_name},
      {"m_paused", scene.m_paused},
      {"m_active", scene.m_active},
      {"m_interactable", scene.m_interactable},
      {"m_current_frame", scene.m_current_frame},
  };
}
