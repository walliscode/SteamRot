
#pragma once

#include "EntityMemoryPool.h"


EntityMemoryPool::EntityMemoryPool(int poolSize) : m_numEntities(poolSize), m_data(std::make_shared<EntityComponentVectorTuple>())
{
	defineFreshTuple(*m_data, poolSize); //call the fresh tuple temple to define all teh vectors in teh tuple
}

int EntityMemoryPool::getNextEntityIndex()
{
	//return the index for the next free entity index
	for (int i = 0; i != std::get<0>(*m_data).size(); ++i) //loop through the size of the active array
	{
		const bool active = std::get<std::vector<CMeta>>(*m_data)[i].isActive; //get the active bool at the current index. std::get<type>(tuple)[index].member
		if (!active)
		{
			return i; //if an index is found to be false, then the position is free and return the index
		}
	}
	return 0; //if no free position was found then return 0 to indicate this
}

std::shared_ptr<EntityComponentVectorTuple> EntityMemoryPool::getData()
{
	return m_data;
}