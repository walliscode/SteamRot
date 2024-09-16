#pragma once
#include <tuple>


class Component {
protected:
	bool has = false; // if the entity has this 
	Component() = default; // default constructor put under protected so it can be instantiated by itself

public:
	bool getHas() const; // return if the entity has this component
	void setHas(bool has); // set if the entity has this component
	
};

 