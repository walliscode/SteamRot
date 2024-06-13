#include "Scene.h"

Scene::Scene(int poolSize): m_entityManager(poolSize)
{
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