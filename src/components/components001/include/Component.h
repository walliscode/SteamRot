#pragma once
#include <tuple>

class Component {
protected:
	bool has = false; // if the entity has this 
	Component() = default; // default constructor put under protected so it can be instantiated by itself
};

 