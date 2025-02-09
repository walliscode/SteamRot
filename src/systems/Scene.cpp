#include "Scene.h"
Scene::Scene(const std::string &name, size_t poolSize)
    : m_name(name), m_action_manager(name) {

  // this->m_entity_manager.intialiseEntities(this->m_name);
}

void Scene::update() {

  // put systems here
}

bool Scene::getActive() const { return m_active; }

void Scene::setActive(bool active) { m_active = active; }

// GameEngine &Scene::getEngine() { return m_engine; }

// SceneManager &Scene::getSceneManager() { return m_sceneManager; }

json Scene::toJSON() {
  json j;
  // j["entities"] = m_entity_manager.toJSON();
  return j;
}
