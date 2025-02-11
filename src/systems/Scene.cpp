#include "Scene.h"
Scene::Scene(const std::string &name, const size_t &pool_size)
    : m_name(name), m_action_manager(name), m_entity_manager(pool_size) {}

void Scene::update() {

  // put systems here
}

bool Scene::getActive() const { return m_active; }

void Scene::setActive(bool active) { m_active = active; }

json Scene::toJSON() {
  json j;
  // j["entities"] = m_entity_manager.toJSON();
  return j;
}
