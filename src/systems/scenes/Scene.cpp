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