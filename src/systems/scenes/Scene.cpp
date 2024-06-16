#include "Scene.h"

Scene::Scene(const std::string& name, size_t poolSize) 
    : m_name(name), m_entityManager(name, poolSize) {

}


void Scene::update() {

	// put systems here
}

bool Scene::getActive() const {
	return m_active;
}

void Scene::setActive(bool active) {
	m_active = active;
}