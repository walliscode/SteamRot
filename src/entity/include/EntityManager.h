
#pragma once
#include "Entity.h"
#include "EntityMemoryPool.h"


class EntityManager
{

	std::vector<Entity> m_entities;  //all active entities
	std::vector<Entity> m_entitiesToAdd;  //list of entities to add next update
	std::vector<Entity> m_entitiesToRemove;  //list of entities to remove next update
	EntityMemoryPool m_pool; //pool of all entity data

	EntityManager(int poolSize);  //constructor to create a new manager with a size of pool to manage
public:
	int	addEntity(); //add a new entity (assign new values into pool at next index)
	void removeEntity(int id); //remove the entity with the supplied id
};