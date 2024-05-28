// class for the entity
#pragma once
#include <string>



class Entity
{
	const size_t  m_id;     //id for the entity 
	std::string   m_tag;  //tag for the enity, may be removed when an archetype system is in place
	bool	      m_active;  //tag for destroying entity

	Entity(const std::string& tag, const size_t& id); //basic constructor for new entities

public:

	//base functions for querying entity variables

	bool                  isActive() const;
	const std::string&	  tag() const;
	const size_t          id() const;
	void                  destroy();
};