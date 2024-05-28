#pragma once
#include "EntityManager.h"


EntityManager::EntityManager(int poolSize)
{
}

int EntityManager::addEntity()
{
	//TODO add entity - get the next free index in the memory pool, set to active and return the index
	//reset the components for the new index

	return 0;
}

void EntityManager::removeEntity(int entityID)
{
	//TODO remove entity - set the index in the memory pool to be free
}