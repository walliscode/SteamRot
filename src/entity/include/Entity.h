// class for the entity
#pragma once
#include <string>
#include "EntityMemoryPool.h"



class Entity
{
	const size_t  m_id;     //id for the entity 
	std::string m_tag;  //tag for the enity, may be removed when an archetype system is in place
	bool m_active;  //tag for destroying entity

	Entity(const std::string& tag, const size_t& id);

public:

	//base functions for querying entity variables

	bool                  isActive() const;
	const std::string&	  tag() const;
	const size_t          id() const;
	void                  destroy();



	//template functiosn for component handling

	template<typename T>
	T& getComponent() {
		return EntityMemoryPool::Instance().getComponent<T>(m_id); //template function to call the get component function from the memory pool when queried for
	}

	template <typename T>
	bool hasComponent() const
	{
		return EntityMemoryPool::Instance().hasComponent<T>(m_id); //template function to call the has component function from the memory pool when queried for
	}


};