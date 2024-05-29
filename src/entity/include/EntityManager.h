
#pragma once
#include "EntityMemoryPool.h"


class EntityManager
{

	std::vector<int> m_entities;  //all active entities
	std::vector<int> m_entitiesToAdd;  //list of entities to add next update
	std::vector<int> m_entitiesToRemove;  //list of entities to remove next update
	std::shared_ptr<EntityMemoryPool>  m_pool; //pool of all entity data
public:
	EntityManager(int poolSize);  //constructor to create a new manager with a size of pool to manage

	int	addEntity(); //add a new entity (assign new values into pool at next index)
	void removeEntity(int id); //remove the entity with the supplied id

	//Entity manager templates: get components and check for has component

	template <typename T>
	T& getComponent(size_t entityID) {
		return std::get<std::vector<T>>((*m_pool).getData())[entityID]; //return the component of the passed component type for the requested entityID
	}

	template <typename T>
	T& hasComponent(size_t entityID) {
		return std::get<std::vector<T>>((*m_pool).getData())[entityID].has(); //return the component of the passed component type for the requested entityID
	}
};