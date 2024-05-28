#pragma once

#include "EntityMemoryPool.h"


EntityMemoryPool::EntityMemoryPool(int poolSize)
{
	m_data = malloc(sizeof(EntityComponentVectorTuple) * poolSize); //Not how this works, needs sorting
}

int EntityMemoryPool::getNextEntityIndex()
{
	//return the index for the next free entity index
	return 0;
}